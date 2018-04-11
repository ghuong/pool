#include "MouseStates.hpp"

#include <glm/glm.hpp>

using namespace glm;

//----------------------------------------------------------------------------------------
MouseStates::MouseStates()
{
  unlockCursor();
}

//----------------------------------------------------------------------------------------
glm::vec2 MouseStates::getCursorPos() {
  return m_cursorPosition;
}

//----------------------------------------------------------------------------------------
glm::vec2 MouseStates::getNormalizedCursorPos() {
  return m_normalizedCursorPosition;
}

//----------------------------------------------------------------------------------------
void MouseStates::setCursorPos(glm::vec2 pos, glm::vec2 normalizedPos) {
  if (! m_isCursorLocked) {
    m_cursorPosition = pos;
    m_normalizedCursorPosition = normalizedPos;
  }
}

//----------------------------------------------------------------------------------------
void MouseStates::unlockCursor() {
  m_isCursorLocked = false;
}

//----------------------------------------------------------------------------------------
void MouseStates::lockCursor() {
  m_isCursorLocked = true;
}

//----------------------------------------------------------------------------------------
bool MouseStates::isCursorLocked() {
  return m_isCursorLocked;
}

//----------------------------------------------------------------------------------------
int MouseStates::getCursorMode() {
  return m_cursorMode;
}

//----------------------------------------------------------------------------------------
void MouseStates::setCursorMode(int mode) {
  m_cursorMode = mode;
}
