#pragma once

/*
  Specifies a range to clamp a value to
*/
template <class T>
class RangeClamp {
  public:
    RangeClamp();
    RangeClamp(T min, T max);
    // Clamp the given variable to the ranges
    void clamp(T & t);
    // Set the min clamp
    void setMin(T min);
    // Set the max clamp
    void setMax(T max);
    // Unset the min clamp
    void unsetMin();
    // Unset the max clamp
    void unsetMax();
  private:
    // Indicates whether there is a min clamp
    bool m_isMinClamped;
    // Indicates whether there is a max clamp
    bool m_isMaxClamped;
    // Min clamp value
    T m_min;
    // Max clamp value
    T m_max;
};

//----------------------------------------------------------------------------------------
template<class T>
RangeClamp<T>::RangeClamp()
{
  unsetMin();
  unsetMax();
}

//----------------------------------------------------------------------------------------
template<class T>
RangeClamp<T>::RangeClamp(T min, T max)
{
  setMin(min);
  setMax(max);
}

//----------------------------------------------------------------------------------------
template<class T>
void RangeClamp<T>::clamp(T & f) {
  if (m_isMinClamped && f < m_min) {
    f = m_min;
  }
  
  if (m_isMaxClamped && f > m_max) {
    f = m_max;
  }
}

//----------------------------------------------------------------------------------------
template<class T>
void RangeClamp<T>::setMin(T min) {
  m_isMinClamped = true;
  m_min = min;
}

//----------------------------------------------------------------------------------------
template<class T>
void RangeClamp<T>::setMax(T max) {
  m_isMaxClamped = true;
  m_max = max;
}

//----------------------------------------------------------------------------------------
template<class T>
void RangeClamp<T>::unsetMin() {
  m_isMinClamped = false;
}

//----------------------------------------------------------------------------------------
template<class T>
void RangeClamp<T>::unsetMax() {
  m_isMaxClamped = false;
}
