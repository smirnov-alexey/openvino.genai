// Copyright (C) 2023-2024 Intel Corporation
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <memory>
#include <filesystem>

#include <openvino/openvino.hpp>

#include "openvino/genai/scheduler_config.hpp"
#include "openvino/genai/tokenizer.hpp"
#include "openvino/genai/generation_config.hpp"
#include "openvino/genai/generation_handle.hpp"
#include "openvino/genai/llm_pipeline.hpp"
#include "openvino/genai/streamer_base.hpp"
#include "openvino/genai/visibility.hpp"
#include "cache_eviction.hpp"

namespace ov::genai {

/**
 * @brief Contains general pipeline metrics, either aggregated throughout the lifetime of the generation pipeline
 * or measured at the previous generation step.
 */
struct PipelineMetrics {
    /**
     * Number of requests to be processed by the pipeline.
     */
    size_t requests = 0;

    /**
     * Number of requests that were scheduled for processing at the previous step of the pipeline.
     */
    size_t scheduled_requests = 0;

    /**
    * Percentage of KV cache usage in the last generation step.
    */
    float cache_usage = 0.0;

    /**
    * Max KV cache usage during the lifetime of the pipeline in %
    */
    float max_cache_usage = 0.0;

    /**
    * Running average of the KV cache usage during the lifetime of the pipeline, with max window size of 1000 steps
    */
    float avg_cache_usage = 0.0;

    /**
     * Number of tokens scheduled for processing at the previous step of the pipeline.
     */
    size_t total_num_scheduled_tokens;
};

class OPENVINO_GENAI_EXPORTS ContinuousBatchingPipeline {
protected:
    class IContinuousBatchingPipeline;
    class ContinuousBatchingImpl;

    class ContinuousBatchingForSpeculativeDecodingImpl;
    class ContinuousBatchingForPromptLookupImpl;
    class SpeculativeDecodingImpl;
    class PromptLookupImpl;

    friend class ContinuousBatchingForSpeculativeDecodingImpl;
    friend class ContinuousBatchingForPromptLookupImpl;
    friend class SpeculativeDecodingImpl;
    friend class PromptLookupImpl;

    std::shared_ptr<IContinuousBatchingPipeline> m_impl;

    ContinuousBatchingPipeline() = default;

public:
    ContinuousBatchingPipeline(const std::filesystem::path& models_path,
                               const SchedulerConfig& scheduler_config,
                               const std::string& device,
                               const ov::AnyMap& properties = {},
                               const ov::AnyMap& tokenizer_properties = {});

    /**
    * @brief Constructs a ContinuousBatchingPipeline when ov::genai::Tokenizer is initialized manually using file from the different dirs.
    *
    * @param models_path Path to the dir with model, tokenizer .xml/.bin files, and generation_configs.json
    * @param scheduler_config
    * @param tokenizer manually initialized ov::genai::Tokenizer
    * @param device optional device
    * @param properties optional properties
    */
    ContinuousBatchingPipeline(
        const std::filesystem::path& models_path,
        const ov::genai::Tokenizer& tokenizer,
        const SchedulerConfig& scheduler_config,
        const std::string& device,
        const ov::AnyMap& properties = {}
    );

    /**
     * @brief Constructs a ContinuousBatchingPipeline from already existing model and tokenizer.
     * 
     * This constructor allows for the creation of a ContinuousBatchingPipeline using an existing model
     * represented as a string and a weights tensor, along with a manually initialized tokenizer.
     * This is useful when the model and tokenizer are already loaded or created in memory and do not
     * need to be loaded from files.
     *
     * @param model_str A string representation of the model.
     * @param weights_tensor A tensor containing the weights of the model.
     * @param tokenizer A manually initialized ov::genai::Tokenizer.
     * @param scheduler_config Configuration for the scheduler.
     * @param device The device to run the pipeline on (e.g., CPU, GPU).
     * @param properties Optional properties for the pipeline.
     * @param generation_config Optional generation configuration for the pipeline.
     */
    ContinuousBatchingPipeline(
        const std::string& model_str,
        const ov::Tensor& weights_tensor,
        const ov::genai::Tokenizer& tokenizer,
        const SchedulerConfig& scheduler_config,
        const std::string& device,
        const ov::AnyMap& properties = {},
        const ov::genai::GenerationConfig& generation_config = {}
    );

    ov::genai::Tokenizer get_tokenizer();

    ov::genai::GenerationConfig get_config() const;

    /**
     * Allows to get the current pipeline metrics.
     * @return The struct with pipeline metrics for the previous generation step.
     */
    ov::genai::PipelineMetrics get_metrics() const;

    GenerationHandle add_request(uint64_t request_id, const ov::Tensor& input_ids, const ov::genai::GenerationConfig& sampling_params);
    GenerationHandle add_request(uint64_t request_id, const std::string& prompt, const ov::genai::GenerationConfig& sampling_params);

    void step();

    bool has_non_finished_requests();

    // more high level interface, which can process multiple prompts in continuous batching manner
    std::vector<EncodedGenerationResult> generate(const std::vector<ov::Tensor>& input_ids, const std::vector<ov::genai::GenerationConfig>& sampling_params, const ov::genai::StreamerVariant& streamer=std::monostate{});
    std::vector<GenerationResult> generate(const std::vector<std::string>& prompts, const std::vector<ov::genai::GenerationConfig>& sampling_params, const ov::genai::StreamerVariant& streamer=std::monostate{});

    /**
    * @brief start chat with keeping history in kv cache.
    * @param system_message optional system message.
    */
    void start_chat(const std::string& system_message = {});

    /**
    * @brief finish chat and clear kv cache.
    */
    void finish_chat();
};
}
