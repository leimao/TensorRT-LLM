/*
 * SPDX-FileCopyrightText: Copyright (c) 2022-2024 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "tensorrt_llm/batch_manager/kvCacheConfig.h"
#include "tensorrt_llm/batch_manager/peftCacheManagerConfig.h"
#include "tensorrt_llm/executor/executor.h"
#include "tensorrt_llm/runtime/common.h"

#include <optional>
#include <utility>
#include <vector>

namespace tensorrt_llm::batch_manager
{

class TrtGptModelOptionalParams
{
    using KvCacheConfig = kv_cache_manager::KvCacheConfig;

public:
    using SizeType32 = tensorrt_llm::runtime::SizeType32;

    explicit TrtGptModelOptionalParams(KvCacheConfig const& kvCacheConfig = KvCacheConfig{},
        bool enableTrtOverlap = false, std::optional<std::vector<SizeType32>> const& deviceIds = std::nullopt,
        bool normalizeLogProbs = true, bool enableChunkedContext = false,
        PeftCacheManagerConfig const& peftCacheManagerConfig = PeftCacheManagerConfig{},
        executor::DecodingConfig decodingConfig = executor::DecodingConfig{}, float gpuWeightsPercent = 1,
        std::optional<SizeType32> maxBeamWidth = std::nullopt, std::optional<SizeType32> maxBatchSize = std::nullopt,
        std::optional<SizeType32> maxNumTokens = std::nullopt,
        executor::SchedulerConfig const& schedulerConfig = executor::SchedulerConfig{},
        executor::ExtendedRuntimePerfKnobConfig const& extendedRuntimePerfKnobConfig
        = executor::ExtendedRuntimePerfKnobConfig{},
        std::optional<executor::DebugConfig> debugConfig = std::nullopt, uint64_t maxSeqIdleMicroseconds = 180000000,
        std::optional<executor::SpeculativeDecodingConfig> specDecConfig = std::nullopt,
        std::optional<executor::GuidedDecodingConfig> guidedDecodingConfig = std::nullopt,
        bool isLeaderInOrchMode = false)
        : kvCacheConfig{kvCacheConfig}
        , enableTrtOverlap{enableTrtOverlap}
        , deviceIds(deviceIds)
        , normalizeLogProbs{normalizeLogProbs}
        , enableChunkedContext{enableChunkedContext}
        , peftCacheManagerConfig(peftCacheManagerConfig)
        , decodingConfig(std::move(decodingConfig))
        , gpuWeightsPercent(gpuWeightsPercent)
        , maxBeamWidth(maxBeamWidth)
        , maxBatchSize(maxBatchSize)
        , maxNumTokens(maxNumTokens)
        , schedulerConfig{schedulerConfig}
        , extendedRuntimePerfKnobConfig(extendedRuntimePerfKnobConfig)
        , debugConfig{std::move(debugConfig)}
        , maxSeqIdleMicroseconds{maxSeqIdleMicroseconds}
        , speculativeDecodingConfig{std::move(specDecConfig)}
        , guidedDecodingConfig{std::move(guidedDecodingConfig)}
        , isLeaderInOrchMode{isLeaderInOrchMode}
    {
        if (guidedDecodingConfig)
        {
            guidedDecodingConfig->validate();
        }
    }

    explicit TrtGptModelOptionalParams(executor::ExecutorConfig const& executorConfig, bool isLeaderInOrchMode)
        : TrtGptModelOptionalParams(KvCacheConfig(executorConfig.getKvCacheConfig()), false,
            executorConfig.getParallelConfig().value_or(executor::ParallelConfig()).getDeviceIds(),
            executorConfig.getNormalizeLogProbs(), executorConfig.getEnableChunkedContext(),
            PeftCacheManagerConfig(executorConfig.getPeftCacheConfig().value_or(executor::PeftCacheConfig())),
            executorConfig.getDecodingConfig().value_or(executor::DecodingConfig{}),
            executorConfig.getGpuWeightsPercent(), executorConfig.getMaxBeamWidth(), executorConfig.getMaxBatchSize(),
            executorConfig.getMaxNumTokens(), executorConfig.getSchedulerConfig(),
            executorConfig.getExtendedRuntimePerfKnobConfig(), executorConfig.getDebugConfig(),
            executorConfig.getMaxSeqIdleMicroseconds(), executorConfig.getSpecDecConfig(),
            executorConfig.getGuidedDecodingConfig(), isLeaderInOrchMode)
    {
    }

    bool operator==(TrtGptModelOptionalParams const& other) const
    {
        return kvCacheConfig == other.kvCacheConfig                                 //
            && enableTrtOverlap == other.enableTrtOverlap                           //
            && deviceIds == other.deviceIds                                         //
            && normalizeLogProbs == other.normalizeLogProbs                         //
            && enableChunkedContext == other.enableChunkedContext                   //
            && decodingConfig == other.decodingConfig                               //
            && gpuWeightsPercent == other.gpuWeightsPercent                         //
            && maxBeamWidth == other.maxBeamWidth                                   //
            && maxBatchSize == other.maxBatchSize                                   //
            && maxNumTokens == other.maxNumTokens                                   //
            && schedulerConfig == other.schedulerConfig                             //
            && extendedRuntimePerfKnobConfig == other.extendedRuntimePerfKnobConfig //
            && debugConfig == other.debugConfig                                     //
            && maxSeqIdleMicroseconds == other.maxSeqIdleMicroseconds               //
            && speculativeDecodingConfig == other.speculativeDecodingConfig         //
            && guidedDecodingConfig == other.guidedDecodingConfig                   //
            && isLeaderInOrchMode == other.isLeaderInOrchMode                       //
            ;
    }

    friend std::ostream& operator<<(std::ostream& os, TrtGptModelOptionalParams const& self);

    KvCacheConfig kvCacheConfig;

    bool enableTrtOverlap;
    std::optional<std::vector<SizeType32>> deviceIds;
    bool normalizeLogProbs;
    bool enableChunkedContext;
    PeftCacheManagerConfig peftCacheManagerConfig;
    executor::DecodingConfig decodingConfig;
    // Percentage of weights on the gpu at runtime
    float gpuWeightsPercent;
    std::optional<SizeType32> maxBeamWidth;
    std::optional<SizeType32> maxBatchSize;
    std::optional<SizeType32> maxNumTokens;
    executor::SchedulerConfig schedulerConfig;
    executor::ExtendedRuntimePerfKnobConfig extendedRuntimePerfKnobConfig;
    std::optional<executor::DebugConfig> debugConfig;
    // Sequence is considered idle if not updated for this amount of time.
    uint64_t maxSeqIdleMicroseconds;
    std::optional<executor::SpeculativeDecodingConfig> speculativeDecodingConfig;
    std::optional<executor::GuidedDecodingConfig> guidedDecodingConfig;
    // This rank is the leader worker in orchestrator mode
    bool isLeaderInOrchMode;
};

} // namespace tensorrt_llm::batch_manager
