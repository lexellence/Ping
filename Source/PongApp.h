#ifndef PONGAPP_H
#define PONGAPP_H
#include "d2d.h"

namespace Pong 
{
	class PongApp : public d2d::Application
	{
	protected:
		bool Init();
		bool Tick();
		void Shutdown();
		void AddController(int deviceIndex);
		void RemoveController(int joystickID);


		std::string m_errorFilename{ "error.txt" };
		const int m_screenWidth{ 1920 };
		const int m_screenHeight{ 1080 };
		const bool m_fullScreen{ true };

		d2d::Timer m_timer;
		std::shared_ptr<d2d::GameState> m_currentStatePtr;
		int m_controller1JoystickID{ -1 };
		int m_controller2JoystickID{ -1 };
	};
}
#endif //PONGAPP_H