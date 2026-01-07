//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_STREAMINGLLM_H
#define GAME3_STREAMINGLLM_H

#include <functional>

#include "llama.h"
#include <iostream>
#include <file/SettingsManager.h>
#include <objects/GameObject.h>

#include "common.h"


class StreamingLLM : public gamelib::GameObject
{
public:
    StreamingLLM()
    {
        settingsManager = gamelib::SettingsManager::Get();
    }
    int32_t GetNumTokensInString(const std::string &userPrompt) const;

    void InitializeContext(const std::string &userPrompt, uint32_t n_predict);

    void Initialize();
    gamelib::GameObjectType GetGameObjectType() override ;

    void Update(unsigned long deltaMs) override;

    void Draw(SDL_Renderer *renderer) override;

    std::string GetSubscriberName() override;

    std::string GetName() override;

    void LoadSettings() override;

    ~StreamingLLM() override;
private:
    static void RunWithoutStdErrOutput(const std::function<void()> &func);
    gamelib::SettingsManager* settingsManager = nullptr;
    std::string inferringLLMModelPath;
    std::string embeddingModelPath;
    struct llama_model * model {};
    struct llama_context * ctx {};
    const struct llama_vocab * vocab {};
    std::vector<llama_token> prompt_tokens;
    std::vector<std::string> promptHistory {};
};

#endif //GAME3_STREAMINGLLM_H