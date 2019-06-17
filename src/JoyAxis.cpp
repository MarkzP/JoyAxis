#include "JoyAxis.h"

#include <Arduino.h>

// https://github.com/ekmett/approximate/blob/master/cbits/fast.c
static float powf_fast(float a, float b) {
  union { float d; int x; } u = { a };
  u.x = (int)(b * (u.x - 1064866805) + 1064866805);
  return u.d;
}

JoyAxis::JoyAxis(uint8_t axisType, float saturation, float deadZone, float exponential, float smooth, float minPos, float maxPos) {
  rev = axisType & AXIS_REVERSED;
  slider = axisType & AXIS_SLIDER;
  sat = constrain(saturation, 0.5f, 1.0f);
  dz = constrain(deadZone, 0.0f, 1.0f);
  expo = constrain(exponential, 0.0f, 3.0f);
  alpha = constrain(smooth, 0.001f, 1.0f);
  oneMinusAlpha = 1.0f - alpha;
  outputMin = min(minPos, maxPos);
  outputMax = max(minPos, maxPos);
  outputMid = outputMin + ((outputMax - outputMin) * 0.5f);

  reset();
}

void JoyAxis::reset() {

  validRange = false;
  midSampleCount = CALIB_SAMPLES;
  midRange = 0;
  minRange = 3.4028235E+38;
  maxRange = -3.4028235E+38;
}

float JoyAxis::update(float rawValue) {

    if (midSampleCount > 0) {

      // Perform axis calibration
      // by averaging many samples
      midRange += rawValue;

      // The min & max will be used to determine valid range
      minRange = min(rawValue, minRange);
      maxRange = max(rawValue, maxRange);

      midSampleCount--;
      if (midSampleCount == 0) {

        midRange /= CALIB_SAMPLES;

        // Initialize filter with average value;
        value = midRange;

        // Minimum valid range is at least 64 times the variation around center
        minValidRange = (maxRange - minRange) * 64.0f;

        // Recompute ranges coefficients
        setRanges();
      }
      else return output = outputMid;
    }

    // Low pass filtering of raw value
    value = (value * oneMinusAlpha) + (rawValue * alpha);

    // Update range as needed
    if (value < minRange) {
      minRange = value;
      setRanges();
    }
    if (value > maxRange) {
      maxRange = value;
      setRanges();
    }

    // Lock output to center if range is insufficient
    if ((maxRange - minRange) < minValidRange) return output = outputMid;
	
	validRange = true;

    float normal = 0.0f;
    if (slider) {
        normal = value - offset;
    }
    else {
      // Convert value to -1.0/+1.0 range
      // apply expo as needed
      if (value < lowMid)       normal = -powf_fast((lowMid - value) * lowRange, expo);
      else if (value > highMid) normal = powf_fast((value - highMid) * highRange, expo);
    }

    // Scale back to Joystick output range
    output = (normal * scale) + outputMid;

    // Constrain output (saturation can push the output outside of output bounds)
    output = constrain(output, outputMin, outputMax);

    return output;
}

void JoyAxis::setRanges() {

  float outputRange = outputMax - outputMin;
  float inputRange = maxRange - minRange;
  float dzoffset = (inputRange * dz) * 0.5f;
  float satoffset = (inputRange * (1.0f - sat)) * 0.5f;
  float high = maxRange - satoffset;
  float low = minRange + satoffset;

  if (slider) {
    scale = (rev ? -outputRange : outputRange) / (high - low);
    offset = (inputRange * 0.5f) + minRange;
  }
  else {
    lowMid = midRange - dzoffset;
    highMid = midRange + dzoffset;
    lowRange = 1.0f / (lowMid - low);
    highRange = 1.0f / (high - highMid);
    scale = (rev ? -outputRange : outputRange) * 0.5f;
    offset = 0;
  }
}
