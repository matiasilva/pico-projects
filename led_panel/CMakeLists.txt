add_executable(led_panel
    led_panel.c
)

pico_add_extra_outputs(led_panel)
#pico_enable_stdio_uart(blink 1)

target_link_libraries(led_panel pico_stdlib hardware_pwm hardware_adc)
