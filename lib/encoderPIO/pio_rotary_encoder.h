#ifndef PIO_ROTARY_ENCODER_H
#define PIO_ROTARY_ENCODER_H

#include <Arduino.h>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/irq.h"

#include "pio_rotary_encoder.pio.h"

// class to read the rotation of the rotary encoder PIO0
class RotaryEncoder0
{
public:
    // constructor
    // rotary_encoder_A is the pin for the A of the rotary encoder.
    // The B of the rotary encoder has to be connected to the next GPIO.
    RotaryEncoder0(uint rotary_encoder_A, uint rotary_encoder_B)
    {
        delay(10);
        PIO pio_0 = pio0;
        
        // state machine 0
        uint8_t sm0 = 0;
        // configure the used pins as input with pull up
        pio_gpio_init(pio_0, rotary_encoder_A);
        gpio_set_pulls(rotary_encoder_A, true, false);
        pio_gpio_init(pio_0, rotary_encoder_B);
        gpio_set_pulls(rotary_encoder_B, true, false);
        // load the pio program into the pio memory
        uint offset = pio_add_program(pio_0, &pio_rotary_encoder_program);
        // make a sm config
        pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
        // set the 'in' pins
        sm_config_set_in_pins(&c, rotary_encoder_A);
        // set shift to left: bits shifted by 'in' enter at the least
        // significant bit (LSB), no autopush
        sm_config_set_in_shift(&c, false, false, 0);
        // set the 'out' pins
        // set the IRQ handler
        irq_set_exclusive_handler(PIO0_IRQ_0, pio_irq_handler0);
        // enable the IRQ
        irq_set_enabled(PIO0_IRQ_0, true);
        pio0_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
        // init the sm.
        // Note: the program starts after the jump table -> initial_pc = 16
        pio_sm_init(pio_0, sm0, 16, &c);
        // enable the sm
        pio_sm_set_enabled(pio_0, sm0, true);
        delay(10);
    }

    // set the current rotation to a specific value
    void set_rotation0(int _rotation)
    {
        rotation0 = _rotation;
    }

    // get the current rotation
    int get_rotation0(void)
    {
        return rotation0;
    }

private:
    static void pio_irq_handler0()
    {
        // test if irq 0 was raised
        if (pio0_hw->irq & 1)
        {
            rotation0 = rotation0 - 1;
        }
        // test if irq 1 was raised
        if (pio0_hw->irq & 2)
        {
            rotation0 = rotation0 + 1;
        }
        // clear both interrupts
        pio0_hw->irq = 3;
    }


    // the pio instance
    PIO pio_0;

    // the state machine
    uint sm0;

    // the current location of rotation
    static int rotation0;
};

// class to read the rotation of the rotary encoder
class RotaryEncoder1
{
public:
    // constructor
    // rotary_encoder_A is the pin for the A of the rotary encoder.
    // The B of the rotary encoder has to be connected to the next GPIO.
    RotaryEncoder1(uint rotary_encoder_A, uint rotary_encoder_B)
    {
        delay(10);
        PIO pio_1 = pio1;
        
        // state machine 0
        uint8_t sm1 = pio_claim_unused_sm(pio_1, true);
        // configure the used pins as input with pull up
        pio_gpio_init(pio_1, rotary_encoder_A);
        gpio_set_pulls(rotary_encoder_A, true, false);
        pio_gpio_init(pio_1, rotary_encoder_B);
        gpio_set_pulls(rotary_encoder_B, true, false);
        // load the pio program into the pio memory
        uint offset = pio_add_program(pio_1, &pio_rotary_encoder_program);
        // make a sm config
        pio_sm_config c = pio_rotary_encoder_program_get_default_config(offset);
        // set the 'in' pins
        sm_config_set_in_pins(&c, rotary_encoder_A);
        // set shift to left: bits shifted by 'in' enter at the least
        // significant bit (LSB), no autopush
        sm_config_set_in_shift(&c, false, false, 0);
        // set the 'out' pins
        // set the IRQ handler
        irq_set_exclusive_handler(PIO1_IRQ_0, pio_irq_handler1);
        // enable the IRQ
        irq_set_enabled(PIO1_IRQ_0, true);
        pio1_hw->inte0 = PIO_IRQ0_INTE_SM0_BITS | PIO_IRQ0_INTE_SM1_BITS;
        // init the sm.
        // Note: the program starts after the jump table -> initial_pc = 16
        pio_sm_init(pio_1, sm1, offset + 16, &c);
        // enable the sm
        pio_sm_set_enabled(pio_1, sm1, true);
        delay(10);
    }

    // set the current rotation to a specific value
    void set_rotation1(int _rotation)
    {
        rotation1 = _rotation;
    }

    // get the current rotation
    int get_rotation1(void)
    {
        return rotation1;
    }

private:
    static void pio_irq_handler1()
    {
        // test if irq 0 was raised
        if (pio1_hw->irq & 1)
        {
            rotation1 = rotation1 - 1;
        }
        // test if irq 1 was raised
        if (pio1_hw->irq & 2)
        {
            rotation1 = rotation1 + 1;
        }
        // clear both interrupts
        pio1_hw->irq = 3;
    }


    // the pio instance
    PIO pio_1;

    // the state machine
    uint sm1;

    // the current location of rotation
    static int rotation1;
};

#endif