#pragma once
class Util
{
public:
  static const float EPSILON;
public:
  Util();
  ~Util();
  static float trim(float value);
  static float clamp(float v, float lo, float hi);
};

