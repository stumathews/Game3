#pragma once
#include <string>
#include <vector>
#include "llama.h"
#include <iostream>



class LLM
{
public:

	/**
	 * @param model_path path to the model gguf file
	 * @param ngl number of layers to offload to the GPU
	 */
	void Initialize(const std::string &model_path = "TinyLlama_TinyLlama-1.1B-Chat-v0.6_ggml-model-q4_0.gguf", int ngl = 99);

	/**
	 * @param userPrompt prompt to generate text from
	 * @param n_predict number of tokens to predict
	 */
	std::string Infer(const std::string &userPrompt, int n_predict);


private:
	llama_sampler* smpl {};
    llama_context* ctx {};
    llama_model* model {};
    const llama_vocab* vocab {};
	bool initialized = false;
	std::vector<llama_token> prompt_tokens;
	int numTokensInPrompt {0};

	void InitializeModel(const std::string &modelPath, const int ngl);
	void InitializeContext(const std::string &userPrompt, const int n_predict);
};

