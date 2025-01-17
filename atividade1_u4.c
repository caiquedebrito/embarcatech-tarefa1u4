#include "pico/stdlib.h"
#include <stdio.h>

// Define os pinos dos LEDs
#define LED_GREEN 11
#define LED_BLUE 12
#define LED_RED 13

//Função para ligar os LEDs
void turn_on_led(bool red, bool blue, bool green){
    gpio_put(LED_RED, red);
    gpio_put(LED_BLUE, blue);
    gpio_put(LED_GREEN, green);
}

// Função para desligar todos os LEDs
void turn_off_all_leds() {
    gpio_put(LED_GREEN, 0);
    gpio_put(LED_BLUE, 0);
    gpio_put(LED_RED, 0); 
}