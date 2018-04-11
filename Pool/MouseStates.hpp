#pragma once

#include "KeyStates.hpp"

#include <glm/glm.hpp>

/*
  Container for Mouse-related state
*/
class MouseStates : public KeyStates {
  public:
    MouseStates();

    // Get the cached cursor position in DCS
    glm::vec2 getCursorPos();
    // Get the cached normalized cursor position in NDCS
    glm::vec2 getNormalizedCursorPos();
    // Cache the cursor position (and its NDCS version)
    void setCursorPos(glm::vec2 pos, glm::vec2 normalizedPos);
    // Set a flag indicating that the cursor position should be locked
    void lockCursor();
    // Unset the flag to lock cursor
    void unlockCursor();
    // Return whether the cursor should be locked
    bool isCursorLocked();
    // Get the cached GLFW cursor mode
    int getCursorMode();
    // Cache the GLFW cursor mode
    void setCursorMode(int mode);

  protected:
    // Cache of the cursor position
    glm::vec2 m_cursorPosition;
    // Cache of the cursor position in NDCS
    glm::vec2 m_normalizedCursorPosition;
    // Flag indicating whether the cursor position should be locked
    bool m_isCursorLocked;
    // GLFW cursor mode
    int m_cursorMode;
};
