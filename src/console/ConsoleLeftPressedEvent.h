//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLELEFTPRESSEDEVENT_H
#define GAME3_CONSOLELEFTPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId LeftPressedEventId(LeftPressed, "LeftPressedEventId");
class ConsoleLeftPressedEvent : public gamelib::Event
{
public:
    ConsoleLeftPressedEvent() : Event(LeftPressedEventId) {  }
    std::string ToString() override;

    ~ConsoleLeftPressedEvent() override;
};

inline std::string ConsoleLeftPressedEvent::ToString()
{
    return "ConsoleLeftPressedEvent";
}

inline ConsoleLeftPressedEvent::~ConsoleLeftPressedEvent()
{
}
#endif //GAME3_CONSOLELEFTPRESSEDEVENT_H
