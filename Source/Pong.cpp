/**************************************************************************************\
** File: Pong.cpp
** Project:
** Author: David Leksen
** Date:
**
** Source code file for the PongLogic
**
\**************************************************************************************/
#include "pch.h"
#include "Pong.h"
#include "d2d.h"

namespace Pong
{
	void PongLogic::CreateGame(bool twoPlayers)
	{
		m_twoPlayers = twoPlayers;
		m_state = PongState::CONFIRM_PLAYERS_READY;

		m_left = 0.0f;
		m_right = m_width;
		m_bottom = 0.0f;
		m_top = m_height;
		m_middleX = 0.5f * (m_left + m_right);
		m_middleY = 0.5f * (m_bottom + m_top);

		// Players
		m_player2.w = m_player1.w = m_playerWidth;
		m_player2.h = m_player1.h = m_playerHeight;
		m_player1.score = m_player2.score = 0;
		m_player1.side = PlayerSide::LEFT;
		m_player2.side = PlayerSide::RIGHT;
		m_player1.human = true;
		if (m_twoPlayers)
			m_player2.human = true;
		else
			m_player2.human = false;
		ResetPlayers();

		// Ball
		m_ball.w = m_ball.h = m_ballSize;
		ResetBall();
	}
	void PongLogic::GetData(PongData& outputData) const
	{
		outputData.width = m_width;
		outputData.height = m_height;
		outputData.player1Score = m_player1.score;
		outputData.player2Score = m_player2.score;
		outputData.countdown = m_countdownSecondsLeft;
		outputData.player1Rect.SetBounds(b2Vec2(m_player1.x, m_player1.y), 
			b2Vec2(m_player1.x + m_player1.w, m_player1.y + m_player1.h));
		outputData.player2Rect.SetBounds(b2Vec2(m_player2.x, m_player2.y),
			b2Vec2(m_player2.x + m_player2.w, m_player2.y + m_player2.h));
		outputData.ballRect.SetBounds(b2Vec2(m_ball.x, m_ball.y),
			b2Vec2(m_ball.x + m_ball.w, m_ball.y + m_ball.h));
	}
	bool PongLogic::TwoPlayers() const
	{
		return m_twoPlayers;
	}
	PongState PongLogic::GetState() const
	{
		return m_state;
	}
	bool PongLogic::WaitingForPlayer1() const
	{
		return !m_player1Ready;
	}
	bool PongLogic::WaitingForPlayer2() const
	{
		return !m_player2Ready;
	}
	void PongLogic::Player1PressedAButton()
	{
		if (m_state == PongState::CONFIRM_PLAYERS_READY)
		{
			m_player1Ready = true;
		}
	}
	void PongLogic::Player2PressedAButton()
	{
		if (m_state == PongState::CONFIRM_PLAYERS_READY)
		{
			m_player2Ready = true;
		}
	}
	// factor: [-1.0,1.0]
	void PongLogic::SetPlayer1MovementFactor(float factor)
	{
		d2d::Clamp(factor, -1.0f, 1.0f);
		m_player1.movementFactor = factor;
	}
	// factor: [-1.0,1.0]
	void PongLogic::SetPlayer2MovementFactor(float factor)
	{
		d2d::Clamp(factor, -1.0f, 1.0f);
		m_player2.movementFactor = factor;
	}
	void PongLogic::Update(float deltaSeconds)
	{
		// Pong Finite State Machine
		switch (m_state)
		{
		case PongState::CONFIRM_PLAYERS_READY:
			if (m_player1Ready && m_player2Ready)
			{
				m_countdownSecondsLeft = m_countdownSeconds;
				m_state = PongState::COUNTDOWN;
			}
			break;
		case PongState::COUNTDOWN:
			m_countdownSecondsLeft -= deltaSeconds;
			if (m_countdownSecondsLeft <= 0.0f)
			{
				m_state = PongState::PLAY;
			}
			break;
		case PongState::PLAY:
			UpdatePlayers(deltaSeconds);
			UpdateBall(deltaSeconds);
			HandleGoals();

			if (!m_ball.gotPastPlayer)
			{
				HandleCollision(m_ball, m_player1);
				HandleCollision(m_ball, m_player2);
			}

			break;
		case PongState::GAME_OVER:
			break;
		default:
			break;
		}
	}
	void PongLogic::UpdatePlayers(float deltaSeconds)
	{
		// Distance = Speed * Time
		const float displacementY = m_playerSpeed * deltaSeconds;

		// Move players
		m_player1.y += displacementY * m_player1.movementFactor;
		m_player2.y += displacementY * m_player2.movementFactor;

		// Stop players at the top/bottom walls
		d2d::Clamp(m_player1.y, m_bottom, m_top - m_player1.h);
		d2d::Clamp(m_player2.y, m_bottom, m_top - m_player2.h);
	}
	void PongLogic::UpdateBall(float deltaSeconds)
	{
		const float ballminY = m_bottom;
		const float ballmaxY = m_top - m_ball.h;
		m_ball.x += m_ball.velocityX * deltaSeconds;
		m_ball.y += m_ball.velocityY * deltaSeconds;

		// Bounce ball off top wall
		if (m_ball.y >= ballmaxY)
		{
			m_ball.y = ballmaxY;
			m_ball.velocityY = -m_ball.velocityY;
		}
		// Bounce ball off bottom wall
		if (m_ball.y <= ballminY)
		{
			m_ball.y = ballminY;
			m_ball.velocityY = -m_ball.velocityY;
		}
	}
	void PongLogic::HandleCollision(Ball& ball, Player& player)
	{
		float overlapX{ 0.0 };

		// If ball is headed towards Player1, do Player1 collision detection
		if (player.side == PlayerSide::LEFT)
			overlapX = player.x + player.w - ball.x;
		else
			overlapX = (ball.x + ball.w) - player.x;

		// Negative overlap means there can't be a collision
		if (overlapX < 0.0f)
			return;

		// If the ball is traveling towards the player
		if ((ball.velocityX < 0.0f && player.side == PlayerSide::LEFT) ||
			(ball.velocityX > 0.0f && player.side == PlayerSide::RIGHT))
		{

			// If there is separation along the Y-axis
			if (ball.y > player.y + player.h ||
				ball.y + ball.h < player.y)
			{
				// The paddle missed
				ball.gotPastPlayer = true;
			}
			else
			{
				// Take ball back in time to where overlap along the x-axis occured.
				float secondsSinceOverlap = overlapX / std::abs(ball.velocityX);
				ball.x -= ball.velocityX * secondsSinceOverlap;
				ball.y -= ball.velocityY * secondsSinceOverlap;

				// Reverse direction
				ball.velocityX = -ball.velocityX;

				// Convert ball's trajectory to polar coordinates
				float angleOut = atan2(ball.velocityY, ball.velocityX);
				float speed = sqrt(pow(ball.velocityX, 2) + pow(ball.velocityY, 2));

				// Determine how far off the player's center the ball hit
				float hitRangeBottom = player.y - ball.h;
				float hitRangeTop = player.y + player.h;
				float hitRangeLength = hitRangeTop - hitRangeBottom;
				float ballPercentFromBottom = (ball.y - hitRangeBottom) / hitRangeLength;

				// Convert to a range of [-1,1] 
				float ballPercentFromCenterToEdge = ballPercentFromBottom * 2.0f - 1.0f;
				d2d::Clamp(ballPercentFromCenterToEdge, -1.0f, 1.0f);

				// Simulate curved paddle
				float maxAngleChange = d2d::PI / 4.0f;
				float angleChange = maxAngleChange * ballPercentFromCenterToEdge;
				if (player.side == PlayerSide::RIGHT)
				{
					angleChange = -angleChange;
				}
				angleOut += angleChange;
				d2d::ConstrainRadians(angleOut);

				// Apply spin
				//ball.angularVelocity 

				// Clamp angle to range
				float angleLimitTop, angleLimitBottom;
				float halfAngleRange = m_angleRange / 2.0f;
				switch(player.side)
				{
				case PlayerSide::LEFT:
					angleLimitTop = halfAngleRange;
					angleLimitBottom = 2.0f * d2d::PI - halfAngleRange;
					if(angleOut > angleLimitTop && angleOut < d2d::PI)
						angleOut = angleLimitTop;
					else if(angleOut < angleLimitBottom && angleOut >= d2d::PI)
						angleOut = angleLimitBottom;
					break;
				case PlayerSide::RIGHT:
					angleLimitTop = d2d::PI - halfAngleRange;
					angleLimitBottom = d2d::PI + halfAngleRange;
					if(angleOut < angleLimitTop)
						angleOut = angleLimitTop;
					else if(angleOut > angleLimitBottom)
						angleOut = angleLimitBottom;
					break;
				}

				// Apply new angle with extra 2% speed boost
				ball.velocityX = speed * cos(angleOut) * m_ballSpeedBoostMultiplier;
				ball.velocityY = speed * sin(angleOut) * m_ballSpeedBoostMultiplier;

				// Now that ball is going in the right direction...
				// Give time back
				ball.x += ball.velocityX * secondsSinceOverlap;
				ball.y += ball.velocityY * secondsSinceOverlap;
			}
		}
	}
	void PongLogic::HandleGoals()
	{
		bool someoneScored{ false };
		if (m_ball.x <= m_left)
		{
			++m_player2.score;
			someoneScored = true;
		}
		else if (m_ball.x + m_ball.w >= m_right)
		{
			++m_player1.score;
			someoneScored = true;
		}
		if (someoneScored)
		{
			if (m_player1.score >= m_scoreToWin ||
				m_player2.score >= m_scoreToWin)
			{
				m_state = PongState::GAME_OVER;
			}
			else
			{
				// Start new match
				Reset();
				m_state = PongState::CONFIRM_PLAYERS_READY;
			}
		}
	}
	void PongLogic::Reset()
	{
		ResetPlayers();
		ResetBall();
	}
	void PongLogic::ResetPlayers()
	{
		m_player1.movementFactor = m_player2.movementFactor = 0;
		m_player1Ready = false;
		if (m_twoPlayers)
			m_player2Ready = false;
		else
			m_player2Ready = true;

		m_player1.x = m_left;
		m_player1.y = m_middleY - (0.5f * m_player1.h);

		m_player2.x = m_right - m_player2.w;
		m_player2.y = m_middleY - (0.5f * m_player2.h);
	}
	void PongLogic::ResetBall()
	{
		m_ball.x = m_middleX - (0.5f * m_ball.w);
		m_ball.y = m_middleY - (0.5f * m_ball.h);

		m_ball.velocityX = m_initialBallSpeed * cos(m_ballAngle);
		m_ball.velocityY = m_initialBallSpeed * sin(m_ballAngle);
		m_ball.angularVelocity = 0.0;

		m_ball.gotPastPlayer = false;
	}
}