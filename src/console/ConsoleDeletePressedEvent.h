//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLEDELETEPRESSEDEVENT_H
#define GAME3_CONSOLEDELETEPRESSEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId DeletePressedEventId(DeletePressed, "DeletePressedEventId");
class ConsoleDeletePressedEvent : public gamelib::Event
{
public:
    ConsoleDeletePressedEvent() : Event(DeletePressedEventId) {  }
    std::string ToString() override { return "ConsoleDeletePressedEventId"; }

    ~ConsoleDeletePressedEvent() override;
};

inline ConsoleDeletePressedEvent::~ConsoleDeletePressedEvent()
{
}

#endif //GAME3_CONSOLEDELETEPRESSEDEVENT_H
