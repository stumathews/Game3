//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLEPAGEUPPRESSEDEVENT_H
#define GAME3_CONSOLEPAGEUPPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId PageUpPressedEventId(PageUpPressed, "PageUpPressedEventId");
class ConsolePageUpPressedEvent : public gamelib::Event
{
public:
    ConsolePageUpPressedEvent() : Event(PageUpPressedEventId) {  }
    std::string ToString() override
    {
        return "ConsolePageUpPressedEvent";
    }

    ~ConsolePageUpPressedEvent() override
    {
    }
};

#endif //GAME3_CONSOLEPAGEUPPRESSEDEVENT_H