// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/Math/SimplexNoise.h"

#include <OpenSimplex2S.hpp>


struct SimplexNoise::Impl
{
    Impl(uint32_t seed) :
        noiseInstance(seed)
    {};
    OpenSimplex2S noiseInstance;
};

SimplexNoise::SimplexNoise(uint32_t seed)
    : pImpl(std::make_unique<Impl>(seed))
{}

SimplexNoise::~SimplexNoise() = default;


float SimplexNoise::Noise2D(float x, float y) const
{
    return static_cast<float>(pImpl->noiseInstance.noise2(x, y));
}

float SimplexNoise::Noise3D(float x, float y, float z) const
{
    return static_cast<float>(pImpl->noiseInstance.noise3_Classic(x, y, z));
}

float SimplexNoise::Noise3D_TimeVaried(float x, float y, float time) const
{
    return static_cast<float>(pImpl->noiseInstance.noise3_XYBeforeZ(x, y, time));
}
