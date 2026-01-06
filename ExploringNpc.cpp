#include "ExploringNpc.h"

#include <GameObjectMoveStrategy.h>
#include <Rooms.h>
#include <events/ControllerMoveEvent.h>
#include "character/Hotspot.h"
#include <file/SettingsManager.h>
#include <ai/ScriptedBehavior.h>
#include <ai/BehaviorTree.h>
#include <ai/BehaviorTreeBuilder.h>
#include "MoveInCurrentDirection.h"

void ExploringNpc::Initialize()
{
	// Read in settings
	drawNpcCross = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawNpcCross");
	drawRoomCross = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawRoomCross");
	drawNpcHotspot = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawNpcHotspot");

	// Set initial direction
	SetDirection(gamelib::Direction::Right);

	// Setup game object move strategy
	gameObjectMoveStrategy = std::make_shared<mazer::GameObjectMoveStrategy>(shared_from_this(), currentRoomInfo);

	// Construct our behaviours
	moveInCurrentDirection = new MoveInCurrentDirection(shared_from_this());
	decide = new DecideNextDirection(shared_from_this());
	notInCenterOfRoom = new NotIncenterOfRoom(shared_from_this());
	isInCenterOfRoom = new IsInCenterOfRoom(shared_from_this());
	scriptedBehavior = new gamelib::ScriptedBehavior("TestScript", 20);

	// Set up the Behaviour tree with our behaviors
	behaviorTree = behaviorTree = BehaviorTreeBuilder()
		.ActiveSelector()
			.Sequence("MoveIntoCenter").Condition(notInCenterOfRoom).Action(moveInCurrentDirection).Condition(isInCenterOfRoom).Action(decide).Finish()
	        .Sequence("MoveOutOfCenter").Condition(isInCenterOfRoom).Action(moveInCurrentDirection).Finish()
			.Action(scriptedBehavior)
		.End();
}

void ExploringNpc::Update(const unsigned long deltaMs)
{
	Npc::Update(deltaMs);
	behaviorTree->Update(deltaMs);
}

void ExploringNpc::DrawRoomCross(SDL_Renderer* renderer) const
{
	const auto horizontalLineStartX = currentRoom->GetX();
	const auto horizontalLineStartY = currentRoom->GetY() + currentRoom->GetHeight() / 2;
	const auto horizontalLineEndX = currentRoom->GetX() + currentRoom->GetWidth();
	const auto horizontalLIneEndY = currentRoom->GetY() + currentRoom->GetHeight() / 2;

	const auto horizontalCenterLine = gamelib::Line(
		horizontalLineStartX, horizontalLineStartY, 
		horizontalLineEndX, horizontalLIneEndY);

	const auto verticalLineStartX = currentRoom->GetX() + currentRoom->GetWidth() / 2;
	const auto verticalLineStartY = currentRoom->GetY();
	const auto verticalLineEndX = currentRoom->GetX() + currentRoom->GetWidth() / 2;
	const auto verticalLineEndY = currentRoom->GetY() + currentRoom->GetHeight();

	const auto verticalCenterLine = gamelib::Line(verticalLineStartX, verticalLineStartY,
	                                              verticalLineEndX, verticalLineEndY);

	mazer::Room::DrawLine(renderer, horizontalCenterLine);
	mazer::Room::DrawLine(renderer, verticalCenterLine);
}

void ExploringNpc::DrawMyCross(SDL_Renderer* renderer) const
{
	const auto horizontalLineStartX = Position.GetX();
	const auto horizontalLineStartY = Position.GetY() + Bounds.h / 2;
	const auto horizontalLineEndX = Position.GetX() + Bounds.w;
	const auto horizontalLIneEndY = Position.GetY() + Bounds.h / 2;
	
	const auto horizontalCenterLine = gamelib::Line(
		horizontalLineStartX, horizontalLineStartY,
		horizontalLineEndX, horizontalLIneEndY);

	const auto verticalLineStartX = Position.GetX() + Bounds.w / 2;
	const auto verticalLineStartY = Position.GetY();
	const auto verticalLineEndX = Position.GetX() + Bounds.w / 2;
	const auto verticalLineEndY = Position.GetY() + Bounds.h;

	const auto verticalCenterLine = gamelib::Line(verticalLineStartX, verticalLineStartY,
		verticalLineEndX, verticalLineEndY);

	mazer::Room::DrawLine(renderer, horizontalCenterLine);
	mazer::Room::DrawLine(renderer, verticalCenterLine);
}

void ExploringNpc::Draw(SDL_Renderer* renderer)
{
	Npc::Draw(renderer);

	if (drawNpcHotspot) {
		TheHotspot->Draw(renderer);
	}

	if (drawRoomCross) {
		DrawRoomCross(renderer);
	}

	if (drawNpcCross) {
		DrawMyCross(renderer);
	}
}

ExploringNpc::~ExploringNpc()
{
	delete moveInCurrentDirection;
	delete decide;
	delete notInCenterOfRoom;
	delete isInCenterOfRoom;
	delete scriptedBehavior;
}
