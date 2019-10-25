#include "pch.h"
#include "Pong.h"
#include "PlayGameState.h"
#include "MenuGameState.h"
#include "d2d.h"

namespace Pong
{
	PlayGameState::PlayGameState(bool twoPlayers)
	{
		/*
		// If the screen aspect ratio is too big or small, clamp the game aspect ratio to reasonable proportions
		double screenAspectRatio = d2d::Renderer::GetXYAspectRatio();
		double gameWidth, gameHeight;
		if (screenAspectRatio > m_maxGameApectRatio)
		{
			// Screen will be wider than game
			gameWidth = m_maxGameApectRatio * m_defaultHeight;
			gameHeight = m_defaultHeight;
		}
		else if (screenAspectRatio < m_minGameApectRatio)
		{
			// Screen will be taller than game
			gameWidth = m_defaultWidth;
			gameHeight = m_defaultWidth / m_minGameApectRatio;
		}
		else
		{
			// Screen will be completely filled by game
			gameWidth = m_defaultWidth;
			gameHeight = m_defaultWidth / screenAspectRatio;
		}*/

		// Start game
		m_pong.CreateGame(twoPlayers);
	}
	PlayGameState::~PlayGameState()
	{}
	void PlayGameState::HandleEvent(const SDL_Event& event)
	{
		switch(event.type)
		{
		case SDL_KEYDOWN:
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_gotoMenu = true; 
				break;
			case SDLK_UP:
				m_player2Up = true;
				m_pong.Player2PressedAButton(); 
				break;
			case SDLK_DOWN:
				m_player2Down = true;
				m_pong.Player2PressedAButton(); 
				break;
			case SDLK_w:
				m_player1Up = true;
				m_pong.Player1PressedAButton(); 
				break;
			case SDLK_s:
				m_player1Down = true;
				m_pong.Player1PressedAButton(); 
				break;
			} 
			break;
		case SDL_KEYUP:
			switch(event.key.keysym.sym)
			{
			case SDLK_UP:
				m_player2Up = false; 
				break;
			case SDLK_DOWN:
				m_player2Down = false; 
				break;
			case SDLK_w:
				m_player1Up = false; 
				break;
			case SDLK_s:
				m_player1Down = false; 
				break;
			} 
			break;
		/*
		case SDL_CONTROLLERBUTTONDOWN:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					std::cerr << "controller device id: " << event.cdevice.which << std::endl;
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
					{
						m_player1Up = true;
						m_pong.Player1PressedAButton();
					}
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
					{
						m_player2Up = true;
						m_pong.Player2PressedAButton();
					}
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					std::cerr << "controller device id: " << event.cdevice.which << std::endl;
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
					{
						m_player1Down = true;
						m_pong.Player1PressedAButton();
					}
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
					{
						m_player2Down = true;
						m_pong.Player2PressedAButton();
					}
					break;
			} break;
		// User releases a button
		case SDL_CONTROLLERBUTTONUP:
			switch(event.cbutton.button)
			{
				case SDL_CONTROLLER_BUTTON_DPAD_UP:
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
						m_player1Up = false;
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
						m_player2Up = false;
					break;
				case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
					if(event.cdevice.which == SDL_NumJoysticks() - 1)
						m_player1Down = false;
					else if(SDL_NumJoysticks() > 1 && event.cdevice.which == SDL_NumJoysticks() - 2)
						m_player2Down = false;
					break;
			} 
			break;
			*/
		}
	}
	std::shared_ptr<d2d::GameState> PlayGameState::Update(float deltaSeconds)
	{		
		if(m_gotoMenu)
		{
			return std::make_shared<MenuGameState>();
		}
		MapInputToGameActions();

		m_pong.Update(deltaSeconds);

		return shared_from_this();
	}
	void PlayGameState::MapInputToGameActions()
	{
		// Map Player1 controller to game
		if (m_player1Up && !m_player1Down)
			m_pong.SetPlayer1MovementFactor(1.0);
		else if (!m_player1Up && m_player1Down)
			m_pong.SetPlayer1MovementFactor(-1.0);
		else
			m_pong.SetPlayer1MovementFactor(0.0);

		if (m_pong.TwoPlayers())
		{
			// Map Player2 controller to game
			if (m_player2Up && !m_player2Down)
				m_pong.SetPlayer2MovementFactor(1.0);
			else if (!m_player2Up && m_player2Down)
				m_pong.SetPlayer2MovementFactor(-1.0);
			else
				m_pong.SetPlayer2MovementFactor(0.0);
		}
	}
	void PlayGameState::Draw()
	{
		d2d::Renderer::SetShowCursor(false);
		d2d::Rect cameraRect;

		PongData game;
		m_pong.GetData(game);
		SDL_assert(game.height > 0.0f);
		SDL_assert(game.width > 0.0f);
		float gameAspectRatio{ game.width / game.height };

		// Calculate camera viewport based on game dimensions and screen proportions
		float screenAspectRatio = d2d::Renderer::GetXYAspectRatio();
		float deadSpaceAboveBelow{ 0.0f };
		float deadSpaceLeftRight{ 0.0f };
		if (screenAspectRatio > gameAspectRatio)
		{
			// Screen will be wider than game
			b2Vec2 cameraDim(screenAspectRatio * game.height, game.height);
			deadSpaceLeftRight = 0.5f * (cameraDim.x - game.width);
			cameraRect.SetBounds({ -deadSpaceLeftRight, 0.0f }, { game.width + deadSpaceLeftRight, cameraDim.y });
		}
		else
		{
			// Screen will be taller than or equal to the game
			b2Vec2 cameraDim(game.width, game.width / screenAspectRatio);
			deadSpaceAboveBelow = 0.5f * (cameraDim.y - game.height);
			cameraRect.SetBounds({ 0.0f, -deadSpaceAboveBelow }, { cameraDim.x, game.height + deadSpaceAboveBelow });
		}

		// Start scene
		d2d::Renderer::SetCameraBounds(cameraRect.GetLowerBound(), cameraRect.GetUpperBound());
		d2d::Renderer::SetColor(d2d::Color(0.8f, 0.3f, 0.3f));

		// Draw game objects
		d2d::Renderer::DrawRect(game.player1Rect, true);
		d2d::Renderer::DrawRect(game.player2Rect, true);
		d2d::Renderer::DrawRect(game.ballRect, true);

		b2Vec2 gameDim{ game.width, game.height };

		// Draw game border
		d2d::Rect gameDrawRect;
		gameDrawRect.SetCenter(0.5f * gameDim, m_slightlyLessThanOne * gameDim);
		d2d::Renderer::DrawRect(gameDrawRect, false);

		// Draw net
		b2Vec2 middleBottom{ 0.5f * gameDim.x, 0.0f };
		b2Vec2 middleTop{ middleBottom.x, gameDim.y };
		d2d::Renderer::DrawLine(middleBottom, middleTop);

		// Update strings
		int fpsInt = (int)(d2d::Renderer::GetFPS() + 0.5);
		m_fps.text = std::to_string(fpsInt);
		m_player1Score.text = std::to_string(game.player1Score);
		m_player2Score.text = std::to_string(game.player2Score);
		int countdownInt = (int)(game.countdown + m_slightlyLessThanOne);
		m_countdown.text = std::to_string(countdownInt);

		// Draw text
		d2d::Renderer::SetCameraScreenMode();
		switch (m_pong.GetState())
		{
		case PongState::CONFIRM_PLAYERS_READY:
			if (m_pong.WaitingForPlayer1())
				d2d::Renderer::DrawTextBlock(m_player1Waiting);
			if (m_pong.WaitingForPlayer2())
				d2d::Renderer::DrawTextBlock(m_player2Waiting);
			break;
		case PongState::COUNTDOWN:
			if (countdownInt > 0)
				d2d::Renderer::DrawTextBlock(m_countdown);
			break;
		case PongState::GAME_OVER:
			if (game.player1Score > game.player2Score)
			{
				m_playerWins.text = "Player 1 Wins!";
				d2d::Renderer::DrawTextBlock(m_playerWins);
			}
			else
			{
				if (m_pong.TwoPlayers())
				{
					m_playerWins.text = "Player 2 Wins!";
					d2d::Renderer::DrawTextBlock(m_playerWins);
				}
				else
				{
					d2d::Renderer::DrawTextBlock(m_youLose);
				}
			}
			break;
		}
		d2d::Renderer::DrawTextBlock(m_player1Score);
		d2d::Renderer::DrawTextBlock(m_player2Score);
		d2d::Renderer::DrawTextBlock(m_fps);
	}
}
