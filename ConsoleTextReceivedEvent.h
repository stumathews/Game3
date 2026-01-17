//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLETEXTRECEIVEDEVENT_H
#define GAME3_CONSOLETEXTRECEIVEDEVENT_H
#include <events/Event.h>

const static gamelib::EventId TextReceivedEventId(TextReceived, "TextReceivedEventId");
class ConsoleTextReceivedEvent : public gamelib::Event
{
public:
    ConsoleTextReceivedEvent() : Event(TextReceivedEventId) {  }
    std::string Text;

    explicit ConsoleTextReceivedEvent(const std::string &text)
        : Event(TextReceivedEventId), Text(text)
    {
    }

    std::string ToString() override
    {
        return "ConsoleTextReceivedEvent";
    }

    ~ConsoleTextReceivedEvent() override
    {
    }
};

#endif //GAME3_CONSOLETEXTRECEIVEDEVENT_H