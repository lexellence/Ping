/**************************************************************************************\
** File: Exceptions.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for exceptions
**
\**************************************************************************************/
#pragma once
namespace Pong
{
	struct InvalidAppStateException : public d2d::Exception 
	{
		using Exception::Exception;
	};
	struct LoadSettingsFileException : public d2d::Exception 
	{
		using Exception::Exception;
	};
	struct SettingOutOfRangeException : public d2d::Exception
	{
		using Exception::Exception;
	};
	struct GameException : public d2d::Exception
	{
		using Exception::Exception;
	};
}