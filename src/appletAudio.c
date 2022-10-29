#include <gtk/gtk.h>
#include "common.h"

void init_applet_audio(GtkBox* box){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/appletAudio.ui", execPath), NULL);
	GtkWidget *appletAudioButton = GTK_WIDGET(gtk_builder_get_object(builder, "appletAudioButton"));
	gtk_box_append(box, appletAudioButton);
}
