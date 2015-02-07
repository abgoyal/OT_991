

#ifndef ST_LL_H
#define ST_LL_H

#include <linux/skbuff.h>
#include "st.h"
#include "st_core.h"

/* ST LL receiver states */
#define ST_W4_PACKET_TYPE       0
#define ST_BT_W4_EVENT_HDR      1
#define ST_BT_W4_ACL_HDR        2
#define ST_BT_W4_SCO_HDR        3
#define ST_BT_W4_DATA           4
#define ST_FM_W4_EVENT_HDR      5
#define ST_GPS_W4_EVENT_HDR	6

/* ST LL state machines */
#define ST_LL_ASLEEP               0
#define ST_LL_ASLEEP_TO_AWAKE      1
#define ST_LL_AWAKE                2
#define ST_LL_AWAKE_TO_ASLEEP      3
#define ST_LL_INVALID		   4

#define LL_SLEEP_IND	0x30
#define LL_SLEEP_ACK	0x31
#define LL_WAKE_UP_IND	0x32
#define LL_WAKE_UP_ACK	0x33

/* initialize and de-init ST LL */
long st_ll_init(struct st_data_s *);
long st_ll_deinit(struct st_data_s *);

void st_ll_enable(struct st_data_s *);
void st_ll_disable(struct st_data_s *);

unsigned long st_ll_getstate(struct st_data_s *);
unsigned long st_ll_sleep_state(struct st_data_s *, unsigned char);
void st_ll_wakeup(struct st_data_s *);
#endif /* ST_LL_H */
