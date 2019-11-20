/**************************************************************************************\
** File: Intro.h
** Project: Pong
** Author: David Leksen
** Date:
**
** Header file for the Intro class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"

namespace Pong
{
	class Intro : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		d2d::Color GetClearColor() override;
		void Draw() override;

	private:
		d2d::FontReference m_alexBrushFont{ "Fonts\\AlexBrush.otf"s };
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf"s };

		const std::string m_titleText{ "Ping"s };
		const d2d::Alignment m_titleAlignment{ d2d::Alignment::CENTER_BOTTOM };
		const d2d::TextStyle m_titleStyle{ m_orbitronLightFont, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.065f };

		const std::string m_authorText{ "Lexellence Games"s };
		const d2d::Alignment m_authorAlignment{ d2d::Alignment::CENTER_TOP };
		d2d::TextStyle m_authorStyle{ m_alexBrushFont, { 0.7f, 0.0f, 0.7f, 1.0f }, 0.055f };

		const float m_titleAccelerationScreensPerSecondSquared{ 0.15f };
		const float m_titleStartScreenY{ 1.0f };
		const float m_titleFinalScreenY{ 0.5f };
		const float m_authorFadeDelay{ 0.6f };
		const float m_authorFadesPerSecond{ 0.4f };
		const float m_authorStartAlpha{ 0.0f };
		const float m_authorFinalAlpha{ 1.0f };
		const float m_fadeTolerance{ 0.001f };
		const b2Vec2 m_authorPosition{ 0.5f, 0.4f };

		bool m_gotoMenu;
		bool m_animationsComplete;
		bool m_titleFalling;
		b2Vec2 m_titlePosition;
		float m_titleSpeedScreensPerSecond;
		bool m_authorFadingIn;
		float m_authorFadeDelayElapsed;
	};
}
