#include "ExploringNpc.h"

#include <GameObjectMoveStrategy.h>
#include <Rooms.h>
#include <character/MovementAtSpeed.h>
#include <events/ControllerMoveEvent.h>
#include <utils/Utils.h>
#include "character/Hotspot.h"
#include <ai\InlineBehavioralAction.h>
#include <file/SettingsManager.h>

namespace
{
	static SDL_Rect line_to_rect(const gamelib::Line& line, const bool isHorizontal)
	{
		const auto w = isHorizontal
			? line.X2 - line.X1
			: 1;
		const auto h = isHorizontal
			? 1
			: line.Y2 - line.Y1;

		return SDL_Rect{ line.X1, line.Y1, w, h};
	}
}

void ExploringNpc::Initialize()
{
	drawNpcCross = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawNpcCross");
	drawRoomCross = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawRoomCross");
	drawNpcHotspot = gamelib::SettingsManager::Get()->GetBool("WanderingNPC", "drawNpcHotspot");

	// Set initial direction
	SetDirection(gamelib::Direction::Right);

	// Setup game object move strategy
	gameObjectMoveStrategy = std::make_shared<mazer::GameObjectMoveStrategy>(shared_from_this(), currentRoomInfo);

	// Initialize the Behavior tree

	
	auto* move = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			// Continue moving in current direction
			const auto movement = std::make_shared<gamelib::MovementAtSpeed>(1, currentFacingDirection, deltaMs);
			const auto isValidMove = gameObjectMoveStrategy->MoveGameObject(movement);

			if (!isValidMove)
			{
				std::cout << "Invalid move!\n";
			}

			// Detect if NPC has moved into the bounds of any adjacent rooms
			const auto myHotspot = this->Hotspot->GetBounds();

			SDL_Rect _;

			const auto topRoom = currentRoom->GetSideRoom(gamelib::Side::Top);
			const auto bottomRoom = currentRoom->GetSideRoom(gamelib::Side::Bottom);
			const auto leftRoom = currentRoom->GetSideRoom(gamelib::Side::Left);
			const auto rightRoom = currentRoom->GetSideRoom(gamelib::Side::Right);

			// If only within one of the adjacent rooms means NPC is in only one room
			auto inCountRooms = 0;

			if (SDL_IntersectRect(&topRoom->InnerBounds, &myHotspot, &_))
			{
				currentRoom = topRoom;
				inCountRooms++;
			}

			if (SDL_IntersectRect(&bottomRoom->InnerBounds, &myHotspot, &_))
			{
				currentRoom = bottomRoom;
				inCountRooms++;
			}

			if (SDL_IntersectRect(&leftRoom->InnerBounds, &myHotspot, &_))
			{
				currentRoom = leftRoom;
				inCountRooms++;
			}

			if (SDL_IntersectRect(&rightRoom->InnerBounds, &myHotspot, &_))
			{
				currentRoom = rightRoom;
				inCountRooms++;
			}

			isWithinSingleRoom = inCountRooms == 1;

			if (isWithinSingleRoom)
			{				
				currentRoomInfo->SetCurrentRoom(currentRoom);
			}

			// Work out the geometry that defines a cross the splits the room equally into 4 sections,
			// such as to define a central point being the intersection of the NPCs hotspot it

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

			// We will use booleans flags to indicate if NPC has intersected with lines of the cross
			auto horizontalLineHit = false;
			auto verticalLineHit = false;

			// Turn the line into s SDL_Rect for collision detection
			const auto horizontalLineAsRect = line_to_rect(horizontalCenterLine, true);
			const auto verticalLineAsRect = line_to_rect(verticalCenterLine, false);

			// Perform collision detection
			if (SDL_IntersectRect(&horizontalLineAsRect, &myHotspot, &_))
			{
				horizontalLineHit = true;
			}

			if (SDL_IntersectRect(&verticalLineAsRect, &myHotspot, &_))
			{
				verticalLineHit = true;
			}

			hasReachedCenter = horizontalLineHit && verticalLineHit;

			return gamelib::BehaviorResult::Success;

		}, "Move");

	auto* decide = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			std::cout << "Deciding.\n";
			// Decide next direction to move in
			const auto validMoveDirection = moveProbabilityMatrix->SelectAction(currentRoom);

			// Set facing direction to the sampled direction
			SetDirection(validMoveDirection);

			return gamelib::BehaviorResult::Failure;

		}, "Move");

	auto* notInCenterOfRoom = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
	{
		return hasReachedCenter
			? gamelib::BehaviorResult::Failure
			: gamelib::BehaviorResult::Success;
	});

	auto* isInCenterOfRoom = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			return hasReachedCenter
				? gamelib::BehaviorResult::Success
				: gamelib::BehaviorResult::Failure;
		});

	behaviorTree = behaviorTree = BehaviorTreeBuilder()
		.ActiveNodeSelector()		
			.Sequence("MoveIntoCenter").Condition(notInCenterOfRoom).Action(move).Condition(isInCenterOfRoom).Action(decide).Finish()      
	        .Sequence("MoveOutOfCenter").Condition(isInCenterOfRoom).Action(move).Finish()
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
		Hotspot->Draw(renderer);
	}

	if (drawRoomCross) {
		DrawRoomCross(renderer);
	}

	if (drawNpcCross) {
		DrawMyCross(renderer);
	}

}
