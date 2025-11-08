#pragma once
#include <Enemy.h>
#include "MoveProbabilityMatrix.h"
#include <mazer/Room.h>
#include <memory>

class ExploringNpc : public gamelib::Npc//, public std::enable_shared_from_this<ExploringNpc>
{
public:
	ExploringNpc(const std::string& name, const std::string& type, const gamelib::Coordinate<int> position, const bool visible,
		const AnimatedSpriteSPtr& sprite, std::shared_ptr<MoveProbabilityMatrix> moveProbabilityMatrix, const std::shared_ptr<mazer::Room>& room)
	: Npc(name, type, position, visible, sprite),
		moveProbabilityMatrix(moveProbabilityMatrix), currentRoom(room)
	{
	}
	std::string GetSubscriberName() override { return GetName(); }
	std::string GetName() override { return "ExploringNPC"; }
	gamelib::GameObjectType GetGameObjectType() override { return gamelib::GameObjectType::game_defined; }
	void Update(const unsigned long deltaMs) override;
private:
	std::shared_ptr<MoveProbabilityMatrix> moveProbabilityMatrix;
	std::shared_ptr<mazer::Room> currentRoom;
};

