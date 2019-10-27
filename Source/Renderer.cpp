/**************************************************************************************\
** File: Renderer.cpp 
** Project: Pong
** Author: David Leksen
** Date: 4/1/16
** 
** Source code file for the Renderer namespace
**
\**************************************************************************************/
#include "pch.h"
#include "Renderer.h"
#include "Rect.h"
#include "Color.h"
#include "Utility.h"
#include "Renderable.h"
#include "Timer.h"
#include "ResourceManager.h"
#include "TextureResource.h"
#include "FontResource.h"
#include "drawtext.h"
#include <SDL.h>
#include <SDL_opengl.h>
#include <GL/GLU.h>
//#include <GL/GL.h>

namespace d2d
{
	namespace Renderer
	{
		namespace
		{
			///-----------------------------\------------------------------------------------------ 
			//|	Private Data				|
			//\-----------------------------/------------------------------------------------------
			SDL_Window* m_windowPtr{};
			SDL_GLContext m_glContext{};
			ResourceManager<TextureResource> m_textureManager;
			ResourceManager<FontResource> m_fontManager;
			bool m_texturesEnabled{ false };
			bool m_blendingEnabled{ false };
			bool m_screenInitialized{ false };

			bool m_textureBinded{ false };
			unsigned int m_bindedTextureID{ 0 };

			bool m_fontBinded{ false };
			unsigned int m_bindedFontID{ 0 };
			int m_bindedDTXFontSize{ 0 };

			bool m_screenSpaceModeEnabled{ false };
			d2d::Rect m_cameraRect;

			const int m_minSamples{ 2 };
			const int m_maxSamples{ 32 };

			bool m_firstPresent{ true };
			Timer m_timer;

			double m_fpsDelaySeconds{};
			double m_secondsSinceFPSUpdate{};
			unsigned int m_frames{};
			double m_fps{};

			///-----------------------\------------------------------------------------------------ 
			//| Name: EnableTextures  |
			//\-----------------------/
			// PostCondition: Enables OpenGL 2D texturing. 
			//-------------------------------------------------------------------------------------
			void EnableTextures()
			{
				if (!m_texturesEnabled)
				{
					glEnable(GL_TEXTURE_2D);
					m_texturesEnabled = true;
				}
			}
			///-----------------------\------------------------------------------------------------ 
			//| Name: DisableTextures |
			//\-----------------------/
			// PostCondition: Disables OpenGL 2D texturing.
			//-------------------------------------------------------------------------------------
			void DisableTextures()
			{
				if (m_texturesEnabled)
				{
					glDisable(GL_TEXTURE_2D);
					m_texturesEnabled = false;
				}
			}
			///-----------------------\------------------------------------------------------------ 
			//| Name: EnableBlending  | 
			//\-----------------------/
			// PostCondition: Enables OpenGL alpha blending.
			//-------------------------------------------------------------------------------------
			void EnableBlending()
			{
				if (!m_blendingEnabled)
				{
					glEnable(GL_BLEND);
					glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
					m_blendingEnabled = true;
				}
			}
			///-----------------------\------------------------------------------------------------ 
			//| Name: DisableBlending |
			//\-----------------------/
			// PostCondition: Disables OpenGL alpha blending.
			//-------------------------------------------------------------------------------------
			void DisableBlending()
			{
				if (m_blendingEnabled)
				{
					glDisable(GL_BLEND);
					m_blendingEnabled = false;
				}
			}
		} // End of Private Data

		///-----------------------------\------------------------------------------------------ 
		//| Name: CreateScreen     		|
		//\-----------------------------/
		// PostCondition: Creates a new window and drawing context
		//-------------------------------------------------------------------------------------
		bool CreateScreen(const ScreenDef& settings)
		{
			// Make sure SDL video subsystem is initialized
			if (!SDL_WasInit(SDL_INIT_VIDEO))
			{
				std::cerr << "Error: Aborting screen initialization. SDL must be initialized first." << std::endl;
				return false;
			}

			// If there is already a window, get rid of it
			DestroyScreen();

			// Use OpenGL 2.1
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
			SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
			SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
			if (settings.samples > 1)
			{
				int clampedSamples = settings.samples;
				d2d::Clamp(clampedSamples, m_minSamples, m_maxSamples);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
				SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, clampedSamples);
			}

			// Create window
			Uint32 windowFlags = SDL_WINDOW_OPENGL;
			if (settings.fullscreen)
			{
				if (settings.width < 1 || settings.height < 1)
					windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
				else
					windowFlags |= SDL_WINDOW_FULLSCREEN;
			}
			m_windowPtr = SDL_CreateWindow(settings.title.c_str(),
				SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
				settings.width, settings.height, windowFlags);
			if (m_windowPtr == nullptr)
			{
				std::cerr << "Error: Failed to create SDL window" << std::endl;
				return false;
			}

			// Create context
			m_glContext = SDL_GL_CreateContext(m_windowPtr);
			if (m_glContext == nullptr)
			{
				std::cerr << "Error: Failed to create SDL OpenGL context" << std::endl;
				return false;
			}

			// Set vsync
			int swapInterval;
			if (settings.vsync)
				swapInterval = 1;
			else
				swapInterval = 0;
			if (SDL_GL_SetSwapInterval(swapInterval) != 0)
			{
				std::cerr << "Warning: Failed to enable vsync" << std::endl;
			}

			EnableTextures();
			EnableBlending();

			// Set Camera to some initial value, even though it will be changed by the user
			SetCameraBounds(b2Vec2(0.0f, 0.0f), GetScreenResolution());

			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// OpenGL settings
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glShadeModel(GL_SMOOTH);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glPointSize(1.0f);
			glLineWidth(1.0f);

			// Initialize FPS counter
			m_timer.Start();
			m_fpsDelaySeconds = settings.fpsDelaySeconds;
			m_secondsSinceFPSUpdate = 0.0;
			m_frames = 0;
			m_fps = 0.0;

			// Record our success
			m_screenInitialized = true;
			return true;
		}
		void SetShowCursor(bool show)
		{
			SDL_ShowCursor(show);
		}
		double GetFPS()
		{
			return m_fps;
		}
		void SetFPSDelay(double seconds)
		{
			if (seconds < 0.0)
				m_fpsDelaySeconds = 0.0;
			else
				m_fpsDelaySeconds = seconds;
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DestroyScreen    		|
		//\-----------------------------/
		// PostCondition: If there is a window, destroys it and cleans up.
		//			If no window exists, gracefully does nothing.
		//-------------------------------------------------------------------------------------
		void DestroyScreen()
		{
			// If there is already a window, get rid of it
			if (m_screenInitialized)
			{
				if (m_windowPtr != nullptr)
				{
					SDL_DestroyWindow(m_windowPtr);
					SDL_ClearError();
					m_windowPtr = nullptr;
				}
				m_screenInitialized = false;
			}
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: LoadTexture     		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool LoadTexture(const std::string &filename, unsigned int& textureID)
		{
			if (m_textureManager.Load(filename, textureID))
				return true;
			else
			{
				std::cerr << "Error: Failed to load texture: " << filename << std::endl;
				textureID = 0;
				return false;
			}
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: LoadFont	     		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool LoadFont(const std::string &filename, unsigned int& fontID)
		{
			if (m_fontManager.Load(filename, fontID))
				return true;
			else
			{
				std::cerr << "Error: Failed to load font: " << filename << std::endl;
				fontID = 0;
				return false;
			}
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: LoadTextureList  		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool LoadTextureList(const std::vector<std::string>& textureFilenameList, std::vector<unsigned int>& m_textureIDList)
		{
			m_textureIDList.clear();
			bool noErrors{ true };
			for (auto textureFilename : textureFilenameList)
			{
				unsigned int textureID{};
				if (!LoadTexture(textureFilename, textureID))
				{
					noErrors = false;
				}
				m_textureIDList.push_back(textureID);
			}
			return noErrors;
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: LoadFontList     		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool LoadFontList(const std::vector<std::string>& fontFilenameList, std::vector<unsigned int>& m_fontIDList)
		{
			m_fontIDList.clear();
			bool noErrors{ true };
			for (auto fontFilename : fontFilenameList)
			{
				unsigned int fontID{};
				if (!LoadFont(fontFilename, fontID))
				{
					noErrors = false;
				}
				m_fontIDList.push_back(fontID);
			}
			return noErrors;
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: UnloadTexture    		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool UnloadTexture(unsigned int textureID)
		{
			if (m_textureManager.Unload(textureID))
			{
				return true;
			}
			else
			{
				std::cerr << "Warning: Tried to unload non-existing textureID: " << textureID << std::endl;
				return false;
			}
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: UnloadFont     		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool UnloadFont(unsigned int fontID)
		{
			if (m_fontManager.Unload(fontID))
			{
				return true;
			}
			else
			{
				std::cerr << "Warning: Tried to unload non-existing fontID: " << fontID << std::endl;
				return false;
			}
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: UnloadTextureList		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool UnloadTextureList(std::vector<unsigned int>& m_textureIDList)
		{
			bool noErrors{ true };
			for (auto textureID : m_textureIDList)
			{
				if (!UnloadTexture(textureID))
					noErrors = false;
			}
			m_textureIDList.clear();
			return noErrors;
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: UnloadFontList   		|
		//\-----------------------------/
		// PostCondition: 
		//-------------------------------------------------------------------------------------
		bool UnloadFontList(std::vector<unsigned int>& m_fontIDList)
		{
			bool noErrors{ true };
			for (auto fontID : m_fontIDList)
			{
				if (!UnloadFont(fontID))
					noErrors = false;
			}
			m_fontIDList.clear();
			return noErrors;
		}
		///-------------------------------------\---------------------------------------------- 
		//| Name: GetXYAspectRatio				|
		//\-------------------------------------/
		// PreCondition: Screen has been initialized.
		//
		// PostCondition: Returns the width to height ratio of the screen's current display mode. 
		//-------------------------------------------------------------------------------------
		float GetXYAspectRatio()
		{
			int width, height;
			SDL_GL_GetDrawableSize(m_windowPtr, &width, &height);
			if (height == 0)
				height = 1;
			return ((float)width / (float)height);
		}
		b2Vec2 GetScreenResolution()
		{
			int width, height;
			SDL_GL_GetDrawableSize(m_windowPtr, &width, &height);
			return b2Vec2((float)width, (float)height);
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: SetCamera     		|
		//\-----------------------------/
		// PostCondition: Future drawing operations will use this camera.
		//-------------------------------------------------------------------------------------
		void SetCamera(const b2Vec2& center,
			const b2Vec2& dimensions)
		{
			m_cameraRect.SetCenter(center, dimensions);

			// Setup a 2D viewport based on camera dimensions
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(m_cameraRect.GetLowerBound().x,		// left
				m_cameraRect.GetUpperBound().x,		// right
				m_cameraRect.GetLowerBound().y,		// bottom
				m_cameraRect.GetUpperBound().y);	// top

			m_screenSpaceModeEnabled = false;
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: SetCameraBounds  		|
		//\-----------------------------/
		// PostCondition: Future drawing operations will use this camera.
		//-------------------------------------------------------------------------------------
		void SetCameraBounds(const b2Vec2& lowerLeft,
			const b2Vec2& upperRight)
		{
			m_cameraRect.SetBounds(lowerLeft, upperRight);

			// Setup a 2D viewport based on camera dimensions
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(lowerLeft.x,			// left
				upperRight.x,		// right
				lowerLeft.y,		// bottom
				upperRight.y);		// top

			m_screenSpaceModeEnabled = false;
		}
		void SetCameraScreenMode()
		{
			m_cameraRect.SetBounds(b2Vec2(0.0f, 0.0f), GetScreenResolution());

			// Setup a 2D viewport based on camera dimensions
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			gluOrtho2D(m_cameraRect.GetLowerBound().x,		// left
				m_cameraRect.GetUpperBound().x,				// right
				m_cameraRect.GetLowerBound().y,				// bottom
				m_cameraRect.GetUpperBound().y);			// top

			m_screenSpaceModeEnabled = true;
		}
		///---------------------\-------------------------------------------------------------- 
		//| Name: SetColor		|
		//\---------------------/
		// PostCondition: Sets the RGBA color for all future drawing operations.
		//-------------------------------------------------------------------------------------
		void SetColor(const d2d::Color& newColor)
		{
			glColor4f(newColor.red, newColor.green, newColor.blue, newColor.alpha);
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawCircle			|
		//\-----------------------------/
		// PreCondition: BeginScene() has been called.
		// PostCondition: A circle with radius is rendered at center.
		//		If radius is negative, its absolute value is used.
		//		If fill is true, the circle is solid. Otherwise, it is an outline.
		//		The color of the circle is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawCircle(const b2Vec2& center, float radius, bool fill)
		{
			int numVertices;
			float radiansPerVertex;
			float angle;

			// Make sure the radius is positive
			if (radius < 0.0f)
				radius *= -1;

			// Primitives need textures off and blending on
			DisableTextures();
			EnableBlending();

			// Save transformation
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			// Move to the local origin
			glTranslatef(center.x, center.y, 0.0f);

			// Begin rendering the circle
			if (fill)
			{
				// Triangle fans for filled mode
				glBegin(GL_TRIANGLE_FAN);
				glVertex2f(0.0f, 0.0f);
			}
			else
			{
				// Line loop for outline mode
				glBegin(GL_LINE_LOOP);
			}

			// Number of vertices is proportional to how big it is
			numVertices = (int)(16.0f + radius * 2.0f);

			// Angle between adjacent vertex pairs
			radiansPerVertex = (b2_pi * 2.0f) / numVertices;

			// Specify vertices
			for (int i = 0; i < numVertices; i++)
			{
				angle = i * radiansPerVertex;
				glVertex2f(cosf(angle) * radius,
					sinf(angle) * radius);
			}

			// The triangle fan needs to close the loop
			if (fill)
			{
				glVertex2f(radius, 0.0f);
			}
			glEnd();

			// Restore transformation
			glPopMatrix();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawPolygon			|
		//\-----------------------------/
		// PreCondition: BeginScene() has been called.
		//		transform specifies the position in world coordinates and the rotation matrix.
		//		vertices is a valid pointer to an array of the polygon's vertices.
		//		vertexCount is the number of vertices in the array.
		//
		// PostCondition: If the vertex array contains garbage, that's what will be drawn.
		//		The color of the polygon is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawPolygon(const b2Transform& transform,
			const b2Vec2* vertices, int vertexCount, bool fill)
		{
			// Check input
			if (vertices == nullptr || vertexCount < 1)
				return;

			// Primitives need textures off and blending on
			DisableTextures();
			EnableBlending();

			// Draw a list of vertices
			b2Vec2 vertex;
			if (fill)
				glBegin(GL_POLYGON);
			else
				glBegin(GL_LINE_LOOP);
			for (int i = 0; i < vertexCount; i++)
			{
				// Apply transform
				vertex = b2Mul(transform, vertices[i]);

				// Specify vertex
				glVertex2f(vertex.x, vertex.y);
			}
			glEnd();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawTexture			|
		//\-----------------------------/
		// PreCondition: BeginScene() has been called.
		//		center specifies the center of the quad in world coordinates.
		//		angleRadians specifies the rotation in radians.
		//		dimensions specifies the size of the quad.  
		//		texID is the ID assigned to the texture you want to draw.
		//
		// PostCondition: The texture is rendered as specified.
		//-------------------------------------------------------------------------------------
		void DrawTexture(const b2Vec2& center, float angleRadians,
			const b2Vec2& dimensions, unsigned int textureID)
		{
			EnableTextures();
			EnableBlending();

			// Bind texture, if it's not already bound.
			if (!m_textureBinded || m_bindedTextureID != textureID)
			{
				// Get OpenGL texture using handle
				TextureResource* texturePtr;
				texturePtr = m_textureManager.GetResourcePtr(textureID);
				if (texturePtr == nullptr)
				{
					std::cerr << "Warning: Attempted to draw texture with invalid textureID: " << textureID << std::endl;
					return;
				}
				glBindTexture(GL_TEXTURE_2D, texturePtr->GetGLTextureID());
				m_bindedTextureID = textureID;
			}

			// Save transformation
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			// Move into position
			glTranslatef(center.x, center.y, 0.0f);

			// Convert angle to degrees for OpenGL
			float angleDegrees{ angleRadians * 57.29578f };

			// Rotate about the z-axis
			glRotatef(angleDegrees, 0.0f, 0.0f, 1.0f);

			// Draw a textured quad
			float halfWidth = dimensions.x * 0.5f;
			float halfHeight = dimensions.y * 0.5f;
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(-halfWidth, -halfHeight);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(halfWidth, -halfHeight);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(halfWidth, halfHeight);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(-halfWidth, halfHeight);
			glEnd();

			// Restore transformation
			glPopMatrix();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawTexturedRect		|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawTexturedRect(const b2Vec2& lowerCorner, const b2Vec2& upperCorner, unsigned int textureID)
		{
			EnableTextures();
			EnableBlending();

			// Bind texture, if it's not already bound.
			if (!m_textureBinded || m_bindedTextureID != textureID)
			{
				// Get OpenGL texture using handle
				TextureResource* texturePtr;
				texturePtr = m_textureManager.GetResourcePtr(textureID);
				if (texturePtr == nullptr)
				{
					std::cerr << "Warning: Attempted to draw texture with invalid textureID: " << textureID << std::endl;
					return;
				}
				glBindTexture(GL_TEXTURE_2D, texturePtr->GetGLTextureID());
				m_bindedTextureID = textureID;
			}

			// Draw a textured quad
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f(lowerCorner.x, lowerCorner.y);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(upperCorner.x, lowerCorner.y);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(upperCorner.x, upperCorner.y);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(lowerCorner.x, upperCorner.y);
			glEnd();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawRect				|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawRect(const b2Vec2& position, const Rect& rect, bool fill)
		{
			DisableTextures();
			EnableBlending();

			float left = rect.GetLowerBound().x;
			float right = rect.GetUpperBound().x;
			float bottom = rect.GetLowerBound().y;
			float top = rect.GetUpperBound().y;

			if (m_screenSpaceModeEnabled)
			{
				b2Vec2 screenDim = GetScreenResolution();
				left *= screenDim.x;
				right *= screenDim.x;
				bottom *= screenDim.y;
				top *= screenDim.y;
			}

			// Save transformation
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			// Move into position
			glTranslatef(position.x, position.y, 0.0f);

			if (fill)
				glBegin(GL_QUADS);
			else
				glBegin(GL_LINE_LOOP);
			glVertex2f(left, bottom);
			glVertex2f(right, bottom);
			glVertex2f(right, top);
			glVertex2f(left, top);
			glEnd();

			// Restore transformation
			glPopMatrix();
		}
		void DrawRect(const Rect& rect, bool fill)
		{
			DisableTextures();
			EnableBlending();

			float left = rect.GetLowerBound().x;
			float right = rect.GetUpperBound().x;
			float bottom = rect.GetLowerBound().y;
			float top = rect.GetUpperBound().y;

			if (m_screenSpaceModeEnabled)
			{
				b2Vec2 screenDim = GetScreenResolution();
				left *= screenDim.x;
				right *= screenDim.x;
				bottom *= screenDim.y;
				top *= screenDim.y;
			}

			if (fill)
				glBegin(GL_QUADS);
			else
				glBegin(GL_LINE_LOOP);
			glVertex2f(left, bottom);
			glVertex2f(right, bottom);
			glVertex2f(right, top);
			glVertex2f(left, top);
			glEnd();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawLine				|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawLine(const b2Vec2& p1, const b2Vec2& p2)
		{
			DisableTextures();
			EnableBlending();

			glBegin(GL_LINES);
			glVertex2f(p1.x, p1.y);
			glVertex2f(p2.x, p2.y);
			glEnd();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawLineStrip			|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawLineStrip(const b2Transform& transform,
			const b2Vec2* vertices, int vertexCount)
		{
			// Check input
			if (vertices == nullptr || vertexCount < 1)
				return;

			DisableTextures();
			EnableBlending();

			// Draw a list of vertices
			b2Vec2 vertex;
			glBegin(GL_LINE_STRIP);
			for (int i = 0; i < vertexCount; i++)
			{
				// Apply transform
				vertex = b2Mul(transform, vertices[i]);

				// Specify vertex
				glVertex2f(vertex.x, vertex.y);
			}
			glEnd();
		}
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawString			|
		//\-----------------------------/
		// PreCondition: fontID was returned by GetFontID.
		//		BeginScene() has been called.
		//		
		// PostCondition: The string is rendered at the specified size, position, and font.
		//		The color of the text is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawString(const std::string &text,
			const b2Vec2& position,
			Alignment alignment,
			float fontSize, unsigned int fontID)
		{
			DisableTextures();
			EnableBlending();

			if (!m_fontBinded || m_bindedFontID != fontID)
			{
				// Find the loaded font with matching id, and enable it.
				FontResource* fontPtr;
				fontPtr = m_fontManager.GetResourcePtr(fontID);
				if (fontPtr == nullptr)
				{
					std::cerr << "Warning: Font ID refers to a non-existing resource: " << fontID << std::endl;
					return;
				}
				m_fontBinded = true;
				m_bindedDTXFontSize = fontPtr->GetDTXFontSize();
				m_bindedFontID = fontID;

				//std::cerr << "Enabling font: " << fontID << " Size: " << fontSize << std::endl;
				dtx_use_font(fontPtr->GetDTXFontPtr(), m_bindedDTXFontSize);
			}
			if (m_bindedDTXFontSize < 0)
			{
				std::cerr << "Error: Cannot have non-positive dtxFontSize" << std::endl;
				return;
			}
			// Draw the text
			glMatrixMode(GL_MODELVIEW);
			glPushMatrix();

			b2Vec2 screenDim = GetScreenResolution();
			if (m_screenSpaceModeEnabled)
			{
				float averageOfScreenDimensions = (screenDim.x + screenDim.y) * 0.5f;
				fontSize *= averageOfScreenDimensions;
			}

			if (m_screenSpaceModeEnabled)
			{
				glTranslatef(position.x * screenDim.x, position.y * screenDim.y, 0.0f);
			}
			else
			{
				glTranslatef(position.x, position.y, 0.0f);
			}

			float scale = fontSize / (float)m_bindedDTXFontSize;
			glScalef(scale, scale, 1.0f);

			// Alignment: dtx string alignment is left bottom of the first character of the first line,
			//	so we have to translate to that point.
			const float fontHeightToLineHeightRatio = 0.6f;
			struct dtx_box stringDimensions;
			dtx_string_box(text.c_str(), &stringDimensions);
			const float lineHeight = dtx_line_height();
			const float fontHeight = fontHeightToLineHeightRatio * lineHeight;
			const float fontPadding = 0.0f;

			float transX, transY;
			switch (alignment)
			{
			case Alignment::LEFT_TOP:
				transX = fontPadding;
				transY = -(fontHeight + fontPadding);
				break;
			case Alignment::CENTER_TOP:
				transX = -0.5f * stringDimensions.width;
				transY = -(fontHeight + fontPadding);
				break;
			case Alignment::RIGHT_TOP:
				transX = -(stringDimensions.width + fontPadding);
				transY = -(fontHeight + fontPadding);
				break;
			case Alignment::LEFT_CENTER:
				transX = fontPadding;
				transY = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::CENTERED:
				transX = -0.5f * stringDimensions.width;
				transY = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::RIGHT_CENTER:
				transX = -(stringDimensions.width + fontPadding);
				transY = 0.5f * stringDimensions.height - fontHeight;
				break;
			case Alignment::LEFT_BOTTOM:  // dtx origin 
			default:
				transX = fontPadding;
				transY = stringDimensions.height + fontPadding - fontHeight;
				break;
			case Alignment::CENTER_BOTTOM:
				transX = -0.5f * stringDimensions.width;
				transY = (stringDimensions.height + fontPadding) - fontHeight;
				break;
			case Alignment::RIGHT_BOTTOM:
				transX = -(stringDimensions.width + fontPadding);
				transY = stringDimensions.height + fontPadding - fontHeight;
				break;
			}
			glTranslatef(transX, transY, 0.0f);

			dtx_string(text.c_str());

			glPopMatrix();
		}
		void DrawTextBlock(const TextBlock& textBlock)
		{
			SetColor(textBlock.color);
			DrawString(textBlock.text,
				textBlock.screenPosition, textBlock.alignment, 
				textBlock.fontSize, textBlock.fontID);
		}
		///---------------------\-------------------------------------------------------------- 
		//| Name: PresentScene	| Call once per frame after drawing
		//\---------------------/
		// PostCondition: Updates the screen and gets isReady for drawing the next frame.
		//-------------------------------------------------------------------------------------
		void PresentScene()
		{
			// Update FPS periodically
			++m_frames;
			m_timer.Tick();
			m_secondsSinceFPSUpdate += m_timer.GetDeltaSeconds();
			if (m_secondsSinceFPSUpdate > 0.0 &&
				m_secondsSinceFPSUpdate >= m_fpsDelaySeconds)
			{
				m_fps = m_frames / m_secondsSinceFPSUpdate;
				m_frames = 0;
				m_secondsSinceFPSUpdate = 0.0;
			}

			// Swap buffers
			if (m_windowPtr)
				SDL_GL_SwapWindow(m_windowPtr);

			// Clear the model matrix and screen for the next frame
			glClear(GL_COLOR_BUFFER_BIT);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();

			// Print out the last SDL error message if one exists
			std::string errorMessage{ SDL_GetError() };
			if (errorMessage.size() > 0)
			{
				std::cerr << "SDL Error: " << SDL_GetError() << std::endl;
				SDL_ClearError();
			}
		}
	}
} // End of d2d namespace