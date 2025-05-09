/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include <string>
#include "arm_book_lib.h"

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

AnalogIn ldrfiltered(A1);
AnalogIn ldr(A3);
PwmOut RGBLed[] = {(PB_4), (PA_0), (PD_12)};
DigitalIn nextStepButton(BUTTON1);

float ldrfilteredreading = 0.0;
float ldrreading = 0.0;
static float dutyCycle0 = 0.0f;
static float dutyCycle1 = 0.25f;
static float dutyCycle2 = 0.5f;
static float dutyCycle3 = 0.75f;
static float dutyCycle4 = 1.0f;

typedef enum {
    RGB_LED_RED,
    RGB_LED_GREEN,
    RGB_LED_BLUE,
} lightSystem_t;

void setPeriod( lightSystem_t light, float period );
void setDutyCycle( lightSystem_t light, float dutyCycle );

void pcSerialComStringWrite(const char* str){
 uartUsb.write( str, strlen(str) );
}

char pcSerialComCharRead()
{
    char receivedChar = '\0';
    if( uartUsb.readable() ) {
        uartUsb.read( &receivedChar, 1 );
    }
    return receivedChar;
}


int main(){

        char str[500] = "";

setPeriod( RGB_LED_RED, 0.01f );
    setPeriod( RGB_LED_GREEN, 0.01f );
    setPeriod( RGB_LED_BLUE, 0.01f );

    setDutyCycle( RGB_LED_RED, 0.0f );
    setDutyCycle( RGB_LED_GREEN, 0.0f );
    setDutyCycle( RGB_LED_BLUE, 0.0f );
    
    pcSerialComStringWrite( "Press B1 USER button to change the RGB values\r\n" );

while(true) {
    ldrfilteredreading = ldrfiltered.read();
    ldrreading = ldr.read();
    str[0] = '\0';

    // Determine duty cycle based on LDR value (inverse relationship)
    float dutyCycle;
    if (ldrfilteredreading >= 1.0f) {
        dutyCycle = dutyCycle0; // Very dim
    }else if (ldrfilteredreading >= 0.75f) {
        dutyCycle = dutyCycle1; // Very dim
    } else if (ldrfilteredreading >= 0.5f) {
        dutyCycle = dutyCycle2;
    } else if (ldrfilteredreading >= 0.25f) {
        dutyCycle = dutyCycle3;
    } else {
        dutyCycle = dutyCycle4; // Very bright
    }

    // Set all RGB to same brightness
    setDutyCycle(RGB_LED_RED, dutyCycle);
    setDutyCycle(RGB_LED_GREEN, dutyCycle);
    setDutyCycle(RGB_LED_BLUE, dutyCycle);

    sprintf(str, "LDR --> %d.%04d | DutyCycle --> %.2f\r\n",
            (int)ldrfilteredreading,
            (int)((ldrfilteredreading - (int)ldrfilteredreading) * 10000),
            dutyCycle);
    pcSerialComStringWrite(str);

    ThisThread::sleep_for(1s);
}

}
void setDutyCycle( lightSystem_t light, float dutyCycle )
{
    RGBLed[light].write(dutyCycle);
}

//=====[Implementations of private functions]==================================

void setPeriod( lightSystem_t light, float period )
{
    RGBLed[light].period(period);
}
