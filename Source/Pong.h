/**************************************************************************************\
** File: Pong.h
** Project: 
** Author: David Leksen
** Date: 
**
** Header file for the PongLogic, PongData, GameType
**
\**************************************************************************************/
#ifndef PONG_H
#define PONG_H
#include "d2d.h"

namespace Pong
{
	enum class GameType
	{
		SINGLE_PLAYER,
		TWO_PLAYERS
	};
	enum class PongState 
	{
		CONFIRM_PLAYERS_READY,
		COUNTDOWN,
		PLAY,
		GAME_OVER
	};

	// Output
	struct PongData
	{
		float width, height;
		unsigned int player1Score;
		unsigned int player2Score;
		float countdown;

		// Local coordinates with lower-left origin
		d2d::Rect player1Rect;
		d2d::Rect player2Rect;
		d2d::Rect ballRect;
	};

	class PongLogic
	{
	public:
		void CreateGame(bool twoPlayers);
		void Update(float deltaSeconds);

		void GetData(PongData& outputData) const;
		bool TwoPlayers() const;
		PongState GetState() const;

		bool WaitingForPlayer1() const;
		bool WaitingForPlayer2() const;
		void Player1PressedAButton();
		void Player2PressedAButton();
		void SetPlayer1MovementFactor(float factor); // [-1.0,1.0]
		void SetPlayer2MovementFactor(float factor); // [-1.0,1.0]

	private:
		class GameObject
		{
		public:
			float x, y; // bottom left
			float w, h;
		};
		enum class PlayerSide
		{
			LEFT,
			RIGHT
		};
		class Player : public GameObject
		{
		public:
			float movementFactor;
			unsigned score;
			bool human;
			PlayerSide side;
		};
		class Ball : public GameObject
		{
		public:
			float velocityX, velocityY;
			float angularVelocity;
			bool gotPastPlayer;
		};

		// Internal Functions
		void UpdatePlayers(float deltaSeconds);
		void UpdateBall(float deltaSeconds);
		void HandleCollision(Ball& ball, Player& player);
		void HandleGoals();
		void Reset();
		void ResetPlayers();
		void ResetBall();

		// Game settings
		float m_width{ 400.0f };
		float m_height{ 200.0f };
		unsigned m_scoreToWin{ 3u };
		float m_countdownSeconds{ 3.0f };
		float m_playerWidth{ m_width * 0.02f };
		float m_playerHeight{ m_height * 0.23f };
		float m_initialBallSpeed{ 230.0f };
		float m_ballSpeedBoostMultiplier{ 1.07f };
		float m_playerSpeed{ m_initialBallSpeed * 0.8f };
		float m_ballSize{ m_width * 0.01f };
		float m_angleRange{ d2d::PI * 0.78f };
		float m_stationaryBounceSpin{ 1.0f };
		float m_movingPlayerSpin{ 4.0f };

		bool m_twoPlayers;
		PongState m_state;
		Player m_player1, m_player2;
		Ball m_ball;

		bool m_player1Ready{ false };
		bool m_player2Ready{ false };

		float m_left;
		float m_right;
		float m_bottom;
		float m_top;
		float m_middleX;
		float m_middleY;

		float m_countdownSecondsLeft{ 0.0f };
		float m_ballAngle{ 2.35f };
	};
}
#endif //PONGLOGIC_H