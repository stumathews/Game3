//
// Created by stuart on 10/01/2026.
//

#ifndef GAME3_LLMTOKENPREDICTEDRECEIVED_H
#define GAME3_LLMTOKENPREDICTEDRECEIVED_H

#include "EventNumbers.h"
#include <events/Event.h>

const static gamelib::EventId LLMPredictedTokenReceivedEventEventId(LLMPredictedTokenReceived, "UpdateAllGameObjectsEventType");


class LLMPredictedTokenReceivedEvent final : public gamelib::Event
{
public:
    explicit LLMPredictedTokenReceivedEvent(std::string token) : Event(LLMPredictedTokenReceivedEventEventId),
                                                                 token(std::move(token))
    {
    }


    std::string ToString() override { return "LLMPredictedTokenReceivedEvent";};
    std::string token;
};


#endif //GAME3_LLMTOKENPREDICTEDRECEIVED_H