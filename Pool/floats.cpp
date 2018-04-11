#include "floats.hpp"

#include <cmath>

using namespace std;

static const float ggEpsilon = 1e-7;

bool isEq(float f1, float f2) {
  return isZero(f1 - f2);
}

bool isPos(float f) {
  return ! isZero(f) && f > 0.0f;
}

bool isNeg(float f) {
  return ! isZero(f) && f < 0.0f;
}

bool isZero(float f) {
  return abs(f) <= ggEpsilon;
}
