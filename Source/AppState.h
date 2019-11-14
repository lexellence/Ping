/**************************************************************************************\
** File: AppState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the AppState interface
**
\**************************************************************************************/
#pragma once
union SDL_Event;
namespace Pong
{
	enum class AppStateID
	{
		INTRO, MAIN_MENU, GAMEPLAY, QUIT
	};
	class AppState
	{
	public:
		virtual void Init() = 0;
		virtual void ProcessEvent(const SDL_Event& event) = 0;
		virtual AppStateID Update(float dt) = 0;
		virtual void Draw() = 0;
	};
}
