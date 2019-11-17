#ifndef __ROCCAT_KONEAIMO_GFX_H__
#define __ROCCAT_KONEAIMO_GFX_H__

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
#include "roccat_device.h"

G_BEGIN_DECLS

#define KONEAIMO_GFX_TYPE (koneaimo_gfx_get_type())
#define KONEAIMO_GFX(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEAIMO_GFX_TYPE, KoneaimoGfx))
#define IS_KONEAIMO_GFX(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEAIMO_GFX_TYPE))
#define KONEAIMO_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), KONEAIMO_GFX_TYPE, KoneaimoGfxClass))
#define IS_KONEAIMO_GFX_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE((klass), KONEAIMO_GFX_TYPE))

typedef struct _KoneaimoGfx KoneaimoGfx;
typedef struct _KoneaimoGfxClass KoneaimoGfxClass;
typedef struct _KoneaimoGfxPrivate KoneaimoGfxPrivate;

struct _KoneaimoGfx {
	GObject parent;
	KoneaimoGfxPrivate *priv;
};

struct _KoneaimoGfxClass {
	GObjectClass parent_class;
};

GType koneaimo_gfx_get_type(void);
KoneaimoGfx *koneaimo_gfx_new(RoccatDevice * const device);

gboolean koneaimo_gfx_update(KoneaimoGfx *gfx, GError **error);

guint32 koneaimo_gfx_get_color(KoneaimoGfx *gfx, guint index);
void koneaimo_gfx_set_color(KoneaimoGfx *gfx, guint index, guint32 color);

G_END_DECLS

#endif
