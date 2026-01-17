//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLERIGHTPRESSEDEVENT_H
#define GAME3_CONSOLERIGHTPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId RightPressedEventId(RightPressed, "RightPressedEventId");
class ConsoleRightPressedEvent : public gamelib::Event
{
public:
    ConsoleRightPressedEvent(): Event(RightPressedEventId) {  }
    std::string ToString() override
    {
        return "ConsoleRightPressedEvent";
    }

    ~ConsoleRightPressedEvent() override
    {
    }
};

#endif //GAME3_CONSOLERIGHTPRESSEDEVENT_H