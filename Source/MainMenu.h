/**************************************************************************************\
** File: MainMenu.h
** Project: Pong
** Author: David Leksen
** Date:
**
** Header file for the MainMenu class
**
\**************************************************************************************/
#pragma once
#include "AppState.h"

namespace Pong
{
	class MainMenu : public AppState
	{
	public:
		void Init() override;
		void ProcessEvent(const SDL_Event& event) override;
		AppStateID Update(float dt) override;
		void Draw() override;

	private:
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };

		// Main Menu
		const std::string m_startTwoPlayerLocalText{ "START LOCAL" };
		const std::string m_startServerText{ "START SERVER" };
		const std::string m_startClientText{ "START CLIENT" };
		const std::string m_quitText{ "QUIT" };
		const std::vector<std::string> m_buttonNames{ m_startTwoPlayerLocalText, m_startServerText, m_startClientText, m_quitText };
		d2d::TextStyle m_buttonTextStyle{ m_orbitronLightFont, { 0.0f, 0.5f, 0.8f, 1.0f }, 0.035f };

		const std::string m_titleText{ "Super Ping Pong Simulator" };
		const d2d::TextStyle m_titleStyle{ m_orbitronLightFont, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.045f };

		const d2d::Color m_buttonColor{ d2d::COLOR_ZERO };
		const d2d::Color m_buttonHighlightColor{ 1.0f, 1.0f, 1.0f, 0.3f };
		const d2d::Color m_buttonBorderColor{ 0.5f, 0.5f, 0.5f, 0.5f };
		const d2d::Color m_backgroundColor{ d2d::COLOR_ZERO };

		d2d::Menu m_menu{ m_buttonNames, m_buttonTextStyle, m_titleText, m_titleStyle,
			m_buttonColor, m_buttonHighlightColor, m_buttonBorderColor, m_backgroundColor };

		// Remote Game
		const std::string m_serverInputPortText{ "Starting server..." };
		const std::string m_serverWaitForClientText{ "Waiting for client..." };
		const d2d::Alignment m_remoteInputInstructionsAlignment{ d2d::Alignment::CENTER_BOTTOM };
		const b2Vec2 m_remoteInputInstructionsPosition{ 0.5f, 0.6f };
		const d2d::TextStyle m_remoteInputInstructionsTextStyle{ m_orbitronLightFont, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.045f };

		const d2d::Alignment m_remoteInputAlignment{ d2d::Alignment::CENTER_TOP };
		const b2Vec2 m_remoteInputPosition{ 0.5f, 0.5f };
		const d2d::TextStyle m_remoteInputTextStyle{ m_orbitronLightFont, { 0.0f, 1.0f, 0.0f, 1.0f }, 0.045f };
	};
}
