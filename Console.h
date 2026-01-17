//
// Created by stuart on 16/01/2026.
//

#ifndef GAME3_CONSOLE_H
#define GAME3_CONSOLE_H
#include <string>
#include <vector>
#include <events/EventSubscriber.h>
#include <objects/GameObject.h>
#include <SDL2/SDL_ttf.h>

class Console : public gamelib::GameObject
{
public:

    void Initialize();
private:
    std::string GetSubscriberName() override;


    static void ExecuteCommand(const std::string & string);

    std::vector<std::shared_ptr<gamelib::Event>> HandleEvent(const std::shared_ptr<gamelib::Event> &evt,
                                                             unsigned long deltaMs) override;

    gamelib::GameObjectType GetGameObjectType() override;

    void Update(unsigned long deltaMs) override;

    void Draw(SDL_Renderer *renderer) override;

    static void DrawText(SDL_Renderer *renderer, TTF_Font *font, const std::string &text, int x, int y, SDL_Color color);

    bool open = false;

    std::vector<std::string> inputHistory;   // output history
    std::string inputLine;                // current input line
    size_t cursorPosition = 0;                // cursor position in input

    int scroll = 0;                   // scroll offset
    TTF_Font* font {};
};


#endif //GAME3_CONSOLE_H