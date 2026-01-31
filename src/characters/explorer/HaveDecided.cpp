//
// Created by stuart on 15/01/2026.
//

#include "HaveDecided.h"

#include "ExploringNpc.h"

HaveDecided::HaveDecided(const std::shared_ptr<ExploringNpc> &npc): npc(npc)
{

}

gamelib::Status HaveDecided::Update(unsigned long deltaMs)
{
    if (npc->hasDecided) return gamelib::Status::Success;
    return gamelib::Status::Failure;
}
