//
// Created by stuart on 15/01/2026.
//

#ifndef GAME3_HAVEDECIDED_H
#define GAME3_HAVEDECIDED_H
#include <memory>
#include <ai/Behavior.h>

class ExploringNpc;

class HaveDecided : public gamelib::Behavior
{
public:
    explicit HaveDecided(const std::shared_ptr<ExploringNpc> &npc); ;

    gamelib::Status Update(unsigned long deltaMs) override;
private:
    std::shared_ptr<ExploringNpc> npc;
};


#endif //GAME3_HAVEDECIDED_H