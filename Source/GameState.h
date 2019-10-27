/**************************************************************************************\
** File: GameState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the GameState interface
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	class GameState : public std::enable_shared_from_this<GameState>
	{
	public:
		virtual void ProcessEvent(const SDL_Event& event) = 0;
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds) = 0;
		virtual void Draw() = 0;
	protected:

	};
}

