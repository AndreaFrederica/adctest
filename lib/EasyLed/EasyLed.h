#ifndef EasyLED_H
#define EasyLED_H

#include "stm32f4xx_hal.h"

class EasyLed {
public:
	EasyLed();

	void init(GPIO_TypeDef* group, uint16_t pin);
	void toggle();
	void switchOn();
	void switchOff();
	void blink(uint32_t on_duration_ms = 0, uint32_t off_duration_ms = 0);

private:
	GPIO_TypeDef* gpio_group;
	uint16_t gpio_pin;
};

#endif // LED_H
