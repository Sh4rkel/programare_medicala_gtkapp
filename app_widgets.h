#ifndef APP_WIDGETS_H
#define APP_WIDGETS_H

#include <gtk/gtk.h>

typedef struct {
    GtkApplication *app;
    GtkWidget *search_result_label;
    GtkWidget *nume_entry;
    GtkWidget *specialitate_entry;
    GtkWidget *loc_de_munca_entry;
    GtkWidget *lucreaza_cu_casa_de_asigurari_checkbox;
    GtkListStore *doctor_store;
    GtkWidget *display_label;
    GtkWidget *name_entry;
    GtkWidget *data_entry;
    GtkWidget *description_entry;
    gchar *selected_doctor;
    GtkWidget *doctor_view;
    GtkWidget *login_window;
    GtkWidget *main_window;
    GtkWidget *username_entry;
    GtkWidget *password_entry;
    GtkWidget *login_message_label;
    GtkWidget *search_entry;
} AppWidgets;

#endif
