/**************************************************************************************\
** File: IntroGameState.h
** Project:
** Author: David Leksen
** Date:
**
** Header file for the IntroGameState class
**
\**************************************************************************************/
#ifndef INTROGAMESTATE_H
#define INTROGAMESTATE_H
#include "d2d.h"

namespace Pong
{
	class IntroGameState : public d2d::GameState
	{
	public:
		IntroGameState();
		virtual ~IntroGameState();
		virtual void HandleEvent(const SDL_Event& event);
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds);
		virtual void Draw();

	private:
		bool m_goToMenu{ false };

		// Intro animation
		bool m_titleFalling{ true };
		float m_titleSpeedScreensPerSecond{ 0.0f };
		float m_titleAccelerationScreensPerSecondSquared{ 0.15f };
		float m_titleStartScreenY{ 1.0f };
		float m_titleFinalScreenY{ 0.5f };

		bool m_authorFadingIn{ false };
		float m_authorFadeDelaySeconds{ 0.6f };
		float m_authorFadesPerSecond{ 0.4f };
		float m_authorStartAlpha{ 0.0f };
		float m_authorFinalAlpha{ 1.0f };
		float m_fadeTolerance{ 0.001f };

		bool m_animationsComplete{ false };

		// Assets
		d2d::FontReference m_timesFont{ "Fonts\\Times.ttf" };
		d2d::FontReference m_alexBrushFont{ "Fonts\\AlexBrush.otf" };
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };
		d2d::FontReference m_orbitronMediumFont{ "Fonts\\OrbitronMedium.otf" };
		d2d::FontReference m_orbitronBoldFont{ "Fonts\\OrbitronBold.otf" };
		d2d::FontReference m_brushRunesFont{ "Fonts\\BrushRunes.otf" };
		d2d::FontReference m_pressStartFont{ "Fonts\\PressStart2P.ttf" };

		// Text data
		d2d::TextBlock m_title{ "Super Ping Pong Simulator", b2Vec2(0.5f, m_titleStartScreenY),
			d2d::Alignment::CENTER_BOTTOM, 
			0.045f, m_orbitronLightFont.GetID(),
			d2d::Color(0.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_author{ "Praxware Game Studio", b2Vec2(0.5f, 0.4f),
			d2d::Alignment::CENTER_TOP, 
			0.045f, m_alexBrushFont.GetID(),
			d2d::Color(0.5f, 0.0f, 0.7f, m_authorStartAlpha) };
	};
}
#endif //INTROGAMESTATE_H