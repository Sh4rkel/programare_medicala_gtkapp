

#ifndef GTKAPP_FUNCTII_H
#define GTKAPP_FUNCTII_H
#include <gtk/gtk.h>
#include "app_widgets.h"
void display_consults_in_new_window(GtkWidget *widget, gpointer user_data);
void on_update_button_clicked(GtkWidget *widget, gpointer user_data);
void on_save_update_button_clicked(GtkWidget *widget, gpointer user_data);
void on_search_button_clicked(GtkWidget *widget, gpointer user_data);
void on_edit_button_clicked(GtkWidget *widget, gpointer user_data);
void create_main_window(GtkApplication *app, gpointer user_data);
gboolean is_valid_user(const gchar *username, const gchar *password);
void add_user(const gchar *username, const gchar *password, const gchar *salt);
void on_register_button_clicked(GtkWidget *widget, gpointer user_data);
void on_delete_button_clicked(GtkWidget *widget, gpointer user_data);
void on_save_button_clicked(GtkWidget *widget, gpointer user_data);
void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data);
void on_data_entry_icon_press(GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, gpointer user_data);
void on_selection_changed(GtkTreeSelection *selection, gpointer user_data);
void adauga_medic(AppWidgets *widgets, const gchar *nume, const gchar *specialitate, const gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari);
void on_adauga_button_clicked(GtkWidget *widget, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);

#endif