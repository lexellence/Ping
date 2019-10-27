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
#include "d2d.h"
#include "Game.h"

namespace Pong
{
	class Gameplay : public d2d::GameState
	{
	public:
		Gameplay(bool twoPlayers);
		virtual void ProcessEvent(const SDL_Event& event);
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds);
		virtual void Draw();

	private:
		void MapInputToGameActions();

		Game m_game;

		bool m_gotoMenu{ false };
		bool m_player1Up{ false };
		bool m_player1Down{ false };
		bool m_player2Up{ false };
		bool m_player2Down{ false };
	};
}
