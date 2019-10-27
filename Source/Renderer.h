/**************************************************************************************\
** File: Renderer.h 
** Project: Pong
** Author: David Leksen
** Date: 4/1/16
** 
** Header file for the Renderer namespace
**
\**************************************************************************************/
#pragma once
#include "Rect.h"
#include "Color.h"
#include "Utility.h"

namespace d2d
{
	class TextBlock;
	class ScreenDef
	{
	public:
		int width, height;	// Resolution, if zero, requests fullscreen desktop resolution
		int x, y;			// Top left
		bool fullscreen, vsync;
		int samples; // [2-32] for antialiasing
		double fpsDelaySeconds;
		std::string title;

		ScreenDef() : width{0}, height{0}, x{0}, y{0}, 
			fullscreen{true}, vsync{true}, samples{0}, fpsDelaySeconds{0.25}{}
	};
	namespace Renderer
	{
		///-----------------------------\------------------------------------------------------ 
		//| Name: CreateScreen     		|
		//\-----------------------------/
		// PostCondition: Creates a new window and drawing context.
		//			If there was already a window, it gets destroyed first.
		//-------------------------------------------------------------------------------------
		bool CreateScreen(const ScreenDef& settings);
		void SetShowCursor(bool show);
		double GetFPS();
		void SetFPSDelay(double seconds);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DestroyScreen    		|
		//\-----------------------------/
		// PostCondition: If there is a window, destroys it and cleans up.
		//			If no window exists, gracefully does nothing.
		//-------------------------------------------------------------------------------------
		void DestroyScreen();

		///-------------------------------------\---------------------------------------------- 
		//| Name: GetXYAspectRatio				|
		//\-------------------------------------/
		// PreCondition: Screen has been initialized.
		//
		// PostCondition: Returns the width to height ratio of the screen's current display mode. 
		//-------------------------------------------------------------------------------------
		float GetXYAspectRatio();
		b2Vec2 GetScreenResolution();

		///-----------------------------\------------------------------------------------------ 
		//| Name: SetCamera     		|
		//\-----------------------------/
		// PostCondition: Future drawing operations will use this camera.
		//-------------------------------------------------------------------------------------
		void SetCamera(const b2Vec2& center,
					   const b2Vec2& dimensions);

		///-----------------------------\------------------------------------------------------ 
		//| Name: SetCameraBounds  		|
		//\-----------------------------/
		// PostCondition: Future drawing operations will use this camera.
		//-------------------------------------------------------------------------------------
		void SetCameraBounds(const b2Vec2& lowerLeft,
							 const b2Vec2& upperRight);
		void SetCameraScreenMode();

		///---------------------\-------------------------------------------------------------- 
		//| Name: SetColor		|
		//\---------------------/
		// PostCondition: Sets the RGBA color for all future drawing operations.
		//-------------------------------------------------------------------------------------
		void SetColor(const Color& newColor);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawCircle			|
		//\-----------------------------/
		// PostCondition: 
		//		A circle with radius is rendered at center.
		//		If radius is negative, its absolute value is used.
		//		If fill is true, the circle is solid. Otherwise, it is an outline.
		//		The color of the circle is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawCircle(const b2Vec2& center, float radius, bool fill);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawPolygon			|
		//\-----------------------------/
		// PreCondition: 
		//		transform specifies the position in world coordinates and the rotation matrix.
		//		vertices is a valid pointer to an array of the polygon's vertices.
		//		vertexCount is the number of vertices in the array.
		//
		// PostCondition: 
		//		If the vertex array contains garbage, that's what will be drawn.
		//		The color of the polygon is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawPolygon(const b2Transform& transform,
			const b2Vec2* vertices, int vertexCount, bool fill = true);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawRect				|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawRect(const b2Vec2& position, const Rect& rect, bool fill = true);
		void DrawRect(const Rect& rect, bool fill);
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawLine				|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawLine(const b2Vec2& p1, const b2Vec2& p2);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawLineStrip			|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawLineStrip(const b2Transform& transform,
			const b2Vec2* vertices, int vertexCount);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawTexture			|
		//\-----------------------------/
		// PreCondition:
		//		center specifies the center of the quad in world coordinates.
		//		angleRadians specifies the rotation in radians.
		//		dimensions specifies the size of the quad.  
		//		texID is the ID assigned to the texture you want to draw.
		//
		// PostCondition: The texture is rendered as specified.
		//-------------------------------------------------------------------------------------
		void DrawTexture(const b2Vec2& center, float angleRadians,
			const b2Vec2& dimensions, unsigned int textureID);
		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawRect				|
		//\-----------------------------/
		//-------------------------------------------------------------------------------------
		void DrawTexturedRect(const b2Vec2& lowerLeft, const b2Vec2& upperRight, unsigned int textureID);

		///-----------------------------\------------------------------------------------------ 
		//| Name: DrawString			|
		//\-----------------------------/
		// PreCondition: fontID was returned by LoadFont.
		//		
		// PostCondition: 
		//		The text is rendered at the specified size, position, and font.
		//		The color of the text is determined by the last call to SetColor().
		//-------------------------------------------------------------------------------------
		void DrawString(const std::string &text,
			const b2Vec2& position,
			Alignment alignment,
			float fontSize, unsigned int fontID);
		void DrawTextBlock(const TextBlock& textBlock);

		///---------------------\-------------------------------------------------------------- 
		//| Name: PresentScene	| Call once per frame after drawing
		//\---------------------/
		// PostCondition: Video buffers are swapped and the drawing buffer is cleared and isReady
		//			for drawing of the next frame.
		//-------------------------------------------------------------------------------------
		void PresentScene();

		// Reference counted texture and font caches
		bool LoadFont(const std::string& filename, unsigned int& fontID);
		bool LoadTexture(const std::string& filename, unsigned int& textureID);
		bool LoadFont(const std::string& filename, unsigned int& fontID);
		bool LoadTextureList(const std::vector<std::string>& textureFilenameList, std::vector<unsigned int>& m_textureIDList);
		bool LoadFontList(const std::vector<std::string>& fontFilenameList, std::vector<unsigned int>& m_fontIDList);
		bool UnloadTexture(unsigned int textureID);
		bool UnloadFont(unsigned int fontID);
		bool UnloadTextureList(std::vector<unsigned int>& m_textureIDList);
		bool UnloadFontList(std::vector<unsigned int>& m_fontIDList);
	}
}
