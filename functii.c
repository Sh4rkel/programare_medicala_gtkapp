#include "functii.h"
#include "medic_info.h"

GtkTreeIter selected_iter;
gboolean is_row_selected = FALSE;

void on_save_button_clicked(GtkWidget *widget, gpointer user_data) {
    GtkTreeModel *model;
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(widgets->name_entry));
    const gchar *data = gtk_entry_get_text(GTK_ENTRY(widgets->data_entry));
    const gchar *description = gtk_entry_get_text(GTK_ENTRY(widgets->description_entry));

    if (g_strcmp0(nume, "") == 0 || g_strcmp0(data, "") == 0 || g_strcmp0(description, "") == 0) {
        gtk_label_set_text(GTK_LABEL(widgets->display_label), "Toate câmpurile trebuie completate pentru a salva o programare.");
        return;
    }

    gchar *display_text = g_strdup_printf("Programarea dvs. a fost salvata. Multumim pentru incredere!");
    gtk_label_set_text(GTK_LABEL(widgets->display_label), display_text);

    FILE *file = fopen("../programari.txt", "a");
    if (file != NULL) {
        if (widgets->selected_doctor != NULL) {
            fprintf(file, "Doctor: %s\nNume: %s\nData: %s\nDescriere: %s\n\n", widgets->selected_doctor, nume, data, description);
        } else {
            fprintf(file, "Doctor: %s\nNume: %s\nData: %s\nDescriere: %s\n\n", "Unknown", nume, data, description);
        }
        fclose(file);
    }

    g_free(display_text);
}

void on_calendar_day_selected(GtkCalendar *calendar, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    guint day, month, year;
    gtk_calendar_get_date(calendar, &year, &month, &day);
    gchar *date_string = g_strdup_printf("%02d-%02d-%04d", day, month + 1, year);
    gtk_entry_set_text(GTK_ENTRY(widgets->data_entry), date_string);
    g_free(date_string);
}

void on_data_entry_icon_press(GtkEntry *entry, GtkEntryIconPosition icon_pos, GdkEvent *event, gpointer user_data) {
    if (icon_pos == GTK_ENTRY_ICON_SECONDARY) {
        GtkWidget *calendar_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(calendar_window), "Select a Date");
        gtk_window_set_default_size(GTK_WINDOW(calendar_window), 200, 200);
        GtkWidget *calendar = gtk_calendar_new();
        g_signal_connect(calendar, "day-selected", G_CALLBACK(on_calendar_day_selected), user_data);
        gtk_container_add(GTK_CONTAINER(calendar_window), calendar);
        gtk_widget_show_all(calendar_window);
    }
}


void on_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
    GtkTreeModel *model;
    AppWidgets *widgets = (AppWidgets *)user_data;

    if (gtk_tree_selection_get_selected(selection, &model, &selected_iter)) {
        gchar *nume;
        gchar *specialitate;
        gchar *loc_de_munca;
        gboolean lucreaza_cu_casa_de_asigurari;

        gtk_tree_model_get(model, &selected_iter, 0, &nume, 1, &specialitate, 2, &loc_de_munca, 3, &lucreaza_cu_casa_de_asigurari, -1);

        g_print("Selected row data: %s, %s, %s, %d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);

        if (widgets->selected_doctor != NULL) {
            g_free(widgets->selected_doctor);
        }
        widgets->selected_doctor = g_strdup(nume);

        GtkWidget *details_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(details_window), "Programeaza-te la medic");
        gtk_window_set_default_size(GTK_WINDOW(details_window), 500, 350);

        GtkWidget *details_grid = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(details_window), details_grid);

        GtkWidget *name_label = gtk_label_new("Nume:");
        widgets->name_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->name_entry), "Introduceți numele aici");
        gtk_grid_attach(GTK_GRID(details_grid), name_label, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), widgets->name_entry, 1, 0, 1, 1);

        GtkWidget *data_label = gtk_label_new("Data:");
        widgets->data_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->data_entry), "Introduceți data aici (DD-MM-YYYY)");
        gtk_entry_set_icon_from_icon_name(GTK_ENTRY(widgets->data_entry), GTK_ENTRY_ICON_SECONDARY, "x-office-calendar");
        g_signal_connect(widgets->data_entry, "icon-press", G_CALLBACK(on_data_entry_icon_press), widgets);
        gtk_grid_attach(GTK_GRID(details_grid), data_label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), widgets->data_entry, 1, 1, 1, 1);

        GtkWidget *description_label = gtk_label_new("Motivul consultatiei dvs.:");
        widgets->description_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->description_entry), "Introduceți motivul consultației aici");
        gtk_entry_set_max_length(GTK_ENTRY(widgets->description_entry), 50);
        gtk_widget_set_size_request(widgets->description_entry, 400, 200);
        gtk_grid_attach(GTK_GRID(details_grid), description_label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), widgets->description_entry, 1, 2, 1, 1);

        GtkWidget *save_button = gtk_button_new_with_label("Salvează");
        gtk_grid_attach(GTK_GRID(details_grid), save_button, 0, 3, 2, 1);

        widgets->display_label = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(details_grid), widgets->display_label, 0, 4, 2, 1);

        g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), widgets);

        gtk_widget_show_all(details_window);

        g_free(nume);
        g_free(specialitate);
        g_free(loc_de_munca);
        is_row_selected = TRUE;
    } else {
        is_row_selected = FALSE;
    }
}
void adauga_medic(AppWidgets *widgets, gchar *nume, gchar *specialitate, gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari) {
    MedicInfo *medic_info = g_slice_new(MedicInfo);
    medic_info->nume = g_strdup(nume);
    medic_info->specialitate = g_strdup(specialitate);
    medic_info->loc_de_munca = g_strdup(loc_de_munca);
    medic_info->lucreaza_cu_casa_de_asigurari = lucreaza_cu_casa_de_asigurari;

    GtkTreeIter iter;
    gtk_list_store_append(widgets->doctor_store, &iter);
    gtk_list_store_set(widgets->doctor_store, &iter, 0, medic_info->nume, 1, medic_info->specialitate, 2, medic_info->loc_de_munca, 3, medic_info->lucreaza_cu_casa_de_asigurari, -1);

    GtkTreeModel *model = GTK_TREE_MODEL(widgets->doctor_store);
    gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc)gtk_tree_model_row_changed, NULL);
}

void on_adauga_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(widgets->nume_entry));
    const gchar *specialitate = gtk_entry_get_text(GTK_ENTRY(widgets->specialitate_entry));
    const gchar *loc_de_munca = gtk_entry_get_text(GTK_ENTRY(widgets->loc_de_munca_entry));
    gboolean lucreaza_cu_casa_de_asigurari = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->lucreaza_cu_casa_de_asigurari_checkbox));

    adauga_medic(widgets, nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
    FILE *file = fopen("../medici.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s,%s,%s,%d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
        fclose(file);
    }
}

void on_delete_button_clicked(GtkWidget *widget, gpointer user_data) {
    g_print("Delete button clicked.\n");
    AppWidgets *widgets = (AppWidgets *)user_data;

    if(is_row_selected) {
        g_print("A row is selected.\n");
        gtk_list_store_remove(widgets->doctor_store, &selected_iter);

        FILE *file = fopen("../medici.txt", "w");
        g_print("File opened\n");
        if (file != NULL) {
            g_print("File opened successfully for writing.\n");
            GtkTreeIter iter;
            gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(widgets->doctor_store), &iter);
            while (valid) {
                gchar *nume;
                gchar *specialitate;
                gchar *loc_de_munca;
                gboolean lucreaza_cu_casa_de_asigurari;

                gtk_tree_model_get(GTK_TREE_MODEL(widgets->doctor_store), &iter, 0, &nume, 1, &specialitate, 2, &loc_de_munca, 3, &lucreaza_cu_casa_de_asigurari, -1);
                fprintf(file, "%s,%s,%s,%d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);

                g_free(nume);
                g_free(specialitate);
                g_free(loc_de_munca);

                valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(widgets->doctor_store), &iter);
            }
            fclose(file);
        } else {
            g_print("Failed to open file for writing.\n");
        }

        gtk_widget_queue_draw(widgets->doctor_view);
    } else {
        g_print("No row is selected.\n");
    }
}

void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    AppWidgets *widgets = g_slice_new(AppWidgets);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Aplicație de programare medicală");
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 600);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    widgets->doctor_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkWidget *doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), doctor_view, 0, 0, 3, 6);
    widgets->doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), widgets->doctor_view, 0, 0, 3, 6);

    if(GTK_IS_TREE_VIEW(widgets->doctor_view)) {
        g_print("Doctor view is a valid GtkTreeView.\n");
    } else {
        g_print("Doctor view is not a valid GtkTreeView.\n");
    }
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(doctor_view));
    g_signal_connect(selection, "changed", G_CALLBACK(on_selection_changed), widgets);
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Medic", renderer, "text", 0, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Specialitate", renderer, "text", 1, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Loc de Munca", renderer, "text", 2, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_view), column);

    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("Lucreaza cu Casa de Asigurari", renderer, "text", 3, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(doctor_view), column);

    FILE *file = fopen("../medici.txt", "r");

    fseek(file, 0, SEEK_SET);
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        gchar nume[256];
        gchar specialitate[256];
        gchar loc_de_munca[256];
        gboolean lucreaza_cu_casa_de_asigurari;

        if (sscanf(line, "%[^,],%[^,], %[^\n]", nume, specialitate, loc_de_munca) == 3) {
            loc_de_munca[strlen(loc_de_munca) - 2] = '\0';
            lucreaza_cu_casa_de_asigurari = line[strlen(line) - 2] - '0';

            adauga_medic(widgets, nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
        } else {
            g_print("Eroare la citirea datelor din fisier!\n");
        }
    }
    fclose(file);

    GtkWidget *delete_button = gtk_button_new_with_label("Șterge Medic");
    gtk_grid_attach(GTK_GRID(grid), delete_button, 3, 6, 2, 1);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_button_clicked), widgets);

    GtkWidget *nume_label = gtk_label_new("Nume:");
    gtk_grid_attach(GTK_GRID(grid), nume_label, 3, 1, 1, 1);
    widgets->nume_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), widgets->nume_entry, 4, 1, 1, 1);

    GtkWidget *specialitate_label = gtk_label_new("Specialitate:");
    gtk_grid_attach(GTK_GRID(grid), specialitate_label, 3, 2, 1, 1);
    widgets->specialitate_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), widgets->specialitate_entry, 4, 2, 1, 1);

    GtkWidget *loc_de_munca_label = gtk_label_new("Loc de Munca:");
    gtk_grid_attach(GTK_GRID(grid), loc_de_munca_label, 3, 3, 1, 1);
    widgets->loc_de_munca_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), widgets->loc_de_munca_entry, 4, 3, 1, 1);

    widgets->lucreaza_cu_casa_de_asigurari_checkbox = gtk_check_button_new_with_label("Lucrează cu Casa de Asigurari");
    gtk_grid_attach(GTK_GRID(grid), widgets->lucreaza_cu_casa_de_asigurari_checkbox, 4, 4, 1, 1);

    GtkWidget *adauga_button = gtk_button_new_with_label("Adaugă Medic");
    gtk_grid_attach(GTK_GRID(grid), adauga_button, 3, 5, 2, 1);
    g_signal_connect(adauga_button, "clicked", G_CALLBACK(on_adauga_button_clicked), widgets);

    gtk_widget_show_all(window);
}
