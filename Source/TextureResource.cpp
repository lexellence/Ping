/**************************************************************************************\
** File: TextureResource.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the TextureResource class
**
\**************************************************************************************/
#include "pch.h"
#include "TextureResource.h"
#include "Resource.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_opengl.h>

namespace d2d
{
	namespace
	{
		void InvertSurface(SDL_Surface* surface)
		{
			if (surface == nullptr)
			{
				std::cerr << "Warning: Attempted to invert a null surface pointer" << std::endl;
				return;
			}

			int pitch = surface->pitch;
			int height = surface->h;
			void *imagePixels = surface->pixels;

			int index;
			void* temp_row;
			int height_div_2;

			temp_row = (void *)malloc(pitch);
			if (temp_row == nullptr)
			{
				std::cerr << "Error: Not enough memory for image inversion" << std::endl;
				return;
			}
			//if height is odd, don't need to swap middle row
			height_div_2 = (int)(height * .5);
			for (index = 0; index < height_div_2; index++)
			{
				//uses string.h
				memcpy((Uint8 *)temp_row,
					(Uint8 *)(imagePixels)+
					pitch * index,
					pitch);

				memcpy(
					(Uint8 *)(imagePixels)+
					pitch * index,
					(Uint8 *)(imagePixels)+
					pitch * (height - index - 1),
					pitch);
				memcpy(
					(Uint8 *)(imagePixels)+
					pitch * (height - index - 1),
					temp_row,
					pitch);
			}
			free(temp_row);
		}
		bool GenerateGLTexture(const std::string& filename, GLuint& texID)
		{
			// Initialize SDL_image on the first call to this function
			static bool SDL_image_initialized = false;
			if (!SDL_image_initialized)
			{
				// If the initialization of SDL_image fails
				int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					std::cerr << "Error: Failed to initialize SDL_image" << std::endl;
					return false;
				}
				else
				{
					// Free SDL_image at the end of the program
					atexit(IMG_Quit);

					// Prevent initializing more than once.
					SDL_image_initialized = true;
				}
			}

			// Load surface from texture file
			SDL_Surface* surface = IMG_Load(filename.c_str());
			if (surface == nullptr)
			{
				std::cerr << "Error: SDL_image failed to load file " << filename << std::endl;
				return false;
			}

			// Flip it so it complies with opengl's lower-left origin
			InvertSurface(surface);

			// Does it use the alpha channel?
			int colorMode = GL_RGB;
			if (surface->format->BytesPerPixel == 4)
				colorMode = GL_RGBA;

			// Create the OpenGL texture
			glGenTextures(1, &texID);
			glBindTexture(GL_TEXTURE_2D, texID);
			glTexImage2D(GL_TEXTURE_2D, 0, colorMode, surface->w, surface->h, 0,
				colorMode, GL_UNSIGNED_BYTE, surface->pixels);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

			// We don't need the surface anymore
			SDL_FreeSurface(surface);
			return true;
		}
	}
	TextureResource::TextureResource(const std::string& filename)
	{
		bool loadSuccess = GenerateGLTexture(filename, m_glTextureID);
		if (!loadSuccess)
		{
			m_glTextureID = 0;
			std::cerr << "Error: Failed to generate OpenGL texture " << filename << std::endl;
		}
		SetLoadSuccess(loadSuccess);
	}
	TextureResource::~TextureResource()
	{
		glDeleteTextures(1, &m_glTextureID);
	}
	GLuint TextureResource::GetGLTextureID() const
	{
		return m_glTextureID;
	}
}