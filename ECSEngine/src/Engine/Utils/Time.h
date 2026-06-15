// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

/**
 * @class Time
 * @brief A static utility class that manages the in-game simulation time.
 *
 * This class tracks the elapsed in-game time (Simulation Time) rather than the
 * actual wall-clock time (Real Time). By accumulating the scaled delta time every frame,
 * it guarantees a pure simulation timeline that correctly reflects the effects of
 * pausing, hit-stops, and slow-motion (via timeScale).
 */
class Time
{
public:
    Time() = delete;

public:
    static inline float timeScale = 1.0f;

    static void UpdateFrame(float dt)
    {
        // Calculate scaled delta time for the current frame
        s_deltaTime = dt * timeScale;
        s_totalTime += static_cast<double>(s_deltaTime);
    }
    [[nodiscard]] static float GetDeltaTime() { return s_deltaTime; }
    [[nodiscard]] static double GetTotalTime() { return s_totalTime; }

    static void UpdateFixed(float fixedDt)
    {
        // Calculate scaled fixed delta time
        s_fixedDeltaTime = fixedDt * timeScale;
        s_fixedTotalTime += static_cast<double>(s_fixedDeltaTime);
    }
    [[nodiscard]] static float GetFixedDeltaTime() { return s_fixedDeltaTime; }
    [[nodiscard]] static double GetFixedTotalTime() { return s_fixedTotalTime; }

private:
    static inline double s_totalTime = 0.0;
    static inline float s_deltaTime = 0.0f;

    static inline double s_fixedTotalTime = 0.0;
    static inline float s_fixedDeltaTime = 0.0f;
};
