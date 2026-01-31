//
// Created by stuart on 06/01/2026.
//

#ifndef GAME3_STREAMINGLLM_H
#define GAME3_STREAMINGLLM_H

#include <functional>

#include "llama.h"
#include <file/SettingsManager.h>
#include <objects/GameObject.h>

#include "common.h"


class StreamingLLM : public gamelib::GameObject
{
public:
    explicit StreamingLLM(std::string eos = ".");

    void Initialize(const std::string &prompt, uint32_t n_predict);

    gamelib::GameObjectType GetGameObjectType() override ;


    void Update(unsigned long deltaMs) override;

    void Draw(SDL_Renderer *renderer) override;

    std::string GetSubscriberName() override;

    std::string GetName() override;

    void LoadSettings() override;

    ~StreamingLLM() override;
    static void RunWithoutStdErrOutput(const std::function<void()> &func);
private:
    llama_batch PromptToBatch();
    int32_t GetNumTokensInString(const std::string &userPrompt) const;
    void InitializeContext(const std::string &userPrompt, uint32_t n_predict);

    gamelib::SettingsManager* settingsManager = nullptr;
    std::string inferringLLMModelPath;
    std::string embeddingModelPath;
    llama_model * model {};
    llama_context * ctx {};
    const llama_vocab * vocab {};
    std::vector<llama_token> prompt_tokens;
    std::vector<std::string> promptHistory {};
    llama_context_params contextParameters {};
    std::string answerEOS;
};

#endif //GAME3_STREAMINGLLM_H