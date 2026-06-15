// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

/**
 * @enum SystemPhase
 * @brief Defines the execution order of systems.
 * Systems are executed sequentially based on these phases.
 */
enum class SystemPhase
{
    Input,    // 1. Handling user input
    Logic,    // 2. Game logic (Movement, AI, etc.)
    Physics,  // 3. Physics calculations and collision detection
    Render,   // 4. Rendering process
    Count
};

/**
 * @enum LogicOrder
 * @brief Execution sequence within the Logic Phase.
 */
enum class LogicOrder
{
    PreUpdate               = 0,   // Input processing, frame cleanup, etc.
    Update                  = 10,  // Main user game logic
    ApplyRequest            = 20,  // Converting Requests to Runtime components
    PostUpdate              = 30,  // State transitions (e.g., State Machines)
    EventProcess            = 40,  // Event handling and messaging
};

/**
 * @enum PhysicsOrder
 * @brief Execution sequence within the Physics Phase.
 * This order represents a data-oriented pipeline for physics integration.
 */
enum class PhysicsOrder
{
    PreSimulation           = 0,   // Preliminary setup
    SpatialPartition        = 10,  // Spatial grid/tree construction
    SpatialCache            = 20,  // Caching spatial data for fast lookups
    ForceClear              = 30,  // Resetting accumulated forces
    TargetGenerator         = 40,  // Generating interaction targets
    TargetModifier          = 45,  // Modifying/Filtering targets
    DirectionGenerator      = 50,  // Generating steering directions
    ForceGenerator          = 60,  // Calculating raw forces
    Steering                = 70,  // Steering behaviors
    VelocityGenerator       = 80,  // Applying forces to velocity (F = ma)
    VelocityModifier        = 83,  // Velocity adjustments (Noise, wind, etc.)
    VelocityDamping         = 86,  // Velocity decay (Air resistance, friction)
    Constraint              = 90,  // Physical constraints and collisions
    TransformIntegration    = 100, // Updating Transform based on Velocity
    Rotation                = 110, // Orientation/Rotation updates
    PostTransform           = 120, // Post-transform processing
    MaterialModifier        = 130, // Visual/Material synchronization
    Finalize                = 140, // Cleanup and frame finalization
};

/**
 * @enum RenderOrder
 * @brief Rendering submission order for the pipeline.
 */
enum class RenderOrder
{
    Background              = 0,    // Background rendering
    Opaque                  = 10,   // Opaque objects (Geometry)
    Transparent             = 20,   // Translucent objects and particles
    UI                      = 30,   // UI overlays
};
