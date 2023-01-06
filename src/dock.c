#include <gtk/gtk.h>
#include "common.h"

//Variables
int menuOpen = FALSE;
int height = 100;

//Gtk Widgets
GtkWidget* menuWindow;
GtkWidget* dockWindow;
GtkWidget* tmpLosEntry;
GtkWidget* tmpLosButton;
GtkWidget* dockMenuButton;

void show_dock(GtkEventControllerMotion*, gdouble x){
	if(x <= 5){
		GtkWidget* dockRevealer = GTK_WIDGET(gtk_builder_get_object(builder, "dockRevealer"));
		gtk_widget_show(dockRevealer);
	}
}

void hide_dock(){
	if(menuOpen == FALSE && FALSE){ //todo implement setting if dock should be hidden
		GtkWidget* dockRevealer = GTK_WIDGET(gtk_builder_get_object(builder, "dockRevealer"));
		gtk_widget_hide(dockRevealer);
		int cHeight = gtk_widget_get_height(dockWindow);
		gtk_window_set_default_size(GTK_WINDOW(dockWindow), 5, cHeight);
	}
}

void menu_activate(){
	if(!menuOpen){
		menuOpen = TRUE;
		gtk_widget_show(menuWindow);
		panel_set_menu_model(NULL, NULL);
	} else {
		menuOpen = FALSE;
		gtk_widget_hide(menuWindow);
	}
}

void tmpLosAction(){
	g_spawn_command_line_async(gtk_editable_get_text(GTK_EDITABLE(tmpLosEntry)), NULL);
	gtk_editable_set_text(GTK_EDITABLE(tmpLosEntry), "");
	menu_activate();
	hide_dock();
}

void init_dock(GtkApplication* app){
	//Get Widgets
	gtk_builder_add_from_file(builder, g_strdup_printf("%s/share/jupiter/dock.ui", execPath), NULL);
	dockWindow = GTK_WIDGET(gtk_builder_get_object(builder, "dockWindow"));
	dockMenuButton = GTK_WIDGET(gtk_builder_get_object(builder, "dockMenuButton"));
	menuWindow = GTK_WIDGET(gtk_builder_get_object(builder, "menuWindow"));
	tmpLosButton = GTK_WIDGET(gtk_builder_get_object(builder, "tmpLosButton"));
	tmpLosEntry = GTK_WIDGET(gtk_builder_get_object(builder, "tmpLosEntry"));
	
	//Start dock and present
	gtk_window_set_application(GTK_WINDOW(dockWindow), app);
	gtk_window_present(GTK_WINDOW(dockWindow));
	
	//audohide etc here (to do)
	GtkWidget* dockRevealer = GTK_WIDGET(gtk_builder_get_object(builder, "dockRevealer"));
	//gtk_widget_hide(dockRevealer);
	GtkEventController* eventController = gtk_event_controller_motion_new();
	g_signal_connect(G_OBJECT(eventController), "motion", G_CALLBACK(show_dock), NULL);
	g_signal_connect(G_OBJECT(eventController), "leave", G_CALLBACK(hide_dock), NULL);
	gtk_widget_add_controller(GTK_WIDGET(gtk_builder_get_object(builder, "dockContainer")), eventController);
	
	//menubutton stuff
	g_signal_connect(G_OBJECT(dockMenuButton), "clicked", G_CALLBACK(menu_activate), NULL);
	gtk_window_set_application(GTK_WINDOW(menuWindow), app);
	gtk_window_present(GTK_WINDOW(menuWindow));
	gtk_widget_hide(menuWindow);
	
	//tmpGo stuff
	g_signal_connect(G_OBJECT(tmpLosButton), "clicked", G_CALLBACK(tmpLosAction), NULL);
	g_signal_connect(G_OBJECT(tmpLosEntry), "activate", G_CALLBACK(tmpLosAction), NULL);
	
	//start manager for foreign toplevels
	toplevel_manager_start(GTK_WIDGET(gtk_builder_get_object(builder, "applistBox")), app);
}
