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

#include "koneaimo_cpi_selector.h"
#include "roccat_cpi_selector.h"
#include "koneaimo.h"

#define KONEAIMO_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEAIMO_CPI_SELECTOR_TYPE, KoneaimoCpiSelectorClass))
#define IS_KONEAIMO_CPI_SELECTOR_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEAIMO_CPI_SELECTOR_TYPE))
#define KONEAIMO_CPI_SELECTOR_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEAIMO_CPI_SELECTOR_TYPE, KoneaimoCpiSelectorPrivate))

typedef struct _KoneaimoCpiSelectorClass KoneaimoCpiSelectorClass;

struct _KoneaimoCpiSelector {
	RoccatCpiSelector parent;
};

struct _KoneaimoCpiSelectorClass {
	RoccatCpiSelectorClass parent_class;
};

G_DEFINE_TYPE(KoneaimoCpiSelector, koneaimo_cpi_selector, ROCCAT_CPI_SELECTOR_TYPE);

void koneaimo_cpi_selector_set_from_rmp(KoneaimoCpiSelector *selector, KoneplusRmp *rmp) {
	RoccatCpiSelector *roccat_selector = ROCCAT_CPI_SELECTOR(selector);
	guint i;

	roccat_cpi_selector_set_selected(roccat_selector, koneplus_rmp_get_cpi_x(rmp));
	roccat_cpi_selector_set_all_active(roccat_selector, koneplus_rmp_get_cpi_all(rmp));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i)
		roccat_cpi_selector_set_value(roccat_selector, i, (gdouble)koneplus_rmp_get_cpi_level_x(rmp, i) * 200.0);
}

void koneaimo_cpi_selector_update_rmp(KoneaimoCpiSelector *selector, KoneplusRmp *rmp) {
	RoccatCpiSelector *roccat_selector = ROCCAT_CPI_SELECTOR(selector);
	guint i;

	koneplus_rmp_set_cpi_x(rmp, roccat_cpi_selector_get_selected(roccat_selector));
	koneplus_rmp_set_cpi_y(rmp, roccat_cpi_selector_get_selected(roccat_selector));
	koneplus_rmp_set_cpi_all(rmp, roccat_cpi_selector_get_all_active(roccat_selector));
	for (i = 0; i < KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM; ++i) {
		koneplus_rmp_set_cpi_level_x(rmp, i, (guint)((gdouble)roccat_cpi_selector_get_value(roccat_selector, i) / 200.0));
		koneplus_rmp_set_cpi_level_y(rmp, i, (guint)((gdouble)roccat_cpi_selector_get_value(roccat_selector, i) / 200.0));
	}
}

GtkWidget *koneaimo_cpi_selector_new(void) {
	KoneaimoCpiSelector *cpi_selector;

	cpi_selector = KONEAIMO_CPI_SELECTOR(g_object_new(KONEAIMO_CPI_SELECTOR_TYPE,
			"count", KONEPLUS_PROFILE_SETTING_CPI_LEVELS_NUM,
			"min", (gdouble)KONEAIMO_PROFILE_SETTING_CPI_MIN * 200.0,
			"max", (gdouble)KONEAIMO_PROFILE_SETTING_CPI_MAX * 200.0,
			"step", (gdouble)200.0,
			NULL));

	return GTK_WIDGET(cpi_selector);
}

static void koneaimo_cpi_selector_init(KoneaimoCpiSelector *cpi_selector) {}

static void koneaimo_cpi_selector_class_init(KoneaimoCpiSelectorClass *klass) {}
