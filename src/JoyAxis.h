#ifndef JOYAXIS_H
#define JOYAXIS_H

#include <stdint.h>

class JoyAxis
{
    public:
        typedef enum {
            AXIS_NORMAL = 0,
            AXIS_REVERSED = 1,
            AXIS_SLIDER = 2
        } AxisTypes;

        JoyAxis(uint8_t axisType = AXIS_NORMAL, float saturation = 0.95f, float deadZone = 0.001f, float exponential = 1.3f, float smooth = 0.6f, float minPos = 0, float maxPos = 1023.0f);

        void reset();
        float update(float rawValue);
        inline float position() { return output; }
        inline bool isCalibrated() { return midSampleCount == 0; }
		inline bool isRangeValid() { return validRange; }

    protected:
        bool rev;
        bool slider;
        float sat;
        float dz;        
        float expo;
        float alpha;
        float oneMinusAlpha;
        
        float value;
        int midSampleCount;
		bool validRange;
        static const int CALIB_SAMPLES = 1024;
        
        float minRange;
        float midRange;
        float maxRange;

        float outputMin;
        float outputMid;
        float outputMax;
        float output;
        float scale;
        float offset;
        
        float minValidRange;
        float lowRange;
        float lowMid;
        float highRange;
        float highMid;

        void setRanges();
};

#endif // JOYAXIS_H
