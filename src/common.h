#ifndef COMMON_H
#define COMMON_H

//Global Variables
extern GtkBuilder* builder;
extern char* execPath;

//public functions
void init_dock(GtkApplication*);
void init_panel(GtkApplication*);
void init_applet_user(GtkBox*);
void init_applet_network(GtkBox*);
void init_applet_audio(GtkBox*);
void init_applet_time(GtkBox*);
void init_applet_power(GtkBox*);
void toplevel_manager_start(GtkWidget*, GtkApplication*);
void panel_set_menu_model(GDBusMenuModel*, GDBusActionGroup*);

#endif
