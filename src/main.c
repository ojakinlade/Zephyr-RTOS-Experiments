/*
 * Copyright (c) 2012-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

 #include <zephyr/random/random.h>

 static const int32_t sleep_time_ms = 1000;

#include <stdio.h>

int main(void)
{
	uint32_t rnd;

	while(1)
	{
		rnd = sys_rand32_get();
		printf("Random number: %u\n", rnd);
		k_msleep(sleep_time_ms);
	}
	// printf("Hello World! %s\n", CONFIG_BOARD_TARGET);

	return 0;
}
