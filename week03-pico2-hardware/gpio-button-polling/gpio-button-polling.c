#include <stdio.h> 
#include "pico/stdlib.h" 

#define BUTTON_PIN 15
#define DEBOUNCE_US 20000 

int main() { 
    const uint LED_PIN = PICO_DEFAULT_LED_PIN; 
    gpio_init(LED_PIN); 
    gpio_set_dir(LED_PIN, GPIO_OUT); 
    gpio_init(BUTTON_PIN); 
    gpio_set_dir(BUTTON_PIN, GPIO_IN); 
    gpio_pull_up(BUTTON_PIN); 
    
    stdio_init_all();

    bool previous_raw_pressed = false;
    bool debounced_pressed = false;
    absolute_time_t last_change_time = get_absolute_time();
    bool led_state = false;

    while (true) {

        bool raw_pressed = !gpio_get(BUTTON_PIN);

        if (raw_pressed != previous_raw_pressed) {
            previous_raw_pressed = raw_pressed;
            last_change_time = get_absolute_time();
        }

        if (raw_pressed != debounced_pressed) {
            int64_t stable_us =
                absolute_time_diff_us(last_change_time, get_absolute_time());

            if (stable_us >= DEBOUNCE_US) {
                debounced_pressed = raw_pressed;

                if (debounced_pressed) {
                    led_state ^= true;
                    
                }
            }
        }
        gpio_put(LED_PIN, led_state);
    }


}
