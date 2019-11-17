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

#include "koneaimo.h"
#include "roccat_helper.h"
#include "koneplus_rmp.h"
#include "roccat.h"
#include "i18n-lib.h"
#include <string.h>

static gchar *koneaimo_profile_dir(void) {
	gchar *base = roccat_profile_dir();
	gchar *dir = g_build_path("/", base, "koneaimo", NULL);
	g_free(base);
	return dir;
}

static gboolean koneaimo_profile_dir_create_if_needed(GError **error) {
	gchar *dir;
	gboolean retval;

	dir = koneaimo_profile_dir();
	retval = roccat_profile_dir_create_if_needed(dir, error);
	g_free(dir);
	return retval;
}

static gboolean koneplus_profile_settings_check_equality(KoneplusProfileSettings const *left, KoneplusProfileSettings const *right) {
	if (memcmp(left, right, sizeof(KoneplusProfileSettings)))
		return FALSE;
	return TRUE;
}

static gchar *koneaimo_build_config_rmp_path(guint profile_number) {
	gchar *dir = koneaimo_profile_dir();
	gchar *filename = g_strdup_printf("actual%i." ROCCAT_MOUSE_PROFILE_EXTENSION, profile_number + 1);
	gchar *path = g_build_path("/", dir, filename, NULL);
	g_free(filename);
	g_free(dir);
	return path;
}

static gboolean koneplus_rmp_profile_buttons_modified(KoneplusRmp const *rmp) {
	guint i;

	for (i = 0; i < KONEPLUS_PROFILE_BUTTON_NUM; ++i) {
		if (rmp->modified_macros[i])
			return TRUE;
	}
	return FALSE;
}

KoneplusRmp *koneaimo_rmp_load_actual(guint profile_index) {
	KoneplusRmp *rmp;
	gchar *config_path;

	config_path = koneaimo_build_config_rmp_path(profile_index);
	rmp = koneplus_rmp_read_with_path(config_path, koneaimo_rmp_defaults(), NULL);
	g_free(config_path);
	if (!rmp) {
		rmp = koneaimo_default_rmp();
		rmp->modified_rmp = TRUE;
	}
	return rmp;
}

static void koneplus_rmp_update_with_hardware_macro(KoneplusRmp *rmp, RoccatDevice *device, guint profile_index, guint profile_buttons_index) {
	GError *error = NULL;
	KoneplusMacro *hardware_macro;
	hardware_macro = koneplus_macro_read(device, profile_index, profile_buttons_index, &error);
	if (error) {
		g_clear_error(&error);
	} else {
		koneplus_rmp_update_with_macro(rmp, profile_buttons_index, hardware_macro);
		g_free(hardware_macro);
	}
}

void koneaimo_rmp_update_from_device(KoneplusRmp *rmp, RoccatDevice *koneaimo, guint profile_index) {
	KoneplusProfileButtons *hardware_profile_buttons, *rmp_profile_buttons;
	KoneplusProfileSettings *hardware_profile_settings, *rmp_profile_settings;
	GError *error = NULL;
	guint i;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(koneaimo));

	/* hardware takes precedence */
	hardware_profile_settings = koneplus_profile_settings_read(koneaimo, profile_index, &error);

	if (hardware_profile_settings) {
		rmp_profile_settings = koneaimo_rmp_to_profile_settings(rmp);
		koneplus_profile_settings_finalize(rmp_profile_settings, profile_index);

		if (!koneplus_profile_settings_check_equality(hardware_profile_settings, rmp_profile_settings)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			koneaimo_rmp_update_with_profile_settings(rmp, hardware_profile_settings);
		}
		g_free(rmp_profile_settings);
		g_free(hardware_profile_settings);
	} else
		g_clear_error(&error);

	hardware_profile_buttons = koneplus_profile_buttons_read(koneaimo, profile_index, &error);

	if (hardware_profile_buttons) {
		rmp_profile_buttons = koneplus_rmp_to_profile_buttons(rmp);

		if (!koneplus_profile_buttons_equal(hardware_profile_buttons, rmp_profile_buttons)) {
			g_warning(_("Hardware data and rmp for profile %u differ"), profile_index + 1);
			koneplus_rmp_update_with_profile_buttons(rmp, hardware_profile_buttons);
		}

		g_free(rmp_profile_buttons);

		for (i = 0; i < KONEPLUS_PROFILE_BUTTON_NUM; ++i) {
			if (hardware_profile_buttons->buttons[i].type == KONEPLUS_PROFILE_BUTTON_TYPE_MACRO)
				koneplus_rmp_update_with_hardware_macro(rmp, koneaimo, profile_index, i);
		}

		g_free(hardware_profile_buttons);
	} else
		g_clear_error(&error);

	gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneaimo));
}

KoneplusRmp *koneaimo_rmp_load(RoccatDevice *koneaimo, guint profile_index, GError **error) {
	KoneplusRmp *rmp;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	rmp = koneaimo_rmp_load_actual(profile_index);

	koneaimo_rmp_update_from_device(rmp, koneaimo, profile_index);

	return rmp;
}

gboolean koneaimo_rmp_save_actual(KoneplusRmp *rmp, guint profile_index, GError **error) {
	gchar *config_path;
	gboolean retval;

	koneaimo_profile_dir_create_if_needed(error);
	if (*error)
		return FALSE;

	config_path = koneaimo_build_config_rmp_path(profile_index);
	retval = koneplus_rmp_write_with_path(config_path, rmp, error);
	g_free(config_path);

	return retval;
}

static gboolean koneaimo_rmp_save_profile_settings(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error) {
	KoneplusProfileSettings *profile_settings;
	gboolean retval;

	profile_settings = koneaimo_rmp_to_profile_settings(rmp);
	retval = koneplus_profile_settings_write(koneaimo, profile_index, profile_settings, error);
	g_free(profile_settings);

	return retval;
}

static gboolean koneplus_rmp_save_profile_buttons(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error) {
	KoneplusProfileButtons *profile_buttons;
	gboolean retval;

	profile_buttons = koneplus_rmp_to_profile_buttons(rmp);
	retval = koneplus_profile_buttons_write(koneaimo, profile_index, profile_buttons, error);
	g_free(profile_buttons);

	return retval;
}

static gboolean koneplus_rmp_save_macro(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, guint key_index, GError **error) {
	KoneplusRmpMacroKeyInfo *macro_key_info;
	KoneplusMacro *macro;
	guint retval = TRUE;

	macro_key_info = koneplus_rmp_get_macro_key_info(rmp, key_index);
	macro = koneplus_rmp_macro_key_info_to_macro(macro_key_info);
	g_free(macro_key_info);

	if (macro) {
		retval = koneplus_macro_write(koneaimo, profile_index, key_index, macro, error);
		g_free(macro);
	}

	return retval;
}

static gboolean koneaimo_rmp_save_actual_when_needed(KoneplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (koneplus_rmp_get_modified(rmp)) {
		retval = koneaimo_rmp_save_actual(rmp, profile_index, error);
		rmp->modified_rmp = FALSE;
	}

	return retval;
}

static gboolean koneaimo_rmp_save_profile_settings_when_needed(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;

	if (rmp->modified_settings) {
		retval = koneaimo_rmp_save_profile_settings(koneaimo, rmp, profile_index, error);
		rmp->modified_settings = FALSE;
	}

	return retval;
}

static gboolean koneplus_rmp_save_macro_when_needed(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, guint key_index, GError **error) {
	gboolean retval = TRUE;

	if (rmp->modified_macros[key_index]) {
		retval = koneplus_rmp_save_macro(koneaimo, rmp, profile_index, key_index, error);
		rmp->modified_macros[key_index] = FALSE;
	}

	return retval;
}

static gboolean koneplus_rmp_save_profile_buttons_when_needed(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = TRUE;
	guint i;

	if (koneplus_rmp_profile_buttons_modified(rmp)) {
		retval = koneplus_rmp_save_profile_buttons(koneaimo, rmp, profile_index, error);
		if (!retval)
			return FALSE;

		for (i = 0; i < KONEPLUS_PROFILE_BUTTON_NUM; ++i) {
			if (!koneplus_rmp_save_macro_when_needed(koneaimo, rmp, profile_index, i, error))
				return FALSE;
		}
	}
	return retval;
}

gboolean koneaimo_rmp_save(RoccatDevice *koneaimo, KoneplusRmp *rmp, guint profile_index, GError **error) {
	gboolean retval = FALSE;

	g_assert(profile_index < KONEPLUS_PROFILE_NUM);

	if (!koneaimo_rmp_save_actual_when_needed(rmp, profile_index, error)) return FALSE;

	gaminggear_device_lock(GAMINGGEAR_DEVICE(koneaimo));

	if (!koneplus_rmp_save_profile_buttons_when_needed(koneaimo, rmp, profile_index, error)) goto error;
	if (!koneaimo_rmp_save_profile_settings_when_needed(koneaimo, rmp, profile_index, error)) goto error;

	retval = TRUE;
error:
	gaminggear_device_unlock(GAMINGGEAR_DEVICE(koneaimo));
	return retval;
}

KoneplusRmp *koneaimo_rmp_load_save_after_reset(RoccatDevice *koneaimo, guint profile_index, GError **error) {
	KoneplusRmp *rmp;

	rmp = koneaimo_default_rmp();

	/* Hardware profiles differ in light-color and -effects */
	koneaimo_rmp_update_from_device(rmp, koneaimo, profile_index);
	koneplus_rmp_set_unmodified(rmp);

	koneaimo_rmp_save_actual(rmp, profile_index, error);
	return rmp;
}
