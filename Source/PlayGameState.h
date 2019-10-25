#ifndef PLAYGAMESTATE_H
#define PLAYGAMESTATE_H
#include "d2d.h"
#include "Pong.h"

namespace Pong
{
	class PlayGameState : public d2d::GameState
	{
	public:
		PlayGameState(bool twoPlayers);
		virtual ~PlayGameState();
		virtual void HandleEvent(const SDL_Event& event);
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds);
		virtual void Draw();

	private:
		void MapInputToGameActions();

		PongLogic m_pong;

		bool m_gotoMenu{ false };
		bool m_player1Up{ false };
		bool m_player1Down{ false };
		bool m_player2Up{ false };
		bool m_player2Down{ false };

		const float m_slightlyLessThanOne{ 0.99999f };

		// Assets
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };

		// Text
		d2d::TextBlock m_fps{ "0", b2Vec2(0.99f, 0.99f),
			d2d::Alignment::RIGHT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player1Score{ "0", b2Vec2(0.45f, 0.95f),
			d2d::Alignment::RIGHT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player2Score{ "0", b2Vec2(0.65f, 0.95f),
			d2d::Alignment::LEFT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_countdown{ "0", b2Vec2(0.5f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player1Waiting{ "Press W or S when ready", b2Vec2(0.25f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player2Waiting{ "Press UP or DOWN when ready", b2Vec2(0.25f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_playerWins{ " ", b2Vec2(0.5f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_youLose{ "You Lose", b2Vec2(0.5f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };
	};
}
#endif //PLAYGAMESTATE_H