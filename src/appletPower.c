#include <gtk/gtk.h>
#include "common.h"

void init_applet_power(GtkBox* box){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/appletPower.ui", execPath), NULL);
	GtkWidget *appletPowerButton = GTK_WIDGET(gtk_builder_get_object(builder, "appletPowerButton"));
	gtk_box_append(box, appletPowerButton);
}
