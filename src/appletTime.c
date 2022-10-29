#include <gtk/gtk.h>
#include "common.h"

GtkWidget* appletTimeButton;

int set_time(){
	GDateTime* dateTime	= g_date_time_new_now_local();
	gchar* timeString = g_date_time_format(dateTime, "%H:%M");
	gtk_menu_button_set_label(GTK_MENU_BUTTON(appletTimeButton), timeString);
	return(TRUE);
}

void init_applet_time(GtkBox* box){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/appletTime.ui", execPath), NULL);
	appletTimeButton = GTK_WIDGET(gtk_builder_get_object(builder, "appletTimeButton"));
	gtk_box_append(box, appletTimeButton);
	
	set_time();
	g_timeout_add_seconds(1, G_SOURCE_FUNC(set_time), NULL);
}
