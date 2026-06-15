// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include <DxLib.h>
#include <cstring>

/**
 * @class InputManager
 * @brief Manages keyboard input states and provides edge-detection for "pressed", "held", and "released" events.
 *
 * By calling Update() at the very beginning of every frame, this class retrieves the latest
 * keyboard state (all 256 keys) from DxLib. It then compares this current state against the
 * previous frame's state to accurately determine precise input triggers (edge detection),
 * enabling robust and responsive gameplay controls.
 */
class InputManager final : private NonCopyable
{
public:
    InputManager()
    {
        std::memset(m_currentKeys, 0, sizeof(m_currentKeys));
        std::memset(m_previousKeys, 0, sizeof(m_previousKeys));
    }
    ~InputManager() = default;

    /**
     * @brief Refreshes the internal input buffers. Must be called exactly once per frame,
     * prior to any game logic processing.
     */
    void Update()
    {
        // 1. Store the current state as the "previous" state
        std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));

        // 2. Fetch and overwrite with the latest keyboard state from DxLib
        GetHitKeyStateAll(m_currentKeys);
    }

    /**
     * @brief Checks if the specified key was pressed exactly this frame (Trigger).
     * @param keyCode The DxLib key code (e.g., KEY_INPUT_SPACE).
     * @return True if the key transitioned from UP to DOWN this frame.
     */
    [[nodiscard]] bool IsKeyDown(int keyCode) const
    {
        return m_currentKeys[keyCode] == 1 && m_previousKeys[keyCode] == 0;
    }

    /**
     * @brief Checks if the specified key is currently being held down (Hold).
     * @param keyCode The DxLib key code (e.g., KEY_INPUT_SPACE).
     * @return True if the key is pressed.
     */
    [[nodiscard]] bool IsKey(int keyCode) const
    {
        return m_currentKeys[keyCode] == 1;
    }

    /**
     * @brief Checks if the specified key was released exactly this frame (Release).
     * @param keyCode The DxLib key code (e.g., KEY_INPUT_SPACE).
     * @return True if the key transitioned from DOWN to UP this frame.
     */
    [[nodiscard]] bool IsKeyUp(int keyCode) const
    {
        return m_currentKeys[keyCode] == 0 && m_previousKeys[keyCode] == 1;
    }

private:
    char m_currentKeys[256];
    char m_previousKeys[256];
};
