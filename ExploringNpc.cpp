#include "ExploringNpc.h"

#include <GameObjectMoveStrategy.h>
#include <Rooms.h>
#include <character/MovementAtSpeed.h>
#include <events/ControllerMoveEvent.h>
#include <utils/Utils.h>
#include "character/Hotspot.h"
#include <ai\InlineBehavioralAction.h>

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
	// Initial direction
	SetDirection(gamelib::Direction::Right);
	gameObjectMoveStrategy = std::make_shared<mazer::GameObjectMoveStrategy>(shared_from_this(), roomInfo);

	// Initialize the Behavior tree

	auto* hasReachedCenterOfRoom = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			SDL_Rect _;
			const auto myHotspot = this->Hotspot->GetBounds();

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

			auto horizontalLineHit = false;	
			// up or down - get horizontal center line

			const auto horizontalLineAsRect = line_to_rect(horizontalCenterLine, true);
			if (SDL_IntersectRect(&horizontalLineAsRect, &myHotspot, &_))
			{
				horizontalLineHit = true;
			}

			auto verticalLineHit = false;
			// left or right - get vertical center line				
			const auto verticalLineAsRect = line_to_rect(verticalCenterLine, false);

			if (SDL_IntersectRect(&verticalLineAsRect, &myHotspot, &_))
			{
				verticalLineHit = true;
			}

			const auto hasReachedCenter = horizontalLineHit && verticalLineHit;

			if (hasReachedCenter)
			{
				std::cout << "reached center of the room:" << currentRoom->GetRoomNumber() << "\n";
			}

			return hasReachedCenter
				? gamelib::BehaviorResult::Success
				: gamelib::BehaviorResult::Failure;
		});

	auto* moveBehavior = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			// Continue moving in current direction
			const auto movement = std::make_shared<gamelib::MovementAtSpeed>(1, currentFacingDirection, deltaMs);
			const auto isValidMove = gameObjectMoveStrategy->MoveGameObject(movement);

			const auto myHotspot = this->Hotspot->GetBounds();

			SDL_Rect _;

			const auto topRoom = currentRoom->GetSideRoom(gamelib::Side::Top);
			const auto bottomRoom = currentRoom->GetSideRoom(gamelib::Side::Bottom);
			const auto leftRoom = currentRoom->GetSideRoom(gamelib::Side::Left);
			const auto rightRoom = currentRoom->GetSideRoom(gamelib::Side::Right);

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

			isInSingleRoom = inCountRooms == 1;

			if (!isValidMove)
			{
				std::cout << "invalid move\n";
			}

			roomInfo->SetCurrentRoom(currentRoom);
															
			return isValidMove == true
				? gamelib::BehaviorResult::Success
				: gamelib::BehaviorResult::Failure;

		}, "Move");

	auto* decideMove = new gamelib::InlineBehavioralAction([&](const unsigned long deltaMs)
		{
			std::cout << "deciding\n";
			// Decide next direction to move in
			const auto validMoveDirection = moveProbabilityMatrix->SelectAction(currentRoom);

			// Set facing direction to the sampled direction
			SetDirection(validMoveDirection);

			return gamelib::BehaviorResult::Failure;

		}, "Move");

	

	behaviorTree = behaviorTree = BehaviorTreeBuilder()
		.ActiveNodeSelector()
			.Sequence("Moving")
				.Action(moveBehavior)
				.ActiveNodeSelector()
					.Sequence()
							.Condition(hasReachedCenterOfRoom)
							.Action(decideMove)
					.Finish()
					.Action(moveBehavior)
				.Finish()
			.Finish()
			.Sequence("Deciding")
				.Action(decideMove)
			.Finish()
		.End();
}

void ExploringNpc::Update(const unsigned long deltaMs)
{
	Npc::Update(deltaMs);
	behaviorTree->Update(deltaMs);
}

void ExploringNpc::Draw(SDL_Renderer* renderer)
{
	Npc::Draw(renderer);

	Hotspot->Draw(renderer);

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
