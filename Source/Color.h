/*************************************************************************************\
** File: Color.h 
** Project: Pong
** Author: David Leksen
** Date: 4/1/16
** 
** Header file for the Color class
**
\**************************************************************************************/
#ifndef COLOR_H
#define COLOR_H

namespace d2d
{
	// Color channel range: [0.0, 1.0]
	class Color
	{
	public:
		float red;
		float green;
		float blue;
		float alpha;

		Color();
		Color(float r, float g, float b, float a = 1.0f);

		const static float COLOR_MIN;
		const static float COLOR_MAX;

	};
}
#endif //COLOR_H