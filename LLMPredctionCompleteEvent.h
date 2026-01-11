//
// Created by stuart on 10/01/2026.
//

#ifndef GAME3_LLMPREDCTIONCOMPLETEEVENT_H
#define GAME3_LLMPREDCTIONCOMPLETEEVENT_H


#include "EventNumbers.h"
#include <events/Event.h>

const static gamelib::EventId LLMPredictionCompleteEventEventId(LLMPredictionComplete, "LLMPredictionCompleteEvent");

class LLMPredictionCompleteEvent final : public gamelib::Event
{
public:
    explicit LLMPredictionCompleteEvent() : Event(LLMPredictionCompleteEventEventId)
    {
    }

    std::string ToString() override { return "LLMPredictionCompleteEvent";};
};

#endif //GAME3_LLMPREDCTIONCOMPLETEEVENT_H