//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_NOTINCENTEROFROOM_H
#define GAME3_NOTINCENTEROFROOM_H
#include <ai/Behavior.h>
#include <memory>
class ExploringNpc;
class NotIncenterOfRoom : public gamelib::Behavior
{
public:
    explicit NotIncenterOfRoom(std::shared_ptr<ExploringNpc> npc) : npc(std::move(npc)) {  }
    gamelib::Status Update(unsigned long deltaMs) override;

private:
    std::shared_ptr<ExploringNpc> npc;
};

#endif //GAME3_NOTINCENTEROFROOM_H