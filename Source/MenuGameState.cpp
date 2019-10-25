#include "pch.h"
#include "MenuGameState.h"
#include "PlayGameState.h"
#include "d2d.h"

namespace Pong
{
	MenuGameState::MenuGameState()
	{}
	MenuGameState::~MenuGameState()
	{}
	void MenuGameState::HandleEvent(const SDL_Event& event)
	{
		if(event.type == SDL_KEYDOWN)
		{
			switch(event.key.keysym.sym)
			{
			case SDLK_ESCAPE:
				m_quit = true;
				return;
			case SDLK_RETURN:
				if(m_highlight1PStartButton)
				{
					// Go to single player mode
					m_goPlay = true;
					m_twoPlayerGame = false;
					return;
				}
				else if(m_highlight2PStartButton)
				{
					// Go to two player mode
					m_goPlay = true;
					m_twoPlayerGame = true;
					return;
				}
				else if(m_highlightQuitButton)
				{
					m_quit = true;
					return;
				}
				break;
			case SDLK_UP:
				// If 2nd button highlighted
				if(m_highlight2PStartButton)
				{
					// Go to 1st button
					m_highlight2PStartButton = false;
					m_highlight1PStartButton = true;
				}
				// If 3rd button highlighted
				else if(m_highlightQuitButton)
				{
					// Go to 2nd button
					m_highlightQuitButton = false;
					m_highlight2PStartButton = true;
				}
				// If no buttons highlighted
				else if(!m_highlightQuitButton)
				{
					// Go to 1st button
					m_highlight1PStartButton = true;
				}
				break;
			case SDLK_DOWN:
				// If 1st button highlighted
				if(m_highlight1PStartButton)
				{
					// Go to 2nd button
					m_highlight1PStartButton = false;
					m_highlight2PStartButton = true;
				}
				// If 2nd button highlighted
				else if(m_highlight2PStartButton)
				{
					// Go to 3rd button
					m_highlight2PStartButton = false;
					m_highlightQuitButton = true;
				}
				// If no buttons highlighted
				else if(!m_highlightQuitButton)
				{
					// Go to 1st button
					m_highlight1PStartButton = true;
				}
				break;
			default: break;
			}
		}
		else if(event.type == SDL_MOUSEMOTION ||
			event.type == SDL_MOUSEBUTTONDOWN)
		{
			//															//
			// Did we click on, or are we hovering over a menu button?	//
			//															//

			// Calculate mouse screen position
			int x, y;
			Uint32 mouseState = SDL_GetMouseState(&x, &y);
			b2Vec2 resolution{ d2d::Renderer::GetScreenResolution() };
			b2Vec2 mousePosition{ x / resolution.x, (1 - (y / resolution.y)) };

			// 1P Start Button vs. Mouse
			d2d::Rect buttonRect;
			MakeButtonRect(m_menuStart1, buttonRect);
			m_highlight1PStartButton = false;
			if(buttonRect.Contains(mousePosition))
			{
				m_highlight1PStartButton = true;
				if(event.type == SDL_MOUSEBUTTONDOWN)
				{
					// Go to single player mode
					m_goPlay = true;
					m_twoPlayerGame = false;
					return;
				}
			}

			// 2P Start Button vs. Mouse
			MakeButtonRect(m_menuStart2, buttonRect);
			m_highlight2PStartButton = false;
			if(buttonRect.Contains(mousePosition))
			{
				m_highlight2PStartButton = true;
				if(event.type == SDL_MOUSEBUTTONDOWN)
				{
					// Go to two player mode
					m_goPlay = true;
					m_twoPlayerGame = false;
					return;
				}
			}

			// Quit Button vs. Mouse
			MakeButtonRect(m_menuQuit, buttonRect);
			m_highlightQuitButton = false;
			if(buttonRect.Contains(mousePosition))
			{
				m_highlightQuitButton = true;
				if(event.type == SDL_MOUSEBUTTONDOWN)
				{
					m_quit = true;
					return;
				}
			}
		}
	}
	std::shared_ptr<d2d::GameState> MenuGameState::Update(float deltaSeconds)
	{
		if(m_quit)
			return nullptr;

		if(m_goPlay)
			return std::make_shared<PlayGameState>(m_twoPlayerGame);	

		return shared_from_this();
	}
	void MenuGameState::MakeButtonRect(const d2d::TextBlock& text,
									  d2d::Rect& rect)
	{
		const static b2Vec2 menuButtonDimensions{ 0.35f, 0.054f };
		//const static b2Vec2 menuButtonOffset{ 0.0f, -0.005f };
		//const static b2Vec2 menuButtonOffset{ 0.0f, 0.010f };
		const static b2Vec2 menuButtonOffset{ 0.0f, 0.008f };
		//const static b2Vec2 menuButtonOffset{ 0.0f, 0.020f };

		// Calculate button area
		rect.SetCenter(text.screenPosition + menuButtonOffset, menuButtonDimensions);
	}
	void MenuGameState::DrawButton(const d2d::TextBlock& textBlock, bool highlight)
	{
		d2d::Rect buttonRect;
		MakeButtonRect(textBlock, buttonRect);
		if (highlight)
		{
			d2d::Renderer::SetColor(d2d::Color(1.0f, 1.0f, 1.0f, 0.3f));
			d2d::Renderer::DrawRect(buttonRect, true);
		}
		d2d::Renderer::SetColor(d2d::Color(0.5f, 0.5f, 0.5f, 0.5f));
		d2d::Renderer::DrawRect(buttonRect, false);
		d2d::Renderer::DrawTextBlock(textBlock);
	}
	void MenuGameState::Draw()
	{
		d2d::Renderer::SetShowCursor(true);
		d2d::Renderer::SetCameraScreenMode();
		d2d::Renderer::DrawTextBlock(m_menuTitle);
		DrawButton(m_menuStart1, m_highlight1PStartButton);
		DrawButton(m_menuStart2, m_highlight2PStartButton);
		DrawButton(m_menuQuit, m_highlightQuitButton);
	}
}
