/**
 * @brief EXAMPLE - BLINK_C
 *        Simple example to initialise the built-in LED on
 *        the Raspberry Pi Pico and then flash it forever. 
 * 
 * @return int  Application return code (zero for success).
 */
#include "pico/stdlib.h"

void sub_Routine(int LED_PIN, int LED_DELAY)
{
    // Do forever...
    while (true)
    {

        // Toggle the LED on and then sleep for delay period
        gpio_put(LED_PIN, 1);
        sleep_ms(LED_DELAY);

        // Toggle the LED off and then sleep for delay period
        gpio_put(LED_PIN, 0);
        sleep_ms(LED_DELAY);
    }
}

int main()
{

    // Specify the PIN number and sleep delay
    const uint LED_PIN = 25;
    const int LED_DELAY = 500;

    // Setup the LED pin as an output.
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    //calling sub_Routine() and passing led_pin and led_delay as parameters
    sub_Routine(LED_PIN, LED_DELAY); 

    // Should never get here due to infinite while-loop.
    return 0;
}