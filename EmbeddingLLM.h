#pragma once
#include <string>
#include <vector>
#include "llama.h"
#include <iostream>
#include "common.h"
#include <ctime>
#include <algorithm>

class EmbeddingLLM
{
public:

    int Initialize(const std::string &modelPath = "CompendiumLabs_bge-small-en-v1.5-gguf_bge-small-en-v1.5-q4_k_m.gguf");

    int GetEmbeddingModelDimensions() const;

    void PrintEmbeddingVectors(std::vector<float> embb, int count) const;

    std::vector<float> GetEmbedding(std::string prompt = "Paris is the capital of France.");

private:
    common_init_result llama_init;
    llama_model* model {};
    llama_context* ctx {};
    common_params params {};

	static std::vector<std::string> split_lines(const std::string& s, const std::string& separator = "\n");
    static void batch_add_seq(llama_batch& batch, const std::vector<int32_t>& tokens, llama_seq_id seq_id);
    static void batch_decode(llama_context* ctx, const llama_batch& batch, float* output, int n_seq, int n_embd, int embd_norm);
};