#include <gtk/gtk.h>
#include "common.h"

GtkWidget* panelWindow;

void panel_set_menu_model(GDBusMenuModel* dbusMenuModel, GDBusActionGroup* dbusActionGroup){
	GtkWidget *panelMenuBar = GTK_WIDGET(gtk_builder_get_object(builder, "panelMenuBar"));
	
	if(G_IS_MENU_MODEL(dbusMenuModel)){
		gtk_popover_menu_bar_set_menu_model(GTK_POPOVER_MENU_BAR(panelMenuBar), G_MENU_MODEL(dbusMenuModel));
		gtk_widget_insert_action_group(panelMenuBar, "app", G_ACTION_GROUP(dbusActionGroup));
	}else{
		gtk_popover_menu_bar_set_menu_model(GTK_POPOVER_MENU_BAR(panelMenuBar), G_MENU_MODEL(gtk_builder_get_object(builder, "panelMenuDefault")));
	}
}

void init_panel(GtkApplication* app){
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/panel.ui", execPath), NULL);
	panelWindow = GTK_WIDGET(gtk_builder_get_object(builder, "panelWindow"));
	gtk_window_set_application(GTK_WINDOW(panelWindow), app);
	gtk_window_present(GTK_WINDOW(panelWindow));
	
	GtkBox* appletBox = GTK_BOX(gtk_builder_get_object(builder, "appletBox"));
	init_applet_time(appletBox);
	init_applet_audio(appletBox);
	init_applet_network(appletBox);
	init_applet_power(appletBox);
	init_applet_user(appletBox);
}
