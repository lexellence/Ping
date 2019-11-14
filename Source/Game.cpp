/**************************************************************************************\
** File: Game.cpp
** Project: Pong
** Author: David Leksen
** Date:
**
** Source code file for the Game class
**
\**************************************************************************************/
#include "pch.h"
#include "Game.h"
#include "GameInitSettings.h"
#include "NetworkDef.h"
#include "Exceptions.h"

using namespace Pong::GameInitSettings;

namespace Pong
{
	//+--------------------------------\--------------------------------------
	//|				Game			   |
	//\--------------------------------/--------------------------------------
	Game::~Game()
	{
		OnQuit();
	}
	void Game::OnQuit()
	{
		CloseNetwork();
	}

	void Game::Init()
	{
		m_mode = GameInitSettings::GetGameMode();

		m_player1.Init(Side::LEFT);
		m_player2.Init(Side::RIGHT);
		m_puck.ResetRound();

		if (IsNetworked())
			InitNetwork();

		if (IsServer())
			m_state = GameState::WAIT_FOR_CLIENT_CONNECTION;
		else
			m_state = GameState::CONFIRM_PLAYERS_READY;
	}
	void Game::ResetRound()
	{
		m_player1.ResetRound();
		m_player2.ResetRound();
		m_puck.ResetRound();
		m_state = GameState::CONFIRM_PLAYERS_READY;
	}

	void Game::InitNetwork()
	{
		// Make sure we start fresh
		CloseNetwork();

		// Double check that we need network
		if (!IsNetworked())
			return;

		// Get our IP addresses and port numbers from file
		m_networkSettings.LoadFrom("Data\\network.hjson");

		// Allocate memory for socket set
		m_socketSet = SDLNet_AllocSocketSet(1);
		if (!m_socketSet)
			throw GameException{ std::string{"Unable to allocate socket set: "} +SDLNet_GetError() };

		// Server: open port to allow clients to connect
		if (IsServer())
		{
			d2LogInfo << "Attempting to open port " << m_networkSettings.server.localPort;

			// Get proper host format
			IPaddress myIP;
			if (SDLNet_ResolveHost(&myIP, nullptr, m_networkSettings.server.localPort) != 0)
				throw GameException{ std::string{"Failed to resolve local port "} +d2d::ToString(m_networkSettings.server.localPort) + ": " + SDLNet_GetError() };

			// Open port
			if (!(m_serverSocketTCP = SDLNet_TCP_Open(&myIP)))
				throw GameException{ std::string{"TCP: Failed to open port "} +d2d::ToString(m_networkSettings.server.localPort) + " for listening: " + SDLNet_GetError() };

			// Add server socket to set so we don't have to block while waiting for client
			if (SDLNet_TCP_AddSocket(m_socketSet, m_serverSocketTCP) == -1)
				throw GameException{ std::string{"Failed to add server TCP socket to socket set: "} +SDLNet_GetError() };

			// Print success confirmation
			d2LogInfo << "Opened port " << d2d::GetPort(myIP) << ". Waiting for client connection...";
		}
		// Client: connect to server
		else
		{
			d2LogInfo << "Attempting to connect to " << m_networkSettings.client.remoteIP << " port " << m_networkSettings.client.remotePort;

			// Get proper host format
			IPaddress serverIP;
			if (SDLNet_ResolveHost(&serverIP, m_networkSettings.client.remoteIP.c_str(), m_networkSettings.client.remotePort) != 0)
				throw GameException{ std::string{"Failed to resolve host "} +m_networkSettings.client.remoteIP +
					" Port " + d2d::ToString(m_networkSettings.client.remotePort) + ": " + SDLNet_GetError() };

			// Connect to server's open port
			if (!(m_clientSocketTCP = SDLNet_TCP_Open(&serverIP)))
				throw GameException{ std::string{"TCP: Failed to connect to port "} +d2d::ToString(m_networkSettings.client.remotePort) +
					" at " + m_networkSettings.client.remoteIP + ": " + SDLNet_GetError() };

			// Add client socket to set so we don't have to block to send/receive
			if (SDLNet_TCP_AddSocket(m_socketSet, m_clientSocketTCP) == -1)
				throw GameException{ std::string{"Failed to add client TCP socket to socket set: "} +SDLNet_GetError() };

			// Print out the server's IP and port
			d2LogInfo << d2d::GetIPOctetsString(serverIP) << " port " << d2d::GetPort(serverIP)
				<< " accepted a connection";
		}

		/*Uint16 updPort;
		if(GameInitSettings::GetGameMode() == GameInitSettings::Mode::SERVER)
			updPort = m_udpServerPort;
		else
			updPort = 0;
		m_socketUDP = SDLNet_UDP_Open(updPort);
		if(!m_socketUDP)
			throw GameException{ std::string{"Unable to open UDP connection: "} +SDLNet_GetError() };
			*/

			//if(SDLNet_UDP_AddSocket(m_socketSet, m_socketUDP) == -1)
			//	throw GameException{ std::string{"Unable to add UDP socket to socket set: "} +SDLNet_GetError() };
	}
	void Game::CloseNetwork()
	{
		if (m_serverSocketTCP)
		{
			if (SDLNet_TCP_DelSocket(m_socketSet, m_serverSocketTCP) == -1)
			{
				std::string message{ "Could not delete TCP socket from socket set: " };
				message += SDLNet_GetError();
				d2LogWarning << message;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message.c_str(), nullptr);
			}
			SDLNet_TCP_Close(m_serverSocketTCP);
			m_serverSocketTCP = nullptr;
		}
		if (m_clientSocketTCP)
		{
			SDLNet_TCP_Close(m_clientSocketTCP);
			m_clientSocketTCP = nullptr;
		}
		/*if(m_socketUDP)
		{
			if(SDLNet_UDP_DelSocket(m_socketSet, m_socketUDP) == -1)
			{
				std::string message{ "Could not delete UDP socket from socket set: " };
				message += SDLNet_GetError();
				d2LogWarning << message;
				SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", message.c_str(), nullptr);
			}
			SDLNet_UDP_Close(m_socketUDP);
			m_socketUDP = nullptr;
		}*/
		if (m_socketSet)
		{
			SDLNet_FreeSocketSet(m_socketSet);
			m_socketSet = nullptr;
		}

		// Clear the message queue
		m_networkInputBuffer.length = 0;
	}

	void Game::Player1PressedAButton()
	{
		if (m_state == GameState::CONFIRM_PLAYERS_READY && !IsClient())
		{
			m_player1.SetReady();
			if (IsServer())
				m_networkOutputBuffer.WriteByte(MESSAGE_PLAYER_READY);
		}
	}
	void Game::Player2PressedAButton()
	{
		if (m_state == GameState::CONFIRM_PLAYERS_READY && !IsServer())
		{
			m_player2.SetReady();
			if (IsClient())
				m_networkOutputBuffer.WriteByte(MESSAGE_PLAYER_READY);
		}
	}
	void Game::SetPlayer1MovementFactor(float factor)
	{
		m_player1.SetMovementFactor(factor);
	}
	void Game::SetPlayer2MovementFactor(float factor)
	{
		m_player2.SetMovementFactor(factor);
	}
	
	void Game::Draw() const
	{
		b2Vec2 gameDim{ GAME_RECT.GetWidth(), GAME_RECT.GetHeight() };
		{
			b2Vec2 deadSpace;
			{
				float gameAspectRatio{ gameDim.x / gameDim.y };
				float screenAspectRatio{ d2d::Window::GetXYAspectRatio() };

				// Fill as much of screen as possible while maintaining game aspect ratio
				if (screenAspectRatio > gameAspectRatio)
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
			d2d::Window::SetCameraRect(
				{ GAME_RECT.lowerBound - deadSpace,
				  GAME_RECT.upperBound + deadSpace });
		}

		d2d::Window::DisableTextures();
		d2d::Window::EnableBlending();

		// Draw game objects
		m_player1.Draw();
		m_player2.Draw();
		m_puck.Draw();

		// Draw game border
		{
			d2d::Rect gameDrawRect;
			gameDrawRect.SetCenter(GAME_RECT.GetCenter(), SLIGHTLY_LESS_THAN_ONE * gameDim);
			d2d::Window::DrawRect(gameDrawRect, false);
		}

		// Draw net
		{
			b2Vec2 middleBottom{ GAME_RECT.GetCenter().x, GAME_RECT.lowerBound.y };
			b2Vec2 middleTop{ middleBottom.x, GAME_RECT.upperBound.y };
			d2d::Window::DrawLine(middleBottom, middleTop);
		}


		// Draw text
		{
			switch (m_state)
			{
			case GameState::WAIT_FOR_CLIENT_CONNECTION:
				d2d::Window::PushMatrix();
				d2d::Window::SetColor(m_waitingForPlayerTextStyle.color);
				d2d::Window::Translate(m_waitingForPlayerLeftPosition);
				d2d::Window::DrawString(m_waitingForClientText, m_waitingForPlayerLeftAlignment,
					m_waitingForPlayerTextStyle.size, m_waitingForPlayerTextStyle.font);
				d2d::Window::PopMatrix();
				break;
			case GameState::CONFIRM_PLAYERS_READY:
				if (!m_player1.IsReady())
					DrawWaitingMessage(m_player1);
				if (!m_player2.IsReady())
					DrawWaitingMessage(m_player2);
				break;
			case GameState::COUNTDOWN:
				DrawCountdown();
				break;
			case GameState::GAME_OVER:
				DrawPlayerResult(m_player1.GetSide(), m_player1.GetScore() > m_player2.GetScore());
				DrawPlayerResult(m_player2.GetSide(), m_player2.GetScore() > m_player1.GetScore());
				break;
			}
			DrawScore(m_player1);
			DrawScore(m_player2);

			// Draw FPS
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_fpsPosition);
			d2d::Window::SetColor(m_fpsTextStyle.color);
			d2d::Window::DrawString(d2d::ToString((int)(d2d::Window::GetFPS() + 0.5f)), m_fpsAlignment,
				m_fpsTextStyle.size, m_fpsTextStyle.font);
			d2d::Window::PopMatrix();
		}
	}
	void Game::DrawPlayerResult(Side playerSide, bool isWinner) const
	{
		const std::string& resultText{ isWinner ? m_playerWinsText : m_playerLosesText };
		const d2d::TextStyle& resultTextStyle{ isWinner ? m_playerWinsTextStyle : m_playerLosesTextStyle };

		d2d::Window::PushMatrix();
		d2d::Window::SetColor(resultTextStyle.color);
		if (playerSide == Side::LEFT && !IsClient())
		{
			d2d::Window::Translate(m_playerResultLeftPosition);
			d2d::Window::DrawString(resultText, m_playerResultLeftAlignment,
				resultTextStyle.size, resultTextStyle.font);
		}
		else if (playerSide == Side::RIGHT && !IsServer())
		{
			d2d::Window::Translate(m_playerResultRightPosition);
			d2d::Window::DrawString(resultText, m_playerResultRightAlignment,
				resultTextStyle.size, resultTextStyle.font);
		}
		d2d::Window::PopMatrix();
	}
	void Game::DrawCountdown() const
	{
		int countdownInt = (int)(m_countdownSecondsLeft + SLIGHTLY_LESS_THAN_ONE);
		if (countdownInt > 0)
		{
			d2d::Window::PushMatrix();
			d2d::Window::Translate(m_countdownPosition);
			d2d::Window::SetColor(m_countdownTextStyle.color);
			d2d::Window::DrawString(d2d::ToString(countdownInt), m_countdownAlignment,
				m_countdownTextStyle.size, m_countdownTextStyle.font);
			d2d::Window::PopMatrix();
		}
	}
	void Game::DrawScore(const Player& player) const
	{
		std::string scoreText{ d2d::ToString(player.GetScore()) };

		d2d::Window::PushMatrix();
		d2d::Window::SetColor(m_scoreTextStyle.color);
		if (player.GetSide() == Side::LEFT)
		{
			d2d::Window::Translate(m_scoreLeftPosition);
			d2d::Window::DrawString(scoreText, m_scoreLeftAlignment,
				m_scoreTextStyle.size, m_scoreTextStyle.font);
		}
		else
		{
			d2d::Window::Translate(m_scoreRightPosition);
			d2d::Window::DrawString(scoreText, m_scoreRightAlignment,
				m_scoreTextStyle.size, m_scoreTextStyle.font);
		}
		d2d::Window::PopMatrix();
	}
	void Game::DrawWaitingMessage(const Player& player) const
	{
		d2d::Window::PushMatrix();
		d2d::Window::SetColor(m_waitingForPlayerTextStyle.color);
		if (player.GetSide() == Side::LEFT)
		{
			// Different text for remote player
			const std::string* textPtr{ nullptr };
			if (IsClient())
				textPtr = &m_waitingForRemotePlayerText;
			else
				textPtr = &m_waitingForPlayerLeftText;

			d2d::Window::Translate(m_waitingForPlayerLeftPosition);
			d2d::Window::DrawString(m_waitingForPlayerLeftText, m_waitingForPlayerLeftAlignment,
				m_waitingForPlayerTextStyle.size, m_waitingForPlayerTextStyle.font);
		}
		else
		{
			// Different text for remote player
			const std::string* textPtr{ nullptr };
			if (IsServer())
				textPtr = &m_waitingForRemotePlayerText;
			else
				textPtr = &m_waitingForPlayerRightText;

			d2d::Window::Translate(m_waitingForPlayerRightPosition);
			d2d::Window::DrawString(m_waitingForPlayerRightText, m_waitingForPlayerRightAlignment,
				m_waitingForPlayerTextStyle.size, m_waitingForPlayerTextStyle.font);
		}
		d2d::Window::PopMatrix();
	}

	void Game::Update(float dt)
	{
		// Update
		switch (m_state)
		{
		case GameState::WAIT_FOR_CLIENT_CONNECTION:
			UpdateWaitForClientConnection(dt);
			break;
		case GameState::CONFIRM_PLAYERS_READY:
			UpdateConfirmPlayersReady(dt);
			break;
		case GameState::COUNTDOWN:
			UpdateCountdown(dt);
			break;
		case GameState::PLAY:
			UpdatePlay(dt);
			break;
		case GameState::GAME_OVER:
			break;
		default:
			break;
		}

		// Process network input
		if (IsNetworked() && m_state != GameState::WAIT_FOR_CLIENT_CONNECTION)
			CheckMessages();

		// Process network output
		if (IsNetworked() && m_state != GameState::WAIT_FOR_CLIENT_CONNECTION)
			SendNetworkData();
	}
	void Game::SendNetworkData()
	{
		if (m_networkOutputBuffer.length > 0)
		{
			int bytesSent = SDLNet_TCP_Send(m_clientSocketTCP, m_networkOutputBuffer.bytes, m_networkOutputBuffer.length);
			if (bytesSent < m_networkOutputBuffer.length)
				throw GameException{ std::string{"Failed to send network data: "} +d2d::ToString(m_networkOutputBuffer.length) +
					" bytes attempted, " + d2d::ToString(bytesSent) + " bytes successfully sent: " + SDLNet_GetError() };
			m_networkOutputBuffer.length = 0;
		}
	}
	void Game::CheckMessages()
	{
		while (SocketReady())
		{
			ReceiveNetworkData(m_networkInputBuffer);
			if (m_networkInputBuffer.IsOverflowing())
				throw GameException{ "Buffer overflow error" };

			if (m_networkInputBuffer.length > 0)
				ProcessMessages(m_networkInputBuffer);
			if (m_networkInputBuffer.IsFull())
				throw GameException{ "Buffer full error: Increase buffer size." };
		}
	}
	bool Game::SocketReady() const
	{
		errno = 0;
		int numSocketsReady = SDLNet_CheckSockets(m_socketSet, 0);
		if (numSocketsReady == -1)
		{
			throw GameException{ std::string{"Failed to check sockets: "} +
				SDLNet_GetError() + " (errno = " + d2d::ToString(errno) + ")" };
		}
		if (!numSocketsReady)
			return false;

		errno = 0;
		if (!SDLNet_SocketReady(m_clientSocketTCP))
			throw GameException{ std::string{"Client socket not ready, even though SDLNet_CheckSockets told us a socket was ready: "} +
				SDLNet_GetError() + (errno ? " (errno = " + d2d::ToString(errno) + ")" : "") };

		return true;
	}
	void Game::ReceiveNetworkData(Buffer& data)
	{
		if (data.IsFull())
			return;

		// Try to receive data
		int byteCount = SDLNet_TCP_Recv(m_clientSocketTCP, data.FirstAvailableBytePtr(), data.BytesAvailable());

		// Check for error
		if (byteCount < 0)
			throw GameException{ std::string{"Failed to get network data: "} +SDLNet_GetError() };

		// Check for disconnection
		if (byteCount == 0)
			throw GameException{ "Connection unexpectedly terminated." };
		else
		{
			// We received something
			data.length += byteCount;
		}
	}
	void Game::ProcessMessages(Buffer& data)
	{
		int lastMessageStart{ 0 };
		int nextMessageStart{ 0 };
		do
		{
			lastMessageStart = nextMessageStart;
			nextMessageStart = ProcessMessage(data, nextMessageStart);
		} while (nextMessageStart != lastMessageStart);

		// Discard processed data
		data.MakeNewFront(nextMessageStart);
	}
	// Returns start of next message
	int Game::ProcessMessage(const Buffer& data, int first)
	{
		if (first > data.length - 1)
			return first;

		switch (data.bytes[first])
		{
			// Player Ready
		case MESSAGE_PLAYER_READY:
			if (IsServer())
				m_player2.SetReady();
			else
				m_player1.SetReady();
			return first + 1;

			// Player Scored
		case MESSAGE_PLAYER_SCORED:
			if (IsServer())
				throw GameException{ "Client should not send PLAYER_SCORED message" };
			else
			{
				// If we only have partial message, do nothing
				int availableDataBytes = data.length - first;
				int messageBytes = 3;
				if (availableDataBytes < messageBytes)
					return first;

				// Read message
				Byte newScore1{ data.bytes[first + 1] };
				Byte newScore2{ data.bytes[first + 2] };

				// Verify validity and process new score
				if (newScore1 == m_player1.GetScore() && newScore2 == m_player2.GetScore() + 1)
					m_player2.ScorePoint();
				else if (newScore1 == m_player1.GetScore() + 1 && newScore2 == m_player2.GetScore())
					m_player1.ScorePoint();
				else
					throw GameException{ std::string{"Invalid PLAYER_SCORED message: score1="} +
						d2d::ToString(m_player1.GetScore()) + " score2=" + d2d::ToString(m_player2.GetScore()) +
						" newScore1=" + d2d::ToString(newScore1) + " newScore2=" + d2d::ToString(newScore2) };

				// See if someone won, otherwise start next round
				if (m_player1.GetScore() >= SCORE_TO_WIN || m_player2.GetScore() >= SCORE_TO_WIN)
					m_state = GameState::GAME_OVER;
				else
					ResetRound();
				return first + messageBytes;
			}

			// Countdown
		case MESSAGE_COUNTDOWN_LEFT:
			if (IsServer())
				throw GameException{ "Client should not send COUNTDOWN_LEFT message" };
			else
			{
				// If we only have partial message, do nothing
				int availableDataBytes = data.length - first;
				int messageBytes = 1 + sizeof(float);
				if (availableDataBytes < messageBytes)
					return first;

				// Read message
				m_countdownSecondsLeft = data.ReadFloat(first + 1);
				return first + messageBytes;
			}

			// Puck position/velocity
		case MESSAGE_PUCK_POSITION_VELOCITY:
			if (IsServer())
				throw GameException{ "Client should not send PUCK_POSITION message" };
			else
			{
				// If we only have partial message, do nothing
				int availableDataBytes = data.length - first;
				int messageBytes = 1 + 4 * sizeof(float);
				if (availableDataBytes < messageBytes)
					return first;

				// Read position
				b2Vec2 inputVector;
				inputVector.x = data.ReadFloat(first + 1);
				inputVector.y = data.ReadFloat(first + 1 + sizeof(float));
				m_puck.SetPosition(inputVector);

				// Read velocity
				inputVector.x = data.ReadFloat(first + 1 + 2 * sizeof(float));
				inputVector.y = data.ReadFloat(first + 1 + 3 * sizeof(float));
				m_puck.SetVelocity(inputVector);

				return first + messageBytes;
			}

			// Player position
		case MESSAGE_PLAYER_POSITION:
		{
			// If we only have partial message, do nothing
			int availableDataBytes = data.length - first;
			int messageBytes = 1 + sizeof(float);
			if (availableDataBytes < messageBytes)
				return first;

			// Read message
			float newY{ data.ReadFloat(first + 1) };
			if (IsServer())
				m_player2.SetY(newY);
			else
				m_player1.SetY(newY);
			return first + messageBytes;
		}
		default:
			throw GameException{ std::string{"Invalid message code: "} +d2d::ToString(data.bytes[first]) };
		}
	}

	void Game::UpdateWaitForClientConnection(float dt)
	{
		errno = 0;
		int numSocketsReady = SDLNet_CheckSockets(m_socketSet, m_checkForClientConnectionTimeoutMilliseconds);
		if (numSocketsReady == -1)
		{
			throw GameException{ std::string{"Failed to check sockets: "} +
				SDLNet_GetError() + (errno ? " (errno = " + d2d::ToString(errno) + ")" : "") };
		}
		else if (numSocketsReady)
		{
			errno = 0;
			if (!SDLNet_SocketReady(m_serverSocketTCP))
			{
				throw GameException{ std::string{"Server socket not ready, even though SDLNet_CheckSockets told us a socket was ready: "} +
					SDLNet_GetError() + (errno ? " (errno = " + d2d::ToString(errno) + ")" : "") };
			}
			else
			{
				m_clientSocketTCP = SDLNet_TCP_Accept(m_serverSocketTCP);
				if (!m_clientSocketTCP)
				{
					SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Socket error",
						"TCP Socket indicated that a client wanted to connect, but failed to accept connection. Continuing to wait for client.", nullptr);
					return;
				}

				// Get the client's IP and port
				IPaddress* clientIPPtr;
				clientIPPtr = SDLNet_TCP_GetPeerAddress(m_clientSocketTCP);
				if (!clientIPPtr)
					throw GameException{ std::string{"SDLNet_TCP_GetPeerAddress: "} +SDLNet_GetError() };

				// Print out the client's IP and port
				d2LogInfo << "Accepted a connection from "
					<< d2d::GetIPOctetsString(*clientIPPtr) << " port " << d2d::GetPort(*clientIPPtr);

				// Replace server socket with client socket in socket set
				if (SDLNet_TCP_DelSocket(m_socketSet, m_serverSocketTCP) == -1)
					throw GameException{ std::string{"After connecting to client, failed to delete server TCP socket from socket set: "} +SDLNet_GetError() };
				if (SDLNet_TCP_AddSocket(m_socketSet, m_clientSocketTCP) == -1)
					throw GameException{ std::string{"After connecting to client, failed to add client TCP socket to socket set: "} +SDLNet_GetError() };

				// Close server socket
				SDLNet_TCP_Close(m_serverSocketTCP);
				m_serverSocketTCP = nullptr;

				// Connection made, start game
				m_state = GameState::CONFIRM_PLAYERS_READY;
			}
		}
	}
	void Game::UpdateConfirmPlayersReady(float dt)
	{
		if (m_player1.IsReady() && m_player2.IsReady())
		{
			m_countdownSecondsLeft = INITIAL_COUNTDOWN;
			m_state = GameState::COUNTDOWN;
		}
	}
	void Game::UpdateCountdown(float dt)
	{
		if (!IsClient())
			m_countdownSecondsLeft -= dt;

		if (m_countdownSecondsLeft <= 0.0f)
			m_state = GameState::PLAY;

		if (IsServer())
		{
			m_networkOutputBuffer.WriteByte(MESSAGE_COUNTDOWN_LEFT);
			m_networkOutputBuffer.WriteFloat(m_countdownSecondsLeft);
		}
	}
	void Game::UpdatePlay(float dt)
	{
		if (!IsClient())
			m_player1.Update(dt);
		if (!IsServer())
			m_player2.Update(dt);

		//if (!IsClient())
			m_puck.Update(dt, m_player1, m_player2);

		if (IsClient())
		{
			m_networkOutputBuffer.WriteByte(MESSAGE_PLAYER_POSITION);
			m_networkOutputBuffer.WriteFloat(m_player2.GetPosition().y);
		}
		else if (IsServer())
		{
			m_networkOutputBuffer.WriteByte(MESSAGE_PUCK_POSITION_VELOCITY);
			m_networkOutputBuffer.WriteFloat(m_puck.GetPosition().x);
			m_networkOutputBuffer.WriteFloat(m_puck.GetPosition().y);
			m_networkOutputBuffer.WriteFloat(m_puck.GetVelocity().x);
			m_networkOutputBuffer.WriteFloat(m_puck.GetVelocity().y);

			m_networkOutputBuffer.WriteByte(MESSAGE_PLAYER_POSITION);
			m_networkOutputBuffer.WriteFloat(m_player1.GetPosition().y);
		}

		if (m_puck.Scored())
		{
			if (IsServer())
			{
				m_networkOutputBuffer.WriteByte(MESSAGE_PLAYER_SCORED);
				m_networkOutputBuffer.WriteByte((Byte)m_player1.GetScore());
				m_networkOutputBuffer.WriteByte((Byte)m_player2.GetScore());
			}

			if (m_player1.GetScore() >= SCORE_TO_WIN || m_player2.GetScore() >= SCORE_TO_WIN)
				m_state = GameState::GAME_OVER;
			else
				ResetRound();
		}
	}

	//+--------------------------------\--------------------------------------
	//|			   Player			   |
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
	void Player::ScorePoint()
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
		d2d::Clamp(factor, { -1.0f, 1.0f });
		m_movementFactor = factor;
	}
	void Player::SetY(float newY)
	{
		if(newY < GAME_RECT.lowerBound.y || newY + PLAYER_SIZE.y > GAME_RECT.upperBound.y)
			throw GameException{ std::string{"Player::SetY: Out of bounds: y="} + d2d::ToString(newY) };
		m_position.y = newY;
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
			const float MIN_Y{ GAME_RECT.lowerBound.y };
			const float MAX_Y{ GAME_RECT.upperBound.y - PLAYER_SIZE.y };
			d2d::Clamp(m_position.y, { MIN_Y, MAX_Y });
		}
	}
	void Player::Init(Side newSide)
	{
		m_score = 0;
		m_side = newSide;
		ResetRound();
	}
	void Player::ResetRound()
	{
		if (m_side == Side::LEFT)
			m_position.x = GAME_RECT.lowerBound.x;
		else
			m_position.x = GAME_RECT.upperBound.x - PLAYER_SIZE.x;
		m_position.y = GAME_RECT.GetCenter().y - (0.5f * PLAYER_SIZE.y);

		m_movementFactor = 0.0f;

		m_isReady = false;
	}
	void Player::Draw() const
	{
		d2d::Window::SetColor(PLAYER_COLOR);
		d2d::Window::DrawRect({ m_position, m_position + PLAYER_SIZE }, true);
	}

	//+--------------------------------\--------------------------------------
	//|				Puck	    	   |
	//\--------------------------------/--------------------------------------
	void Puck::ResetRound()
	{
		m_position = GAME_RECT.GetCenter() - 0.5f * PUCK_SIZE;

		if (IsClient())
			m_velocity = b2Vec2_zero;
		else
		{
			// Randomize puck angle
			d2d::SeedRandomNumberGenerator();
			float halfAngleRange = BOUNCE_ANGLE_RANGE * 0.5f;
			float angle = d2d::RandomFloat({ -halfAngleRange, halfAngleRange });
			if (d2d::RandomBool())
				angle += d2d::PI;

			m_velocity.Set(cos(angle), sin(angle));
			m_velocity *= INITIAL_PUCK_SPEED;
		}

		m_gotPastPlayer = false;
		m_scored = false;
	}
	bool Puck::Scored() const
	{
		return m_scored;
	}
	void Puck::Update(float dt, Player& player1, Player& player2)
	{
		UpdatePosition(dt);
		if (!IsClient())
		{
			if (!m_gotPastPlayer)
			{
				HandlePlayerCollision(player1);
				HandlePlayerCollision(player2);
			}
			HandleGoal(player1);
			HandleGoal(player2);
		}
	}
	void Puck::UpdatePosition(float dt)
	{
		const float MIN_Y{ GAME_RECT.lowerBound.y };
		const float MAX_Y{ GAME_RECT.upperBound.y - PUCK_SIZE.y };

		m_position += dt * m_velocity;

		// Bounce ball off top wall
		if (m_position.y >= MAX_Y)
		{
			const float PROTRUSION_Y{ m_position.y - MAX_Y };
			m_position.y = MAX_Y - PROTRUSION_Y;
			m_velocity.y = -m_velocity.y;
		}

		// Bounce ball off bottom wall
		if (m_position.y <= MIN_Y)
		{
			const float PROTRUSION_Y{ MIN_Y - m_position.y };
			m_position.y = MIN_Y + PROTRUSION_Y;
			m_velocity.y = -m_velocity.y;
		}
	}
	const b2Vec2& Puck::GetPosition() const
	{
		return m_position;
	}
	const b2Vec2& Puck::GetVelocity() const
	{
		return m_velocity;
	}
	void Puck::SetPosition(const b2Vec2& position)
	{
		m_position = position;
	}
	void Puck::SetVelocity(const b2Vec2& velocity)
	{
		m_velocity = velocity;
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
		if (overlapX >= 0.0f)
		{
			// If the ball is traveling towards the player
			if ((m_velocity.x < 0.0f && player.GetSide() == Side::LEFT) ||
				(m_velocity.x > 0.0f && player.GetSide() == Side::RIGHT))
			{
				// If there is separation along the Y-axis
				if (m_position.y > player.GetPosition().y + PLAYER_SIZE.y ||
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
					float speed = sqrt(pow(m_velocity.x, 2) + pow(m_velocity.y, 2));

					// Simulate curved paddle
					{
						// Determine how far off the player's center the ball hit
						float hitRangeBottom = player.GetPosition().y - PUCK_SIZE.y;
						float hitRangeTop = player.GetPosition().y + PLAYER_SIZE.y;
						float hitRangeLength = hitRangeTop - hitRangeBottom;
						float puckPercentFromBottom = (m_position.y - hitRangeBottom) / hitRangeLength;

						// Convert to a range of [-1,1] 
						float puckPercentFromCenterToEdge = puckPercentFromBottom * 2.0f - 1.0f;
						d2d::Clamp(puckPercentFromCenterToEdge, { -1.0f, 1.0f });

						// Change angle based on how far off center it hit the player
						float maxAngleChange = d2d::PI / 4.0f;
						float angleChange = maxAngleChange * puckPercentFromCenterToEdge;
						if (player.GetSide() == Side::LEFT)
							angleOut += angleChange;
						else
							angleOut -= angleChange;
						d2d::WrapRadians(angleOut);
					}

					// Clamp angle to range
					float angleLimitTop, angleLimitBottom;
					float halfAngleRange = BOUNCE_ANGLE_RANGE / 2.0f;
					if (player.GetSide() == Side::LEFT)
					{
						angleLimitTop = halfAngleRange;
						angleLimitBottom = d2d::TWO_PI - halfAngleRange;
						if (angleOut > angleLimitTop&& angleOut < d2d::PI)
							angleOut = angleLimitTop;
						else if (angleOut < angleLimitBottom && angleOut >= d2d::PI)
							angleOut = angleLimitBottom;
					}
					else
					{
						angleLimitTop = d2d::PI - halfAngleRange;
						angleLimitBottom = d2d::PI + halfAngleRange;
						if (angleOut < angleLimitTop)
							angleOut = angleLimitTop;
						else if (angleOut > angleLimitBottom)
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
		if (m_scored)
			return;

		if ((player.GetSide() == Side::LEFT && m_position.x + PUCK_SIZE.x >= GAME_RECT.upperBound.x) ||
			(player.GetSide() == Side::RIGHT && m_position.x <= GAME_RECT.lowerBound.x))
		{
			player.ScorePoint();
			m_scored = true;
		}
	}
	void Puck::Draw() const
	{
		d2d::Window::SetColor(PUCK_COLOR);
		d2d::Window::DrawRect({ m_position, m_position + PUCK_SIZE }, true);
	}
}