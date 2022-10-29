#include <gtk/gtk.h>
#include "common.h"

void init_applet_user(GtkBox* box){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/appletUser.ui", execPath), NULL);
	GtkWidget *appletUserButton = GTK_WIDGET(gtk_builder_get_object(builder, "appletUserButton"));
	gtk_box_append(box, appletUserButton);
}
