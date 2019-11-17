/*
 * This file is part of roccat-tools.
 *
 * roccat-tools is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * roccat-tools is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with roccat-tools. If not, see <http://www.gnu.org/licenses/>.
 */

#include "koneaimo_eventhandler_channel.h"
#include "g_cclosure_roccat_marshaller.h"
#include "koneaimo.h"
#include "roccat_device_hidraw.h"
#include "roccat.h"
#include "roccat_helper.h"
#include "i18n-lib.h"

#define KONEAIMO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEAIMO_EVENTHANDLER_CHANNEL_TYPE, KoneaimoEventhandlerChannelClass))
#define IS_KONEAIMO_EVENTHANDLER_CHANNEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEAIMO_EVENTHANDLER_CHANNEL_TYPE))
#define KONEAIMO_EVENTHANDLER_CHANNEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEAIMO_EVENTHANDLER_CHANNEL_TYPE, KoneaimoEventhandlerChannelPrivate))

typedef struct _KoneaimoEventhandlerChannelClass KoneaimoEventhandlerChannelClass;
typedef struct _KoneaimoEventhandlerChannelPrivate KoneaimoEventhandlerChannelPrivate;

struct _KoneaimoEventhandlerChannel {
	GObject parent;
	KoneaimoEventhandlerChannelPrivate *priv;
};

struct _KoneaimoEventhandlerChannelClass {
	GObjectClass parent_class;
};

struct _KoneaimoEventhandlerChannelPrivate {
	guint event_source_id;
	gboolean running;
};

enum {
	CPI_CHANGED, /* cpi_index */
	OPEN_DRIVER,
	PROFILE_CHANGED, /* profile_number */
	QUICKLAUNCH, /* key_index */
	SENSITIVITY_CHANGED, /* x, y */
	TALK, /* key_index, event */
	TIMER_START, /* key_index */
	TIMER_STOP,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(KoneaimoEventhandlerChannel, koneaimo_eventhandler_channel, G_TYPE_OBJECT);

static void process_chrdev_event(KoneaimoEventhandlerChannel *eventhandler_channel, KoneplusSpecial const *event) {
	gchar *string;

	switch (event->type) {
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_LEFT:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_RIGHT:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_MIDDLE:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_THUMB_1:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_THUMB_2:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_UP:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_DOWN:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_EASYSHIFT:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_EASYAIM:
	case KONEAIMO_CHRDEV_EVENT_TYPE_RAD_DISTANCE:
		// silently ignore rad events
		break;
	case KONEPLUS_SPECIAL_TYPE_PROFILE:
		g_signal_emit((gpointer)eventhandler_channel, signals[PROFILE_CHANGED], 0, event->data1);
		break;
	case KONEPLUS_SPECIAL_TYPE_CPI:
		g_signal_emit((gpointer)eventhandler_channel, signals[CPI_CHANGED], 0, event->data1 - 1);
		break;
	case KONEPLUS_SPECIAL_TYPE_SENSITIVITY:
		g_signal_emit((gpointer)eventhandler_channel, signals[SENSITIVITY_CHANGED], 0, event->data1, event->data2);
		break;
	case KONEPLUS_SPECIAL_TYPE_QUICKLAUNCH:
		if (event->data2 == KONEPLUS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[QUICKLAUNCH], 0, event->data1 - 1);
		break;
	case KONEPLUS_SPECIAL_TYPE_OPEN_DRIVER:
		g_signal_emit((gpointer)eventhandler_channel, signals[OPEN_DRIVER], 0);
		break;
	case KONEPLUS_SPECIAL_TYPE_TILT:
		/* just ignore this event */
		break;
	case KONEPLUS_SPECIAL_TYPE_TIMER:
		if (event->data2 == KONEPLUS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_START], 0, event->data1 - 1);
		break;
	case KONEPLUS_SPECIAL_TYPE_TIMER_STOP:
		if (event->data2 == KONEPLUS_SPECIAL_ACTION_PRESS)
			g_signal_emit((gpointer)eventhandler_channel, signals[TIMER_STOP], 0);
		break;
	case KONEPLUS_SPECIAL_TYPE_MULTIMEDIA:
		/* TODO implement multimedia handler */
		break;
	case KONEPLUS_SPECIAL_TYPE_TALK:
		g_signal_emit((gpointer)eventhandler_channel, signals[TALK], 0, event->data1 - 1, event->data2);
		break;
	default:
		string = roccat_data8_to_string((guchar const *)event, sizeof(KoneplusSpecial));
		g_debug(_("Got unknown event: %s"), string);
		g_free(string);
	}
}

static gboolean io_cb(GIOChannel *channel, GIOCondition condition, gpointer user_data) {
	KoneaimoEventhandlerChannel *eventhandler_channel = KONEAIMO_EVENTHANDLER_CHANNEL(user_data);
	KoneplusSpecial event;
	GIOStatus status;
	GError *error = NULL;
	gsize length;

	if (condition & G_IO_HUP)
		return FALSE;

	status = g_io_channel_read_chars(channel, (gchar *)&event, sizeof(KoneplusSpecial), &length, &error);

	if (status == G_IO_STATUS_AGAIN)
		return TRUE;

	if (status == G_IO_STATUS_ERROR) {
		g_critical(_("Could not read io_channel: %s"), error->message);
		return FALSE;
	}

	if (status == G_IO_STATUS_EOF) {
		g_warning(_("Could not read io_channel: %s"), _("End of file"));
		return FALSE;
	}

	if (length < sizeof(KoneplusSpecial))
		return TRUE;

	switch (event.report_id) {
	case KONEPLUS_REPORT_ID_SPECIAL:
		process_chrdev_event(eventhandler_channel, &event);
		break;
	default:
		break;
	}

	return TRUE; // keep on going
}

KoneaimoEventhandlerChannel *koneaimo_eventhandler_channel_new(void) {
	return KONEAIMO_EVENTHANDLER_CHANNEL(g_object_new(KONEAIMO_EVENTHANDLER_CHANNEL_TYPE, NULL));
}

static void koneaimo_eventhandler_channel_init(KoneaimoEventhandlerChannel *channel) {
	KoneaimoEventhandlerChannelPrivate *priv = KONEAIMO_EVENTHANDLER_CHANNEL_GET_PRIVATE(channel);
	channel->priv = priv;
	priv->running = FALSE;
}

gboolean koneaimo_eventhandler_channel_start(KoneaimoEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error) {
	GError *local_error = NULL;
	KoneaimoEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	priv->event_source_id = roccat_device_hidraw_add_event_watch(device, KONEPLUS_INTERFACE_MOUSE, io_cb, eventhandler_channel, &local_error);
	if (local_error) {
		g_propagate_error(error, local_error);
		priv->running = FALSE;
		return FALSE;
	} else {
		priv->running = TRUE;
		return TRUE;
	}
}

void koneaimo_eventhandler_channel_stop(KoneaimoEventhandlerChannel *eventhandler_channel) {
	KoneaimoEventhandlerChannelPrivate *priv = eventhandler_channel->priv;
	if (priv->running) {
		g_source_remove(priv->event_source_id);
		priv->running = FALSE;
	}
}

static void finalize(GObject *object) {
	KoneaimoEventhandlerChannel *eventhandler_channel = KONEAIMO_EVENTHANDLER_CHANNEL(object);
	koneaimo_eventhandler_channel_stop(eventhandler_channel);
	G_OBJECT_CLASS(koneaimo_eventhandler_channel_parent_class)->finalize(object);
}

static void koneaimo_eventhandler_channel_class_init(KoneaimoEventhandlerChannelClass *klass) {
	GObjectClass *gobject_class;

	gobject_class = (GObjectClass *)klass;

	gobject_class->finalize = finalize;

	g_type_class_add_private(klass, sizeof(KoneaimoEventhandlerChannelPrivate));

	signals[CPI_CHANGED] = g_signal_new("cpi-changed", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[OPEN_DRIVER] = g_signal_new("open-driver", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);

	signals[PROFILE_CHANGED] = g_signal_new("profile-changed", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[QUICKLAUNCH] = g_signal_new("quicklaunch", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[SENSITIVITY_CHANGED] = g_signal_new("sensitivity-changed", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TALK] = g_signal_new("talk", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_roccat_marshal_VOID__UCHAR_UCHAR, G_TYPE_NONE,
			2, G_TYPE_UCHAR, G_TYPE_UCHAR);

	signals[TIMER_START] = g_signal_new("timer-start", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__UCHAR, G_TYPE_NONE,
			1, G_TYPE_UCHAR);

	signals[TIMER_STOP] = g_signal_new("timer-stop", KONEAIMO_EVENTHANDLER_CHANNEL_TYPE,
			G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			0, NULL, NULL, g_cclosure_marshal_VOID__VOID, G_TYPE_NONE,
			0);
}
