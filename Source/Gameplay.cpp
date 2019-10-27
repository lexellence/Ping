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
#include "MainMenu.h"
#include "d2d.h"

namespace Pong
{
	Gameplay::Gameplay(bool twoPlayers)
	{
		/*
		// If the screen aspect ratio is too big or small, clamp the game aspect ratio to reasonable proportions
		double screenAspectRatio = d2d::Renderer::GetXYAspectRatio();
		double m_gameWidth, m_gameHeight;
		if (screenAspectRatio > m_maxGameApectRatio)
		{
			// Screen will be wider than game
			m_gameWidth = m_maxGameApectRatio * m_defaultHeight;
			m_gameHeight = m_defaultHeight;
		}
		else if (screenAspectRatio < m_minGameApectRatio)
		{
			// Screen will be taller than game
			m_gameWidth = m_defaultWidth;
			m_gameHeight = m_defaultWidth / m_minGameApectRatio;
		}
		else
		{
			// Screen will be completely filled by game
			m_gameWidth = m_defaultWidth;
			m_gameHeight = m_defaultWidth / screenAspectRatio;
		}*/

		// Start game
		m_game.Init(twoPlayers);
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
	std::shared_ptr<d2d::GameState> Gameplay::Update(float deltaSeconds)
	{		
		if(m_gotoMenu)
		{
			return std::make_shared<MainMenu>();
		}
		MapInputToGameActions();

		m_game.Update(deltaSeconds);

		return shared_from_this();
	}
	void Gameplay::MapInputToGameActions()
	{
		// Map Player1 controller to game
		if (m_player1Up && !m_player1Down)
			m_game.SetPlayer1MovementFactor(1.0);
		else if (!m_player1Up && m_player1Down)
			m_game.SetPlayer1MovementFactor(-1.0);
		else
			m_game.SetPlayer1MovementFactor(0.0);

		if (m_game.TwoPlayers())
		{
			// Map Player2 controller to game
			if (m_player2Up && !m_player2Down)
				m_game.SetPlayer2MovementFactor(1.0);
			else if (!m_player2Up && m_player2Down)
				m_game.SetPlayer2MovementFactor(-1.0);
			else
				m_game.SetPlayer2MovementFactor(0.0);
		}
	}
	void Gameplay::Draw()
	{
		d2d::Renderer::SetShowCursor(false);
		m_game.Draw();
	}
}
