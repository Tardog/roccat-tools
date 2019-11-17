#ifndef __ROCCAT_KONEAIMOCONFIG_CONFIGURATION_EDITOR_H__
#define __ROCCAT_KONEAIMOCONFIG_CONFIGURATION_EDITOR_H__

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

#include "roccat_key_file.h"
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define KONEAIMOCONFIG_CONFIGURATION_DIALOG_TYPE (koneaimoconfig_configuration_dialog_get_type())
#define KONEAIMOCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), KONEAIMOCONFIG_CONFIGURATION_DIALOG_TYPE, KoneaimoconfigConfigurationDialog))
#define IS_KONEAIMOCONFIG_CONFIGURATION_DIALOG(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), KONEAIMOCONFIG_CONFIGURATION_DIALOG_TYPE))

typedef struct _KoneaimoconfigConfigurationDialog KoneaimoconfigConfigurationDialog;
typedef struct _KoneaimoconfigConfigurationDialogPrivate KoneaimoconfigConfigurationDialogPrivate;

struct _KoneaimoconfigConfigurationDialog {
	GtkDialog dialog;
	KoneaimoconfigConfigurationDialogPrivate *priv;
};

GType koneaimoconfig_configuration_dialog_get_type(void);
gboolean koneaimoconfig_configuration_dialog(GtkWindow *parent, RoccatKeyFile *config);
GtkWidget *koneaimoconfig_configuration_dialog_new(GtkWindow *parent);

void koneaimoconfig_configuration_dialog_get_configuration(KoneaimoconfigConfigurationDialog *koneaimoconfig_configuration_dialog, RoccatKeyFile *config);
void koneaimoconfig_configuration_dialog_set_configuration(KoneaimoconfigConfigurationDialog *koneaimoconfig_configuration_dialog, RoccatKeyFile *config);

gboolean koneaimoconfig_configuration_save(GtkWindow *parent, RoccatKeyFile *config);

G_END_DECLS

#endif
