/**************************************************************************************\
** File: Utility.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for miscellaneous useful things
**
\**************************************************************************************/
#include "pch.h"
#include "Utility.h"

namespace d2d
{
	void ConstrainRadians(float& theta)
	{
		const double TWO_PI = 2.0f * PI;
		theta = fmodf(theta, TWO_PI);
		if (theta < 0.0f)
			theta += TWO_PI;
	}
	void ConstrainDegrees(float& angle)
	{
		angle = fmodf(angle, 360.0f);
		if (angle < 0.0f)
			angle += 360.0f;
	}
}