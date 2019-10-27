/*************************************************************************************\
** File: Timer.h
** Project: Pong 
** Author: David Leksen 
** Date: 4/1/16
** 
** Header file for the Timer class
**
\**************************************************************************************/
#pragma once
namespace d2d
{
	///-------------------------------------\---------------------------------------------- 
	//| Class: Timer						| 
	//\-------------------------------------/
	//		Uses the SDL functions QueryPerformanceCounter() and
	//	QueryPerformanceFrequency() to provide the highest resolution timer 
	//	with 1 microsecond accuracy on most systems.
	//		Call Start() to initialize the timer. Then call Update() once per frame, and
	//	use the accessors to get deltaTime in ticks or seconds.		
	//-------------------------------------------------------------------------------------
	class Timer
	{
	public:
		Timer();

		///-------------------------\---------------------------------------------------------- 
		//| Name: Start				| 
		//\-------------------------/
		// PostCondition: Initializes the timer
		//-------------------------------------------------------------------------------------
		void Start();
		void Tick();
		///-------------------------\---------------------------------------------------------- 
		//| Name: GetDeltaSeconds	| 
		//\-------------------------/
		// PostCondition: Returns the number of seconds between ticks.
		//			If this is the first call, returns the seconds since start.
		//-------------------------------------------------------------------------------------
		float GetDeltaSeconds() const;
	private:	
		float m_deltaSeconds{};
		Uint64 m_lastTicks;
	};
}


