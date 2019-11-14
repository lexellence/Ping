/**************************************************************************************\
** File: GameInitSettings.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the GameInitSettings namespace
**
\**************************************************************************************/
#pragma once
namespace Pong
{
	namespace GameInitSettings
	{
		enum class Mode
		{
			LOCAL,
			SERVER,
			CLIENT
		};
		void SetGameMode(Mode mode);
		Mode GetGameMode();
		bool IsClient();
		bool IsServer();
		bool IsNetworked();
	}
}
