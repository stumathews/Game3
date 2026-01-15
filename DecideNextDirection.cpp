//
// Created by stuart on 06/01/2026.
//
#include "DecideNextDirection.h"

#include "ExploringNpc.h"

gamelib::Status DecideNextDirection::Update(const unsigned long deltaMs)
{
    npc->cooldownTimer.DoIfReady([&]
    {
        std::cout << "Deciding.\n";
        // Decide next direction to move in
        const auto validMoveDirection = npc->GetProbabilityMatrix()->SelectAction(npc->GetCurrentRoom());

        // Set facing direction to the sampled direction
        npc->SetDirection(validMoveDirection);

        npc->hasDecided = true;
    });


    return gamelib::Status::Failure;
}