
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/freezer.h>

#include "uwb-internal.h"

typedef int (*uwbd_evt_handler_f)(struct uwb_event *);

struct uwbd_event {
	uwbd_evt_handler_f handler;
	const char *name;
};

/* Table of handlers for and properties of the UWBD Radio Control Events */
static struct uwbd_event uwbd_urc_events[] = {
	[UWB_RC_EVT_IE_RCV] = {
		.handler = uwbd_evt_handle_rc_ie_rcv,
		.name = "IE_RECEIVED"
	},
	[UWB_RC_EVT_BEACON] = {
		.handler = uwbd_evt_handle_rc_beacon,
		.name = "BEACON_RECEIVED"
	},
	[UWB_RC_EVT_BEACON_SIZE] = {
		.handler = uwbd_evt_handle_rc_beacon_size,
		.name = "BEACON_SIZE_CHANGE"
	},
	[UWB_RC_EVT_BPOIE_CHANGE] = {
		.handler = uwbd_evt_handle_rc_bpoie_change,
		.name = "BPOIE_CHANGE"
	},
	[UWB_RC_EVT_BP_SLOT_CHANGE] = {
		.handler = uwbd_evt_handle_rc_bp_slot_change,
		.name = "BP_SLOT_CHANGE"
	},
	[UWB_RC_EVT_DRP_AVAIL] = {
		.handler = uwbd_evt_handle_rc_drp_avail,
		.name = "DRP_AVAILABILITY_CHANGE"
	},
	[UWB_RC_EVT_DRP] = {
		.handler = uwbd_evt_handle_rc_drp,
		.name = "DRP"
	},
	[UWB_RC_EVT_DEV_ADDR_CONFLICT] = {
		.handler = uwbd_evt_handle_rc_dev_addr_conflict,
		.name = "DEV_ADDR_CONFLICT",
	},
};



struct uwbd_evt_type_handler {
	const char *name;
	struct uwbd_event *uwbd_events;
	size_t size;
};

/* Table of handlers for each UWBD Event type. */
static struct uwbd_evt_type_handler uwbd_urc_evt_type_handlers[] = {
	[UWB_RC_CET_GENERAL] = {
		.name        = "URC",
		.uwbd_events = uwbd_urc_events,
		.size        = ARRAY_SIZE(uwbd_urc_events),
	},
};

static const struct uwbd_event uwbd_message_handlers[] = {
	[UWB_EVT_MSG_RESET] = {
		.handler = uwbd_msg_handle_reset,
		.name = "reset",
	},
};

static
int uwbd_event_handle_urc(struct uwb_event *evt)
{
	int result = -EINVAL;
	struct uwbd_evt_type_handler *type_table;
	uwbd_evt_handler_f handler;
	u8 type, context;
	u16 event;

	type = evt->notif.rceb->bEventType;
	event = le16_to_cpu(evt->notif.rceb->wEvent);
	context = evt->notif.rceb->bEventContext;

	if (type >= ARRAY_SIZE(uwbd_urc_evt_type_handlers))
		goto out;
	type_table = &uwbd_urc_evt_type_handlers[type];
	if (type_table->uwbd_events == NULL)
		goto out;
	if (event >= type_table->size)
		goto out;
	handler = type_table->uwbd_events[event].handler;
	if (handler == NULL)
		goto out;

	result = (*handler)(evt);
out:
	if (result < 0)
		dev_err(&evt->rc->uwb_dev.dev,
			"UWBD: event 0x%02x/%04x/%02x, handling failed: %d\n",
			type, event, context, result);
	return result;
}

static void uwbd_event_handle_message(struct uwb_event *evt)
{
	struct uwb_rc *rc;
	int result;

	rc = evt->rc;

	if (evt->message < 0 || evt->message >= ARRAY_SIZE(uwbd_message_handlers)) {
		dev_err(&rc->uwb_dev.dev, "UWBD: invalid message type %d\n", evt->message);
		return;
	}

	result = uwbd_message_handlers[evt->message].handler(evt);
	if (result < 0)
		dev_err(&rc->uwb_dev.dev, "UWBD: '%s' message failed: %d\n",
			uwbd_message_handlers[evt->message].name, result);
}

static void uwbd_event_handle(struct uwb_event *evt)
{
	struct uwb_rc *rc;
	int should_keep;

	rc = evt->rc;

	if (rc->ready) {
		switch (evt->type) {
		case UWB_EVT_TYPE_NOTIF:
			should_keep = uwbd_event_handle_urc(evt);
			if (should_keep <= 0)
				kfree(evt->notif.rceb);
			break;
		case UWB_EVT_TYPE_MSG:
			uwbd_event_handle_message(evt);
			break;
		default:
			dev_err(&rc->uwb_dev.dev, "UWBD: invalid event type %d\n", evt->type);
			break;
		}
	}

	__uwb_rc_put(rc);	/* for the __uwb_rc_get() in uwb_rc_notif_cb() */
}

static int uwbd(void *param)
{
	struct uwb_rc *rc = param;
	unsigned long flags;
	struct uwb_event *evt;
	int should_stop = 0;

	while (1) {
		wait_event_interruptible_timeout(
			rc->uwbd.wq,
			!list_empty(&rc->uwbd.event_list)
			  || (should_stop = kthread_should_stop()),
			HZ);
		if (should_stop)
			break;
		try_to_freeze();

		spin_lock_irqsave(&rc->uwbd.event_list_lock, flags);
		if (!list_empty(&rc->uwbd.event_list)) {
			evt = list_first_entry(&rc->uwbd.event_list, struct uwb_event, list_node);
			list_del(&evt->list_node);
		} else
			evt = NULL;
		spin_unlock_irqrestore(&rc->uwbd.event_list_lock, flags);

		if (evt) {
			uwbd_event_handle(evt);
			kfree(evt);
		}

		uwb_beca_purge(rc);	/* Purge devices that left */
	}
	return 0;
}


/** Start the UWB daemon */
void uwbd_start(struct uwb_rc *rc)
{
	rc->uwbd.task = kthread_run(uwbd, rc, "uwbd");
	if (rc->uwbd.task == NULL)
		printk(KERN_ERR "UWB: Cannot start management daemon; "
		       "UWB won't work\n");
	else
		rc->uwbd.pid = rc->uwbd.task->pid;
}

/* Stop the UWB daemon and free any unprocessed events */
void uwbd_stop(struct uwb_rc *rc)
{
	kthread_stop(rc->uwbd.task);
	uwbd_flush(rc);
}

void uwbd_event_queue(struct uwb_event *evt)
{
	struct uwb_rc *rc = evt->rc;
	unsigned long flags;

	spin_lock_irqsave(&rc->uwbd.event_list_lock, flags);
	if (rc->uwbd.pid != 0) {
		list_add(&evt->list_node, &rc->uwbd.event_list);
		wake_up_all(&rc->uwbd.wq);
	} else {
		__uwb_rc_put(evt->rc);
		if (evt->type == UWB_EVT_TYPE_NOTIF)
			kfree(evt->notif.rceb);
		kfree(evt);
	}
	spin_unlock_irqrestore(&rc->uwbd.event_list_lock, flags);
	return;
}

void uwbd_flush(struct uwb_rc *rc)
{
	struct uwb_event *evt, *nxt;

	spin_lock_irq(&rc->uwbd.event_list_lock);
	list_for_each_entry_safe(evt, nxt, &rc->uwbd.event_list, list_node) {
		if (evt->rc == rc) {
			__uwb_rc_put(rc);
			list_del(&evt->list_node);
			if (evt->type == UWB_EVT_TYPE_NOTIF)
				kfree(evt->notif.rceb);
			kfree(evt);
		}
	}
	spin_unlock_irq(&rc->uwbd.event_list_lock);
}
