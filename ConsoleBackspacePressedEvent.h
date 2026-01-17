//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLEBACKSPACEPRESSEDEVENT_H
#define GAME3_CONSOLEBACKSPACEPRESSEDEVENT_H
#include <events/Event.h>

#include "ConsoleEventNumbers.h"

const static gamelib::EventId BackspacePressedEventId(BackspacePressed, "BackspacePressedEventId");

class ConsoleBackspacePressedEvent : public gamelib::Event
{
public:
    ConsoleBackspacePressedEvent() : Event(BackspacePressedEventId){}

    std::string ToString() override { return "BackspacePressedEvent"; }

    ~ConsoleBackspacePressedEvent() override;
};

inline ConsoleBackspacePressedEvent::~ConsoleBackspacePressedEvent()
{
}
#endif //GAME3_CONSOLEBACKSPACEPRESSEDEVENT_H
