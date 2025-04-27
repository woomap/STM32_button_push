#include "main.h"

#define GPIOB_BASE 0x40020400
#define GPIOD_BASE 0x40020C00
#define GPIO_MODER_OFFSET 0x00
#define GPIO_ODR_OFFSET 0x14
#define GPIO_AFRL_OFFSET 0x20

#define USART1_BASE 0x40011000
#define USART_SR_OFFSET 0x00
#define USART_BRR_OFFSET 0x08
#define USART_CR1_OFFSET 0x0C
#define USART_DR_OFFSET 0x04

void leds_init();
void led_blink(uint8_t pin, uint8_t state);
void uart_init();
void uart_transmit(char data);
char uart_receive();

int main() {
	HAL_Init();
	leds_init();
	uart_init();
	while(1) {
		char data = uart_receive();
		switch(data) {
		case 'a':
			led_blink(12, 1);
			break;
		case 'b':
			led_blink(13, 1);
			break;
		case 'c':
			led_blink(14, 1);
			break;
		case 'd':
			led_blink(15, 1);
			break;
		default:
			led_blink(12, 0);
			led_blink(13, 0);
			led_blink(14, 0);
			led_blink(15, 0);
			break;
		}
	}
	return 0;
}

void leds_init() {
	__HAL_RCC_GPIOD_CLK_ENABLE();
	uint32_t *GPIOD_MODER = (uint32_t*)(GPIOD_BASE + GPIO_MODER_OFFSET);
	*GPIOD_MODER &= ~(0xFF << 24);
	*GPIOD_MODER |= (0b01 << 24) | (0b01 << 26) | (0b01 << 28) | (0b01 << 30);
}

void led_blink(uint8_t pin, uint8_t state) {
	uint32_t *GPIOD_ODR = (uint32_t*)(GPIOD_BASE + GPIO_ODR_OFFSET);
	if (state) {
		*GPIOD_ODR |= (0b01 << pin);
	} else {
		*GPIOD_ODR &= ~(0b01 << pin);
	}
}

void uart_init() {
	__HAL_RCC_GPIOB_CLK_ENABLE();
	uint32_t *GPIOB_MODER = (uint32_t*)(GPIOB_BASE + GPIO_MODER_OFFSET);
	*GPIOB_MODER &= ~(0b1111 << 12);
	*GPIOB_MODER |= (0b10 << 12) | (0b10 << 14);
	uint32_t *GPIOB_AFRL = (uint32_t*)(GPIOB_BASE + GPIO_AFRL_OFFSET);
	*GPIOB_AFRL &= ~(0xFF << 24);
	*GPIOB_AFRL |= (0b111 << 24) | (0b111 << 28);

	__HAL_RCC_USART1_CLK_ENABLE();
	uint32_t *UART_BRR = (uint32_t*)(USART1_BASE + USART_BRR_OFFSET);
	*UART_BRR = (104 << 4) | (3 << 0);
	uint32_t *UART_CR1 = (uint32_t*)(USART1_BASE + USART_CR1_OFFSET);
	*UART_CR1 |= (0b01 << 13) | (0b01 << 12) | (0b01 << 10) | (0 << 9) | (0b01 << 3) | (0b01 << 2);
}

void uart_transmit(char data) {
	uint32_t *UART_DR = (uint32_t*)(USART1_BASE + USART_DR_OFFSET);
	uint32_t *UART_SR = (uint32_t*)(USART1_BASE + USART_SR_OFFSET);
	while (((*UART_SR >> 7) & 1) == 0);
	*UART_DR = data;
	while (((*UART_SR >> 6) & 1) == 0);
}

char uart_receive() {
	uint32_t *UART_DR = (uint32_t*)(USART1_BASE + USART_DR_OFFSET);
	uint32_t *UART_SR = (uint32_t*)(USART1_BASE + USART_SR_OFFSET);
	while (((*UART_SR >> 5) & 1) == 0);
	char data = *UART_DR;
	return data;
}
