#include "CountdownTimer.hpp"

//----------------------------------------------------------------------------------------
CountdownTimer::CountdownTimer()
  : m_timer(0.0f)
{}

//----------------------------------------------------------------------------------------
void CountdownTimer::set(float timer) {
  m_timer = timer;
}

//----------------------------------------------------------------------------------------
void CountdownTimer::tick(float deltaTime) {
  m_timer -= deltaTime;
  if (m_timer < 0.0f) {
    m_timer = 0.0f;
  }
}

//----------------------------------------------------------------------------------------
bool CountdownTimer::isTicking() {
  return m_timer > 0.0f;
}

//----------------------------------------------------------------------------------------
float CountdownTimer::getTime() {
  return m_timer;
}
