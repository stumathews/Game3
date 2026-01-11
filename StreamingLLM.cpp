//
// Created by stuart on 06/01/2026.
//


#include "StreamingLLM.h"
#include "common.h"
#include <file/SettingsManager.h>

#include "LLMPredctionCompleteEvent.h"
#include "LLMTokenPredictedReceived.h"
#ifdef _WIN32
#include <io.h>
#define dup2 _dup2
#define STDERR_DEV "NUL"
#else
#include <unistd.h>
#define STDERR_DEV "/dev/null"
#endif
#include <string>

#include <iostream>


StreamingLLM::StreamingLLM(std::string eos) : answerEOS(std::move(eos))
{
    settingsManager = gamelib::SettingsManager::Get();
}

void StreamingLLM::Initialize(const std::string &prompt, const uint32_t n_predict)
{
    // Set the user prompt

    LoadSettings();

    RunWithoutStdErrOutput([&]
    {
        auto modelParameters = llama_model_default_params();

        modelParameters.n_gpu_layers = 99;

        model = llama_model_load_from_file(inferringLLMModelPath.c_str(), modelParameters);

        if (model == nullptr)
        {
            fprintf(stderr, "%s: error: unable to load model\n", __func__);
        }

        // Initialize vocab
        vocab = llama_model_get_vocab(model);

        // Initialize context
        InitializeContext(prompt, n_predict);
    });
}

void StreamingLLM::InitializeContext(const std::string &prompt, const uint32_t n_predict)
{
    prompt_tokens.clear();

    const uint32_t numTokensInPrompt = GetNumTokensInString(prompt);

    prompt_tokens.resize(numTokensInPrompt);

    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0)
    {
        fprintf(stderr, "%s: error: failed to tokenize the prompt\n", __func__);
    }

    contextParameters = llama_context_default_params();

    contextParameters.n_ctx = numTokensInPrompt + n_predict - 1; // Set the context size (memory)
    contextParameters.n_batch = numTokensInPrompt; // Set the maximum number of tokens that can be processed in a single call to llama_decode
    contextParameters.no_perf = false; // Enable performance counters
    contextParameters.n_threads = 16;

    // Initialize the context
    ctx = llama_init_from_model(model, contextParameters);

    if (ctx == nullptr)
    {
        fprintf(stderr, "%s: error: failed to create the llama_context\n", __func__);
    }
}

gamelib::GameObjectType StreamingLLM::GetGameObjectType()
{
    return gamelib::GameObjectType::game_defined;
}


void StreamingLLM::Update(unsigned long deltaMs)
{
    uint32_t n_predict = contextParameters.n_ctx ;

    // print the prompt token-by-token

    for (const auto id : prompt_tokens)
    {
        char buf[128];
        const int n = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
        }
        std::string s(buf, n);
        printf("%s", s.c_str());
    }

    // Convert the prompt into a batch so the decoder can understand it
    llama_batch batch = PromptToBatch();

    const auto numTokensInPrompt = contextParameters.n_batch;

    // Initialize the sampler
    auto samplerParameters = llama_sampler_chain_default_params();

    // Don't measure performance timing
    samplerParameters.no_perf = false;

    llama_sampler * sampler = llama_sampler_chain_init(samplerParameters);

    // Use the greedy sampler to always selects the highest probability token
    llama_sampler_chain_add(sampler, llama_sampler_init_greedy());

    // main loop

    const auto t_main_start = ggml_time_us();
    int n_decode = 0;
    llama_token new_token_id;

    // The context window represents the memory available for the attention mechanisms to work with
    // So you have the initial prompt tokens + how many you'd like to predict ie: (3) "My name is"  + (1) "Stuart"
    for (int n_pos = 0; n_pos + batch.n_tokens < numTokensInPrompt + n_predict; )
    {
        // Position 0 is the first prediction after the prompt
        // Position 1 is the second prediction after the prompt including the first prediction etc..

        // Evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch))
        {
            fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
        }

        n_pos += batch.n_tokens; // normally first time batch is called, the same number of tokens in prompt is returned, then 1 token each time thereafter

        // sample the next token
        {
            new_token_id = llama_sampler_sample(sampler, ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id))
            {
                break;
            }

            // Convert token to text
            char buf[128];
            const int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0)
            {
                fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
            }

            std::string text(buf, n);

            if (text == answerEOS)
            {
                std::cout << "\nExplicit EOS character ("<< answerEOS << ") found in predicted token, completing generation.\n";
                break;
            }

            printf("%s", text.c_str());
            fflush(stdout);

            RaiseEvent(std::make_shared<LLMPredictedTokenReceivedEvent>(text));

            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);

            n_decode += 1;
        }
    }

    printf("\n");
    RaiseEvent(std::make_shared<LLMPredictionCompleteEvent>());

    const auto t_main_end = ggml_time_us();

    fprintf(stderr, "%s: decoded %d tokens in %.2f s, speed: %.2f t/s\n",
            __func__, n_decode, (t_main_end - t_main_start) / 1000000.0f, n_decode / ((t_main_end - t_main_start) / 1000000.0f));

    fprintf(stderr, "\n");


}


llama_batch StreamingLLM::PromptToBatch()
{
    auto batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    if (llama_model_has_encoder(model))
    {
        // This is an Encoder–decoder model (Whisper/T5)
        if (llama_encode(ctx, batch))
        {
            fprintf(stderr, "%s : failed to eval\n", __func__);
        }

        auto decoder_start_token_id = llama_model_decoder_start_token(model);

        if (decoder_start_token_id == LLAMA_TOKEN_NULL)
        {
            decoder_start_token_id = llama_vocab_bos(vocab);
        }

        batch = llama_batch_get_one(&decoder_start_token_id, 1);
    }

    // This is a normal GPT-style model (decoder-only model)

    return batch;
}

void StreamingLLM::Draw(SDL_Renderer *renderer)
{
    // We dont be drawing anything yet
}

std::string StreamingLLM::GetSubscriberName()
{
    return "StreamingLLM";
}

std::string StreamingLLM::GetName()
{
    return GetSubscriberName();
}

void StreamingLLM::LoadSettings()
{
    inferringLLMModelPath = settingsManager->GetString("llm", "InferringLLMModelPath");
    embeddingModelPath = settingsManager->GetString("llm", "EmbeddingModelPath");
}

StreamingLLM::~StreamingLLM()
{
}


int32_t StreamingLLM::GetNumTokensInString(const std::string &prompt) const
{
    return -llama_tokenize(vocab, prompt.c_str(), prompt.size(), nullptr, 0, true, true);
}

void StreamingLLM::RunWithoutStdErrOutput(const std::function<void()> &func)
{
    // Save original stderr
    const int oldStdErr = dup(fileno(stderr));

    // Redirect stderr to /dev/null

    FILE* devnull = fopen(STDERR_DEV, "w");

    dup2(fileno(devnull), fileno(stderr));
    fclose(devnull);

    // Run function - it will not show any std error messages
    func();

    // Restore stderr
    dup2(oldStdErr, fileno(stderr));
#ifdef _WIN32
    _close(old_stderr);
#else
    close(oldStdErr);
#endif
}
