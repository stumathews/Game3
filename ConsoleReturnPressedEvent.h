//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLERETURNPRESSEDEVENT_H
#define GAME3_CONSOLERETURNPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId ReturnPressedEventId(ReturnPressed, "ReturnPressedEventId");
class ConsoleReturnPressedEvent : public gamelib::Event
{
public:
    ConsoleReturnPressedEvent() : Event(ReturnPressedEventId) {  }
    std::string ToString() override
    {
        return "ConsoleReturnPressedEvent";
    }

    ~ConsoleReturnPressedEvent() override
    {
    }
};

#endif //GAME3_CONSOLERETURNPRESSEDEVENT_H