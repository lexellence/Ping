/**************************************************************************************\
** File: Gameplay.h
** Project: Pong
** Author: David Leksen
** Date:
**
** Header file for the Gameplay class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"
#include "Game.h"

namespace Pong
{
	/*enum class MenuState
	{
		MAINMENU,
		SERVER_INPUT_PORT,
		SERVER_WAIT_FOR_CLIENT,
		SERVER_ACCEPTING_MESSAGES,
		CLIENT_INPUT_IP,
		CLIENT_INPUT_PORT
	};*/

	class Gameplay : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		d2d::Color GetClearColor() override;
		void Draw() override;

	private:
		void MapInputToGameActions();

		Game m_game;

		bool m_gotoMenu;
		bool m_player1Up;
		bool m_player1Down;
		bool m_player2Up;
		bool m_player2Down;
	};
}
