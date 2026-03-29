//
// Created by stuart on 31/01/2026.
//

#ifndef GAME3_ENVIRONMENTSENSOR_H
#define GAME3_ENVIRONMENTSENSOR_H
#include <processes/Process.h>
#include <time/PeriodicTimer.h>


class ExploringNpc;

// Perception service is responsible or updating the npc blackboard with informtion about the npc
class PerceptionService : public gamelib::Process
{
public:
    PerceptionService(std::shared_ptr<ExploringNpc> npc);

protected:
    void OnUpdate(unsigned long deltaMs) override;
private:
    gamelib::PeriodicTimer periodicTimer;
    std::shared_ptr<ExploringNpc> npc;
};


#endif //GAME3_ENVIRONMENTSENSOR_H