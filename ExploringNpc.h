#pragma once
#include <Enemy.h>
#include "MoveProbabilityMatrix.h"
#include <mazer/Room.h>
#include <memory>
#include <GameObjectMoveStrategy.h>
#include <RoomInfo.h>
#include "DecideNextDirection.h"
#include "IsInCenterOfRoom.h"
#include "MoveInCurrentDirection.h"
#include "NotInCenterOfRoom.h"

namespace gamelib
{
	class ScriptedBehavior;
}

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

		currentRoomInfo = std::make_shared<mazer::RoomInfo>(initialRoom);
	}

	~ExploringNpc() override;

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

	void Update(unsigned long deltaMs) override;
	void DrawRoomCross(SDL_Renderer* renderer) const;
	void DrawMyCross(SDL_Renderer* renderer) const;
	void Draw(SDL_Renderer* renderer) override;

	gamelib::Direction GetCurrentFacingDirection() const { return this->currentFacingDirection; }
	std::shared_ptr<mazer::Room> GetCurrentRoom() const { return this->currentRoom; }
	std::shared_ptr<mazer::RoomInfo> GetCurrentRoomInfo() const { return this->currentRoomInfo; }
	std::shared_ptr<MoveProbabilityMatrix> GetProbabilityMatrix() const { return moveProbabilityMatrix; }
	std::shared_ptr<gamelib::IGameObjectMoveStrategy> GetGameObjectMoveStrategy(){ return gameObjectMoveStrategy; }
	std::shared_ptr<gamelib::Hotspot> GetHotspot() const { return this->TheHotspot; }
	bool HasReachedCenterOfRoom() const { return hasReachedCenter; }

private:
	std::shared_ptr<MoveProbabilityMatrix> moveProbabilityMatrix;
	std::shared_ptr<mazer::Room> currentRoom;
	std::shared_ptr<mazer::RoomInfo> currentRoomInfo;
	gamelib::BehaviorTree* behaviorTree;
	std::shared_ptr<mazer::Room> lastRoom;
	bool isWithinSingleRoom = false;
	bool hasReachedCenter = false;
	bool drawNpcCross = false;
	bool drawRoomCross = false;
	bool drawNpcHotspot = false;
	MoveInCurrentDirection* moveInCurrentDirection;
	DecideNextDirection* decide;
	NotIncenterOfRoom* notInCenterOfRoom;
	IsInCenterOfRoom* isInCenterOfRoom;
	gamelib::ScriptedBehavior* scriptedBehavior;

};

