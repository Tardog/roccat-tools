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

#include "koneaimo_gfx.h"
#include "talkfx.h"
#include <gaminggear/gfx.h>
#include <string.h>

#define KONEAIMO_GFX_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE((obj), KONEAIMO_GFX_TYPE, KoneaimoGfxPrivate))

typedef struct _KoneaimoGfxColor KoneaimoGfxColor;
typedef struct _KoneaimoGfxData KoneaimoGfxData;

struct _KoneaimoGfxColor {
	guint8 red;
	guint8 green;
	guint8 blue;
} __attribute__ ((packed));

struct _KoneaimoGfxData {
	guint8 report_id; /* KONEPLUS_REPORT_ID_TALK */
	guint8 size; /* always 16 */
	guint8 mode;
	guint8 effect;
	KoneaimoGfxColor colors[KONEPLUS_PROFILE_SETTING_LIGHTS_NUM];
} __attribute__ ((packed));

typedef enum {
	KONEAIMO_GFX_MODE_GFX = 0x02,
} KoneaimoGfxMode;

typedef enum {
	KONEAIMO_GFX_EFFECT_OFF = 0x00,
	KONEAIMO_GFX_EFFECT_ON = 0x01,
	KONEAIMO_GFX_EFFECT_BLINK = 0x02,
} KoneaimoGfxEffect;

struct _KoneaimoGfxPrivate {
	RoccatDevice *device;
	KoneaimoGfxData *data;
};

enum {
	PROP_0,
	PROP_DEVICE,
};

G_DEFINE_TYPE(KoneaimoGfx, koneaimo_gfx, G_TYPE_OBJECT);

KoneaimoGfx *koneaimo_gfx_new(RoccatDevice * const device) {
	return KONEAIMO_GFX(g_object_new(KONEAIMO_GFX_TYPE,
			"device", device,
			NULL));
}

static void koneaimo_gfx_init(KoneaimoGfx *gfx) {
	gfx->priv = KONEAIMO_GFX_GET_PRIVATE(gfx);
}

static void koneaimo_gfx_set_property(GObject *object, guint prop_id, GValue const *value, GParamSpec *pspec) {
	KoneaimoGfxPrivate *priv = KONEAIMO_GFX(object)->priv;
	switch(prop_id) {
	case PROP_DEVICE:
		priv->device = ROCCAT_DEVICE(g_value_get_object(value));
		g_object_ref(G_OBJECT(priv->device));
		break;
	default:
		G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
		break;
	}
}

static GObject *koneaimo_gfx_constructor(GType gtype, guint n_properties, GObjectConstructParam *properties) {
	KoneaimoGfxPrivate *priv;
	KoneaimoGfx *gfx;
	GObject *obj;

	obj = G_OBJECT_CLASS(koneaimo_gfx_parent_class)->constructor(gtype, n_properties, properties);
	gfx = KONEAIMO_GFX(obj);
	priv = gfx->priv;

	priv->data = (KoneaimoGfxData *)koneplus_device_read(priv->device, KONEPLUS_REPORT_ID_TALK, sizeof(KoneaimoGfxData), NULL);
	if (!priv->data)
		priv->data = (KoneaimoGfxData *)g_malloc0(sizeof(KoneaimoGfxData));

	priv->data->report_id = KONEPLUS_REPORT_ID_TALK;
	priv->data->size = sizeof(KoneaimoGfxData);
	priv->data->mode = KONEAIMO_GFX_MODE_GFX;
	priv->data->effect = KONEAIMO_GFX_EFFECT_ON;

	return obj;
}

static void koneaimo_gfx_finalize(GObject *object) {
	KoneaimoGfxPrivate *priv = KONEAIMO_GFX(object)->priv;
	g_object_unref(G_OBJECT(priv->device));
	g_free(priv->data);
	G_OBJECT_CLASS(koneaimo_gfx_parent_class)->finalize(object);
}

static void koneaimo_gfx_class_init(KoneaimoGfxClass *gfx_class) {
	GObjectClass *gobject_class = (GObjectClass*)gfx_class;
	gobject_class->constructor = koneaimo_gfx_constructor;
	gobject_class->finalize = koneaimo_gfx_finalize;
	gobject_class->set_property = koneaimo_gfx_set_property;

	g_type_class_add_private(gfx_class, sizeof(KoneaimoGfxPrivate));

	g_object_class_install_property(gobject_class, PROP_DEVICE,
			g_param_spec_object("device",
					"device",
					"Device",
					ROCCAT_DEVICE_TYPE,
					G_PARAM_WRITABLE | G_PARAM_CONSTRUCT_ONLY));
}

gboolean koneaimo_gfx_update(KoneaimoGfx *gfx, GError **error) {
	KoneaimoGfxPrivate *priv = gfx->priv;
	return koneplus_device_write(priv->device, (gchar const *)priv->data, sizeof(KoneaimoGfxData), error);
}

guint32 koneaimo_gfx_get_color(KoneaimoGfx *gfx, guint index) {
	KoneaimoGfxData *data = gfx->priv->data;
	guint32 color = 0;
	gfx_color_set_brightness(&color, 0xff);
	gfx_color_set_red(&color, data->colors[index].red);
	gfx_color_set_green(&color, data->colors[index].green);
	gfx_color_set_blue(&color, data->colors[index].blue);
	return color;
}

void koneaimo_gfx_set_color(KoneaimoGfx *gfx, guint index, guint32 color) {
	KoneaimoGfxData *data = gfx->priv->data;
	data->colors[index].red = gfx_color_get_red(color);
	data->colors[index].green = gfx_color_get_green(color);
	data->colors[index].blue = gfx_color_get_blue(color);
}
