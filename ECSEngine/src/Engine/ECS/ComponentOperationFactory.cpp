// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/ECS/ComponentOperationFactory.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/Math/Constants.h"

#include <random>
#include <cassert>


uint32_t ComponentOperationFactory::HashCombine(uint32_t a, uint32_t b)
{
    return a ^ (b + MathConstants::HASH_GOLDEN_RATIO_32 + (a << 6) + (a >> 2));
}

uint32_t ComponentOperationFactory::GenerateSeed(ComponentTrait::SeedPolicy policy, Entity entity, const ComponentOperationContext& ctx)
{
    switch (policy) {
        case ComponentTrait::SeedPolicy::None:
        {
            return 0;
        }
        case ComponentTrait::SeedPolicy::Entity:
        {
            return entity * MathConstants::HASH_GOLDEN_RATIO_32;
        }
        case ComponentTrait::SeedPolicy::EntityAndCreationTime:
        {
            uint32_t entityHash = static_cast<uint32_t>(entity);
            uint32_t timeHash = static_cast<uint32_t>(ctx.creationTime * 1000.0);
            return HashCombine(entityHash, timeHash);
        }
        case ComponentTrait::SeedPolicy::Random:
        {
            static std::random_device rd;
            return rd();
        }
    }
    assert(false);
    return 0;
}
