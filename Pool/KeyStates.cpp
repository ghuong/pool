#include "KeyStates.hpp"

KeyStates::KeyStates()
{}

bool KeyStates::isKeyHeldDown(int key) {
  if (m_isKeyDown.find(key) == m_isKeyDown.end()) {
    return false;
  }
  else {
    return m_isKeyDown[key];
  }
}

void KeyStates::setKeyPressed(int key) {
  m_isKeyDown[key] = true;
}

void KeyStates::setKeyReleased(int key) {
  m_isKeyDown[key] = false;
}
