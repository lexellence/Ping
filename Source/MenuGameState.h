#ifndef MENUGAMESTATE_H
#define MENUGAMESTATE_H
#include "d2d.h"

namespace Pong
{
	class MenuGameState : public d2d::GameState
	{
	public:
		MenuGameState();
		virtual ~MenuGameState();
		virtual void HandleEvent(const SDL_Event& event);
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds);
		virtual void Draw();

	private:
		void MakeButtonRect(const d2d::TextBlock& text,
							d2d::Rect& rect);
		void DrawButton(const d2d::TextBlock& textBlock, bool highlight);

		bool m_quit{ false };
		bool m_goPlay{ false };
		bool m_twoPlayerGame{ false };

		bool m_highlight1PStartButton{ false };
		bool m_highlight2PStartButton{ false };
		bool m_highlightQuitButton{ false };

		// Assets
		d2d::FontReference m_timesFont{ "Fonts\\Times.ttf" };
		d2d::FontReference m_alexBrushFont{ "Fonts\\AlexBrush.otf" };
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };
		d2d::FontReference m_orbitronMediumFont{ "Fonts\\OrbitronMedium.otf" };
		d2d::FontReference m_orbitronBoldFont{ "Fonts\\OrbitronBold.otf" };
		d2d::FontReference m_brushRunesFont{ "Fonts\\BrushRunes.otf" };
		d2d::FontReference m_pressStartFont{ "Fonts\\PressStart2P.ttf" };
		d2d::FontReference m_snigletRegularFont{ "Fonts\\SnigletRegular.otf" };
		
		// Text data
		d2d::TextBlock m_menuTitle{ "Super Ping Pong Simulator", 
			b2Vec2(0.5f, 0.7f), d2d::Alignment::CENTERED,
			0.045f, m_orbitronLightFont.GetID(),
			d2d::Color(0.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_menuStart1{ "1P Start", 
			b2Vec2(0.5f, 0.6f), d2d::Alignment::CENTERED,
			0.035f, m_orbitronLightFont.GetID(),
			d2d::Color(0.0f, 0.5f, 0.8f, 1.0f) };

		d2d::TextBlock m_menuStart2{ "2P Start", 
			b2Vec2(0.5f, 0.5f), d2d::Alignment::CENTERED,
			0.035f, m_orbitronLightFont.GetID(),
			d2d::Color(0.0f, 0.5f, 0.8f, 1.0f) };

		d2d::TextBlock m_menuQuit{ "Quit", 
			b2Vec2(0.5f, 0.4f), d2d::Alignment::CENTERED,
			0.035f, m_orbitronLightFont.GetID(),
			d2d::Color(0.0f, 0.5f, 0.8f, 1.0f) };
	};
}
#endif //MENUGAMESTATE_H