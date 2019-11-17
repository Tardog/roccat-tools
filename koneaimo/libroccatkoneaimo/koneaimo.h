#ifndef __ROCCAT_KONEAIMO_H__
#define __ROCCAT_KONEAIMO_H__

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

#include "koneplus.h"

G_BEGIN_DECLS

#define USB_DEVICE_ID_ROCCAT_KONEAIMO 0x2e27

enum {
	KONEAIMO_PROFILE_SETTING_CPI_MIN = 1,
	KONEAIMO_PROFILE_SETTING_CPI_MAX = 41,
};

typedef enum {
	/* data1 = count, usually 0x0a */
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_LEFT = 0xe1,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_RIGHT = 0xe2,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_MIDDLE = 0xe3,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_THUMB_1 = 0xe4,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_THUMB_2 = 0xe5,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_UP = 0xe6,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_SCROLL_WHEEL_DOWN = 0xe7,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_DISTANCE = 0xea, // TODO unit?
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_EASYSHIFT = 0xed,
	KONEAIMO_CHRDEV_EVENT_TYPE_RAD_EASYAIM = 0xee,
} KoneaimoChrdevEventType;

typedef enum {
	KONEAIMO_REPORT_ID_SROM_WRITE = 0x0d,
} KoneaimoReportId;

typedef enum {
	KONEAIMO_RMP_LIGHT_CHOSE_TYPE_TABLE = 0,
	KONEAIMO_RMP_LIGHT_CHOSE_TYPE_CUSTOM = 1,
} KoneaimoRmpLightChoseType;


RoccatDevice *koneaimo_device_first(void);
RoccatDeviceScanner *koneaimo_device_scanner_new(void);

RoccatKeyFile *koneaimo_configuration_load(void);
gboolean koneaimo_configuration_save(RoccatKeyFile *config, GError **error);

KoneplusRmp *koneaimo_default_rmp(void);
GKeyFile *koneaimo_rmp_defaults(void);

DBusGProxy *koneaimo_dbus_proxy_new(void);
gboolean koneaimo_dbus_emit_profile_data_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean koneaimo_dbus_emit_profile_data_changed_outside_instant(guint profile_number);
gboolean koneaimo_dbus_emit_profile_changed_outside(DBusGProxy *proxy, guint profile_number);
gboolean koneaimo_dbus_emit_profile_changed_outside_instant(guint profile_number);
gboolean koneaimo_dbus_emit_configuration_changed_outside(DBusGProxy *proxy);

KoneplusRmp *koneaimo_rmp_load(RoccatDevice *koneaimo, guint profile_number, GError **error);
gboolean koneaimo_rmp_save(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error);
KoneplusRmp *koneaimo_rmp_load_save_after_reset(RoccatDevice *koneaimo, guint profile_index, GError **error);

KoneplusRmp *koneaimo_rmp_load_actual(guint profile_index);
gboolean koneaimo_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error);
void koneaimo_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *koneaimo, guint profile_index);

void koneaimo_rmp_set_light_chose_type(KoneplusRmp *rmp, guint value);
guint koneaimo_rmp_get_light_chose_type(KoneplusRmp *rmp);
void koneaimo_rmp_set_custom_light_info(KoneplusRmp *rmp, guint index, KoneplusRmpLightInfo *rmp_light_info);
KoneplusRmpLightInfo *koneaimo_rmp_get_custom_light_info(KoneplusRmp *rmp, guint index);

KoneplusProfileSettings *koneaimo_rmp_to_profile_settings(KoneplusRmp *rmp);
void koneaimo_rmp_update_with_profile_settings(KoneplusRmp *rmp, KoneplusProfileSettings const *profile_settings);

guint koneaimo_rmp_cpi_to_bin(guint rmp_value);
guint koneaimo_bin_cpi_to_rmp(guint bin_value);

#define KONEAIMO_DBUS_SERVER_PATH "/org/roccat/Koneaimo"
#define KONEAIMO_DBUS_SERVER_INTERFACE "org.roccat.Koneaimo"

#define KONEAIMO_DEVICE_NAME "Kone AIMO"

G_END_DECLS

#endif
