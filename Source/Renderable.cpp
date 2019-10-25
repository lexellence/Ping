/**************************************************************************************\
** File: Renderable.cpp
** Project: Pong
** Author: David Leksen
** Date: 4/16/2016
**
** Source code file for the Renderable interface
**
\**************************************************************************************/
#include "pch.h"
#include "Renderable.h"
#include "Color.h"
#include "Renderer.h"

namespace d2d
{
	RenderableElement::RenderableElement(const Color& newColor)
	  :	color(newColor)
	{}

	Circle::Circle(b2Vec2 newPosition, 
				   float newRadius,
				   bool newFill, 
				   const Color& newColor)
	  :	RenderableElement{ newColor },
		position{ newPosition },
		radius{ newRadius },
		fill{ newFill }
	{}

	TextBlock::TextBlock(const std::string& newText, 
						 const b2Vec2& newPosition,
						 Alignment newAlignment,
						 float newFontSize, 
						 unsigned int newFontID,
						 const Color& newColor)
	  :	RenderableElement{ newColor }, 
		text{ newText },
		screenPosition{ newPosition },
		alignment{ newAlignment },
		fontSize{ newFontSize },
		fontID{ newFontID }
	{}
} // namespace Renderable
