/**************************************************************************************\
** File: App.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the App class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "Intro.h"
#include "MainMenu.h"
#include "Gameplay.h"
#include "Exceptions.h"

namespace Pong
{
	const AppStateID FIRST_APP_STATE{ AppStateID::INTRO };
	const float MAX_APP_STEP{ 1.0f };
	class App
	{
	public:
		~App();
		void Run();
	private:
		void Init();
		void Step(float dt);

		std::shared_ptr<AppState> GetStatePtr(AppStateID appState);
		void UpdateCurrentState(float dt);
		void Shutdown();

		std::shared_ptr<Intro> m_introPtr;
		std::shared_ptr<MainMenu> m_mainMenuPtr;
		std::shared_ptr<Gameplay> m_gameplayPtr;

		AppStateID m_currentState{ FIRST_APP_STATE };
		AppStateID m_nextState{ FIRST_APP_STATE };

		bool m_hasFocus;
	};
}