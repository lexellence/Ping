/**************************************************************************************\
** File: MainMenu.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the MainMenu class
**
\**************************************************************************************/
#include "pch.h"
#include "MainMenu.h"
#include "AppState.h"
#include "GameInitSettings.h"

namespace Pong
{
	//+--------------------------------\--------------------------------------
	//|			 MainMenu	    	   |
	//\--------------------------------/--------------------------------------
	void MainMenu::Init()
	{
		m_menu.Init();

		d2d::Window::SetClearColor(d2d::BLACK_OPAQUE);
	}
	void MainMenu::ProcessEvent(const SDL_Event& event)
	{
		m_menu.ProcessEvent(event);
	}
	AppStateID MainMenu::Update(float dt)
	{
		std::string pressedButton;
		if(m_menu.PollPressedButton(pressedButton))
		{
			if(pressedButton == m_startTwoPlayerLocalText)
			{
				GameInitSettings::SetGameMode(GameInitSettings::Mode::LOCAL);
				return AppStateID::GAMEPLAY;
			}
			else if(pressedButton == m_startServerText)
			{
				GameInitSettings::SetGameMode(GameInitSettings::Mode::SERVER);
				return AppStateID::GAMEPLAY;
			}
			else if(pressedButton == m_startClientText)
			{
				GameInitSettings::SetGameMode(GameInitSettings::Mode::CLIENT);
				return AppStateID::GAMEPLAY;
			}
			else if(pressedButton == m_quitText)
				return AppStateID::QUIT;
		}
		return AppStateID::MAIN_MENU;
	}
	d2d::Color MainMenu::GetClearColor()
	{
		return d2d::BLACK_OPAQUE;
	}
	void MainMenu::Draw()
	{
		d2d::Window::SetShowCursor(true);
		m_menu.Draw();
	}
}
