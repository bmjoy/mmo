// Copyright (C) 2019, Robin Klimonow. All rights reserved.

#include "player.h"
#include "player_manager.h"
#include "database.h"

#include "base/random.h"
#include "base/constants.h"
#include "base/sha1.h"
#include "base/clock.h"
#include "base/weak_ptr_function.h"
#include "log/default_log_levels.h"

#include <iomanip>
#include <functional>


namespace mmo
{
	Player::Player(
		PlayerManager& playerManager,
		AsyncDatabase& database, 
		std::shared_ptr<Client> connection, 
		const String & address)
		: m_manager(playerManager)
		, m_database(database)
		, m_connection(std::move(connection))
		, m_address(address)
	{
		// Generate random encryption seed
		std::uniform_int_distribution<uint32> dist;
		m_seed = dist(RandomGenerator);

		// Setup ourself as listener
		m_connection->setListener(*this);
	}

	void Player::Destroy()
	{
		m_connection->resetListener();
		m_connection.reset();

		m_manager.PlayerDisconnected(*this);
	}

	void Player::connectionLost()
	{
		ILOG("Client " << m_address << " disconnected");
		Destroy();
	}

	void Player::connectionMalformedPacket()
	{
		ILOG("Client " << m_address << " sent malformed packet");
		Destroy();
	}

	PacketParseResult Player::connectionPacketReceived(auth::IncomingPacket & packet)
	{
		const auto packetId = packet.GetId();
		bool isValid = true;

		PacketHandler handler = nullptr;

		{
			// Lock packet handler access
			std::scoped_lock lock{ m_packetHandlerMutex };

			// Check for packet handlers
			auto handlerIt = m_packetHandlers.find(packetId);
			if (handlerIt == m_packetHandlers.end())
			{
				WLOG("Packet 0x" << std::hex << (uint16)packetId << " is either unhandled or simply currently not handled");
				return PacketParseResult::Disconnect;
			}

			handler = handlerIt->second;
		}

		// Execute the packet handler and return the result
		return handler(packet);
	}

	PacketParseResult Player::OnLogonChallenge(auth::IncomingPacket & packet)
	{
		ILOG("Received OnLogonChallenge packet");

		return PacketParseResult::Pass;
	}

	void Player::SendAuthChallenge()
	{
		// We will start accepting LogonChallenge packets from the client
		RegisterPacketHandler(auth::client_packet::LogonChallenge, *this, &Player::OnLogonChallenge);

		// Send the LogonChallenge packet to the client including our generated seed
		m_connection->sendSinglePacket([this](mmo::auth::OutgoingPacket& packet) {
			packet.Start(mmo::auth::server_packet::LogonChallenge);
			packet << io::write<uint32>(m_seed);
			packet.Finish();
		});
	}

	void Player::RegisterPacketHandler(uint8 opCode, PacketHandler && handler)
	{
		std::scoped_lock lock{ m_packetHandlerMutex };

		auto it = m_packetHandlers.find(opCode);
		if (it == m_packetHandlers.end())
		{
			m_packetHandlers.emplace(std::make_pair(opCode, std::forward<PacketHandler>(handler)));
		}
		else
		{
			it->second = std::forward<PacketHandler>(handler);
		}
	}

	void Player::ClearPacketHandler(uint8 opCode)
	{
		std::scoped_lock lock{ m_packetHandlerMutex };

		auto it = m_packetHandlers.find(opCode);
		if (it != m_packetHandlers.end())
		{
			m_packetHandlers.erase(it);
		}
	}
}
