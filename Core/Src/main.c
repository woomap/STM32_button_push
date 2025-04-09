#include "main.h"

#define GPIOA_BASE 0x40020000
#define GPIOD_BASE 0x40020C00
#define GPIO_MODER_OFFSET 0x00
#define GPIO_ODR_OFFSET 0x14
#define GPIO_IDR_OFFSET 0x10

#define EXTI_BASE 0x40013C00
#define EXTI_FTSR_OFFSET 0x0C
#define EXTI_RTSR_OFFSET 0x08
#define EXTI_IMR_OFFSET 0x00
#define EXTI_PR_OFFSET 0x14

#define NVIC_ISER0 0xE000E100

void leds_init();
void button_init();
void led_blink(uint8_t pin);
char button_read();
void exti_init();
void simultaneous_blink(uint8_t led1, uint8_t led2, uint8_t led3, int time1, int time2, int time3);

int main() {
	HAL_Init();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	leds_init();
	button_init();
	exti_init();
	simultaneous_blink(12, 13, 15, 1200, 120, 650);

	return 0;
}

void leds_init() {
	uint32_t *GPIOD_MODER = (uint32_t*)(GPIOD_BASE + GPIO_MODER_OFFSET);
	*GPIOD_MODER &= ~(0b11 << 24);
	*GPIOD_MODER |= (0b01 << 24);

	*GPIOD_MODER &= ~(0b11 << 26);
	*GPIOD_MODER |= (0b01 << 26);

	*GPIOD_MODER &= ~(0b11 << 28);
	*GPIOD_MODER |= (0b01 << 28);

	*GPIOD_MODER &= ~(0b11 << 30);
	*GPIOD_MODER |= (0b01 << 30);
}

void button_init() {
	uint32_t *GPIOA_MODER = (uint32_t*)(GPIOA_BASE + GPIO_MODER_OFFSET);
	*GPIOA_MODER &= ~(0b11 << 0);
}

void led_blink(uint8_t pin) {
	uint32_t *GPIOD_ODR = (uint32_t*)(GPIOD_BASE + GPIO_ODR_OFFSET);
	*GPIOD_ODR ^= (0b01 << pin);
}

char button_read() {
	uint32_t *GPIOA_IDR = (uint32_t*)(GPIOA_BASE + GPIO_IDR_OFFSET);
	*GPIOA_IDR &= (0b01 << 0);
	return *GPIOA_IDR;
}

void exti_init() {
	uint32_t *EXTI0_FTSR = (uint32_t*)(EXTI_BASE + EXTI_FTSR_OFFSET);
	*EXTI0_FTSR |= (0b01 << 0);
	uint32_t *EXTI0_RTSR = (uint32_t*)(EXTI_BASE + EXTI_RTSR_OFFSET);
	*EXTI0_RTSR |= (0b01 << 0);
	uint32_t *EXTI0_IMR = (uint32_t*)(EXTI_BASE + EXTI_IMR_OFFSET);
	*EXTI0_IMR |= (0b01 << 0);
	uint32_t *NVIC_ISER6 = (uint32_t*)(NVIC_ISER0);
	*NVIC_ISER6 |= (0b01 << 6);
}

void EXTI0_IRQHandler() {
	led_blink(14);
	uint32_t *EXTI0_PR = (uint32_t*)(EXTI_BASE + EXTI_PR_OFFSET);
	*EXTI0_PR = (0b01 << 0);
}

void simultaneous_blink(uint8_t led1, uint8_t led2, uint8_t led3, int time1, int time2, int time3) {
	int off1 = 0;
	int off2 = 0;
	int off3 = 0;
	while(1) {
		int current_time = HAL_GetTick();
		if (current_time - off1 == time1) {
			led_blink(led1);
			off1 = current_time;
		}
		if (current_time - off2 == time2) {
			led_blink(led2);
			off2 = current_time;
		}
		if (current_time - off3 == time3) {
			led_blink(led3);
			off3 = current_time;
		}
	}
}
