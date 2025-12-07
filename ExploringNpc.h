#pragma once
#include <Enemy.h>
#include "MoveProbabilityMatrix.h"
#include <mazer/Room.h>
#include <memory>
#include <GameObjectMoveStrategy.h>
#include <RoomInfo.h>
#include <ai/BehaviorTree.h>
#include <ai/BehaviorTreeBuilder.h>

class OnceVariable
{
public:
	OnceVariable() : evaluated(false)
	{

	}


	void Set()
	{
		if (!evaluated) 
		{
			set = true;
		}
		set = false;
	}

	bool IsSet()
	{
		if (!evaluated)
		{
			evaluated = true;
			return set;
		}
		return false;
	}
	void Reset() { evaluated = false; }
private:
	bool evaluated;
	bool set;
};

class ExploringNpc : public gamelib::Npc, public std::enable_shared_from_this<ExploringNpc>
{
public:
	ExploringNpc(const std::string& name, 
		const std::string& type, 
		const gamelib::Coordinate<int> position, 
		const bool visible,
		const AnimatedSpriteSPtr& sprite,
		std::shared_ptr<MoveProbabilityMatrix> moveProbabilityMatrix, 
		const std::shared_ptr<mazer::Room>& initialRoom)
			: Npc(name, type, position, visible, sprite),
				moveProbabilityMatrix(moveProbabilityMatrix),
				currentRoom(initialRoom)
	{
		// empty
		roomInfo = std::make_shared<mazer::RoomInfo>(initialRoom);
	}

	void Initialize();

	std::string GetSubscriberName() override
	{
		return GetName();
	}
	std::string GetName() override
	{
		return "ExploringNPC";
	}

	gamelib::GameObjectType GetGameObjectType() override
	{
		return gamelib::GameObjectType::game_defined;
	}

	void Update(const unsigned long deltaMs) override;
	void Draw(SDL_Renderer* renderer) override;
private:
	std::shared_ptr<MoveProbabilityMatrix> moveProbabilityMatrix;
	std::shared_ptr<mazer::Room> currentRoom;
	std::shared_ptr<mazer::RoomInfo> roomInfo;
	OnceVariable actionSelected;
	gamelib::BehaviorTree* behaviorTree;
	std::shared_ptr<mazer::Room> lastRoom;
	bool isInSingleRoom = false;
};

