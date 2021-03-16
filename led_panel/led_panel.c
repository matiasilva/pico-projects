#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "pico/binary_info.h"

#define RED_OUT_PIN 15
#define GREEN_OUT_PIN 14
#define BLUE_OUT_PIN 13
#define RED_IN_PIN 26
#define GREEN_IN_PIN 27
#define BLUE_IN_PIN 28

#define NUM_COLORS 3
#define ADC_FIRST 26


int main() {
    bi_decl(bi_program_description("LED panel controller using ADC and PWM block"));
    bi_decl(bi_1pin_with_name(RED_OUT_PIN, "Output PWM pin for red component"));
    bi_decl(bi_1pin_with_name(GREEN_OUT_PIN, "Output PWM pin for green component"));
    bi_decl(bi_1pin_with_name(BLUE_OUT_PIN, "Output PWM pin for blue component"));
    bi_decl(bi_1pin_with_name(RED_IN_PIN, "Input ADC pot pin for red component"));
    bi_decl(bi_1pin_with_name(GREEN_IN_PIN, "Input ADC pot pin for green component"));
    bi_decl(bi_1pin_with_name(BLUE_IN_PIN, "Input ADC pot pin for blue component"));


    //stdio_init_all();
    adc_init();

    // arrays so we don't have to repeat everything 3 times
    const uint pwm_pins[] = { RED_OUT_PIN, GREEN_OUT_PIN, BLUE_OUT_PIN };
    const uint adc_pins[] = { RED_IN_PIN, GREEN_IN_PIN, BLUE_IN_PIN };

    for (uint i = 0; i < NUM_COLORS; i++) {
        uint pin = adc_pins[i];
        adc_gpio_init(pin);
    }

    // PWM config:
    // free-running counter from 0 to 65535
    // slice clock increments only every 2 increments of sysclock
    for (uint i = 0; i < NUM_COLORS; i++) {
        uint pin = pwm_pins[i];
        gpio_set_function(pin, GPIO_FUNC_PWM);
        uint slice_num = pwm_gpio_to_slice_num(pin);

        pwm_config config = pwm_get_default_config();
        pwm_config_set_clkdiv(&config, 2.f);
        pwm_init(slice_num, &config, true);
    }

    // note! you can't set the level before init!!

    // 12-bit ADC, 16-bit PWM counter
    const uint adc_max = (1 << 12) - 1;
    const uint pwm_max = (1 << 16) - 1;

    while (1) {
        uint adc_pin, pwm_pin, adc_raw, pwm_output;
        for (uint i = 0; i < NUM_COLORS; i++) {
            // read ADC value -> PWM duty cycle
            adc_pin = adc_pins[i] - ADC_FIRST;
            pwm_pin = pwm_pins[i];

            adc_select_input(adc_pin);
            adc_raw = adc_read();
            // note importance of operation order here
            // integer division will equal 0, if raw/max first
            pwm_output = adc_raw * pwm_max / adc_max;
            pwm_set_gpio_level(pwm_pin, pwm_output);
        }
        sleep_ms(10);
    }
}