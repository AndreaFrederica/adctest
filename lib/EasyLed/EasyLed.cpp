#include <EasyLed.h>
#include "stm32f4xx_hal.h"

// void LED_Init(void) {
// 	__HAL_RCC_GPIOC_CLK_ENABLE();
// 	__HAL_RCC_GPIOB_CLK_ENABLE();

// 	GPIO_InitTypeDef GPIO_InitStruct;
// 	GPIO_InitStruct.Pin = GPIO_PIN_5;
// 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
// 	GPIO_InitStruct.Pull = GPIO_PULLUP;
// 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
// 	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

// 	GPIO_InitTypeDef GPIO_InitStruct;
// 	GPIO_InitStruct.Pin = GPIO_PIN_2;
// 	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
// 	GPIO_InitStruct.Pull = GPIO_PULLUP;
// 	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
// 	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
// 	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_5,GPIO_PIN_RESET);
// 	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2,GPIO_PIN_RESET);
// }

// class EasyLed {
// public:
// 	EasyLed()
// 	    : gpio_group(nullptr)
// 	    , gpio_pin(0) {
// 		// 默认构造函数，确保在初始化之前 GPIO 变量被设置为有效值
// 	}

// 	void init(GPIO_TypeDef* group, uint16_t pin) {
// 		gpio_group = group;
// 		gpio_pin = pin;
// 		// 初始化 GPIO
// 		GPIO_InitTypeDef GPIO_InitStruct;
// 		GPIO_InitStruct.Pin = gpio_pin;
// 		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
// 		GPIO_InitStruct.Pull = GPIO_PULLUP;
// 		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
// 		HAL_GPIO_Init(gpio_group, &GPIO_InitStruct);
// 		switchOff();
// 	}

// 	void toggle() {
// 		// 切换 LED 状态
// 		HAL_GPIO_TogglePin(gpio_group, gpio_pin);
// 	}

// 	void switchOn() {
// 		// 打开 LED
// 		HAL_GPIO_WritePin(gpio_group, gpio_pin, GPIO_PIN_RESET);
// 	}

// 	void switchOff() {
// 		// 关闭 LED
// 		HAL_GPIO_WritePin(gpio_group, gpio_pin, GPIO_PIN_SET);
// 	}

// 	void blink(uint32_t on_duration_ms = 0, uint32_t off_duration_ms = 0) {
// 		// 闪烁 LED
// 		switchOn();
// 		if (on_duration_ms > 0) {
// 			HAL_Delay(on_duration_ms);
// 		}
// 		switchOff();
// 		if (off_duration_ms > 0) {
// 			HAL_Delay(off_duration_ms);
// 		}
// 	}

// private:
// 	GPIO_TypeDef* gpio_group;
// 	uint16_t gpio_pin;
// };

EasyLed::EasyLed()
    : gpio_group(nullptr), gpio_pin(0) {}

void EasyLed::init(GPIO_TypeDef* group, uint16_t pin) {
    gpio_group = group;
    gpio_pin = pin;
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = gpio_pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(gpio_group, &GPIO_InitStruct);
    switchOff();
}

void EasyLed::toggle() {
    HAL_GPIO_TogglePin(gpio_group, gpio_pin);
}

void EasyLed::switchOn() {
    HAL_GPIO_WritePin(gpio_group, gpio_pin, GPIO_PIN_RESET);
}

void EasyLed::switchOff() {
    HAL_GPIO_WritePin(gpio_group, gpio_pin, GPIO_PIN_SET);
}

void EasyLed::blink(uint32_t on_duration_ms, uint32_t off_duration_ms) {
    switchOn();
    if (on_duration_ms > 0) {
        HAL_Delay(on_duration_ms);
    }
    switchOff();
    if (off_duration_ms > 0) {
        HAL_Delay(off_duration_ms);
    }
}
