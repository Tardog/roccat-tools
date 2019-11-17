#ifndef __ROCCAT_KONEAIMO_EVENTHANDLER_CHANNEL_H__
#define __ROCCAT_KONEAIMO_EVENTHANDLER_CHANNEL_H__

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

#include "roccat_device.h"

G_BEGIN_DECLS

#define KONEAIMO_EVENTHANDLER_CHANNEL_TYPE (koneaimo_eventhandler_channel_get_type())
#define KONEAIMO_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEAIMO_EVENTHANDLER_CHANNEL_TYPE, KoneaimoEventhandlerChannel))
#define IS_KONEAIMO_EVENTHANDLER_CHANNEL(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEAIMO_EVENTHANDLER_CHANNEL_TYPE))

typedef struct _KoneaimoEventhandlerChannel KoneaimoEventhandlerChannel;

GType koneaimo_eventhandler_channel_get_type(void);
KoneaimoEventhandlerChannel *koneaimo_eventhandler_channel_new(void);

gboolean koneaimo_eventhandler_channel_start(KoneaimoEventhandlerChannel *eventhandler_channel, RoccatDevice *device, GError **error);
void koneaimo_eventhandler_channel_stop(KoneaimoEventhandlerChannel *eventhandler_channel);

G_END_DECLS

#endif
