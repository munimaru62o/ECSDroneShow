// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include <cstdint>
#include <memory>

/**
 * @class SimplexNoise
 * @brief A robust wrapper for OpenSimplex2S noise generation.
 *
 * This class utilizes the Pimpl (Pointer to Implementation) idiom to completely hide
 * the external OpenSimplex2S library dependency from the header. This prevents namespace
 * pollution and significantly reduces compile times across the engine.
 * It provides smooth, artifact-free gradient noise suitable for procedural generation,
 * organic movement (like Boids wandering), and wind/turbulence simulations.
 */
class SimplexNoise final : private NonCopyable
{
public:
    [[nodiscard]] explicit SimplexNoise(uint32_t seed = 0);
    ~SimplexNoise();

    [[nodiscard]] float Noise2D(float x, float y) const;
    [[nodiscard]] float Noise3D(float x, float y, float z) const;
    [[nodiscard]] float Noise3D_TimeVaried(float x, float y, float time) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
