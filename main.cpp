//#include "SDL.h"
#include <vector>
#include <cppgamelib\file\Logger.h>
#include <cppgamelib\file\SettingsManager.h>
#include <cppgamelib\file/TextFile.h>
#include <cppgamelib\structure/GameStructure.h>
#include <cppgamelib\structure/FixedStepGameLoop.h>
#include <cppgamelib/objects/GameWorldData.h>
#include <direct.h>
#include <GameData.h>
#include <GameDataManager.h>
#include <iostream>
#include <common/Common.h>
#include <events/AddGameObjectToCurrentSceneEvent.h>
#include <events/PlayerMovedEvent.h>
#include <events/UpdateProcessesEvent.h>
#include <Logging/ErrorLogManager.h>
#include <testlib/messages.h>
#include <mazer/Enemy.h>
#include <mazer/Room.h>
#include <net/NetworkManager.h>
#include <scene/SceneManager.h>
#include <cppgamelib/events/AddGameObjectToCurrentSceneEvent.h>
#include <cppgamelib/events/UpdateAllGameObjectsEvent.h>
#include <direct.h>

#include "LevelManager.h"
#include <mazer/EnemyMovedEvent.h>

using namespace std;

using namespace std;
using namespace gamelib;

typedef SettingsManager Settings;

namespace
{
	/*
		##   ##    ##     ### ##   ### ###  ### ##            ## ##    ### ##
		## ##      ##    ##  ##    ##  ##   ##  ##           ##  ##    ##  ##
		# ### #   ## ##      ##     ##       ##  ##               ##    ##  ##
		## # ##   ##  ##    ##      ## ##    ## ##               ##     ##  ##
		##   ##   ## ###   ##       ##       ## ##              ##      ##  ##
		##   ##   ##  ##  ##  ##    ##  ##   ##  ##            #   ##   ##  ##
		##   ##  ###  ##  # ####   ### ###  #### ##           ######   ### ##
	*/

	static void InitializeGameSubSystems(GameStructure& gameStructure);
	static void PrepareFirstLevel();
	static shared_ptr<FixedStepGameLoop> CreateGameLoopStrategy();
	static void GetInput(unsigned long deltaMs);
	static void SetupEventTap();

	void PrepareFirstLevel()
	{
		const auto isSinglePlayerGame = mazer::GameData::Get()->IsSinglePlayerGame();

		if (isSinglePlayerGame)
		{
			auto _ = LevelManager::Get()->ChangeLevel(1);

			if (Settings::Bool("global", "createAutoLevel"))
			{
				LevelManager::Get()->CreateAutoLevel();
			}
			else
			{
				// Get the name of the level file for the first level
				const auto firstLevelFilePath = Settings::String("global", "level1FileName");

				LevelManager::Get()->CreateLevel(firstLevelFilePath);
			}
		}

		Logger::Get()->LogThis(!isSinglePlayerGame
			? NetworkManager::Get()->IsGameServer()
			? "Waiting for network players. Press 'n' to start network game."
			: "Waiting for the server to start the network game to begin."
			: "Creating Single player level...");
	}

	void InitializeGameSubSystems(GameStructure& gameStructure)
	{
		constexpr auto screenWidth = 0; // 0 will mean it will get read from the settings file
		constexpr auto screenHeight = 0; // 0 will mean it will get read from the settings file
		constexpr auto windowTitle = "Mazer2D!";
		constexpr auto resourcesFilePath = "data\\Resources.xml";
		constexpr auto sceneFolderPath = "data\\";

		// Initialize network subsystem
		const auto isNetworkGame = SettingsManager::Get()->GetBool("global", "isNetworkGame");
		
		mazer::GameDataManager::Get()->Initialize(isNetworkGame);

		// Initialize the logging system
		ErrorLogManager::GetErrorLogManager()->Create("GameErrors.txt");

		// Initialize game structure
		const auto isGameStructureInitialized = gameStructure.Initialize(
			screenWidth, screenHeight, windowTitle, resourcesFilePath,
			sceneFolderPath);

		// Initialize level manager
		const auto isLevelManagerInitialized = LevelManager::Get()->Initialize();

		const auto anyInitFailures = !IsSuccess(isGameStructureInitialized, "Successfully initialized game structure.") ||
			!IsSuccess(isLevelManagerInitialized, "Successfully initialized Level Manager...");

		// Check for any initialization failures
		if (anyInitFailures)
		{
			const auto verboseLoggingEnabled = Settings::Bool("global", "verbose");

			LogMessage("Game subsystem initialization failed.", verboseLoggingEnabled, true);

			THROW(12, "There was a problem initializing the game subsystems", "Initialize Subsystems");
		}
	}

	static void Update(const unsigned long deltaMs)
	{
		// Process all pending events
		EventManager::Get()->ProcessAllEvents(deltaMs);

		// Send update event - will dispatch events to subscribers who have subscribed to game object 'update' event 
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateAllGameObjectsEvent(), deltaMs);

		// Send update processes event - will dispatch event to processes
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateUpdateProcessesEvent(), deltaMs);
	}

	static void Draw()
	{
		// Time-sensitive, skip queue. Draws the current scene
		EventManager::Get()->DispatchEventToSubscriber(EventFactory::Get()->CreateGenericEvent(DrawCurrentSceneEventId, "Game"), 0UL);
	}

	void GetInput(const unsigned long deltaMs)
	{
		LevelManager::Get()->GetInputManager()->Sample(deltaMs);
		NetworkManager::Get()->Listen(mazer::GameDataManager::Get()->GameWorldData.ElapsedGameTime);
	}

	shared_ptr<FixedStepGameLoop> CreateGameLoopStrategy()
	{
		// Create a fixed step game loop with a update timestep of 16ms (approx 60fps). Use our update/draw functions
		return std::make_shared<FixedStepGameLoop>(16, Update, Draw, GetInput);
	}

	void SetupEventTap()
	{
		// Register a Window to show causal relationships and events
		//auto toolWindow = libcausality::ToolWindow("ToolWindow1", 340,340, "This is a Tool Window Title");

		// Tap into the fetchPickupEvent to track relationships between player and pickups
		EventManager::Get()->SetEventTap([](const shared_ptr<Event>& event, const IEventSubscriber* subscriber)
			{
				// Ignore tapping some events:
				if (event->Id == PlayerMovedEventTypeEventId) return;
				if (event->Id == AddGameObjectToCurrentSceneEventId) return;
				if (event->Id == mazer::EnemyMovedEventId) return;
				if (event->Id == DrawCurrentSceneEventId) return;
				if (event->Id == UpdateAllGameObjectsEventTypeEventId) return;
				if (event->Id == UpdateProcessesEventId) return;
				if (event->Id == ControllerMoveEventId) return;

				// Generate causality data
				// libcausality::EventTap::Get()->Tap(event, const_cast<IEventSubscriber*>(subscriber)->GetSubscriberName(), GameDataManager::Get()->GameWorldData.ElapsedGameTime);
			});
	}
}

int main(int, char* [])
{
	try
	{
		// Load settings file
		if (!SettingsManager::Get()->ReadSettingsFile("data/settings.xml"))
		{
			char cwd[1024];
			if (_getcwd(cwd, sizeof(cwd)) != nullptr)
			{
				std::cout << "Current working directory: " << cwd << "\n";
			}
			else
			{
				std::cout << "Failed to get current working directory.\n";

				THROW(1, "Could not load settings file", "Settings");
			}
		}

		// Initialize the game structure
		GameStructure infrastructure(CreateGameLoopStrategy());

		// Initialize all game subsystems
		InitializeGameSubSystems(infrastructure);

		// Allow tapping into all events diagnostic purposes
		SetupEventTap();

		// Load level and create/add game objects
		PrepareFirstLevel();

		// Start the game loop.
		// This will pump update/draw events onto the event system, which level objects subscribe to
		infrastructure.DoGameLoop(&mazer::GameDataManager::Get()->GameWorldData);

		// Game is finished, unload subsystems

		auto isUnloaded = infrastructure.Unload();

		return IsSuccess(isUnloaded, "Unloading game subsystems successful.");
	}
	catch (const EngineException& e)
	{
		//MessageBoxA(nullptr, e.what(), "Game Error!", MB_OK);
		std::cout << "Game Error: " << e.what() << "\n";
		ErrorLogManager::GetErrorLogManager()->Buffer << "****ERROR****\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
		ErrorLogManager::GetErrorLogManager()->LogException(e);
		ErrorLogManager::GetErrorLogManager()->Buffer << "*************\n";
		ErrorLogManager::GetErrorLogManager()->Flush();
	}
	return 0;
}