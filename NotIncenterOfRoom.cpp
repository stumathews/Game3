//
// Created by stuart on 06/01/2026.
//

#include "NotInCenterOfRoom.h"
#include "ExploringNpc.h"

gamelib::Status NotIncenterOfRoom::Update(unsigned long deltaMs)
{
    return npc->HasReachedCenterOfRoom()
               ? gamelib::Status::Failure
               : gamelib::Status::Success;
}
