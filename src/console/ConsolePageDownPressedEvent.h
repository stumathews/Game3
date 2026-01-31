//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLEPAGEDOWNPRESSEDEVENT_H
#define GAME3_CONSOLEPAGEDOWNPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId PageDownPressedEventId(PageDownPressed, "PageDownPressedEventId");
class ConsolePageDownPressedEvent : public gamelib::Event
{
public:
    ConsolePageDownPressedEvent() : Event(PageDownPressedEventId) {  }
    std::string ToString() override
    {
        return "ConsolePageDownPressedEvent";
    }

    ~ConsolePageDownPressedEvent() override
    {
    }
};

#endif //GAME3_CONSOLEPAGEDOWNPRESSEDEVENT_H