// Copyright (c) 2026 munimaru62o. All rights reserved.

#pragma once

#include "Engine/Utils/NonCopyable.h"
#include <cstring>
#include <GLFW/glfw3.h>

struct GLFWwindow;

/**
 * @class InputManager
 * @brief Manages keyboard input states and provides edge-detection for "pressed", "held", and "released" events.
 */
class InputManager final : private NonCopyable
{
public:
    explicit InputManager(GLFWwindow* window)
    {
        m_window = window;
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
        if (!m_window) return;

        // 1. Store the current state as the "previous" state
        std::memcpy(m_previousKeys, m_currentKeys, sizeof(m_currentKeys));

        // 2. Fetch and overwrite with the latest keyboard state
        for (int i = 0; i <= GLFW_KEY_LAST; ++i) {
            m_currentKeys[i] = (glfwGetKey(m_window, i) == GLFW_PRESS) ? 1 : 0;
        }

    }

    /**
     * @brief Checks if the specified key was pressed exactly this frame (Trigger).
     * @param keyCode The GLFW key code (e.g., GLFW_KEY_SPACE).
     * @return True if the key transitioned from UP to DOWN this frame.
     */
    [[nodiscard]] bool IsKeyDown(int keyCode) const
    {
        if (keyCode < 0 || keyCode > GLFW_KEY_LAST) return false;
        return m_currentKeys[keyCode] == 1 && m_previousKeys[keyCode] == 0;
    }

    /**
     * @brief Checks if the specified key is currently being held down (Hold).
     * @param keyCode The GLFW key code (e.g., GLFW_KEY_SPACE).
     * @return True if the key is pressed.
     */
    [[nodiscard]] bool IsKey(int keyCode) const
    {
        if (keyCode < 0 || keyCode > GLFW_KEY_LAST) return false;
        return m_currentKeys[keyCode] == 1;
    }

    /**
     * @brief Checks if the specified key was released exactly this frame (Release).
     * @param keyCode The GLFW key code (e.g., GLFW_KEY_SPACE).
     * @return True if the key transitioned from DOWN to UP this frame.
     */
    [[nodiscard]] bool IsKeyUp(int keyCode) const
    {
        if (keyCode < 0 || keyCode > GLFW_KEY_LAST) return false;
        return m_currentKeys[keyCode] == 0 && m_previousKeys[keyCode] == 1;
    }

private:
    GLFWwindow* m_window = nullptr;
    char m_currentKeys[GLFW_KEY_LAST + 1];
    char m_previousKeys[GLFW_KEY_LAST + 1];
};
