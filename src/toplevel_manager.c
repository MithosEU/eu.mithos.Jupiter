#include <gtk/gtk.h>
#include <wayland-client.h>
#include <gdk/wayland/gdkwayland.h>
#include "foreing-toplevel.h"
#include "common.h"

struct wl_display* wlDisplay;
struct wl_seat* wlSeat;
struct wl_registry* wlRegistry;
static struct zwlr_foreign_toplevel_manager_v1 *toplevel_manager;
GtkWidget* applistBox;
GDBusConnection* dbusConnection;

//Switch to app if according button is pressed
void switch_app(GtkWidget* button){
	struct zwlr_foreign_toplevel_handle_v1* toplevel = g_object_get_data(G_OBJECT(button), "toplevel-main");
	zwlr_foreign_toplevel_handle_v1_activate(toplevel, wlSeat);
	zwlr_foreign_toplevel_handle_v1_set_maximized(toplevel);
}

//there are some unimportant signals which must be connected this function is used for those
static void not_care(){/*nobody cares*/};

//handle closing of toplevel
static void z_toplevel_handle_close(void** data, struct zwlr_foreign_toplevel_handle_v1*){
	GtkWidget* appButton = *data;
	
	gchar* test = g_object_get_data(G_OBJECT(appButton), "toplevel-count");
	g_object_set_data(G_OBJECT(appButton), "toplevel-count", g_strdup_printf("%d", atoi(test)-1));
	
	if((atoi(test)-1)<=0){
		gtk_box_remove(GTK_BOX(applistBox), appButton);
		free(data);
	}
}

//handling a newly opend toplevel
static void z_toplevel_handle_app(void** data, struct zwlr_foreign_toplevel_handle_v1* toplevel, const char *appID){
	GtkWidget* appButton;
	
	GtkWidget* applistItem = gtk_widget_get_first_child(applistBox);
	do{
		if(!GTK_IS_WIDGET(applistItem)){break;}
	
		if(GTK_IS_BUTTON(applistItem) && g_strcmp0(appID, gtk_widget_get_name(applistItem)) == 0){
			appButton = applistItem;
			*data = (void *)appButton;
			
			gchar* test = g_object_get_data(G_OBJECT(appButton), "toplevel-count");
			g_object_set_data(G_OBJECT(appButton), "toplevel-count", g_strdup_printf("%d", atoi(test)+1));

			return;
		}
	} while(GTK_IS_WIDGET(applistItem = gtk_widget_get_next_sibling(applistItem)));
	
	appButton = gtk_button_new();	
	gtk_widget_set_name(appButton, appID);
	
	//Look for a fitting icon
	GtkIconTheme* icTh = gtk_icon_theme_get_for_display(gdk_display_get_default());
	if(gtk_icon_theme_has_icon(icTh, appID)){
		GtkWidget* img = gtk_image_new_from_icon_name(appID);
		gtk_image_set_pixel_size(GTK_IMAGE(img), 60);
		gtk_button_set_child(GTK_BUTTON(appButton), img);
	} else {
		gtk_button_set_icon_name(GTK_BUTTON(appButton), "application-default-icon");
	}
	// ---
	
	gtk_style_context_add_class(gtk_widget_get_style_context(appButton), "dashButton");
	gtk_box_prepend(GTK_BOX(applistBox), appButton);
	
	g_object_set_data(G_OBJECT(appButton), "toplevel-main", toplevel);
	gchar* test = "1"; //would be better to use int* instead but i failed (pointers are hard)
	g_object_set_data(G_OBJECT(appButton), "toplevel-count", test);
	
	zwlr_foreign_toplevel_handle_v1_set_maximized(toplevel);
	
	//connect button
	g_signal_connect(G_OBJECT(appButton), "clicked", G_CALLBACK(switch_app), NULL);
	
	*data = (void *)appButton;
}

static void action_group_add(GActionGroup *ag){
	g_action_group_list_actions(G_ACTION_GROUP(ag));
}

//handle activation of window
static void wh_state(void** data, struct zwlr_foreign_toplevel_handle_v1 *, struct wl_array *state){
	uint32_t *entry;
	wl_array_for_each(entry, state){
		if(*entry == ZWLR_FOREIGN_TOPLEVEL_HANDLE_V1_STATE_ACTIVATED){
			const gchar* appId = gtk_widget_get_name(*data);
			if(g_dbus_connection_get_unique_name(dbusConnection) != NULL && appId != NULL && g_dbus_is_name(appId) != 0){
				gchar* appIdPath = g_strjoinv("/", g_strsplit(appId, ".", -1));
				gchar* dbusPath = g_strdup_printf("/%s/menus/menubar", appIdPath);
				GDBusMenuModel* dbusMenuModel = g_dbus_menu_model_get(dbusConnection, appId, dbusPath);
				gchar* dbusPathAction = g_strdup_printf("/%s", appIdPath);
				GDBusActionGroup* dbusActionGroup = g_dbus_action_group_get(dbusConnection, appId, dbusPathAction);
				g_signal_connect(dbusActionGroup, "action-added", G_CALLBACK(action_group_add), NULL);
				g_action_group_list_actions(G_ACTION_GROUP(dbusActionGroup));
				panel_set_menu_model(dbusMenuModel, dbusActionGroup);
			}else{
				panel_set_menu_model(NULL, NULL);
			}
		}
	}
}

//struct for ft_handle_toplevel (possible actions of/on windows)
static const struct zwlr_foreign_toplevel_handle_v1_listener z_toplevel_listener = {
	.title = not_care,
	.app_id = z_toplevel_handle_app,
	.output_enter = not_care,
	.output_leave = not_care,
	.done = not_care,
	.state = wh_state,
	.closed = z_toplevel_handle_close,
	.parent = not_care
};

//handle if new toplevel is opend
static void toplevel_manager_handle_toplevel(void*, struct zwlr_foreign_toplevel_manager_v1*,  struct zwlr_foreign_toplevel_handle_v1 *ztoplevel){
	void** data = calloc(2, sizeof(gpointer));
	zwlr_foreign_toplevel_handle_v1_add_listener(ztoplevel, &z_toplevel_listener, data);
}

//struct for wl_registry_handle_global
static const struct zwlr_foreign_toplevel_manager_v1_listener toplevel_listener = {
	.toplevel = toplevel_manager_handle_toplevel,
	.finished = not_care,
};

//connecting from global wayland listener to foreign toplevel listener
static void wl_registry_handle_global(void*, struct wl_registry *wlRegistry, uint32_t id, const char *interface, uint32_t version){
	if (strcmp (interface, zwlr_foreign_toplevel_manager_v1_interface.name) == 0){
		//bind and add listener if fitting protocol (wlr-foreign-toplevel) is found
		toplevel_manager = wl_registry_bind(wlRegistry, id, &zwlr_foreign_toplevel_manager_v1_interface, version);
		zwlr_foreign_toplevel_manager_v1_add_listener(toplevel_manager, &toplevel_listener, NULL);
	}
}

//struct for toplevel_manager_start/global listener
static const struct wl_registry_listener wlRegistryListener = {
    .global = wl_registry_handle_global,
    .global_remove = not_care,
};

void toplevel_manager_start(GtkWidget* applistBoxArg, GtkApplication* appArg){
	dbusConnection = g_application_get_dbus_connection(G_APPLICATION(appArg));

	applistBox = applistBoxArg;
	wlDisplay = gdk_wayland_display_get_wl_display(gdk_display_get_default());
	GdkSeat* gdkSeat = gdk_display_get_default_seat(gdk_display_get_default());
	wlSeat = gdk_wayland_seat_get_wl_seat(gdkSeat);
	wlRegistry = wl_display_get_registry(wlDisplay);
	wl_registry_add_listener(wlRegistry, &wlRegistryListener, NULL);
	wl_display_roundtrip(wlDisplay);
}