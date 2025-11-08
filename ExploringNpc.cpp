#include "ExploringNpc.h"

#include <GameObjectMoveStrategy.h>
#include <character/MovementAtSpeed.h>
#include <events/ControllerMoveEvent.h>
#include <utils/Utils.h>

void ExploringNpc::Update(const unsigned long deltaMs)
{
	this->SetDirection(gamelib::Direction::Right);
	gamelib::Direction dir = gamelib::Direction::Down;
	const std::map<gamelib::Direction, gamelib::ControllerMoveEvent::KeyState> theMap;
	std::shared_ptr<gamelib::IMovement> movement = std::make_shared<gamelib::MovementAtSpeed>(1, moveProbabilityMatrix->SelectAction(currentRoom), deltaMs);
	
	this->gameObjectMoveStrategy->MoveGameObject(movement);
}
