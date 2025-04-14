#pragma once
#include "Arduino.h"

class ISR_Encoder
{
public:
    ISR_Encoder(uint8_t PINA, uint8_t PINB);
    ISR_Encoder(uint8_t PINA, uint8_t PINB, int8_t BUTTON_PIN);
    void enableEncoder(bool enable, void (*ISR_callback)(void));
    bool isValueChanged();
    float Read();
    void SetValue(float Val, float Increament_);
    void InterruptRoutine();
    void SetRange(float maxVal, float minVal);
    bool enabled = false;
    long rotaryAccelerationCoef = 10;
    int encoderStepFactor = 2;
    float Increament = 0;

private:
    typedef enum
    {
        MAX_REACHED = 0,
        MIN_REACHED,
        NORMAL,
    } encoderState;
    encoderState ENC_STATE;

    uint8_t encPinA;
    uint8_t encPinB;
    int8_t encButtonPin;
    int8_t enc_states[16] = {0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0};
    int8_t old_AB = 0;
    int8_t lastMovementDirection = 0;
    unsigned long lastMovementAt = 0, now = 0, diff = 0;
    bool ValueChanged = 0;
    long encoder0Value;
    int encoderSteps = 2;
    long LastReadValue = 0;
    long EncoderValuePrev = 0; //, EncoderValueNow = 0;
    float ValueSet;
    long encoder0Value_MAX = UINT16_MAX / 2;
    long encoder0Value_MIN = -UINT16_MAX / 2;
    float ValueSetMAX;
    float ValueSetMIN;
};