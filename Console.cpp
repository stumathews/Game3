//
// Created by stuart on 16/01/2026.
//

#include "Console.h"

#include <cstring>
#include <iostream>
#include <resource/ResourceManager.h>
#include <utils/Utils.h>

#include "ConsoleBackspacePressedEvent.h"
#include "ConsoleDeletePressedEvent.h"
#include "ConsoleLeftPressedEvent.h"
#include "ConsolePageDownPressedEvent.h"
#include "ConsolePageUpPressedEvent.h"
#include "ConsoleReturnPressedEvent.h"
#include "ConsoleRightPressedEvent.h"
#include "ConsoleTextReceivedEvent.h"
#include "ConsoleToggledEvent.h"
#include <font/FontManager.h>
#include <font/FontAsset.h>

std::string Console::GetSubscriberName()
{
    return "Console";
}

void Console::Initialize()
{
    SubscribeToEvent(BackspacePressedEventId);
    SubscribeToEvent(DeletePressedEventId);
    SubscribeToEvent(LeftPressedEventId);
    SubscribeToEvent(RightPressedEventId);
    SubscribeToEvent(ReturnPressedEventId);
    SubscribeToEvent(PageUpPressedEventId);
    SubscribeToEvent(PageDownPressedEventId);
    SubscribeToEvent(TextReceivedEventId);
    SubscribeToEvent(ConsoleToggledEventId);
    TTF_Init();


    auto t = gamelib::ResourceManager::Get()->GetAssetInfo("kenvector_future2.ttf");
    auto theFont = gamelib::FontManager::Get()->ToFontAsset(t);

    font = TTF_OpenFont(theFont->FilePath.c_str(), 16);

}

void Console::ExecuteCommand(const std::string &string)
{
    // Send game command off to the event queue
    std::cout << "Processing command: " << string << std::endl;
}

std::vector<std::shared_ptr<gamelib::Event>> Console::HandleEvent(const std::shared_ptr<gamelib::Event> &evt,
                                                                  unsigned long deltaMs)
{
    if (evt->Id == TextReceivedEventId)
    {
        const auto event = To<ConsoleTextReceivedEvent>(evt);

        inputLine.insert(cursorPosition, event->Text);
        cursorPosition += std::strlen(event->Text.c_str());
    }

    if (evt->Id == BackspacePressedEventId)
    {
        if (cursorPosition > 0)
        {
            // remove a character from the inputLine
            inputLine.erase(cursorPosition - 1, 1);

            // move the cursor position by one
            cursorPosition--;
        }
    }

    if (evt->Id == DeletePressedEventId)
    {
        // Delete character a cursor position
        if (cursorPosition < inputLine.size())
        {
            inputLine.erase(cursorPosition, 1);
        }
    }

    if (evt->Id == LeftPressedEventId)
    {
        // Move cursor position down by one
        if (cursorPosition > 0)
        {
            cursorPosition--;
        }
    }

    if (evt->Id == RightPressedEventId)
    {
        // Move the cursor position up by one
        if (cursorPosition < inputLine.size())
        {
            cursorPosition++;
        }
    }

    if (evt->Id == ReturnPressedEventId)
    {
        // Process the input line
        ExecuteCommand(inputLine);

        // Save input line in history
        inputHistory.push_back("> " + inputLine);

        // Clear input line, ready for new characters
        inputLine.clear();

        // Set position of cursor at the beginning of the line
        cursorPosition = 0;
    }

    if (evt->Id == PageUpPressedEventId)
    {
        scroll++;
    }

    if (evt->Id == PageDownPressedEventId)
    {
        if (scroll > 0)
        {
            scroll--;
        }
    }

    if (evt->Id == ConsoleToggledEventId)
    {
        open = !open;
    }


    return {};
}

gamelib::GameObjectType Console::GetGameObjectType()
{
    return gamelib::GameObjectType::game_defined;
}

void Console::Update(unsigned long deltaMs)
{
    // Tick the console logic
}

void Console::Draw(SDL_Renderer *renderer)
{

    if (!open) return;

    constexpr SDL_Color colour = { 255, 0, 0, 0 };
    SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);

    // Draw the console
    //SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    int windowWidth = 300;
    int windowHeight = 300;
    if (inputLine.length() > 0)
    {
        DrawText(renderer, font, inputLine, 0,0, colour);
    }
}

void Console::DrawText(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const std::string& text,
    int x, int y,
    SDL_Color color
) {
    SDL_Surface * surface = TTF_RenderText_Blended(font, text.c_str(), color);
    if (surface == NULL) {
        printf("TTF_RenderText_Blended Error: %s\n", TTF_GetError());
    }
    // surface -> Texture
    SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dst{ x, y, surface->w, surface->h };
    // texture -> renderer
    SDL_RenderCopy(renderer, texture, nullptr, &dst);

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);

}