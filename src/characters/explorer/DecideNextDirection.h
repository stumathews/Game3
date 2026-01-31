//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_DECIDENEXTDIRECTION_H
#define GAME3_DECIDENEXTDIRECTION_H
#include <memory>
#include <ai/Behavior.h>
#include <time/PeriodicTimer.h>


class ExploringNpc;

class DecideNextDirection : public gamelib::Behavior
{
public:
    explicit DecideNextDirection(std::shared_ptr<ExploringNpc> npc) : npc(std::move(npc))
    {
    }


private:
    std::shared_ptr<ExploringNpc> npc;

    gamelib::Status Update(unsigned long deltaMs) override;
};

#endif //GAME3_DECIDENEXTDIRECTION_H