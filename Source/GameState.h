#ifndef GAMESTATE_H
#define GAMESTATE_H

namespace d2d
{
	class GameState : public std::enable_shared_from_this<GameState>
	{
	public:
		virtual void HandleEvent(const SDL_Event& event) = 0;
		virtual std::shared_ptr<d2d::GameState> Update(float deltaSeconds) = 0;
		virtual void Draw() = 0;
	protected:

	};
}
#endif //GAMESTATE_H

