//
// Created by stuart on 06/01/2026.
//

#include "IsInCenterOfRoom.h"

#include "ExploringNpc.h"

gamelib::Status IsInCenterOfRoom::Update(unsigned long deltaMs)
{
    return npc->HasReachedCenterOfRoom()
        ? gamelib::Status::Success
        : gamelib::Status::Failure;
}
