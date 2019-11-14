/**************************************************************************************\
** File: App.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the App class
**
\**************************************************************************************/
#include "pch.h"
#include "App.h"
#include "AppState.h"
#include "AppDef.h"
#include "Intro.h"
#include "MainMenu.h"
#include "Gameplay.h"
#include "Exceptions.h"

namespace Pong
{
	void App::Run()
	{
		Init();

		d2d::Timer timer;
		timer.Start();
		while(m_nextState != AppStateID::QUIT)
		{
			timer.Update();
			Step(timer.Getdt());
		}
		Shutdown();
	}
	void App::Init()
	{
		// Init d2d
		d2d::Init(d2LogSeverityTrace, "PongLog.txt");
		{
			AppDef settings;
			settings.LoadFrom("Data\\app.hjson");

			d2d::InitGamepads(settings.gamepads);
			d2d::Window::Init(settings.window);
			m_hasFocus = false;
		}
		d2d::SeedRandomNumberGenerator();

		// Allocate AppState memory
		m_introPtr = std::make_shared<Intro>();
		m_mainMenuPtr = std::make_shared<MainMenu>();
		m_gameplayPtr = std::make_shared<Gameplay>();

		// Start first app state
		m_currentState = FIRST_APP_STATE;
		m_nextState = FIRST_APP_STATE;
		//InitCurrentState();
		GetStatePtr(m_currentState)->Init();
	}
	void App::Step(float dt)
	{
		d2d::ClampHigh(dt, MAX_APP_STEP);

		UpdateCurrentState(dt);

		if(m_nextState != AppStateID::QUIT)
		{
			if(m_nextState != m_currentState)
			{
				m_currentState = m_nextState;
				//InitCurrentState();	
				GetStatePtr(m_currentState)->Init();
			}
			else
				//if(m_hasFocus)
				{
					//DrawCurrentState();
					d2d::Window::StartScene();
					GetStatePtr(m_currentState)->Draw();
					d2d::Window::EndScene();
				}
		}
	}
	App::~App()
	{
		// Just in case an exception brings us out of the main loop
		Shutdown();
	}
	std::shared_ptr<AppState> App::GetStatePtr(AppStateID appState)
	{
		switch(appState)
		{
		case AppStateID::INTRO:		return m_introPtr;
		case AppStateID::MAIN_MENU: return m_mainMenuPtr;
		case AppStateID::GAMEPLAY:	return m_gameplayPtr;
		default: throw InvalidAppStateException{ "GetStatePtr(): No pointer exists for AppState (calling code must ensure argument is not QUIT)" };
		}
	}
	/*void App::InitCurrentState()
	{
		try
		{
			GetStatePtr(m_currentState)->Init();
		}
		catch(const GameException& e)
		{
			std::string message{ "AppState init error: " + std::string{ e.what() } };
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), nullptr);
			m_currentState = AppStateID::QUIT;
		}
	}*/
	void App::UpdateCurrentState(float dt)
	{
		std::shared_ptr<AppState> currentStatePtr{ GetStatePtr(m_currentState) };

		// Handle events
		SDL_Event event;
		while(SDL_PollEvent(&event) != 0)
		{
			switch(event.type)
			{
			case SDL_QUIT:
				m_nextState = AppStateID::QUIT;
				return;
			case SDL_WINDOWEVENT:
				switch(event.window.event)
				{
				case SDL_WINDOWEVENT_FOCUS_LOST:
					m_hasFocus = false;
					break;
				case SDL_WINDOWEVENT_FOCUS_GAINED:
					m_hasFocus = true;
					break;
				case SDL_WINDOWEVENT_CLOSE:
					m_nextState = AppStateID::QUIT;
					return;
				}
			}
			currentStatePtr->ProcessEvent(event);
		}

		//if(m_hasFocus)
		//{
			//try
			//{
				m_nextState = currentStatePtr->Update(dt);
			//}
			//catch(const GameException& e)
			//{
			//	std::string message{ "Game update error: " + std::string{ e.what() } };
			//	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", message.c_str(), nullptr);
			//	m_nextState = AppStateID::QUIT;
			//}
		//}
	}	
	/*void App::DrawCurrentState()
	{
		std::shared_ptr<AppState> currentStatePtr{ GetStatePtr(m_currentState) };

		d2d::Window::StartScene();
		currentStatePtr->Draw();
		d2d::Window::EndScene();
	}*/
	void App::Shutdown()
	{
		d2d::Shutdown();
	}
}