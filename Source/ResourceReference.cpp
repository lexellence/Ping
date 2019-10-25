/**************************************************************************************\
** File: ResourceReference.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the ResourceReference, TextureResource, FontResource classes
**
\**************************************************************************************/
#include "pch.h"
#include "ResourceReference.h"
#include "Renderer.h"

namespace d2d
{
	// Base class
	ResourceReference::ResourceReference(std::string filename)
		:m_filename{ filename }
	{}
	unsigned int ResourceReference::GetID() const
	{
		return m_id;
	}
	std::string ResourceReference::GetFilename() const
	{
		return m_filename;
	}

	// Texture
	TextureReference::TextureReference(std::string filename)
	  :	ResourceReference(filename)
	{
		Renderer::LoadTexture(filename, m_id);
	}
	TextureReference::~TextureReference()
	{
		Renderer::UnloadTexture(m_id);
	}

	// Font
	FontReference::FontReference(std::string filename)
	  :	ResourceReference(filename)
	{
		Renderer::LoadFont(filename, m_id);
	}
	FontReference::~FontReference()
	{
		Renderer::UnloadFont(m_id);
	}
}