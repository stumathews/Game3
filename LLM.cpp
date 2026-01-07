#include "LLM.h"

void LLM::Initialize(const std::string &model_path, const int ngl)
{
    // load dynamic backends
    ggml_backend_load_all();

    InitializeModel(model_path, ngl);

    vocab = llama_model_get_vocab(model);
}

void LLM::Infer(const std::string &userPrompt, const int n_predict)
{
    InitializeContext(userPrompt, n_predict);

    auto samplerParameters = llama_sampler_chain_default_params();

    samplerParameters.no_perf = false;

    // Initialize the sampler
    smpl = llama_sampler_chain_init(samplerParameters);

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

void LLM::InitializeModel(const std::string &modelPath, const int ngl)
{
    auto modelParameters = llama_model_default_params();

    modelParameters.n_gpu_layers = ngl;

    model = llama_model_load_from_file(modelPath.c_str(), modelParameters);

    if (model == nullptr)
    {
        fprintf(stderr, "%s: error: unable to load model\n", __func__);
    }
}

void LLM::InitializeContext(const std::string &userPrompt, const int n_predict)
{

    prompt_tokens.clear();

    numTokensInPrompt = -llama_tokenize(vocab, userPrompt.c_str(), userPrompt.size(), nullptr, 0, true, true);

    prompt_tokens.resize(numTokensInPrompt);

    if (llama_tokenize(vocab, userPrompt.c_str(), userPrompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0)
    {
        fprintf(stderr, "%s: error: failed to tokenize the prompt\n", __func__);
    }

    auto contextParameters = llama_context_default_params();

    contextParameters.n_ctx = numTokensInPrompt + n_predict - 1; // Set the context size
    contextParameters.n_batch = numTokensInPrompt; // Set the maximum number of tokens that can be processed in a single call to llama_decode
    contextParameters.no_perf = false; // Enable performance counters

    // Initialize the context
    ctx = llama_init_from_model(model, contextParameters);

    if (ctx == nullptr)
    {
        fprintf(stderr, "%s: error: failed to create the llama_context\n", __func__);
    }
}
