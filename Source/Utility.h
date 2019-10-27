/**************************************************************************************\
** File: Utility.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for miscellaneous useful things
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	template <typename T>
	void Clamp(T& toBeClamped, T min, T max)
	{
		if (toBeClamped < min)
			toBeClamped = min;
		else if (toBeClamped > max)
			toBeClamped = max;
	}

	enum class Alignment
	{
		LEFT_TOP, CENTER_TOP, RIGHT_TOP,
		LEFT_CENTER, CENTERED, RIGHT_CENTER,
		LEFT_BOTTOM, CENTER_BOTTOM, RIGHT_BOTTOM
	};

	const float PI{ 3.141593f };
	void ConstrainRadians(float& theta);
	void ConstrainDegrees(float& angle);
}
