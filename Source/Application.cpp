/**************************************************************************************\
** File: Application.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the Application class
**
\**************************************************************************************/
#include "pch.h"
#include "Application.h"

namespace d2d
{
	void Application::Run()
	{
		bool keepRunning = Init();
		while (keepRunning)
		{
			keepRunning = Tick();
		}
		Shutdown();
	}
}