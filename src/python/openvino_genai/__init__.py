# Copyright (C) 2024 Intel Corporation
# SPDX-License-Identifier: Apache-2.0

"""openvino genai module namespace, exposing pipelines and configs to create these pipelines."""

import openvino  # add_dll_directory for openvino lib
import os

if hasattr(os, "add_dll_directory"):
    os.add_dll_directory(os.path.dirname(__file__))

from .py_openvino_genai import (
    DecodedResults,
    EncodedResults,
    RawPerfMetrics,
    PerfMetrics,
    StreamerBase,
    get_version,
)

__version__ = get_version()

# VLM pipeline

from .py_openvino_genai import (
    VLMPipeline,
)

# LLM pipeline
from .py_openvino_genai import (
    LLMPipeline, 
    draft_model,
)

# LoRA
from .py_openvino_genai import (
    Adapter,
    AdapterConfig
)

# Generation config
from .py_openvino_genai import (
    GenerationConfig,
    StopCriteria
)

# Tokenizers
from .py_openvino_genai import (
    TokenizedInputs,
    Tokenizer
)

# Whisper
from .py_openvino_genai import (
    WhisperGenerationConfig,
    WhisperPipeline,
    ChunkStreamerBase,
    WhisperRawPerfMetrics,
    WhisperPerfMetrics
)

# Image generation
from .py_openvino_genai import (
    CLIPTextModel,
    CLIPTextModelWithProjection,
    T5EncoderModel,
    UNet2DConditionModel,
    FluxTransformer2DModel,
    SD3Transformer2DModel,
    AutoencoderKL,
    Text2ImagePipeline,
    Image2ImagePipeline,
    InpaintingPipeline,
    Scheduler,
    ImageGenerationConfig,
    Generator,
    CppStdGenerator,
    TorchGenerator,
)

# Continuous batching
from .py_openvino_genai import (
    ContinuousBatchingPipeline,
    GenerationResult,
    SchedulerConfig,
    CacheEvictionConfig,
    AggregationMode,
)
