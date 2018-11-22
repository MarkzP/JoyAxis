#ifndef JOYAXIS_H
#define JOYAXIS_H

class JoyAxis
{
    public:
        JoyAxis(bool reversed = false, bool centered = true, float saturation = 0.95f, float deadzone = 0.001f, float exponential = 1.3f, float smooth = 0.3f, float minPos = 0, float maxPos = 1023.0f);

        void reset();
        float update(float rawValue);
        inline float position() { return output; }
        inline bool isCalibrated() { return midSampleCount == 0; }

    protected:
        bool rev;
        bool center;
        float sat;
        float dz;        
        float expo;
        float alpha;
        float oneMinusAlpha;
        
        float value;
        int midSampleCount;
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
