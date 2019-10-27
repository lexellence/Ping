/**************************************************************************************\
** File: Intro.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the Intro class
**
\**************************************************************************************/
#include "pch.h"
#include "Intro.h"
#include "MainMenu.h"
#include "d2d.h"

namespace Pong
{
	void Intro::ProcessEvent(const SDL_Event& event)
	{
		if (event.type == SDL_KEYDOWN ||
			/*event.type == SDL_CONTROLLERBUTTONDOWN ||*/
			event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (m_animationsComplete)
				m_goToMenu = true;
			else
			{
				// Complete animations
				m_titleFalling = false;
				m_title.screenPosition.y = m_titleFinalScreenY;

				m_authorFadingIn = false;
				m_author.color.alpha = m_authorFinalAlpha;
				m_animationsComplete = true;
			}
		}
	}
	std::shared_ptr<d2d::GameState> Intro::Update(float deltaSeconds)
	{
		if (m_goToMenu)
			return std::make_shared<MainMenu>();

		// Update intro animations
		if (!m_animationsComplete)
		{
			if (m_titleFalling)
			{
				// Move text
				m_titleSpeedScreensPerSecond += m_titleAccelerationScreensPerSecondSquared * deltaSeconds;
				m_title.screenPosition.y -= m_titleSpeedScreensPerSecond * deltaSeconds;

				// If the text has reached it's destination or beyond
				if (m_title.screenPosition.y <= m_titleFinalScreenY)
				{
					// In case it went too far, set it's final destination.
					m_title.screenPosition.y = m_titleFinalScreenY;

					// Goto next animation
					m_titleFalling = false;
					m_authorFadingIn = true;
				}
			}
			if (m_authorFadingIn)
			{
				if (m_authorFadeDelaySeconds > 0.0f)
				{
					m_authorFadeDelaySeconds -= deltaSeconds;
				}
				if (m_authorFadeDelaySeconds < 0.0f)
				{
					// Fade text in
					m_author.color.alpha += m_authorFadesPerSecond * deltaSeconds;

					// If the text's alpha has reached it's destination within the fade tolerance
					if (m_author.color.alpha > (m_authorFinalAlpha - m_fadeTolerance))
					{
						// Set text's final destination
						m_author.color.alpha = m_authorFinalAlpha;

						// End animation
						m_authorFadingIn = false;
						m_animationsComplete = true;
					}
				}
			}
		}
		return shared_from_this();
	}
	void Intro::Draw()
	{
		d2d::Renderer::SetShowCursor(false);
		d2d::Renderer::SetCameraScreenMode();

		d2d::Renderer::DrawTextBlock(m_title);
		d2d::Renderer::DrawTextBlock(m_author);
	}
}