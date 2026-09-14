#include <stdio.h> 
#include "pico/stdlib.h" 

#define BUTTON_PIN 15 

int main() { 
    const uint LED_PIN = PICO_DEFAULT_LED_PIN; 
    gpio_init(LED_PIN); 
    gpio_set_dir(LED_PIN, GPIO_OUT); 
    gpio_init(BUTTON_PIN); 
    gpio_set_dir(BUTTON_PIN, GPIO_IN); 
    gpio_pull_up(BUTTON_PIN); 
    
    stdio_init_all();

    bool led_state = false;
    bool previous_pressed = false;

    while (true) {
        bool pressed = !gpio_get(BUTTON_PIN);

        if (pressed != previous_pressed) {
            sleep_ms(20);

            pressed = !gpio_get(BUTTON_PIN);

            if (pressed && !previous_pressed) {
            led_state ^= true;
            }

            previous_pressed = pressed;
        }

        gpio_put(LED_PIN, led_state);
    }
}