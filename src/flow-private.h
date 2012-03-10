/* Copyright (C) 2007-2012 Open Information Security Foundation
 *
 * You can copy, redistribute or modify this Program under the terms of
 * the GNU General Public License version 2 as published by the Free
 * Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * version 2 along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */

/**
 * \file
 *
 * \author Victor Julien <victor@inliniac.net>
 */

#ifndef __FLOW_PRIVATE_H__
#define __FLOW_PRIVATE_H__

#include "flow-hash.h"
#include "util-atomic.h"
#include "util-stack.h"

/* global flow flags */

/** Flow engine is in emergency mode. This means it doesn't have enough spare
 *  flows for new flows and/or it's memcap limit it reached. In this state the
 *  flow engine with evaluate flows with lower timeout settings. */
#define FLOW_EMERGENCY   0x01

/* Flow Time out values */
#define FLOW_DEFAULT_NEW_TIMEOUT 30
#define FLOW_DEFAULT_EST_TIMEOUT 300
#define FLOW_DEFAULT_CLOSED_TIMEOUT 0
#define FLOW_IPPROTO_TCP_NEW_TIMEOUT 30
#define FLOW_IPPROTO_TCP_EST_TIMEOUT 300
#define FLOW_IPPROTO_UDP_NEW_TIMEOUT 30
#define FLOW_IPPROTO_UDP_EST_TIMEOUT 300
#define FLOW_IPPROTO_ICMP_NEW_TIMEOUT 30
#define FLOW_IPPROTO_ICMP_EST_TIMEOUT 300

#define FLOW_DEFAULT_EMERG_NEW_TIMEOUT 10
#define FLOW_DEFAULT_EMERG_EST_TIMEOUT 100
#define FLOW_DEFAULT_EMERG_CLOSED_TIMEOUT 0
#define FLOW_IPPROTO_TCP_EMERG_NEW_TIMEOUT 10
#define FLOW_IPPROTO_TCP_EMERG_EST_TIMEOUT 100
#define FLOW_IPPROTO_UDP_EMERG_NEW_TIMEOUT 10
#define FLOW_IPPROTO_UDP_EMERG_EST_TIMEOUT 100
#define FLOW_IPPROTO_ICMP_EMERG_NEW_TIMEOUT 10
#define FLOW_IPPROTO_ICMP_EMERG_EST_TIMEOUT 100

enum {
    FLOW_PROTO_DEFAULT = 0,
    FLOW_PROTO_TCP,
    FLOW_PROTO_UDP,
    FLOW_PROTO_ICMP,
    FLOW_PROTO_SCTP,

    /* should be last */
    FLOW_PROTO_MAX,
};

/*
 * Variables
 */

/** FlowProto specific timeouts and free/state functions */
FlowProto flow_proto[FLOW_PROTO_MAX];

/** spare/unused/prealloced flows live here */
Stack flow_spare_stack;

#define FlowSpareInit(void) do {                    \
    STACK_INIT(&flow_spare_stack);                  \
    SC_ATOMIC_INIT(flow_spare_cnt);                 \
} while (0)

#define FlowSpareDestroy(void) do {                 \
    STACK_DESTROY(&flow_spare_stack);               \
    SC_ATOMIC_DESTROY(flow_spare_cnt);              \
} while (0)

#define FlowSpareGet(void) ({                       \
    Flow *f = STACK_POP(&flow_spare_stack, Flow_);  \
    if (f != NULL) {                                \
        FlowSpareDecr();                            \
    }                                               \
    f;                                              \
})

#define FlowSpareStore(f) ({                        \
    STACK_PUSH(&flow_spare_stack, (f), Flow_);      \
    FlowSpareIncr();                                \
})

#define FlowSpareSize(void)                         \
    SC_ATOMIC_GET(flow_spare_cnt)
#define FlowSpareIncr(void)                         \
    SC_ATOMIC_ADD(flow_spare_cnt, 1)
#define FlowSpareDecr(void)                         \
    SC_ATOMIC_SUB(flow_spare_cnt, 1)

FlowBucket *flow_hash;
FlowConfig flow_config;
SC_ATOMIC_DECLARE(unsigned int, flow_spare_cnt);

/** flow memuse counter (atomic), for enforcing memcap limit */
SC_ATOMIC_DECLARE(long long unsigned int, flow_memuse);

//#define FLOWBITS_STATS
#ifdef FLOWBITS_STATS
uint64_t flowbits_memuse;
uint64_t flowbits_memuse_max;
uint32_t flowbits_added;
uint32_t flowbits_removed;
SCMutex flowbits_mutex;
#endif /* FLOWBITS_STATS */

#endif /* __FLOW_PRIVATE_H__ */

