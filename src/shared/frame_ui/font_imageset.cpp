// Copyright (C) 2019, Robin Klimonow. All rights reserved.

#include "font_imageset.h"
#include "geometry_buffer.h"


namespace mmo
{
	void FontImageset::SetTexture(TexturePtr texture)
	{
		m_texture = texture;
	}

	FontImage & FontImageset::DefineImage(const Point & position, const Size & size, const Point & renderOffset)
	{
		return DefineImage(Rect(position, size), renderOffset);
	}

	FontImage & FontImageset::DefineImage(const Rect & imageRect, const Point & renderOffset)
	{
		m_images.emplace_back(FontImage(*this, imageRect, renderOffset, 1.0f, 1.0f));
		return m_images.back();
	}

	void FontImageset::Draw(const Rect & srcRect, const Rect & dstRect, GeometryBuffer & buffer) const
	{
		if (m_texture)
		{
			// Bind the texture object
			buffer.SetActiveTexture(m_texture);

			// Determine width and height
			const float w = (float)m_texture->GetWidth();
			const float h = (float)m_texture->GetHeight();

			const float srcU = srcRect.m_left / w;
			const float srcV = srcRect.m_top / h;
			const float dstU = srcRect.m_right / w;
			const float dstV = srcRect.m_bottom / h;

			// Setup geometry
			GeometryBuffer::Vertex vertices[6]{
				// First triangle
				{ { dstRect.m_left, dstRect.m_bottom, 0.0f }, 0xffffffff, { srcU, dstV } },
				{ { dstRect.m_left, dstRect.m_top, 0.0f }, 0xffffffff, { srcU, srcV } },
				{ { dstRect.m_right, dstRect.m_top, 0.0f }, 0xffffffff, { dstU, srcV } },
				// Second triangle
				{ { dstRect.m_right, dstRect.m_top, 0.0f }, 0xffffffff, { dstU, srcV } },
				{ { dstRect.m_right, dstRect.m_bottom, 0.0f }, 0xffffffff, { dstU, dstV } },
				{ { dstRect.m_left, dstRect.m_bottom, 0.0f }, 0xffffffff, { srcU, dstV } }
			};

			// Append vertices
			buffer.AppendGeometry(vertices, 6);
		}
	}
}