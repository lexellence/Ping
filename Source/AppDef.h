/**************************************************************************************\
** File: AppDef.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the AppDef struct
**
\**************************************************************************************/
#pragma once
namespace Pong
{
	struct AppDef
	{
		void LoadFrom(const std::string& filePath);
		void Validate() const;

		d2d::GamepadSettings gamepads;
		d2d::WindowDef window;
	};
}
