/**
 * @file
 * Simple SNMPv2c Agent Example for ESP32 by Leandro Silva
 * Tested on... 
 * Board: ESP32_Core_board_V2
 * Version: Framework ESP-IDF 3.0
 * Version: LWIP 2.0.3
 *
 */
 
/*
 * Copyright (c) July 2018 Leandro Silva <@gmail.com>
 * Student of Computer Engineering from Polytechnic Institute of Leiria - Portugal
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission. 
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED 
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
 * OF SUCH DAMAGE.
 *
 * This is a simple example of SNMPv2c Agent w/ private MIB for ESP32.
 * 
 * Author: Leandro Silva <@gmail.com>
 * Student of Computer Engineering from Polytechnic Institute of Leiria - Portugal
 *
 * Please be kind and support my work for further release, any donation is apreciated
 * at https://www.paypal.me/LeandroAdonis/5 for 5€ or 5$ amount. Any amount you wish, 
 * I will be very gratefull and real happy to see my work worth something to you. Thank you.
 *
 */

//--- global library
#include <string.h>

//--- RTOS library and Espressif ESP32 library
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_log.h"


//--- LwIP library
#include "lwip/err.h"
#include "lwip/sys.h"

#include "lwip/apps/snmp_opts.h"
#include "lwip/snmp.h"
#include "lwip/apps/snmp.h"
#include "lwip/apps/snmp_core.h"
#include "lwip/apps/snmp_mib2.h"
#include "lwip/apps/snmp_scalar.h"

//-- private MIB library (included)
#include "my_mib.h"

/* library for the app */
#include "simple_snmp_agent.h"
/* library for static IP */
#include "lwip/sockets.h"

static const char *TAG = "ubirch_snmp";

/*
 * ----- TODO: Global variables for SNMP Trap vvv
 * Define your own vars SNMP_SYSDESCR for System Description, SNMP_SYSCONTACT 
 * for your contact mail, SNMP_SYSNAME for your system name, SNMP_SYSLOCATION
 * for your location. Also consider the size of each string in _LEN functions.
*/
static const struct snmp_mib *my_snmp_mibs[] = { &mib2, &ubirch_mib };
//1.3.6.1.2.1.1.1.0
u8_t * SNMP_SYSDESCR = (u8_t*) "ubirch_snmp_agent";
u16_t SNMP_SYSDESCR_LEN = sizeof("ubirch_snmp_agent");
//1.3.6.1.2.1.1.4.0
u8_t * SNMP_SYSCONTACT = (u8_t*) "waldemar.gruenwald@ubirch.com";
u16_t SNMP_SYSCONTACT_LEN = sizeof("waldemar.gruenwald@ubirch.com");
//1.3.6.1.2.1.1.5.0
u8_t *SNMP_SYSNAME = (u8_t *) "ESP32-WROOM-32";
u16_t SNMP_SYSNAME_LEN = sizeof("ESP32-WROOM-32");
//1.3.6.1.2.1.1.6.0
u8_t * SNMP_SYSLOCATION = (u8_t*) "ubirch GmbH";
u16_t SNMP_SYSLOCATION_LEN = sizeof("ubirch GmbH");
/* 
 * ----- TODO: Global variables for SNMP Trap ^^^
*/

/* buffer for snmp service */
u16_t snmp_buffer = 128;



static void initialize_snmp(void);

void snmp_start(void){
	initialize_snmp();
}

/**
 * Should be called at the beginning of the program to set up the snmp agent.
 *
 * @note You can look updated instructions in the link below
 * 		 http://www.nongnu.org/lwip/2_0_x/group__snmp.html
 */
static void initialize_snmp(void)
{
	snmp_mib2_set_syscontact(SNMP_SYSCONTACT, &SNMP_SYSCONTACT_LEN, snmp_buffer);
	snmp_mib2_set_syslocation(SNMP_SYSLOCATION, &SNMP_SYSLOCATION_LEN, snmp_buffer);
	snmp_mib2_set_sysdescr(SNMP_SYSDESCR, &SNMP_SYSDESCR_LEN);
	snmp_mib2_set_sysname(SNMP_SYSNAME, &SNMP_SYSNAME_LEN, snmp_buffer);

	snmp_set_mibs(my_snmp_mibs, LWIP_ARRAYSIZE(my_snmp_mibs));

	
    snmp_init();

    // reserve memory for the snmp data buffer
    snmp_data_buffer = malloc(SNMP_BUFFER_SIZE * sizeof(char));

    const struct snmp_obj_id* oid = snmp_get_device_enterprise_oid();
	ESP_LOGI(TAG, "initialize_snmp(%d.%d.%d.%d.%d.%d.%d) finished.", oid->id[0], oid->id[1], oid->id[2], oid->id[3], oid->id[4], oid->id[5], oid->id[6]);
}

