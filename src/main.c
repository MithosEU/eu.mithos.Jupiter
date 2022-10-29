#include <stdio.h>
#include <gtk/gtk.h>
#include "common.h"

char* execPath;
GtkBuilder* builder;

static void on_activate(GtkApplication *app){
	//Set var with path to application data
	char buffer[1024];
	readlink("/proc/self/exe", buffer, sizeof(buffer));
	execPath = g_path_get_dirname(buffer);
	execPath[strlen(execPath)-4] = '\0';
	
	//Add custom icons
	GtkIconTheme* iconTheme = gtk_icon_theme_get_for_display(gdk_display_get_default());
	gtk_icon_theme_add_search_path(iconTheme, g_strdup_printf("%s/share/jupiter/icons/", execPath));
	g_print("%s/share/jupiter/icons", execPath);
	
	//Set custom theme
	GtkCssProvider* cssProvider = gtk_css_provider_new();
	gtk_css_provider_load_from_path(cssProvider, g_strdup_printf("%s/share/jupiter/twoModernExtra.css", execPath));
	gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(cssProvider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
	
	//Make an common builder
	builder = gtk_builder_new();

	//Load the two windows
	init_dock(app);
	init_panel(app);
}

//Just the starup itself, everything else is managed in on_activate
int main(int argc, char *argv[]){
	GtkApplication *app = gtk_application_new ("eu.mithos.Jupiter", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect (app, "activate", G_CALLBACK (on_activate), NULL);

	return(g_application_run(G_APPLICATION(app), argc, argv));
}
