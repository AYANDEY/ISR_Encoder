#include "ISR_Encoder.h"

ISR_Encoder::ISR_Encoder(uint8_t PINA, uint8_t PINB, int8_t BUTTON_PIN)
{
    this->encPinA = PINA;
    this->encPinB = PINB;
    this->encButtonPin = BUTTON_PIN;
    pinMode(encPinA, INPUT_PULLUP);
    pinMode(encPinB, INPUT_PULLUP);
    pinMode(encButtonPin, INPUT_PULLUP);
}
ISR_Encoder::ISR_Encoder(uint8_t PINA, uint8_t PINB)
{
    this->encPinA = PINA;
    this->encPinB = PINB;
    this->encButtonPin = -1;
    pinMode(encPinA, INPUT_PULLUP);
    pinMode(encPinB, INPUT_PULLUP);
    this->encoder0Value = 0;
}

void ISR_Encoder::InterruptRoutine()
{

    if (this->enabled)
    {
        unsigned long now = millis();
        this->old_AB <<= 2; // remember previous state

        int8_t ENC_PORT = ((digitalRead(PB12)) ? (1 << 1) : 0) | ((digitalRead(PB13)) ? (1 << 0) : 0);

        this->old_AB |= (ENC_PORT & 0x03); // add current state

        // encoder0Value += ( enc_states[( old_AB & 0x0f )]);
        int8_t currentDirection = (this->enc_states[(old_AB & 0x0f)]); //-1,0 or 1
        if (currentDirection != 0)
        {
            long lastEncoderValue = this->encoder0Value;

            long prevRotaryPosition = this->encoder0Value / this->encoderSteps;

            this->encoder0Value += currentDirection;

            if (ENC_STATE == MAX_REACHED && this->encoder0Value > lastEncoderValue)
            {
                this->encoder0Value = lastEncoderValue;
            }
            else if (ENC_STATE == MIN_REACHED && this->encoder0Value < lastEncoderValue)
            {
                this->encoder0Value = lastEncoderValue;
            }
            long newRotaryPosition = this->encoder0Value / this->encoderSteps;
            /*
            if (this->encoder0Value >= this->encoder0Value_MAX)
            {
                currentDirection = 0;
                this->encoder0Value = this->encoder0Value_MAX;
                this->LastReadValue = this->encoder0Value;
            }
            else if (this->encoder0Value <= this->encoder0Value_MIN)
            {
                this->encoder0Value = this->encoder0Value_MIN;
                this->LastReadValue = this->encoder0Value;
                currentDirection = 0;
            }*/

            if (newRotaryPosition != prevRotaryPosition && this->rotaryAccelerationCoef > 1)
            {

                // additional movements cause acceleration?
                //  at X ms, there should be no acceleration.
                unsigned long accelerationLongCutoffMillis = 200;
                // at Y ms, we want to have maximum acceleration
                unsigned long accelerationShortCutffMillis = 4;

                // compute linear acceleration
                if (currentDirection == lastMovementDirection && currentDirection != 0 && lastMovementDirection != 0)
                {
                    // ... but only of the direction of rotation matched and there
                    // actually was a previous rotation.
                    unsigned long millisAfterLastMotion = now - this->lastMovementAt;

                    if (millisAfterLastMotion < accelerationLongCutoffMillis)
                    {
                        if (millisAfterLastMotion < accelerationShortCutffMillis)
                        {
                            millisAfterLastMotion = accelerationShortCutffMillis; // limit to maximum acceleration
                        }
                        if (currentDirection > 0)
                        {
                            this->encoder0Value += rotaryAccelerationCoef / millisAfterLastMotion;
                        }
                        else
                        {
                            this->encoder0Value -= rotaryAccelerationCoef / millisAfterLastMotion;
                        }
                    }
                }
            }

            this->lastMovementAt = now;
        }

        this->lastMovementDirection = currentDirection;
    }
}

void ISR_Encoder::enableEncoder(bool enable, void (*ISR_callback)(void))
{
    if (enable)
    {
        this->enabled = true;
        // void *ISR_callback=&this->InterruptRoutine;
        attachInterrupt(digitalPinToInterrupt(this->encPinA), ISR_callback, CHANGE);
        attachInterrupt(digitalPinToInterrupt(this->encPinB), ISR_callback, CHANGE);
    }
    else
    {
        this->enabled = false;
        detachInterrupt(this->encPinA);
        detachInterrupt(this->encPinB);
    }
}

bool ISR_Encoder::isValueChanged()
{
    if (((this->encoder0Value - this->LastReadValue) % this->encoderStepFactor) == 0 && (this->encoder0Value - this->LastReadValue) != 0)
    {
        return true;
    }
    else
        return false;
    /*if (this->LastReadValue != this->encoder0Value)
        return true;
    else
        return false;*/
}

void ISR_Encoder::SetValue(float Val, float Increament_)
{
    this->LastReadValue = this->encoder0Value;
    this->ValueSet = Val;
    this->Increament = Increament_;
}

float ISR_Encoder::Read()
{
    long delta = this->encoder0Value - this->LastReadValue;
    if (delta != 0 && (delta % this->encoderStepFactor) == 0)
    {
        this->LastReadValue = this->encoder0Value;
        this->ValueSet += ((int)delta / this->encoderStepFactor) * this->Increament;

        if (this->ValueSet >= this->ValueSetMAX)
        {
            this->ValueSet = this->ValueSetMAX;
            ENC_STATE = MAX_REACHED;

            return this->ValueSetMAX;
        }
        else if (this->ValueSet <= this->ValueSetMIN)
        {
            this->ValueSet = this->ValueSetMIN;
            ENC_STATE = MIN_REACHED;
            return this->ValueSetMIN;
        }
        else
        {
            ENC_STATE = NORMAL;
        }
    }

    return this->ValueSet;
}

void ISR_Encoder::SetRange(float minVal, float maxVal)
{
    this->ValueSetMIN = minVal;
    this->ValueSetMAX = maxVal;
}