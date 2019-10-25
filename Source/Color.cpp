/**************************************************************************************\
** File: Color.cpp
** Project: Pong
** Author: David Leksen
** Date: 4/1/16
**
** Source code file for the Color class
**
\**************************************************************************************/
#include "pch.h"
#include "Color.h"

namespace d2d
{
	const float Color::COLOR_MIN{ 0.0f };
	const float Color::COLOR_MAX{ 1.0f };

	Color::Color()
		: red{ COLOR_MIN },
		green{ COLOR_MIN },
		blue{ COLOR_MIN },
		alpha{ COLOR_MIN }
	{}
	Color::Color(float r, float g, float b, float a)
		: red{ r <= COLOR_MIN ? COLOR_MIN : r >= COLOR_MAX ? COLOR_MAX : r },
		green{ g <= COLOR_MIN ? COLOR_MIN : g >= COLOR_MAX ? COLOR_MAX : g },
		blue{ b <= COLOR_MIN ? COLOR_MIN : b >= COLOR_MAX ? COLOR_MAX : b },
		alpha{ a <= COLOR_MIN ? COLOR_MIN : a >= COLOR_MAX ? COLOR_MAX : a }
	{}
}