#include "InputManager.h"

#include <iostream>
#include <cppgamelib/exceptions/EngineException.h>

#include "Console.h"
#include "ConsoleBackspacePressedEvent.h"
#include "ConsoleDeletePressedEvent.h"
#include "ConsoleLeftPressedEvent.h"
#include "ConsolePageDownPressedEvent.h"
#include "ConsolePageUpPressedEvent.h"
#include "ConsoleReturnPressedEvent.h"
#include "ConsoleRightPressedEvent.h"
#include "ConsoleTextReceivedEvent.h"
#include "ConsoleToggledEvent.h"

bool graveWasPressed = false;

gamelib::ControllerMoveEvent::KeyState GetKeyState(const SDL_Event& e)
{
	return e.type == SDL_KEYDOWN 
						? gamelib::ControllerMoveEvent::KeyState::Pressed
						: gamelib::ControllerMoveEvent::KeyState::Released;
}

void InputManager::Sample(const unsigned long deltaMs)
{
	if(gameCommands == nullptr)
	{
		THROW(12, "No game commands set on the input manager. No input will be sampled.", "InputManager");
	}
	Console console;
	SDL_Event e;
	while(SDL_PollEvent(&e))
	{
		if (inputMode == InputMode::Console)
		{
			if (e.type == SDL_TEXTINPUT)
			{
				// Send the character to the Console
				auto event = std::make_shared<ConsoleTextReceivedEvent>(e.text.text);
				gamelib::EventManager::Get()->RaiseEvent(event, this);
			}
		}

		if (e.type == SDL_QUIT) 
		{ 
			gameCommands->Quit(verbose); return;
		}

		if (e.key.repeat == 0) // we ignore key repeats
		{
			switch(e.key.keysym.sym)
			{
				case SDLK_w:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveUp(verbose, GetKeyState(e));
					}
					break;
				case SDLK_UP:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveUp(verbose, GetKeyState(e));
					}
					break;
		        case SDLK_s:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveDown(verbose, GetKeyState(e));
					}
					break;
		        case SDLK_DOWN:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveDown(verbose, GetKeyState(e));
					}
		            break;
		        case SDLK_a:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveLeft(verbose, GetKeyState(e));
					}
					break;
		        case SDLK_LEFT:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveLeft(verbose, GetKeyState(e));
					}
					else if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleLeftPressedEvent>(), this);
					}
		            break;
		        case SDLK_d:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveRight(verbose, GetKeyState(e));
					}
					break;
		        case SDLK_RIGHT:
					if (inputMode == InputMode::Game)
					{
						gameCommands->MoveRight(verbose, GetKeyState(e));
					}
					else if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleRightPressedEvent>(), this);
					}
		            break;
				case SDLK_BACKSPACE:
					if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleBackspacePressedEvent>(), this);
					}
					break;
				case SDLK_DELETE:
					if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleDeletePressedEvent>(), this);
					}
					break;
				case SDLK_RETURN:
					if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleReturnPressedEvent>(), this);
					}
					break;
				case SDLK_PAGEUP:
					if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsolePageUpPressedEvent>(), this);
					}
					break;
				case SDLK_PAGEDOWN:
					if (inputMode == InputMode::Console)
					{
						gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsolePageDownPressedEvent>(), this);
					}
					break;
            default: ;
			}
		}
	}

	// Input -> Game Commands

	const auto keyState = SDL_GetKeyboardState(nullptr);

	if (keyState[SDL_SCANCODE_Q] || keyState[SDL_SCANCODE_ESCAPE]) { gameCommands->Quit(verbose); }
	if (GetInputMode() == InputMode::Game)
	{
		if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose);  }
		if (keyState[SDL_SCANCODE_P]) { gameCommands->PingGameServer(0);  }
		if (keyState[SDL_SCANCODE_R]) { gameCommands->ReloadSettings(verbose); }
		if (keyState[SDL_SCANCODE_N]) { gameCommands->StartNetworkLevel();  }
		if (keyState[SDL_SCANCODE_SPACE]) { gameCommands->Fire(verbose);  }
		if (keyState[SDL_SCANCODE_0]) { gameCommands->ToggleMusic(false);  }
	}
	bool graveIsDown = keyState[SDL_SCANCODE_GRAVE];

	if (graveIsDown && !graveWasPressed)
	{
		if (GetInputMode() == InputMode::Game)
		{
			SetInputMode(InputMode::Console);

			gamelib::EventManager::Get()->RaiseEvent(std::make_shared<ConsoleToggleEvent>(), this);

			SDL_StartTextInput();
			std::cout << "Text input is in game console mode\n";
		}
		else if (GetInputMode() == InputMode::Console)
		{
			SetInputMode(InputMode::Game);
			SDL_StopTextInput();
			std::cout << "Text input is in game mode\n";
		}
	}

	graveWasPressed = graveIsDown;


}

void InputManager::SetInputMode(const InputMode mode)
{
	inputMode = mode;
}

InputManager::InputMode InputManager::GetInputMode() const
{
	return inputMode;
}

std::string InputManager::GetSubscriberName()
{
	return "InputManager";
}

std::vector<std::shared_ptr<gamelib::Event>> InputManager::HandleEvent(const std::shared_ptr<gamelib::Event> &evt,
                                                                       unsigned long deltaMs)
{
	return {};
}
