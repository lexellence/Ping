/**************************************************************************************\
** File: Renderable.h
** Project: Pong
** Author: David Leksen
** Date: 4/16/2016
**
** Header file for the Renderable interface
**
\**************************************************************************************/
#ifndef RENDERABLE_H
#define RENDERABLE_H
#include "Color.h"
#include "Renderer.h"

namespace d2d
{
	class RenderableElement
	{
	public:
		Color color;

		RenderableElement(const Color& newColor);
	};
	class Circle : public RenderableElement
	{
	public:
		b2Vec2 position;
		float radius;
		bool fill;

		Circle(b2Vec2 newPosition, 
			   float newRadius,
			   bool newFill, 
			   const Color& newColor);
	};
	class TextBlock : public RenderableElement
	{
	public:
		std::string text;
		b2Vec2 screenPosition;
		Alignment alignment;
		float fontSize;
		unsigned int fontID;

		TextBlock(const std::string& newText, 
				  const b2Vec2& newPosition,
				  Alignment newAlignment,
				  float newFontSize, 
				  unsigned int newFontID,
				  const Color& newColor);
	};
} // namespace Renderable
#endif //RENDERABLE_H