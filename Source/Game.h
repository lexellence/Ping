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
#include "NetworkDef.h"
#include "GameInitSettings.h"
#include "Exceptions.h"

namespace Pong
{
	const d2d::Rect GAME_RECT{ b2Vec2_zero, { 400.0f, 200.0f } };
	const b2Vec2 PLAYER_SIZE{ 0.02f * GAME_RECT.GetWidth(), 
							  0.23f * GAME_RECT.GetHeight() };
	const b2Vec2 PUCK_SIZE{ 0.01f * GAME_RECT.GetWidth(), 
							0.01f * GAME_RECT.GetWidth() };

	const unsigned SCORE_TO_WIN{ 3u };
	const float INITIAL_COUNTDOWN{ 3.0f };
	const float INITIAL_PUCK_SPEED{ 0.35f * GAME_RECT.GetWidth() };
	const float START_ANGLE{ 0.2f * d2d::PI };
	const float BOUNCE_ANGLE_RANGE{ 0.72f * d2d::PI };
	const float MAX_CURVATURE_ANGLE_CHANGE = d2d::PI / 4.0f;

	const float PUCK_SPEED_BOOST_MULTIPLIER{ 1.07f };
	//const float PLAYER_MAX_SPEED{ 0.9f * INITIAL_PUCK_SPEED };
	const float PLAYER_MAX_SPEED{ 250.0f };

	const d2d::Color BOUNDARY_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color NET_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color PLAYER_COLOR{ 0.8f, 0.3f, 0.3f };
	const d2d::Color PUCK_COLOR{ 0.8f, 0.3f, 0.3f };

	const float SLIGHTLY_LESS_THAN_ONE{ 0.99999f };

	const Uint32 TCP_SERVER_CHECK_FOR_CLIENT_CONNECTION_TIMEOUT_MILLISECONDS{ 15 };
	const float MAX_TIME_TO_WAIT_FOR_CLIENT_UDP{ 10.0f };

	using Byte = Uint8;
	const Byte UDP_MESSAGE_INIT_CLIENT_TO_SERVER = 100;
	const Byte TCP_MESSAGE_CLIENT_INIT_UDP_TIMEOUT = 101;
	const Byte TCP_MESSAGE_PLAYER_READY = 102;
	const Byte TCP_MESSAGE_PLAYER_SCORED = 103;
	const Byte UDP_MESSAGE_COUNTDOWN_LEFT = 104;
	const Byte TCP_MESSAGE_COUNTDOWN_OVER = 105;
	const Byte UDP_MESSAGE_PUCK_POSITION_VELOCITY = 106;
	const Byte UDP_MESSAGE_PLAYER_Y = 107;
	const Byte TCP_MESSAGE_PLAYER_QUIT = 108;

	const int BUFFER_SIZE{ 100 };
	using ByteBuffer = Byte[BUFFER_SIZE];
	struct Buffer
	{
		ByteBuffer bytes;
		int length{ 0 };

		void Clear()
		{
			length = 0;
		}
		bool IsOverflowing() const
		{
			return length > BUFFER_SIZE;
		}
		bool IsFull() const
		{
			return length >= BUFFER_SIZE;
		}
		Byte* FirstBytePtr()
		{
			return bytes;
		}
		Byte* FirstAvailableBytePtr()
		{
			return &(bytes[length]);
		}
		int BytesAvailable() const
		{
			return BUFFER_SIZE - length;
		}
		void WriteByte(Byte value)
		{
			if(length + sizeof(Byte) > BUFFER_SIZE)
				throw GameException{ "Buffer::WriteByte: Overflow: Increase buffer size" };
			bytes[length] = value;
			++length;
		}
		void WriteUInt(unsigned value)
		{
			static_assert(sizeof(Uint32) == sizeof(unsigned));
			if(length + sizeof(unsigned) > BUFFER_SIZE)
				throw GameException{ "Buffer::WriteUInt: Overflow: Increase buffer size" };
			SDLNet_Write32(value, &bytes[length]);
			length += sizeof(Uint32);
		}
		unsigned ReadUInt(int index) const
		{
			static_assert(sizeof(Uint32) == sizeof(unsigned));
			if(index < 0 || index + (int)sizeof(unsigned) > length)
				throw GameException{ "Buffer::ReadUInt: out of range" };
			return SDLNet_Read32(&bytes[index]);
		}
		void WriteFloat(float value)
		{
			static_assert(sizeof(Uint32) == sizeof(float));
			if(length + sizeof(float) > BUFFER_SIZE)
				throw GameException{ "Buffer::WriteFloat: Overflow: Increase buffer size" };
			union { float f; Uint32 i; } u;
			u.f = value;
			SDLNet_Write32(u.i, &bytes[length]);
			length += sizeof(Uint32);
		}
		float ReadFloat(int index) const
		{
			static_assert(sizeof(Uint32) == sizeof(float));
			if(index < 0 || index + (int)sizeof(float) > length)
				throw GameException{ "Buffer::ReadFloat: out of range" };
			union { float f; Uint32 i; } u;
			u.i = SDLNet_Read32(&bytes[index]);
			return u.f;
		}
		void MakeNewFront(int newStartIndex)
		{
			if(newStartIndex < 0 || newStartIndex > length)
				throw GameException{ "Buffer::MakeNewFront: out of range" };
			if(newStartIndex == 0)
				return;
			int newLength{ length - newStartIndex };
			if(newLength > 0)
				memmove(FirstBytePtr(), &bytes[newStartIndex], newLength);
			length = newLength;
		}
	};

	enum class Side
	{
		LEFT,
		RIGHT
	};
	class Player
	{
	public:
		void Init(Side newSide);
		void ResetRound();
		unsigned GetScore() const;
		void ScorePoint();
		Side GetSide() const;
		const b2Vec2& GetPosition() const;
		bool IsReady() const;
		void SetReady();
		void SetMovementFactor(float factor);
		void SetY(float newY);
		void Update(float dt);
		void Draw() const;

	private:
		unsigned m_score;
		Side m_side;

		b2Vec2 m_position;	// Lower-left corner
		float m_movementFactor;
		bool m_isReady;
	};

	struct Puck
	{
	public:
		void ResetRound();
		bool Scored() const;
		void Update(float dt, Player& player1, Player& player2);
		const b2Vec2& GetPosition() const;
		const b2Vec2& GetVelocity() const;
		void SetPosition(const b2Vec2& position);
		void SetVelocity(const b2Vec2& velocity);
		void Draw() const;

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
		void Init();
		void Update(float dt);
		void Draw() const;

		~Game();
		void OnQuit();

		void Player1PressedAButton();
		void Player2PressedAButton();
		void SetPlayer1MovementFactor(float factor); // [-1.0,1.0]
		void SetPlayer2MovementFactor(float factor); // [-1.0,1.0]

	private:
		void ResetRound();

		void UpdateUDPInit(float dt);

		void UpdateWaitForClientTCPConnection(float dt);
		void UpdateConfirmPlayersReady(float dt);
		void UpdateCountdown(float dt);
		void UpdatePlay(float dt);

		void DrawPlayerResult(Side playerSide, bool isWinner) const;
		void DrawCountdown() const;
		void DrawScore(const Player& player) const;
		void DrawWaitingMessage(const Player& player) const;

		void InitNetwork();
		void CloseNetwork();
		void SendNetworkData();
		void CheckMessages();
		bool TCPReady() const;
		void GetDataTCP();
		bool GetDataUDP();

		void ProcessMessagesTCP(Buffer& data);
		// Returns start of next message
		int ProcessMessageTCP(const Buffer& data, int first);

		void ProcessMessagesUDP(Buffer& data);
		// Returns start of next message
		int ProcessMessageUDP(const Buffer& data, int first);

		// Game
		enum class GameState
		{
			WAIT_FOR_CLIENT_TCP_CONNECTION,
			CONFIRM_PLAYERS_READY,
			COUNTDOWN,
			PLAY,
			GAME_OVER,
			GAME_OVER_DEFAULT_WIN
		} m_state;
		Player m_player1, m_player2;
		Puck m_puck;
		float m_countdownSecondsLeft;

		// Network
		NetworkDef m_networkSettings;
		TCPsocket m_clientSocketTCP{ nullptr };
		UDPsocket m_clientSocketUDP{ nullptr };
		SDLNet_SocketSet m_socketSet{ nullptr };
		Buffer m_inputBufferTCP;
		Buffer m_outputBufferTCP;
		Buffer m_inputBufferUDP;
		Buffer m_outputBufferUDP;
		unsigned m_nextUDPSequenceNum{ 0 };
		unsigned m_lastUDPPlayerSequenceNum{ 0 };
		UDPpacket* m_inputUDPPacketPtr{ nullptr };
		UDPpacket* m_outputUDPPacketPtr{ nullptr };
		bool m_serverWaitingForClientUDP{ false };
		bool m_serverTimedOutWaitingForClientUDP{ false };

		// For server use only
		TCPsocket m_serverSocketTCP{ nullptr };
		float m_timeWaitingForClientUDP{ 0.0f };

		// For client use only
		unsigned m_lastUDPCountdownSequenceNum{ 0 };
		unsigned m_lastUDPPuckSequenceNum{ 0 };

		// Assets
		d2d::FontReference m_orbitronLightFont{ "Fonts\\OrbitronLight.otf" };

		// Text
		const b2Vec2 edgeGapPercent{ 0.015f, 0.015f };

		// FPS text
		const b2Vec2 m_fpsPosition{ GAME_RECT.GetPointAtPercent(b2Vec2{1.0f, 1.0f } - edgeGapPercent) };
		const d2d::Alignment m_fpsAlignment{ d2d::Alignment::RIGHT_TOP };
		const d2d::TextStyle m_fpsTextStyle{ m_orbitronLightFont, { 1.0f, 1.0f, 0.0f }, 0.05f * GAME_RECT.GetHeight() };

		// Score text
		const b2Vec2 m_scoreLeftPosition{ GAME_RECT.GetPointAtPercent({ 0.5f - edgeGapPercent.x, 1.0f - edgeGapPercent.y }) };
		const d2d::Alignment m_scoreLeftAlignment{ d2d::Alignment::RIGHT_TOP };
		
		const b2Vec2 m_scoreRightPosition{ GAME_RECT.GetPointAtPercent({ 0.5f + edgeGapPercent.x, 1.0f - edgeGapPercent.y }) };
		const d2d::Alignment m_scoreRightAlignment{ d2d::Alignment::LEFT_TOP };
		
		const d2d::TextStyle m_scoreTextStyle{ m_orbitronLightFont, { 1.0f, 1.0f, 0.0f }, 0.05f * GAME_RECT.GetHeight() };

		// Waiting for client text
		const std::string m_waitingForClientText{ "Waiting for client to connect..." };

		// Waiting for player text
		const std::string m_waitingForRemotePlayerText{ "Waiting for player to press a button..." };

		const std::string m_waitingForPlayerLeftText{ "Press W or S when ready" };
		const b2Vec2 m_waitingForPlayerLeftPosition{ GAME_RECT.GetPointAtPercent({ 0.25f, 0.5f }) };
		const d2d::Alignment m_waitingForPlayerLeftAlignment{ d2d::Alignment::CENTERED };

		const std::string m_waitingForPlayerRightText{ "Press UP or DOWN when ready" };
		const b2Vec2 m_waitingForPlayerRightPosition{ GAME_RECT.GetPointAtPercent({ 0.75f, 0.5f }) };
		const d2d::Alignment m_waitingForPlayerRightAlignment{ d2d::Alignment::CENTERED };

		const d2d::TextStyle m_waitingForPlayerTextStyle{ m_orbitronLightFont, { 1.0f, 1.0f, 0.0f }, 0.05f * GAME_RECT.GetHeight() };

		// Countdown text
		const b2Vec2 m_countdownPosition{ GAME_RECT.GetCenter() };
		const d2d::Alignment m_countdownAlignment{ d2d::Alignment::CENTERED };
		const d2d::TextStyle m_countdownTextStyle{ m_orbitronLightFont, { 1.0f, 1.0f, 0.0f }, 0.20f * GAME_RECT.GetHeight() };

		// Result text
		const std::string m_playerWinsText{ "You win!" };
		const std::string m_playerLosesText{ "You lose!" };
		const b2Vec2 m_playerResultLeftPosition{ GAME_RECT.GetPointAtPercent({ 0.25f, 0.5f }) };
		const b2Vec2 m_playerResultRightPosition{ GAME_RECT.GetPointAtPercent({ 0.75f, 0.5f }) };
		const d2d::Alignment m_playerResultLeftAlignment{ d2d::Alignment::CENTERED };
		const d2d::Alignment m_playerResultRightAlignment{ d2d::Alignment::CENTERED };
		const d2d::TextStyle m_playerWinsTextStyle{ m_orbitronLightFont, { 0.1f, 0.9f, 0.1f }, 0.10f * GAME_RECT.GetHeight() };
		const d2d::TextStyle m_playerLosesTextStyle{ m_orbitronLightFont, { 0.6f, 0.1f, 0.1f }, 0.10f * GAME_RECT.GetHeight() };
	};
}
