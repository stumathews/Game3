#pragma once
#include "Console.h"
#include "GameCommands.h"
#include "input/IInputManager.h"

class InputManager final : public gamelib::IInputManager, gamelib::EventSubscriber
{
public:

	enum class InputMode
	{
		// Process input mean for the game
		Game,

		// Process input meant for the game console
		Console
	};

	explicit InputManager(std::shared_ptr<GameCommands> gameCommands, const bool verbose)
		: inputMode(InputMode::Game), gameCommands(std::move(gameCommands)), verbose(verbose)
	{
	}

	void Sample(unsigned long deltaMs) override;

	void SetInputMode(InputMode mode);
	[[nodiscard]] InputMode GetInputMode() const;

	std::string GetSubscriberName() override;;

	std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(const std::shared_ptr<gamelib::Event> &evt,
		unsigned long deltaMs) override;

private:
	InputMode inputMode;
	std::shared_ptr<GameCommands> gameCommands;
	const bool verbose;
};

