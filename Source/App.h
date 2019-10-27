/**************************************************************************************\
** File: App.h
** Project: Pong
** Author: David Leksen
** Date:
**
** Header file for the App class
**
\**************************************************************************************/
#pragma once
#include "d2d.h"

namespace Pong 
{
	class App : public d2d::Application
	{
	protected:
		bool Init();
		bool Tick();
		void Shutdown();
		/*void AddController(int deviceIndex);
		void RemoveController(int joystickID);*/

		std::string m_logFilename{ "Pong.log" };

		d2d::Timer m_timer;
		std::shared_ptr<d2d::GameState> m_currentStatePtr;
		/*int m_controller1JoystickID{ -1 };
		int m_controller2JoystickID{ -1 };*/
	};
}
