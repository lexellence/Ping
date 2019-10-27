/**************************************************************************************\
** File: Pong.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the Game, PongData, GameType
**
\**************************************************************************************/
#pragma once
#include "d2d.h"

namespace Pong
{
	enum class GameState 
	{
		CONFIRM_PLAYERS_READY,
		COUNTDOWN,
		PLAY,
		GAME_OVER
	};
	enum class Side
	{
		LEFT,
		RIGHT
	};

	const d2d::Rect GAME_RECT{ b2Vec2_zero, { 400.0f, 200.0f } };
	const b2Vec2 PLAYER_SIZE{ 0.02f * GAME_RECT.GetWidth(), 
							  0.23f * GAME_RECT.GetHeight() };
	const b2Vec2 PUCK_SIZE{ 0.01f * GAME_RECT.GetWidth(), 
							0.01f * GAME_RECT.GetWidth() };

	const unsigned SCORE_TO_WIN{ 3u };
	const float INITIAL_COUNTDOWN{ 3.0f };
	const float INITIAL_PUCK_SPEED{ 0.575f * GAME_RECT.GetWidth() };
	const float INITIAL_PUCK_ANGLE{ 0.748f * d2d::PI };
	const float BOUNCE_ANGLE_RANGE{ 0.780f * d2d::PI };
	const float PUCK_SPEED_BOOST_MULTIPLIER{ 1.07f };
	const float PLAYER_MAX_SPEED{ 0.8f * INITIAL_PUCK_SPEED };

	const d2d::Color BOUNDARY_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color NET_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color PLAYER_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color PUCK_COLOR{ 0.8f, 0.3f, 0.3f };

	class Player
	{
	public:
		void Init(Side newSide, bool newIsHuman);
		void ResetRound();
		Side GetSide() const;
		const b2Vec2& GetPosition() const;
		unsigned GetScore() const;
		void Score();
		bool IsReady() const;
		void SetReady();
		void SetMovementFactor(float factor);
		void Update(float dt);
		void Draw();

	private:
		unsigned m_score;
		Side m_side;
		bool m_isHuman;

		b2Vec2 m_position;	// Lower-left corner
		float m_movementFactor;
		bool m_isReady;
	};

	struct Puck
	{
	public:
		void ResetRound();
		//bool GotPastPlayer() const;
		bool Scored() const;
		void Update(float dt, Player& player1, Player& player2);
		void Draw();

	private:
		b2Vec2 m_position;	// Lower-left corner
		b2Vec2 m_velocity;
		bool m_gotPastPlayer;
		bool m_scored;

		void UpdatePosition(float dt);
		void HandlePlayerCollision(const Player& player);
		void HandleGoal(Player& player);
	};

	class Game
	{
	public:
		void Init(bool twoPlayers);
		void Update(float dt);
		//void Update(float dt, PlayerController& playerController);
		void Draw();

		bool TwoPlayers() const;
		GameState GetState() const;

		bool WaitingForPlayer1() const;
		bool WaitingForPlayer2() const;
		void Player1PressedAButton();
		void Player2PressedAButton();
		void SetPlayer1MovementFactor(float factor); // [-1.0,1.0]
		void SetPlayer2MovementFactor(float factor); // [-1.0,1.0]

	private:
		void ResetRound();

		bool m_twoPlayers;
		GameState m_state;
		Player m_player1, m_player2;
		Puck m_puck;

		float m_countdownSecondsLeft;

		// Assets
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };

		// Text
		float edgeGap{ 0.015f };

		d2d::TextBlock m_fps{ "0", b2Vec2(1.0f - edgeGap, 1.0f - edgeGap),
			d2d::Alignment::RIGHT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player1Score{ "0", b2Vec2(0.5f - edgeGap, 1.0f - edgeGap),
			d2d::Alignment::RIGHT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player2Score{ "0", b2Vec2(0.5f + edgeGap, 1.0f - edgeGap),
			d2d::Alignment::LEFT_TOP,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_countdown{ "0", b2Vec2(0.5f, 0.5f),
			d2d::Alignment::CENTERED,
			0.05f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player1Waiting{ "Press W or S when ready", b2Vec2(0.25f, 0.5f),
			d2d::Alignment::CENTERED,
			0.025f, m_orbitronLightFont.GetID(),
			d2d::Color(1.0f, 1.0f, 0.0f, 1.0f) };

		d2d::TextBlock m_player2Waiting{ "Press UP or DOWN when ready", b2Vec2(0.75f, 0.5f),
			d2d::Alignment::CENTERED,
			0.025f, m_orbitronLightFont.GetID(),
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
