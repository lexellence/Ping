/**************************************************************************************\
** File: main.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for main program entry point
**
\**************************************************************************************/
#include "pch.h"
#include "App.h"

int main(int argc, char *argv[])
{
	try
	{
		Pong::App pongApp;
		pongApp.Run();
	}
	catch(const std::exception & e)
	{
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal Exception", e.what(), nullptr);
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;

}