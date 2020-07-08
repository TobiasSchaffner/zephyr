/*
 * Copyright (c) 2017 Intel Corporation.
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <zephyr.h>
#include <power/power.h>
#include <hal/nrf_power.h>

#include <logging/log.h>
LOG_MODULE_DECLARE(soc, CONFIG_SOC_LOG_LEVEL);

static void _issue_low_power_command(void)
{
	__WFE();
	__SEV();
	__WFE();
}

/* Invoke Low Power/System Off specific Tasks */
void sys_set_power_state(enum power_states state)
{
	switch (state) {
#ifdef CONFIG_SYS_POWER_DEEP_SLEEP_STATES
 #ifdef CONFIG_HAS_SYS_POWER_STATE_DEEP_SLEEP_1
	case SYS_POWER_STATE_DEEP_SLEEP_1:
		nrf_power_system_off(NRF_POWER);
		break;
 #endif
#endif
#ifdef CONFIG_SYS_POWER_SLEEP_STATES
 #ifdef CONFIG_HAS_SYS_POWER_STATE_SLEEP_1
	case SYS_POWER_STATE_SLEEP_1:
		nrf_power_task_trigger(NRF_POWER, NRF_POWER_TASK_LOWPWR);
		_issue_low_power_command();
		break;
 #endif
#endif
	default:
		LOG_DBG("Unsupported power state %u", state);
		break;
	}
}

/* Handle SOC specific activity after Low Power Mode Exit */
void _sys_pm_power_state_exit_post_ops(enum power_states state)
{
	switch (state) {
#ifdef CONFIG_SYS_POWER_DEEP_SLEEP_STATES
 #ifdef CONFIG_HAS_SYS_POWER_STATE_DEEP_SLEEP_1
	case SYS_POWER_STATE_DEEP_SLEEP_1:
		/* Nothing to do. */
		break;
 #endif
#endif
#ifdef CONFIG_SYS_POWER_SLEEP_STATES
 #ifdef CONFIG_HAS_SYS_POWER_STATE_SLEEP_1
	case SYS_POWER_STATE_SLEEP_1:
		__set_BASEPRI(0);
		break;
 #endif
#endif
	default:
		LOG_DBG("Unsupported power state %u", state);
		break;
	}

	/*
	 * System is now in active mode. Reenable interrupts which were disabled
	 * when OS started idling code.
	 */
	irq_unlock(0);
}
