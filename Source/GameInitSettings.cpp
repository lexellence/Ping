/**************************************************************************************\
** File: GameInitSettings.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the GameInitSettings namespace
**
\**************************************************************************************/
#include "pch.h"
#include "GameInitSettings.h"

namespace Pong
{
	namespace GameInitSettings
	{
		namespace
		{
			Mode m_mode;
		}
		void SetGameMode(Mode mode)
		{
			m_mode = mode;
		}
		Mode GetGameMode()
		{
			return m_mode;
		}
	}
}
