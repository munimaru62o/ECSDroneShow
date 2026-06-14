// Copyright (c) 2026 munimaru62o. All rights reserved.

#include "Engine/ECS/Coordinator.h"
#include "Engine/ECS/ECSTypes.h"
#include "Engine/ECS/SystemPhases.h"
#include "Engine/ECS/ComponentManager.h"
#include "Engine/ECS/EntityManager.h"
#include "Engine/ECS/SystemManager.h"
#include "Engine/ECS/ThreadCommandContext.h"
#include "Engine/Utils/ThreadPool.h"

#include <memory>
#include <functional>


Coordinator::Coordinator() = default;
Coordinator::~Coordinator() = default;

void Coordinator::Init()
{
    unsigned int threads = std::thread::hardware_concurrency();
    m_ThreadPool = std::make_unique<ThreadPool>(threads > 0 ? threads : 4);

    m_entityManager = std::make_unique<EntityManager>();
    m_componentManager = std::make_unique<ComponentManager>();
    m_systemManager = std::make_unique<SystemManager>();

    m_workerThreadCommandContexts.resize(GetWorkerThreadCount());

    m_sceneEntity = CreateEntity();
}

void Coordinator::EndFrame()
{
    m_componentManager->ClearEvents();
}

Entity Coordinator::CreateEntity()
{
    return m_entityManager->CreateEntity();
}

void Coordinator::DestroyEntity(Entity entity)
{
    Signature signature = m_entityManager->GetSignature(entity);

    m_entityManager->DestroyEntity(entity);
    m_componentManager->EntityDestroyed(entity, signature);
    m_systemManager->EntityDestroyed(entity);
}

Entity Coordinator::GetSceneEntity() const
{
    return m_sceneEntity;
}

uint32_t Coordinator::GetLivingEntityCount() const
{
    return m_entityManager->GetLivingEntityCount();
}

void Coordinator::ApplyCoreComponents(Entity entity)
{
    for (const auto& applier : m_coreComponentAppliers) {
        applier(*this, entity);
    }
}

void Coordinator::InitSystems()
{
    m_systemManager->InitSystems(*this);
}

void Coordinator::UpdatePhase(SystemPhase phase, float dt, double simulationTime)
{
    m_systemManager->UpdateSystems(phase, *this, dt, simulationTime);
    FlushCommands();
}

int Coordinator::GetWorkerThreadCount() const
{
    return m_ThreadPool ? m_ThreadPool->GetWorkerCount() : 0;
}

ThreadCommandContext& Coordinator::GetCommandContext()
{
    int workerId = ThreadPool::GetCurrentWorkerIndex();

    if (workerId < 0) {
        return m_mainThreadCommandContext;
    }

    return m_workerThreadCommandContexts[workerId];
}

void Coordinator::FlushCommands()
{
    FlushWorkerCommands();
    FlushMainThreadCommands();
}

void Coordinator::FlushWorkerCommands()
{
    for (auto& context : m_workerThreadCommandContexts) {
        FlushCommandContext(context);
    }
}

void Coordinator::FlushMainThreadCommands()
{
    FlushCommandContext(m_mainThreadCommandContext);
}

void Coordinator::FlushCommandContext(ThreadCommandContext& context)
{
    for (auto type : m_registeredComponentTypes) {
        if (m_flushers[type]) {
            m_flushers[type](*this, context, type);
        }
    }
}
