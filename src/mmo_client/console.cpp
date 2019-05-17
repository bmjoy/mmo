// Copyright (C) 2019, Robin Klimonow. All rights reserved.

#include "console.h"
#include "console_commands.h"
#include "event_loop.h"
#include "screen.h"

#include "base/filesystem.h"
#include "log/default_log_levels.h"
#include "graphics/graphics_device.h"


namespace mmo
{
	std::map<std::string, Console::ConsoleCommand, StrCaseIComp> Console::s_consoleCommands;

	void Console::Initialize(const std::string& configFile)
	{
		// Ensure the folder is created
		std::filesystem::path folder = configFile;
		std::filesystem::create_directories(folder.parent_path());

		// Register some default console commands
		RegisterCommand("ver", console_commands::ConsoleCommand_Ver, ConsoleCommandCategory::Default, "Displays the client version.");
		RegisterCommand("run", console_commands::ConsoleCommand_Run, ConsoleCommandCategory::Default, "Runs a console script.");

		// Load the config file
		console_commands::ConsoleCommand_Run("run", configFile);

		// Initialize the graphics api
		auto& device = GraphicsDevice::CreateD3D11();
		device.SetWindowTitle("MMORPG");

		// Initialize the screen system
		Screen::Initialize();
	}

	void Console::Destroy()
	{
		// Destroy the screen system
		Screen::Destroy();

		// Destroy the graphics device
		GraphicsDevice::Destroy();

		// Remove default console commands
		UnregisterCommand("run");
		UnregisterCommand("ver");
	}

	inline void Console::RegisterCommand(const std::string & command, ConsoleCommandHandler handler, ConsoleCommandCategory category, const std::string & help)
	{
		// Don't do anything if this console command is already registered
		auto it = s_consoleCommands.find(command);
		if (it != s_consoleCommands.end())
		{
			return;
		}

		// Build command structure and add it
		ConsoleCommand cmd;
		cmd.category = category;
		cmd.help = std::move(help);
		cmd.handler = std::move(handler);
		s_consoleCommands.emplace(command, cmd);
	}

	inline void Console::UnregisterCommand(const std::string & command)
	{
		// Remove the respective iterator
		auto it = s_consoleCommands.find(command);
		if (it != s_consoleCommands.end())
		{
			s_consoleCommands.erase(it);
		}
	}

	void Console::ExecuteCommand(std::string commandLine)
	{
		// Will hold the command name
		std::string command;
		std::string arguments;

		// Find the first space and use it to get the command
		auto space = commandLine.find(' ');
		if (space == commandLine.npos)
		{
			command = commandLine;
		}
		else
		{
			command = commandLine.substr(0, space);
			arguments = commandLine.substr(space + 1);
		}

		// If somehow the command is empty, just stop here without saying anything.
		if (command.empty())
		{
			return;
		}

		// Check if such argument exists
		auto it = s_consoleCommands.find(command);
		if (it == s_consoleCommands.end())
		{
			ELOG("Unknown console command \"" << command << "\"");
			return;
		}

		// Now execute the console commands handler if there is any
		if (it->second.handler)
		{
			it->second.handler(command, arguments);
		}
	}
}