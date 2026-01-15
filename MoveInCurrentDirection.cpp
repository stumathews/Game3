//
// Created by stuart on 06/01/2026.
//

#include "MoveInCurrentDirection.h"

#include <iostream>
#include <character/Hotspot.h>
#include <character/MovementAtSpeed.h>
#include "ExploringNpc.h"

SDL_Rect MoveInCurrentDirection::line_to_rect(const gamelib::Line &line, const bool isHorizontal)
{
	const auto w = isHorizontal
		               ? line.X2 - line.X1
		               : 1;
	const auto h = isHorizontal
		               ? 1
		               : line.Y2 - line.Y1;

	return SDL_Rect{ line.X1, line.Y1, w, h};
}

gamelib::Status MoveInCurrentDirection::Update(unsigned long deltaMs)
{
    // Continue moving in current direction
	const auto movement = std::make_shared<gamelib::MovementAtSpeed>(1, npc->GetCurrentFacingDirection(), deltaMs);
	const auto isValidMove = npc->GetGameObjectMoveStrategy()->MoveGameObject(movement);

	currentRoom = npc->GetCurrentRoom();

	if (!isValidMove)
	{
		std::cout << "Invalid move!\n";
		//return gamelib::Status::Failure;
	}

	// Detect if NPC has moved into the bounds of any adjacent rooms
	const auto myHotspot = npc->GetHotspot()->GetBounds();

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


	npc->GetCurrentRoomInfo()->SetCurrentRoom(currentRoom);

	isWithinSingleRoom = inCountRooms == 0;

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

	this->npc->SetHasReachedCenterOfRoom(hasReachedCenter);

	if(hasReachedCenter)
	{
		std::cout << "Has reached center of room " << currentRoom->GetRoomNumber() << "\n";
		return gamelib::Status::Failure;
	}

	std::cout << "current room is " << currentRoom->GetRoomNumber() << "\n";

	return gamelib::Status::Success;
}
