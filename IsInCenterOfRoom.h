//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_ISINCENTEROFROOM_H
#define GAME3_ISINCENTEROFROOM_H
#include <memory>
#include <ai/Behavior.h>


class ExploringNpc;

class IsInCenterOfRoom : public gamelib::Behavior
{
public:
public:
    explicit IsInCenterOfRoom(std::shared_ptr<ExploringNpc> npc) : npc(std::move(npc)) {  }

    gamelib::Status Update(unsigned long deltaMs) override;
private:
    std::shared_ptr<ExploringNpc> npc;
};

#endif //GAME3_ISINCENTEROFROOM_H