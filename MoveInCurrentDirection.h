//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_MOVEINCURRENTDIRECTION_H
#define GAME3_MOVEINCURRENTDIRECTION_H
#include <memory>
#include <SDL_rect.h>
#include <ai/Behavior.h>
#include <geometry/Line.h>

namespace mazer
{
	class Room;
}

class ExploringNpc;

class MoveInCurrentDirection : public gamelib::Behavior
{
public:
	explicit MoveInCurrentDirection(std::shared_ptr<ExploringNpc> exploringNpc) : npc(std::move(exploringNpc)) {  }

	static SDL_Rect line_to_rect(const gamelib::Line& line, const bool isHorizontal);

	gamelib::Status Update(unsigned long deltaMs) override;
private:
	std::shared_ptr<ExploringNpc> npc;
	bool isWithinSingleRoom = false;
	bool hasReachedCenter = false;
	std::shared_ptr<mazer::Room> currentRoom = nullptr;
};

#endif //GAME3_MOVEINCURRENTDIRECTION_H