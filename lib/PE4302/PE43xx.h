#ifndef PE43xx_h
#define PE43xx_h

#include <stm32f4xx_hal.h>

// Define types of sensors.
#define PE4302 02
// untested as I don't own these
#define PE4306 06
#define PE4312 12

void PE43xx_f(uint8_t type);
void begin();
int setLevel(float level);
void _writeLevel();
float getLevel();
float getMax();
float getStep();



void PE43xx_init(uint8_t type, float level);
void PE43xx_changelevel(float level);



// Define GPIO Port and Pins for PE4302
#define _pin_le_GPIO_Port   GPIOA
#define _pin_le_Pin         GPIO_PIN_2
#define _pin_clock_GPIO_Port  GPIOA
#define _pin_clock_Pin        GPIO_PIN_3
#define _pin_data_GPIO_Port  GPIOA
#define _pin_data_Pin        GPIO_PIN_4

#endif