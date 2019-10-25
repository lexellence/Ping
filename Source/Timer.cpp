/*************************************************************************************\
** File: Timer.cpp
** Project: Pong
** Author: David Leksen
** Date: 4/1/16
** 
** Source code file for the Timer class
**
\**************************************************************************************/
#include "pch.h"
#include "Timer.h"

namespace d2d
{
	Timer::Timer()
	  :	m_lastTicks{ SDL_GetPerformanceCounter() }
	{}
	///-------------------------\---------------------------------------------------------- 
	//| Name: Start				| 
	//\-------------------------/
	// PostCondition: Initializes the timer
	//-------------------------------------------------------------------------------------
	void Timer::Start()
	{
		m_deltaSeconds = 0.0f;
		m_lastTicks = SDL_GetPerformanceCounter();
	}
	///-------------------------\---------------------------------------------------------- 
	//| Name: GetDeltaSeconds	| 
	//\-------------------------/
	// PostCondition: Returns the number of seconds between ticks.
	//			If this is the first call, returns the seconds since start.
	//-------------------------------------------------------------------------------------
	float Timer::GetDeltaSeconds() const
	{
		return m_deltaSeconds;
	}
	void Timer::Tick()
	{
		// Get the new time and tick-rate
		const Uint64 ticksPerSecond = SDL_GetPerformanceFrequency();
		const Uint64 currentTicks = SDL_GetPerformanceCounter();

		// Calculate dt
		const Uint64 deltaTicks = currentTicks - m_lastTicks;
		m_lastTicks = currentTicks;

		SDL_assert(ticksPerSecond != 0);
		m_deltaSeconds = (float)((double)deltaTicks / (double)ticksPerSecond);
	}
}