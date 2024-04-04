#ifndef APP_WIDGETS_H
#define APP_WIDGETS_H

#include <gtk/gtk.h>

typedef struct {
    GtkWidget *nume_entry;
    GtkWidget *specialitate_entry;
    GtkWidget *loc_de_munca_entry;
    GtkWidget *lucreaza_cu_casa_de_asigurari_checkbox;
    GtkListStore *doctor_store;
    GtkWidget *display_label;
    GtkWidget *name_entry;
    GtkWidget *data_entry;
    GtkWidget *description_entry;
} AppWidgets;

#endif
