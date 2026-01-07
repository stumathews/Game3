//
// Created by stuart on 06/01/2026.
//


#include "StreamingLLM.h"
#include "common.h"
#include <file/SettingsManager.h>
#ifdef _WIN32
#include <io.h>
#define dup2 _dup2
#define STDERR_DEV "NUL"
#else
#include <unistd.h>
#define STDERR_DEV "/dev/null"
#endif
#include <string>

namespace
{
    void trim_inplace(std::string& s) {
        // Trim leading
        s.erase(0, s.find_first_not_of(" \t\n\r\f\v"));
        // Trim trailing
        s.erase(s.find_last_not_of(" \t\n\r\f\v") + 1);
    }
}

void StreamingLLM::InitializeContext(const std::string &userPrompt, const uint32_t n_predict)
{
    prompt_tokens.clear();

    const uint32_t numTokensInPrompt = GetNumTokensInString(userPrompt);

    prompt_tokens.resize(numTokensInPrompt);

    if (llama_tokenize(vocab, userPrompt.c_str(), userPrompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0)
    {
        fprintf(stderr, "%s: error: failed to tokenize the prompt\n", __func__);
    }

    auto contextParameters = llama_context_default_params();

    contextParameters.n_ctx = numTokensInPrompt + n_predict - 1; // Set the context size
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

void StreamingLLM::Initialize()
{
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

        vocab = llama_model_get_vocab(model);
    });

}

gamelib::GameObjectType StreamingLLM::GetGameObjectType()
{
    return gamelib::GameObjectType::game_defined;
}

void StreamingLLM::Update(unsigned long deltaMs)
{
    std::stringstream p;
    p << "It has been " << deltaMs << " milliseconds since we last spoke. What have you been doing?\n";
    std::string userPrompt = p.str();

    constexpr uint32_t n_predict = 15;

    RunWithoutStdErrOutput([&]
    {
        InitializeContext(userPrompt, n_predict);
    });

    auto samplerParameters = llama_sampler_chain_default_params();

    samplerParameters.no_perf = false;

    // Initialize the sampler
    struct llama_sampler * smpl = llama_sampler_chain_init(samplerParameters);

    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());

    // print the prompt token-by-token

    for (auto id : prompt_tokens)
    {
        char buf[128];
        int n = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
        }
        std::string s(buf, n);
        printf("%s", s.c_str());
    }

    // Prepare a batch for the prompt

    auto batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    if (llama_model_has_encoder(model))
    {
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

    // main loop

    const auto numTokensInPrompt = GetNumTokensInString(userPrompt);

    const auto t_main_start = ggml_time_us();
    int n_decode = 0;
    llama_token new_token_id;

    for (int n_pos = 0; n_pos + batch.n_tokens < numTokensInPrompt + n_predict; )
    {
        // Evaluate the current batch with the transformer model
        if (llama_decode(ctx, batch)) {
            fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
        }

        n_pos += batch.n_tokens;

        // sample the next token
        {
            new_token_id = llama_sampler_sample(smpl, ctx, -1);

            // is it an end of generation?
            if (llama_vocab_is_eog(vocab, new_token_id)) {
                break;
            }

            // Convert token to character?
            char buf[128];
            int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
            if (n < 0) {
                fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
            }
            std::string s(buf, n);
            printf("%s", s.c_str());

            fflush(stdout);

            // prepare the next batch with the sampled token
            batch = llama_batch_get_one(&new_token_id, 1);

            n_decode += 1;
        }
    }

    printf("\n");

    const auto t_main_end = ggml_time_us();

    fprintf(stderr, "%s: decoded %d tokens in %.2f s, speed: %.2f t/s\n",
            __func__, n_decode, (t_main_end - t_main_start) / 1000000.0f, n_decode / ((t_main_end - t_main_start) / 1000000.0f));

    fprintf(stderr, "\n");


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

int32_t StreamingLLM::GetNumTokensInString(const std::string &userPrompt) const
{
    return -llama_tokenize(vocab, userPrompt.c_str(), userPrompt.size(), nullptr, 0, true, true);
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
