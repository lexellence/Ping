/**************************************************************************************\
** File: App.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the App class
**
\**************************************************************************************/
#include "pch.h"
#include "App.h"
#include "Intro.h"
#include "d2d.h"

namespace Pong
{
	bool App::Init()
	{
		// Redirect cerr/stderr output to file for logging
		FILE *file;
		file = new FILE;
		if (freopen_s(&file, m_logFilename.c_str(), "w", stderr))
			std::cerr << "Warning: Failed to redirect stderr to" << m_logFilename << std::endl;

		// Init all SDL subsystems
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			std::cerr << "Error: Failed to initialize SDL " << SDL_GetError() << std::endl;
			SDL_ClearError();
			return false;
		}

		// Create window
		d2d::ScreenDef settings;
		if (!d2d::Renderer::CreateScreen(settings))
		{
			std::cerr << "Error: Unable to initialize screen: " << SDL_GetError() << std::endl;
			SDL_ClearError();
			return false;
		}

		// Init timer
		m_timer.Start();

		/*
		// Init gamepads
		int numJoysticks = SDL_NumJoysticks();
		for(int j = 0; j < numJoysticks; ++j)
		{
			AddController(j);
		}*/

		// Start Intro
		m_currentStatePtr = std::make_shared<Intro>();
		return true;
	}
	void App::Shutdown()
	{
		d2d::Renderer::DestroyScreen();
		SDL_Quit();
		fclose(stderr);
	}
	bool App::Tick()
	{
		// Update timer
		m_timer.Tick();

		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					return false;
				/*case SDL_CONTROLLERDEVICEADDED:
					AddController(event.cdevice.which);
					break;
				case SDL_CONTROLLERDEVICEREMOVED:
					RemoveController(event.cdevice.which);
					break;*/
				default:
					if(m_currentStatePtr)
						m_currentStatePtr->ProcessEvent(event);
			}
		}

		// Update game state
		if (m_currentStatePtr)
			m_currentStatePtr = m_currentStatePtr->Update(m_timer.GetDeltaSeconds());

		// Draw frame
		if(m_currentStatePtr)
			m_currentStatePtr->Draw();
		else
			return false;

		d2d::Renderer::PresentScene();
		return true;
	}
	/*void App::AddController(int deviceIndex)
	{
		// If the joystick is a gamepad
		if(SDL_IsGameController(deviceIndex))
		{
			// Enable the gamepad for use
			SDL_GameController *controllerPtr = SDL_GameControllerOpen(deviceIndex);

			// If there was an error, bail
			if(!controllerPtr)
			{
				std::cerr << "Could not open gamecontroller " << deviceIndex  
						  << ": " << SDL_GetError() << std::endl;
				return;
			}

			// Get and store joystick ID if there's a slot open.
			SDL_Joystick *joyPtr = SDL_GameControllerGetJoystick(controllerPtr);
			int joystickID = SDL_JoystickInstanceID(joyPtr);
			if(m_controller1JoystickID < 0)
			{
				m_controller1JoystickID = joystickID;
			}
			else if(m_controller2JoystickID < 0)
			{
				m_controller2JoystickID = joystickID;
			}
		}
	}
	void App::RemoveController(int joystickID)
	{
		if(m_controller1JoystickID == joystickID)
		{
			m_controller1JoystickID = -1;
		}
		if(m_controller2JoystickID == joystickID)
		{
			m_controller2JoystickID = -1;
		}
	}*/
}

