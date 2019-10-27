/**************************************************************************************\
** File: Pong.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the Game class
**
\**************************************************************************************/
#include "pch.h"
#include "Game.h"
#include "d2d.h"

namespace Pong
{
	//+--------------------------------\--------------------------------------
	//|	   Initialization/Resetting	   |
	//\--------------------------------/--------------------------------------
	void Game::Init(bool twoPlayers)
	{
		m_twoPlayers = twoPlayers;
		m_state = GameState::CONFIRM_PLAYERS_READY;

		m_player1.Init(Side::LEFT, true);
		m_player2.Init(Side::RIGHT, twoPlayers);

		m_puck.ResetRound();
	}
	void Game::ResetRound()
	{
		m_player1.ResetRound();
		m_player2.ResetRound();
		m_puck.ResetRound();
		m_state = GameState::CONFIRM_PLAYERS_READY;
	}
	void Player::Init(Side newSide, bool newIsHuman)
	{
		m_score = 0;
		m_side = newSide;
		m_isHuman = newIsHuman;
		ResetRound();
	}
	void Player::ResetRound()
	{
		if(m_side == Side::LEFT)
			m_position.x = GAME_RECT.GetLowerBound().x;
		else
			m_position.x = GAME_RECT.GetUpperBound().x - PLAYER_SIZE.x;
		m_position.y = GAME_RECT.GetCenter().y - (0.5f * PLAYER_SIZE.y);

		m_movementFactor = 0.0f;

		if(m_isHuman)
			m_isReady = false;
		else
			m_isReady = true;
	}
	void Puck::ResetRound()
	{
		m_position = GAME_RECT.GetCenter() - 0.5f * PUCK_SIZE;

		// TODO: Randomize puck angle
		m_velocity.Set(cos(INITIAL_PUCK_ANGLE), sin(INITIAL_PUCK_ANGLE));
		m_velocity *= INITIAL_PUCK_SPEED;

		m_gotPastPlayer = false;
		m_scored = false;
	}

	//+--------------------------------\--------------------------------------
	//|		 Getters and Setters	   |
	//\--------------------------------/--------------------------------------
	Side Player::GetSide() const
	{
		return m_side;
	}
	const b2Vec2& Player::GetPosition() const
	{
		return m_position;
	}
	unsigned Player::GetScore() const
	{
		return m_score;
	}
	void Player::Score()
	{
		++m_score;
	}
	bool Player::IsReady() const
	{
		return m_isReady;
	}
	void Player::SetReady()
	{
		if(!m_isReady)
			m_isReady = true;
	}
	void Player::SetMovementFactor(float factor)
	{
		d2d::Clamp(factor, -1.0f, 1.0f);
		m_movementFactor = factor;
	}
	/*bool Puck::GotPastPlayer() const
	{
		return m_gotPastPlayer;
	}*/
	bool Puck::Scored() const
	{
		return m_scored;
	}
	bool Game::TwoPlayers() const
	{
		return m_twoPlayers;
	}
	GameState Game::GetState() const
	{
		return m_state;
	}
	bool Game::WaitingForPlayer1() const
	{
		return !m_player1.IsReady();
	}
	bool Game::WaitingForPlayer2() const
	{
		return !m_player2.IsReady();
	}
	void Game::Player1PressedAButton()
	{
		if (m_state == GameState::CONFIRM_PLAYERS_READY)
			m_player1.SetReady();
	}
	void Game::Player2PressedAButton()
	{
		if (m_state == GameState::CONFIRM_PLAYERS_READY)
			m_player2.SetReady();
	}
	void Game::SetPlayer1MovementFactor(float factor)
	{
		m_player1.SetMovementFactor(factor);
	}
	void Game::SetPlayer2MovementFactor(float factor)
	{
		m_player2.SetMovementFactor(factor);
	}

	//+--------------------------------\--------------------------------------
	//|			 Updates	    	   |
	//\--------------------------------/--------------------------------------
	void Game::Update(float dt)
	{
		switch (m_state)
		{
		case GameState::CONFIRM_PLAYERS_READY:
			if (m_player1.IsReady() && m_player2.IsReady())
			{
				m_countdownSecondsLeft = INITIAL_COUNTDOWN;
				m_state = GameState::COUNTDOWN;
			}
			break;
		case GameState::COUNTDOWN:
			m_countdownSecondsLeft -= dt;
			if (m_countdownSecondsLeft <= 0.0f)
				m_state = GameState::PLAY;
			break;
		case GameState::PLAY:
			m_player1.Update(dt);
			m_player2.Update(dt);
			m_puck.Update(dt, m_player1, m_player2);
			if(m_puck.Scored())
			{
				if(m_player1.GetScore() >= SCORE_TO_WIN || m_player2.GetScore() >= SCORE_TO_WIN)
					m_state = GameState::GAME_OVER;
				else
					ResetRound();
			}
			break;
		case GameState::GAME_OVER:
			break;
		default:
			break;
		}
	}
	void Player::Update(float dt)
	{
		// Move
		{
			const float MAX_DISPLACEMENT{ PLAYER_MAX_SPEED * dt };
			m_position.y += m_movementFactor * MAX_DISPLACEMENT;
		}

		// Clamp to top/bottom walls
		{
			const float MIN_Y{ GAME_RECT.GetLowerBound().y };
			const float MAX_Y{ GAME_RECT.GetUpperBound().y - PLAYER_SIZE.y };
			d2d::Clamp(m_position.y, MIN_Y, MAX_Y);
		}
	}
	void Puck::Update(float dt, Player& player1, Player& player2)
	{
		UpdatePosition(dt);
		if(!m_gotPastPlayer)
		{
			HandlePlayerCollision(player1);
			HandlePlayerCollision(player2);
		}
		HandleGoal(player1);
		HandleGoal(player2);
	}
	void Puck::UpdatePosition(float dt)
	{
		const float MIN_Y{ GAME_RECT.GetLowerBound().y };
		const float MAX_Y{ GAME_RECT.GetUpperBound().y - PUCK_SIZE.y };

		m_position += dt * m_velocity;

		// Bounce ball off top wall
		if(m_position.y >= MAX_Y)
		{
			const float PROTRUSION_Y{ m_position.y - MAX_Y };
			m_position.y = MAX_Y - PROTRUSION_Y;
			m_velocity.y = -m_velocity.y;
		}

		// Bounce ball off bottom wall
		if(m_position.y <= MIN_Y)
		{
			const float PROTRUSION_Y{ MIN_Y - m_position.y };
			m_position.y = MIN_Y + PROTRUSION_Y;
			m_velocity.y = -m_velocity.y;
		}
	}
	void Puck::HandlePlayerCollision(const Player& player)
	{
		float overlapX;

		// How much did the puck go past front of player along the x-axis?
		if (player.GetSide() == Side::LEFT)
			overlapX = (player.GetPosition().x + PLAYER_SIZE.x) - m_position.x;
		else
			overlapX = (m_position.x + PUCK_SIZE.x) - player.GetPosition().x;

		// If puck went past front of player along x-axis
		if(overlapX >= 0.0f)
		{
			// If the ball is traveling towards the player
			if((m_velocity.x < 0.0f && player.GetSide() == Side::LEFT) ||
				(m_velocity.x > 0.0f && player.GetSide() == Side::RIGHT))
			{
				// If there is separation along the Y-axis
				if(m_position.y > player.GetPosition().y + PLAYER_SIZE.y ||
					m_position.y + PUCK_SIZE.y < player.GetPosition().y)
				{
					// The paddle missed
					m_gotPastPlayer = true;
				}
				else
				{
					// Take ball back in time to where overlap along the x-axis occured.
					float secondsSinceOverlap = overlapX / std::abs(m_velocity.x);
					m_position -= secondsSinceOverlap * m_velocity;

					// Reverse direction
					m_velocity.x = -m_velocity.x;

					// Convert ball's velocity to polar coordinates
					float angleOut = atan2(m_velocity.y, m_velocity.x);
					float speed = sqrt( pow(m_velocity.x, 2) + pow(m_velocity.y, 2) );

					// Simulate curved paddle
					{
						// Determine how far off the player's center the ball hit
						float hitRangeBottom = player.GetPosition().y - PUCK_SIZE.y;
						float hitRangeTop = player.GetPosition().y + PLAYER_SIZE.y;
						float hitRangeLength = hitRangeTop - hitRangeBottom;
						float puckPercentFromBottom = (m_position.y - hitRangeBottom) / hitRangeLength;

						// Convert to a range of [-1,1] 
						float puckPercentFromCenterToEdge = puckPercentFromBottom * 2.0f - 1.0f;
						d2d::Clamp(puckPercentFromCenterToEdge, -1.0f, 1.0f);

						// Change angle based on how far off center it hit the player
						float maxAngleChange = d2d::PI / 4.0f;
						float angleChange = maxAngleChange * puckPercentFromCenterToEdge;
						if(player.GetSide() == Side::LEFT)
							angleOut += angleChange;
						else
							angleOut -= angleChange;
						d2d::ConstrainRadians(angleOut);
					}

					// Clamp angle to range
					float angleLimitTop, angleLimitBottom;
					float halfAngleRange = BOUNCE_ANGLE_RANGE / 2.0f;
					if(player.GetSide() == Side::LEFT)
					{
						angleLimitTop = halfAngleRange;
						angleLimitBottom = 2.0f * d2d::PI - halfAngleRange;
						if(angleOut > angleLimitTop&& angleOut < d2d::PI)
							angleOut = angleLimitTop;
						else if(angleOut < angleLimitBottom && angleOut >= d2d::PI)
							angleOut = angleLimitBottom;
					}
					else
					{
						angleLimitTop = d2d::PI - halfAngleRange;
						angleLimitBottom = d2d::PI + halfAngleRange;
						if(angleOut < angleLimitTop)
							angleOut = angleLimitTop;
						else if(angleOut > angleLimitBottom)
							angleOut = angleLimitBottom;
					}

					// Apply new angle with extra 2% speed boost
					m_velocity.Set(cos(angleOut), sin(angleOut));
					m_velocity *= speed * PUCK_SPEED_BOOST_MULTIPLIER;

					// Now that ball is going in the right direction...
					// Give time back
					m_position += secondsSinceOverlap * m_velocity;
				}
			}
		}
	}
	void Puck::HandleGoal(Player& player)
	{
		if(m_scored)
			return;

		if( (player.GetSide() == Side::LEFT && m_position.x + PUCK_SIZE.x >= GAME_RECT.GetUpperBound().x) ||
			(player.GetSide() == Side::RIGHT && m_position.x <= GAME_RECT.GetLowerBound().x) )
		{
			player.Score();
			m_scored = true;
		}
	}

	//+--------------------------------\--------------------------------------
	//|			   Drawing	    	   |
	//\--------------------------------/--------------------------------------
	void Game::Draw()
	{
		b2Vec2 gameDim{ GAME_RECT.GetWidth(), GAME_RECT.GetHeight() };
		{
			b2Vec2 deadSpace;
			{
				float gameAspectRatio{ gameDim.x / gameDim.y };
				float screenAspectRatio{ d2d::Renderer::GetXYAspectRatio() };

				// Fill as much of screen as possible while maintaining game aspect ratio
				if(screenAspectRatio > gameAspectRatio)
				{
					// Screen will be wider than game
					float cameraWidth{ screenAspectRatio * gameDim.y };
					deadSpace.Set(0.5f * (cameraWidth - gameDim.x), 0.0f);
				}
				else
				{
					// Screen will be taller than or equal to the game
					float cameraHeight{ gameDim.x / screenAspectRatio };
					deadSpace.Set(0.0f, 0.5f * (cameraHeight - gameDim.y));
				}
			}
			d2d::Renderer::SetCameraBounds(
				GAME_RECT.GetLowerBound() - deadSpace,
				GAME_RECT.GetUpperBound() + deadSpace );
		}

		m_player1.Draw();
		m_player2.Draw();
		m_puck.Draw();

		// Draw game
		const float slightlyLessThanOne{ 0.99999f };
		{
			// Draw game border
			d2d::Rect gameDrawRect;
			gameDrawRect.SetCenter(GAME_RECT.GetCenter(), slightlyLessThanOne * gameDim);
			d2d::Renderer::DrawRect(gameDrawRect, false);

			// Draw net
			b2Vec2 middleBottom{ GAME_RECT.GetCenter().x, GAME_RECT.GetLowerBound().y };
			b2Vec2 middleTop{ middleBottom.x, GAME_RECT.GetUpperBound().y };
			d2d::Renderer::DrawLine(middleBottom, middleTop);

		}

		// Update strings
		int countdownInt = (int)(m_countdownSecondsLeft + slightlyLessThanOne);
		{
			int fpsInt = (int)(d2d::Renderer::GetFPS() + 0.5);
			m_fps.text = std::to_string(fpsInt);
			m_player1Score.text = std::to_string(m_player1.GetScore());
			m_player2Score.text = std::to_string(m_player2.GetScore());
			m_countdown.text = std::to_string(countdownInt);
		}

		// Draw text
		{
			d2d::Renderer::SetCameraScreenMode();
			switch(m_state)
			{
			case GameState::CONFIRM_PLAYERS_READY:
				if(WaitingForPlayer1())
					d2d::Renderer::DrawTextBlock(m_player1Waiting);
				if(WaitingForPlayer2())
					d2d::Renderer::DrawTextBlock(m_player2Waiting);
				break;
			case GameState::COUNTDOWN:
				if(countdownInt > 0)
					d2d::Renderer::DrawTextBlock(m_countdown);
				break;
			case GameState::GAME_OVER:
				if(m_player1.GetScore() > m_player2.GetScore())
				{
					m_playerWins.text = "Player 1 Wins!";
					d2d::Renderer::DrawTextBlock(m_playerWins);
				}
				else
				{
					if(m_twoPlayers)
					{
						m_playerWins.text = "Player 2 Wins!";
						d2d::Renderer::DrawTextBlock(m_playerWins);
					}
					else
						d2d::Renderer::DrawTextBlock(m_youLose);
				}
				break;
			}
			d2d::Renderer::DrawTextBlock(m_player1Score);
			d2d::Renderer::DrawTextBlock(m_player2Score);
			d2d::Renderer::DrawTextBlock(m_fps);
		}
	}
	void Player::Draw()
	{
		d2d::Renderer::SetColor(PLAYER_COLOR);
		d2d::Renderer::DrawRect({ m_position, m_position + PLAYER_SIZE }, true);
	}
	void Puck::Draw()
	{
		d2d::Renderer::SetColor(PUCK_COLOR);
		d2d::Renderer::DrawRect({ m_position, m_position + PUCK_SIZE }, true);
	}
}