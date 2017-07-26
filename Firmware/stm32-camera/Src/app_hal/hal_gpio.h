/**
 * @file      hal_gpio.h
 *
 * @brief     GPIO Pin Declarations. Should be the only central definition
 *            of GPIO output and input names and the physical mapping to I/O.
 *            Application code to use the NAME.port and NAME.pin values when
 *            using the HAL GPIO functions.
 *
 *            For example:
 *
 *                hal_gpio_write_pin( _STATUS_0, on );
 *
 *                hal_gpio_read_pin( _AUX_IN_0 );
 *
 * @author    Marco Hess <marcoh@applidyne.com.au>
 *
 * @copyright (c) 2015 Applidyne Pty. Ltd. - All rights reserved.
*/

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

/* ----- System Includes ---------------------------------------------------- */

#include <stdbool.h>

/* ----- Local Includes ----------------------------------------------------- */

#include "global.h"

/* ----- Defines ------------------------------------------------------------ */

/** Readability macros to remind us which is a HIGH level or LOW level */

#define GPIO_HIGH       true
#define GPIO_LOW        false

/* ----- Types ------------------------------------------------------------- */

/** Operational mode for pins */

typedef enum
{
    MODE_INPUT = 0, /* Normal input */
    MODE_INPUT_PU,  /* Input with pullup */
    MODE_OUT_PP,    /* Push-Pull Output */
    MODE_OUT_OD     /* Open drain Output */
} HalGpioMode_t;

/** Enum with all the GPIO pins defined. */

typedef enum
{
	//Board hardware
    _BTN,
    _STATUS_0,            /* Output: LED RED */
    _STATUS_1,            /* Output: LED GREEN */

	//AUX IO pins
	_AUX_GPIO_1,
	_AUX_GPIO_2,
	_AUX_GPIO_0,

	//SD Card
	_SD_DETECT,
	_AMBIENT_LIGHT,
	_REBOOT_LATCH,

	//Camera Control and Misc
	_I2C_SDA,
    _I2C_SCL,

	_PWR_EN,
	_CAM_STROBE,
	_CAM_EXPST,
	_CAM_FREX,
	_CAM_XCLK,
	_CAM_RESET,
	_CAM_PWDN,

    /* --- Defining Total Entries --- */
    _NUMBER_OF_GPIO_PORT_PINS

} HalGpioPortPin_t;

/* -------------------------------------------------------------------------- */

/** Configure all internal default I/O configurations */

PUBLIC void
hal_gpio_configure_mcu_defaults( void );

/* -------------------------------------------------------------------------- */

/** Configure all external default I/O configurations */

PUBLIC void
hal_gpio_configure_i2c_defaults( void );

/* -------------------------------------------------------------------------- */

/** Configure an indicated pin for the give mode */

PUBLIC void
hal_gpio_init( HalGpioPortPin_t gpio_port_pin_nr, HalGpioMode_t mode, bool initial );

/* -------------------------------------------------------------------------- */

/** Return true when input pin is high, false when input is low */

PUBLIC bool
hal_gpio_read_pin( HalGpioPortPin_t gpio_port_pin_nr );

/* -------------------------------------------------------------------------- */

/** Set output pin high when on = true, low when on = false */

PUBLIC void
hal_gpio_write_pin( HalGpioPortPin_t gpio_port_pin_nr, bool on );

/* -------------------------------------------------------------------------- */

/** Toggle the current output state */

PUBLIC void
hal_gpio_toggle_pin( HalGpioPortPin_t gpio_port_pin_nr );

/* ----- End ---------------------------------------------------------------- */

#ifdef __cplusplus
}
#endif

#endif /* HAL_GPIO_H */