#include <gtk/gtk.h>
#include "common.h"

void init_applet_network(GtkBox* box){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/appletNetwork.ui", execPath), NULL);
	GtkWidget *appletNetwork = GTK_WIDGET(gtk_builder_get_object(builder, "appletNetworkButton"));
	gtk_box_append(box, appletNetwork);
}
