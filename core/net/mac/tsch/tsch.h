/*
 * Copyright (c) 2015, SICS Swedish ICT.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 */

#ifndef __TSCH_H__
#define __TSCH_H__

/********** Includes **********/

#include "contiki.h"
#include "net/mac/mac.h"
#include "net/mac/tsch/tsch-security.h"

/******** Configuration *******/

/* Max time before sending a unicast keep-alive message to the time source */
#ifdef TSCH_CONF_KEEPALIVE_TIMEOUT
#define TSCH_KEEPALIVE_TIMEOUT TSCH_CONF_KEEPALIVE_TIMEOUT
#else
/* Time to desynch assuming a drift of 40 PPM (80 PPM between two nodes) and guard time of +/-1ms: 12.5s. */
#define TSCH_KEEPALIVE_TIMEOUT (12 * CLOCK_SECOND)
#endif

/* With TSCH_ADAPTIVE_TIMESYNC enabled: keep-alive timeout used after reaching
 * accurate drift compensation. */
#ifdef TSCH_CONF_MAX_KEEPALIVE_TIMEOUT
#define TSCH_MAX_KEEPALIVE_TIMEOUT TSCH_CONF_MAX_KEEPALIVE_TIMEOUT
#else
#define TSCH_MAX_KEEPALIVE_TIMEOUT (60 * CLOCK_SECOND)
#endif

/* Max time without synchronization before leaving the PAN */
#ifdef TSCH_CONF_DESYNC_THRESHOLD
#define TSCH_DESYNC_THRESHOLD TSCH_CONF_DESYNC_THRESHOLD
#else
#define TSCH_DESYNC_THRESHOLD (2 * TSCH_MAX_KEEPALIVE_TIMEOUT)
#endif

/* Period between two consecutive EBs */
#ifdef TSCH_CONF_EB_PERIOD
#define TSCH_EB_PERIOD TSCH_CONF_EB_PERIOD
#else
//origin
#define TSCH_EB_PERIOD (16 * CLOCK_SECOND)
// 0614 EB interval = 1
//#define TSCH_EB_PERIOD (1 * CLOCK_SECOND)
#endif

/* Max Period between two consecutive EBs */
#ifdef TSCH_CONF_MAX_EB_PERIOD
#define TSCH_MAX_EB_PERIOD TSCH_CONF_MAX_EB_PERIOD
#else
#define TSCH_MAX_EB_PERIOD (50 * CLOCK_SECOND)
#endif

/* Max acceptable join priority */
#ifdef TSCH_CONF_MAX_JOIN_PRIORITY
#define TSCH_MAX_JOIN_PRIORITY TSCH_CONF_MAX_JOIN_PRIORITY
#else
#define TSCH_MAX_JOIN_PRIORITY 32
#endif

/* Start TSCH automatically after init? If not, the upper layers
 * must call NETSTACK_MAC.on() to start it. Useful when the
 * application needs to control when the nodes are to start
 * scanning or advertising.*/
#ifdef TSCH_CONF_AUTOSTART
#define TSCH_AUTOSTART TSCH_CONF_AUTOSTART
#else
#define TSCH_AUTOSTART 1
#endif

/* Join only secured networks? (discard EBs with security disabled) */
#ifdef TSCH_CONF_JOIN_SECURED_ONLY
#define TSCH_JOIN_SECURED_ONLY TSCH_CONF_JOIN_SECURED_ONLY
#else
/* By default, set if LLSEC802154_ENABLED is also non-zero */
#define TSCH_JOIN_SECURED_ONLY LLSEC802154_ENABLED
#endif

/* By default, join any PAN ID. Otherwise, wait for an EB from IEEE802154_PANID */
#ifdef TSCH_CONF_JOIN_MY_PANID_ONLY
#define TSCH_JOIN_MY_PANID_ONLY TSCH_CONF_JOIN_MY_PANID_ONLY
#else
#define TSCH_JOIN_MY_PANID_ONLY 0
#endif

/* The radio polling frequency (in Hz) during association process */
#ifdef TSCH_CONF_ASSOCIATION_POLL_FREQUENCY
#define TSCH_ASSOCIATION_POLL_FREQUENCY TSCH_CONF_ASSOCIATION_POLL_FREQUENCY
#else
#define TSCH_ASSOCIATION_POLL_FREQUENCY 100
#endif

/* When associating, check ASN against our own uptime (time in minutes)..
 * Useful to force joining only with nodes started roughly at the same time.
 * Set to the max number of minutes acceptable. */
#ifdef TSCH_CONF_CHECK_TIME_AT_ASSOCIATION
#define TSCH_CHECK_TIME_AT_ASSOCIATION TSCH_CONF_CHECK_TIME_AT_ASSOCIATION
#else
#define TSCH_CHECK_TIME_AT_ASSOCIATION 0
#endif

/* By default: initialize schedule from EB when associating, using the
 * slotframe and links Information Element */
#ifdef TSCH_CONF_INIT_SCHEDULE_FROM_EB
#define TSCH_INIT_SCHEDULE_FROM_EB TSCH_CONF_INIT_SCHEDULE_FROM_EB
#else
#define TSCH_INIT_SCHEDULE_FROM_EB 1
#endif

/* An ad-hoc mechanism to have TSCH select its time source without the
 * help of an upper-layer, simply by collecting statistics on received
 * EBs and their join priority. Disabled by default as we recomment
 * mapping the time source on the RPL preferred parent
 * (via tsch_rpl_callback_parent_switch) */
#ifdef TSCH_CONF_AUTOSELECT_TIME_SOURCE
#define TSCH_AUTOSELECT_TIME_SOURCE TSCH_CONF_AUTOSELECT_TIME_SOURCE
#else
#define TSCH_AUTOSELECT_TIME_SOURCE 0
#endif /* TSCH_CONF_EB_AUTOSELECT */

/*********** Callbacks *********/

/* Called by TSCH when joining a network */
#ifdef TSCH_CALLBACK_JOINING_NETWORK
void TSCH_CALLBACK_JOINING_NETWORK();
#endif

/* Called by TSCH when leaving a network */
#ifdef TSCH_CALLBACK_LEAVING_NETWORK
void TSCH_CALLBACK_LEAVING_NETWORK();
#endif

/***** External Variables *****/

/* Are we coordinator of the TSCH network? */
extern int tsch_is_coordinator;
/* Are we associated to a TSCH network? */
extern int tsch_is_associated;
/* Is the PAN running link-layer security? */
extern int tsch_is_pan_secured;
/* The TSCH MAC driver */
extern const struct mac_driver tschmac_driver;

/********** Functions *********/

/* The the TSCH join priority */
void tsch_set_join_priority(uint8_t jp);
/* The period at which EBs are sent */
void tsch_set_eb_period(uint32_t period);
/* The keep-alive timeout */
void tsch_set_ka_timeout(uint32_t timeout);
/* Set the node as PAN coordinator */
void tsch_set_coordinator(int enable);
/* Set the pan as secured or not */
void tsch_set_pan_secured(int enable);

/********** Project *********/

/* The Electricity is an absolute slot number over 6 bytes. */
struct tsch_electricity_t {
    uint32_t ls4b; /* least significant 4 bytes */
    uint16_t ms2b; /* most significant 2 byte */
};
/* The current Electricity Jamie       3000000000000 */
#define TSCH_FULL_MAH                   100000000000  
extern uint64_t tsch_current_mAh ;
extern uint32_t tsch_TXslot_consumption ;
extern uint32_t tsch_RXslot_consumption ;
extern uint32_t tsch_EBslot_consumption ;
extern uint32_t tsch_IDLEslot_consumption ;

#define TSCH_MAH_INT_SHIFT               1000000000

/* Initialize Electricity */
#define TSCH_AH_INIT(mAh, ms2b_, ls4b_) do { \
    (mAh).ms2b = (ms2b_); \
    (mAh).ls4b = (ls4b_); \
} while(0);

/* Decrement an Electricity by inc (32 bits) */
#define TSCH_AH_DEC(mAh, dec) do { \
    uint32_t new_ls4b = (mAh).ls4b - (dec); \
    if(new_ls4b > (mAh).ls4b) { (mAh).ms2b--; } \
    (mAh).ls4b = new_ls4b; \
} while(0);


#endif /* __TSCH_H__ */
