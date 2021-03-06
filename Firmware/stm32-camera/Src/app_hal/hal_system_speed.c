/**
 * @file      hal_system_speed.h
 *
 * @brief     Low level control of the system clock speed for power saving purposes.
 *            Also includes helper functions for getting clock speed and CPU load.
 *
 * @author    Scott Rapson <scottr@applidyne.com.au>
 *
 * @copyright (c) 2017 Applidyne Pty. Ltd. - All rights reserved.
 */

/* ----- System Includes ---------------------------------------------------- */
#include <inttypes.h>
#include <stdbool.h>

/* ----- Local Includes ----------------------------------------------------- */

#include "hal_system_speed.h"
#include "stm32f4xx_hal.h"

/* ----- Private Types ------------------------------------------------------ */

/* ----- Private Prototypes ------------------------------------------------- */

/* ----- Private Data ------------------------------------------------------- */

volatile uint32_t cc_when_sleeping;     //timestamp when we go to sleep
volatile uint32_t cc_when_woken = 0;    //timestamp when we wake up
volatile uint32_t cc_awake_time = 0;    //duration of 'active'
volatile uint32_t cc_asleep_time = 0;   //duration of 'sleep'

PRIVATE SystemSpeed_RCC_PLL_t pll_working;	//TODO rename this local variable

/* ----- Public Functions --------------------------------------------------- */

PUBLIC void
hal_system_speed_init(  )
{
	//Enable the DWT timer
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

/* -------------------------------------------------------------------------- */

//Measured to take about 200us to execute

PUBLIC void
hal_system_speed_set_pll( SystemSpeed_RCC_PLL_t* pll_settings )
{
	uint16_t timeout;
	SystemSpeed_RCC_PLL_t tmp;

	// Read the existing PLL registers
	hal_system_speed_get_pll(&tmp);

	if ( memcmp(pll_settings, &tmp, sizeof(SystemSpeed_RCC_PLL_t)) == 0 )
	{
		return; // Don't change PLL settings if settings are the same
	}

	//Enable the HSI48
	RCC->CR |= RCC_CR_HSION;

	// Ensure the HSI is ready
	timeout = 0xFFFF;
	while (!hal_system_speed_hsi_ready() && timeout--);

	// Use the HSI48 internal clock as main clock because the current PLL clock will stop.
	RCC->CFGR = (RCC->CFGR & ~(RCC_CFGR_SW)) | RCC_CFGR_SW_HSI;

	//Disable the PLL
	RCC->CR &= ~RCC_CR_PLLON;

	// Set PLL settings from structure
	if (pll_settings->PLLM)
	{
		RCC->PLLCFGR = ( RCC->PLLCFGR & ~RCC_PLLM_MASK ) | ( (pll_settings->PLLM << RCC_PLLM_POS) & RCC_PLLM_MASK );
	}

	if (pll_settings->PLLN)
	{
		RCC->PLLCFGR = ( RCC->PLLCFGR & ~RCC_PLLN_MASK ) | ( (pll_settings->PLLN << RCC_PLLN_POS) & RCC_PLLN_MASK );
	}

	if (pll_settings->PLLP)
	{
		RCC->PLLCFGR = ( RCC->PLLCFGR & ~RCC_PLLP_MASK ) | ( (((pll_settings->PLLP >> 1) - 1) << RCC_PLLP_POS) & RCC_PLLP_MASK );
	}

	if (pll_settings->PLLQ)
	{
		RCC->PLLCFGR = ( RCC->PLLCFGR & ~RCC_PLLQ_MASK) | ( (pll_settings->PLLQ << RCC_PLLQ_POS) & RCC_PLLQ_MASK );
	}

	if (pll_settings->PLLR)
	{
		RCC->PLLCFGR = ( RCC->PLLCFGR & ~RCC_PLLR_MASK ) | ( (pll_settings->PLLR << RCC_PLLR_POS) & RCC_PLLR_MASK );
	}

	//Enable the PLL
	RCC->CR |= RCC_CR_PLLON;

	//Blocking wait until PLL is ready
	timeout = 0xFFFF;
	while (!hal_system_speed_pll_ready() && timeout--);

	//Enable PLL as main clock
	RCC->CFGR = ( RCC->CFGR & ~(RCC_CFGR_SW) ) | RCC_CFGR_SW_PLL;

	//Update system core clock variable
	SystemCoreClockUpdate();
}

/* -------------------------------------------------------------------------- */

PUBLIC void
hal_system_speed_get_pll( SystemSpeed_RCC_PLL_t* pll_settings )
{
	pll_settings->PLLM = ( RCC->PLLCFGR & RCC_PLLM_MASK ) >> RCC_PLLM_POS;
	pll_settings->PLLN = ( RCC->PLLCFGR & RCC_PLLN_MASK ) >> RCC_PLLN_POS;
	pll_settings->PLLP = (((RCC->PLLCFGR & RCC_PLLP_MASK ) >> RCC_PLLP_POS) + 1) << 1;
	pll_settings->PLLQ = ( RCC->PLLCFGR & RCC_PLLQ_MASK ) >> RCC_PLLQ_POS;
	pll_settings->PLLR = ( RCC->PLLCFGR & RCC_PLLR_MASK ) >> RCC_PLLR_POS;
}

/* -------------------------------------------------------------------------- */

PUBLIC bool
hal_system_speed_pll_ready( void )
{
	return ( RCC->CR & RCC_CR_PLLRDY );
}

/* -------------------------------------------------------------------------- */

PUBLIC bool
hal_system_speed_hsi_ready( void )
{
	return ( RCC->CR & RCC_CR_HSIRDY );
}
/* -------------------------------------------------------------------------- */

PUBLIC void
hal_system_speed_sleep( void )
{
	CRITICAL_SECTION_VAR();
	CRITICAL_SECTION_START();

	cc_awake_time += DWT->CYCCNT - cc_when_woken;
	cc_when_sleeping = DWT->CYCCNT;

	//Go to sleep. Wake on interrupt.
    HAL_PWR_EnterSLEEPMode( PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI );

    cc_asleep_time += DWT->CYCCNT - cc_when_sleeping;
	cc_when_woken = DWT->CYCCNT;

	CRITICAL_SECTION_END();
}

/* -------------------------------------------------------------------------- */

PUBLIC void
hal_system_speed_stop( void )
{
    //TODO stop the clocks and other peripherals before we sleep
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    //TODO start clocks and continue running
}
/* -------------------------------------------------------------------------- */

//return a 0-100% value for CPU load
PUBLIC float
hal_system_speed_get_load( void )
{
	float cpu_util = (float)cc_awake_time
	                 /
	                 (float)(cc_asleep_time + cc_awake_time) * 100;
	cc_asleep_time = 0;
	cc_awake_time = 0;

	return cpu_util;
}

/* -------------------------------------------------------------------------- */

PUBLIC uint32_t
hal_system_speed_get_speed( void )
{
	return HAL_RCC_GetSysClockFreq();
}

/* -------------------------------------------------------------------------- */

PUBLIC void
hal_system_speed_high( void )
{
	hal_system_speed_get_pll(&pll_working);

	//New value for PLLN
	pll_working.PLLN = 180;

	hal_system_speed_set_pll(&pll_working);
}

/* -------------------------------------------------------------------------- */

PUBLIC void
hal_system_speed_low( void )
{
	hal_system_speed_get_pll(&pll_working);

	//New value for PLLN
	pll_working.PLLN = 72;

	hal_system_speed_set_pll(&pll_working);
}

/* ----- End ---------------------------------------------------------------- */

