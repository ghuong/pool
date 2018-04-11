#pragma once

class CountdownTimer {
  public:
    CountdownTimer();
    void set(float timer);
    void tick(float deltaTime);
    bool isTicking();
    float getTime();

  protected:
    float m_timer;
};
