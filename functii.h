

#ifndef GTKAPP_FUNCTII_H
#define GTKAPP_FUNCTII_H
#include <gtk/gtk.h>
#include "app_widgets.h"
void on_delete_button_clicked(GtkWidget *widget, gpointer user_data);
void on_save_button_clicked(GtkWidget *widget, gpointer user_data);
void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data);
void on_data_entry_icon_press(GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, gpointer user_data);
void on_selection_changed(GtkTreeSelection *selection, gpointer user_data);
void adauga_medic(AppWidgets *widgets, gchar *nume, gchar *specialitate, gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari);
void on_adauga_button_clicked(GtkWidget *widget, gpointer user_data);
void activate(GtkApplication *app, gpointer user_data);

#endif