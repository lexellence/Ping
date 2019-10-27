/**************************************************************************************\
** File: Application.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Application class
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	class Application
	{
	public:
		void Run();

	protected:
		virtual bool Init()=0;
		virtual bool Tick()=0;
		virtual void Shutdown()=0;
	};
}
