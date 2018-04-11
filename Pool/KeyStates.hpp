#pragma once

#include <map>

/*
  Keeps track of which keys are held down
*/
class KeyStates {
public:
  KeyStates();
  // Return whether the given GLFW key is held down
  bool isKeyHeldDown(int key);
  // Set the GLFW key as being held down
  void setKeyPressed(int key);
  // Unset the GLFW key as being held down
  void setKeyReleased(int key);
private:
  std::map<int, bool> m_isKeyDown;
};
