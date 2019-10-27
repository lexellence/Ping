/**************************************************************************************\
** File: TextureResource.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the TextureResource class
**
\**************************************************************************************/
#pragma once
#include "Resource.h"
#include <SDL_opengl.h>

namespace d2d
{
	class TextureResource : public Resource
	{
	public:
		TextureResource(const std::string& filename);
		~TextureResource();
		GLuint GetGLTextureID() const;

	private:
		GLuint m_glTextureID{ 0 };
	};
}
