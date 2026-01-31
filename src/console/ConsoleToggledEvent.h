//
// Created by stuart on 17/01/2026.
//

#ifndef GAME3_CONSOLETOGGLEDEVENT_H
#define GAME3_CONSOLETOGGLEDEVENT_H
#include <events/Event.h>

#include "ConsoleEventNumbers.h"

const static gamelib::EventId ConsoleToggledEventId(ConsoleToggled, "ConsoleToggledEventId");
class ConsoleToggleEvent : public gamelib::Event
{
public:
    ConsoleToggleEvent() : Event(ConsoleToggledEventId){  }
    std::string ToString() override;

    ~ConsoleToggleEvent() override;
};

inline std::string ConsoleToggleEvent::ToString()
{
    return "ConsoleToggleEvent";
}

inline ConsoleToggleEvent::~ConsoleToggleEvent()
{
}
#endif //GAME3_CONSOLETOGGLEDEVENT_H
