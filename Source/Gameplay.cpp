/**************************************************************************************\
** File: Gameplay.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the Gameplay class
**
\**************************************************************************************/
#include "pch.h"
#include "Gameplay.h"
#include "Game.h"
#include "AppState.h"
#include "Exceptions.h"

namespace Pong
{
	void Gameplay::Init()
	{
		m_gotoMenu = false;
		m_player1Up = false;
		m_player1Down = false;
		m_player2Up = false;
		m_player2Down = false;

		try {
			m_game.Init();
		}
		catch(const GameException & e) {
			SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game Initialization Error", e.what(), nullptr);
			m_gotoMenu = true;
		}

		d2d::Window::SetClearColor(d2d::BLACK_OPAQUE);

		//m_showFPS = true;
	}
	void Gameplay::ProcessEvent(const SDL_Event& event)
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_gotoMenu = true; 
				break;
			case SDLK_UP:
				m_player2Up = true;
				m_game.Player2PressedAButton(); 
				break;
			case SDLK_DOWN:
				m_player2Down = true;
				m_game.Player2PressedAButton(); 
				break;
			case SDLK_w:
				m_player1Up = true;
				m_game.Player1PressedAButton(); 
				break;
			case SDLK_s:
				m_player1Down = true;
				m_game.Player1PressedAButton(); 
				break;
			} 
			break;
		case SDL_KEYUP:
			switch(event.key.keysym.sym)
			{
			case SDLK_UP:
				m_player2Up = false; 
				break;
			case SDLK_DOWN:
				m_player2Down = false; 
				break;
			case SDLK_w:
				m_player1Up = false; 
				break;
			case SDLK_s:
				m_player1Down = false; 
				break;
			} 
			break;

		// Controller logic
		/*
		case SDL_CONTROLLERBUTTONDOWN:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					std::cerr << "controller device id: " << event.cdevice.which << std::endl;
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
					{
						m_player1Up = true;
						m_game.Player1PressedAButton();
					}
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
					{
						m_player2Up = true;
						m_game.Player2PressedAButton();
					}
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					std::cerr << "controller device id: " << event.cdevice.which << std::endl;
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
					{
						m_player1Down = true;
						m_game.Player1PressedAButton();
					}
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
					{
						m_player2Down = true;
						m_game.Player2PressedAButton();
					}
					break;
			} break;
		// User releases a button
		case SDL_CONTROLLERBUTTONUP:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
						m_player1Up = false;
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
						m_player2Up = false;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
						m_player1Down = false;
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
						m_player2Down = false;
					break;
			} 
			break;
			*/
		}
	}
	AppStateID Gameplay::Update(float dt)
	{
		if(!m_gotoMenu)
		{
			try {
				MapInputToGameActions();
				m_game.Update(dt);
			}
			catch(const GameException & e) {
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Game Update Error", e.what(), nullptr);
				m_gotoMenu = true;
			}
		}

		if(m_gotoMenu)
		{
			m_game.OnQuit();
			return AppStateID::MAIN_MENU;
		}

		return AppStateID::GAMEPLAY;
	}
	void Gameplay::MapInputToGameActions()
	{
		// Map Player1 controller to game
		if (m_player1Up && !m_player1Down)
			m_game.SetPlayer1MovementFactor(1.0f);
		else if (!m_player1Up && m_player1Down)
			m_game.SetPlayer1MovementFactor(-1.0f);
		else
			m_game.SetPlayer1MovementFactor(0.0f);

		// Map Player2 controller to game
		if (m_player2Up && !m_player2Down)
			m_game.SetPlayer2MovementFactor(1.0f);
		else if (!m_player2Up && m_player2Down)
			m_game.SetPlayer2MovementFactor(-1.0f);
		else
			m_game.SetPlayer2MovementFactor(0.0f);
	}
	void Gameplay::Draw()
	{
		d2d::Window::SetShowCursor(false);
		m_game.Draw();
	}
}
