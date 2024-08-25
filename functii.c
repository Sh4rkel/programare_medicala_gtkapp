#include "functii.h"
#include "medic_info.h"
#include <glib-2.0/glib/gchecksum.h>

GtkTreeIter selected_iter;
gboolean is_row_selected = FALSE;

#include <string.h>

typedef struct {
    gchar *username;
    gchar *password;
} User;

GList *users = NULL;

void display_search_result(AppWidgets *widgets) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Search Result");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 350);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    GtkWidget *name_label = gtk_label_new("Nume:");
    widgets->name_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->name_entry), "Introduceți numele aici");
    gtk_grid_attach(GTK_GRID(grid), name_label, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->name_entry, 1, 0, 1, 1);

    GtkWidget *data_label = gtk_label_new("Data:");
    widgets->data_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->data_entry), "Introduceți data aici (DD-MM-YYYY)");
    gtk_grid_attach(GTK_GRID(grid), data_label, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->data_entry, 1, 1, 1, 1);

    GtkWidget *description_label = gtk_label_new("Motivul consultatiei dvs.:");
    widgets->description_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->description_entry), "Introduceți motivul consultației aici");
    gtk_entry_set_max_length(GTK_ENTRY(widgets->description_entry), 50);
    gtk_widget_set_size_request(widgets->description_entry, 400, 200);
    gtk_grid_attach(GTK_GRID(grid), description_label, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), widgets->description_entry, 1, 2, 1, 1);

    GtkWidget *save_button = gtk_button_new_with_label("Salvează");
    gtk_grid_attach(GTK_GRID(grid), save_button, 0, 3, 2, 1);

    widgets->display_label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets->display_label, 0, 4, 2, 1);

    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), widgets);

    gtk_widget_show_all(window);
}

void on_search_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *search_text = gtk_entry_get_text(GTK_ENTRY(widgets->search_entry));

    g_print("%s\n", search_text);

    GtkTreeIter iter;
    gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(widgets->doctor_store), &iter);

    while (valid) {
        gchar *doctor_name;
        gtk_tree_model_get(GTK_TREE_MODEL(widgets->doctor_store), &iter, 0, &doctor_name, -1);

        if (g_strcmp0(search_text, doctor_name) == 0) {
            const gchar *label_text = gtk_label_get_text(GTK_LABEL(widgets->search_result_label));
            g_print("Updated label text: %s\n", label_text);
            gtk_label_set_text(GTK_LABEL(widgets->search_result_label), label_text);


            display_search_result(widgets);

            break;
        }

        g_free(doctor_name);
        valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(widgets->doctor_store), &iter);
    }
}

gchar* generate_salt() {
    GRand* rand = g_rand_new();
    gchar* salt = g_strdup_printf("%08x%08x", g_rand_int(rand), g_rand_int(rand));
    g_rand_free(rand);
    return salt;
}

gchar* hash_password(const gchar *password, const gchar *salt) {
    GChecksum *checksum = g_checksum_new(G_CHECKSUM_SHA256);
    gchar *salted_password = g_strconcat(salt, password, NULL);
    g_checksum_update(checksum, (const guchar *)salted_password, -1);
    gchar *hashed_password = g_strdup(g_checksum_get_string(checksum));
    g_checksum_free(checksum);
    g_free(salted_password);
    return hashed_password;
}

gboolean is_valid_user(const gchar *username, const gchar *password) {
    FILE *file = fopen("../users.txt", "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file)) {
            gchar stored_username[256];
            gchar stored_password[256];
            gchar stored_salt[256];

            if (sscanf(line, "%[^,],%[^,],%[^\n]", stored_username, stored_password, stored_salt) == 3) {
                gchar *hashed_password = hash_password(password, stored_salt);

                if (g_strcmp0(username, stored_username) == 0 && g_strcmp0(hashed_password, stored_password) == 0) {
                    g_free(hashed_password);
                    fclose(file);
                    return TRUE;
                }

                g_free(hashed_password);
            } 
        }
        fclose(file);
    } else {
        g_print("Eroare la deschiderea fisierului pentru citire.\n");
    }

    return FALSE;
}

void create_main_window(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    GtkWidget *window;
    GtkWidget *grid;

    widgets->main_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->main_window), "Aplicație de programare medicală");
    gtk_window_set_default_size(GTK_WINDOW(widgets->main_window), 700, 600);
    gtk_window_set_icon_from_file(GTK_WINDOW(widgets->main_window), "C:\\books\\C programming\\untitled\\icon_med.ico", NULL);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(widgets->main_window), grid);

    widgets->doctor_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);
    widgets->search_entry = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(grid), widgets->search_entry, 0, 7, 2, 1);

    widgets->search_result_label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets->search_result_label, 0, 8, 2, 1); // Place the label under the search bar

    const gchar *label_text = gtk_label_get_text(GTK_LABEL(widgets->search_result_label));
    g_print("Label text: %s\n", label_text);
    GtkWidget *search_button = gtk_button_new_with_label("Search");
    gtk_grid_attach(GTK_GRID(grid), search_button, 2, 7, 1, 1);
    g_signal_connect(search_button, "clicked", G_CALLBACK(on_search_button_clicked), widgets);
    GtkWidget *doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), doctor_view, 0, 0, 3, 6);
    widgets->doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), widgets->doctor_view, 0, 0, 3, 6);

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

    GtkWidget *edit_button = gtk_button_new_with_label("Edit Consult");
    gtk_grid_attach(GTK_GRID(grid), edit_button, 3, 7, 2, 1);
    g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_button_clicked), widgets);

    GtkWidget *display_button = gtk_button_new_with_label("Display Consults");
    gtk_grid_attach(GTK_GRID(grid), display_button, 3, 8, 2, 1);
    g_signal_connect(display_button, "clicked", G_CALLBACK(display_consults_in_new_window), widgets);

    gtk_widget_show_all(widgets->main_window);
}

void add_user(const gchar *username, const gchar *password, const gchar *salt) {
    FILE *file = fopen("../users.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s,%s,%s\n", username, password, salt);
        fclose(file);
    } else {
        g_print("Eroare la deschiderea fisierului pentru citire.\n");
    }
}

void on_login_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));

    if (is_valid_user(username, password)) {
        gtk_widget_hide(widgets->login_window);
        create_main_window(widgets->app, user_data);
    } else {
        gtk_label_set_text(GTK_LABEL(widgets->login_message_label), "Invalid username or password.");
    }
}

void on_register_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *username = gtk_entry_get_text(GTK_ENTRY(widgets->username_entry));
    const gchar *password = gtk_entry_get_text(GTK_ENTRY(widgets->password_entry));

    gchar *salt = generate_salt();
    gchar *hashed_password = hash_password(password, salt);
    add_user(username, hashed_password, salt);
    g_free(hashed_password);

    gtk_label_set_text(GTK_LABEL(widgets->login_message_label), "Registration successful. You can now log in.");

    FILE *file = fopen("../users.txt", "a");
    if (file != NULL) {
        fprintf(file, "%s,%s,%s\n", username, hashed_password, salt);
        fclose(file);
    } else {
        g_print("Eroare la deschiderea fisierului pentru citire.\n");
    }
    g_free(salt);
}

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

        gtk_widget_set_size_request(widgets->search_result_label, 200, 50); // Set width and height
        gtk_widget_show(widgets->search_result_label);

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
        printf("No row selected.\n");
        is_row_selected = FALSE;
    }
}

void adauga_medic(AppWidgets *widgets, const gchar *nume, const gchar *specialitate, const gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari) {
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

    if (g_strcmp0(nume, "") != 0 && g_strcmp0(specialitate, "") != 0 && g_strcmp0(loc_de_munca, "") != 0) {
        adauga_medic(widgets, nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
        FILE *file = fopen("../medici.txt", "a");
        if (file != NULL) {
            fprintf(file, "%s,%s,%s,%d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
            fclose(file);
        }
    }
}

void on_delete_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    if(is_row_selected) {
        GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widgets->doctor_view));
        g_signal_connect(selection, "changed", G_CALLBACK(on_selection_changed), widgets);

        gulong handler_id = g_signal_handler_find(selection, G_SIGNAL_MATCH_FUNC, 0, 0, NULL, G_CALLBACK(on_selection_changed), NULL);

        if (handler_id > 0) {
            g_signal_handler_block(selection, handler_id);
            gtk_list_store_remove(widgets->doctor_store, &selected_iter);
            g_signal_handler_unblock(selection, handler_id);
        } else {
            g_print("Eraore la gasirea ID-ului.\n");
        }

        FILE *file = fopen("../medici.txt", "w");
        if (file != NULL) {
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
            g_print("Eroare la deschiderea fisierului pentru citire.\n");
        }

        gtk_widget_queue_draw(widgets->doctor_view);
    } else {
        g_print("No row is selected.\n");
    }
}

void display_medical_consults() {
    FILE *file = fopen("../programari.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului  programari.txt\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }

    fclose(file);
}

void edit_medical_consult(AppWidgets *widgets, const gchar *old_nume, const gchar *new_nume, const gchar *new_data, const gchar *new_description) {
    FILE *file = fopen("../programari.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului  programari.txt\n");
        return;
    }

    FILE *temp_file = fopen("../temp.txt", "w");
    if (temp_file == NULL) {
        printf("Eroare la deschiderea fisierului  temp.txt\n");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        gchar stored_nume[256];
        gchar stored_data[256];
        gchar stored_description[256];

        if (sscanf(line, "Doctor: %[^,]\nNume: %[^,]\nData: %[^,]\nDescriere: %[^\n]\n\n", stored_nume, stored_data, stored_description) == 4) {
            if (g_strcmp0(old_nume, stored_nume) == 0) {
                fprintf(temp_file, "Doctor: %s\nNume: %s\nData: %s\nDescriere: %s\n\n", widgets->selected_doctor, new_nume, new_data, new_description);
            } else {
                fprintf(temp_file, "%s", line);
            }
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("../programari.txt");
    rename("../temp.txt", "../programari.txt");
}

void on_delete_consult_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *frame = gtk_widget_get_parent(widget);
    GtkWidget *box = gtk_widget_get_parent(frame);
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
    GtkWidget *inner_box = GTK_WIDGET(children->data);
    g_list_free(children);

    children = gtk_container_get_children(GTK_CONTAINER(inner_box));
    GtkWidget *label = GTK_WIDGET(children->data);
    const gchar *consult_text = gtk_label_get_text(GTK_LABEL(label));
    g_list_free(children);

    FILE *file = fopen("../programari.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului programari.txt\n");
        return;
    }

    FILE *temp_file = fopen("../temp.txt", "w");
    if (temp_file == NULL) {
        printf("Eroare la deschiderea fisierului  temp.txt\n");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, consult_text) == 0) {
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            fgets(line, sizeof(line), file);
            continue;
        }

        fputs(line, temp_file);
        fputs("\n", temp_file);
    }

    fclose(file);
    fclose(temp_file);

    remove("../programari.txt");
    rename("../temp.txt", "../programari.txt");

    gtk_widget_destroy(frame);
}

void on_edit_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *frame = gtk_widget_get_parent(widget);
    GtkWidget *box = gtk_widget_get_parent(frame);
    GList *children = gtk_container_get_children(GTK_CONTAINER(box));
    GtkWidget *inner_box = GTK_WIDGET(children->data);
    g_list_free(children);

    children = gtk_container_get_children(GTK_CONTAINER(inner_box));
    GtkWidget *label = GTK_WIDGET(children->data);
    const gchar *consult_text = gtk_label_get_text(GTK_LABEL(label));
    g_list_free(children);

    widgets->name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(widgets->name_entry), consult_text);

    widgets->data_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(widgets->data_entry), "Enter new date here");

    widgets->description_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(widgets->description_entry), "Enter new description here");

    GtkWidget *edit_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(edit_window), "Edit Consult");
    gtk_container_add(GTK_CONTAINER(edit_window), widgets->name_entry);
    gtk_container_add(GTK_CONTAINER(edit_window), widgets->data_entry);
    gtk_container_add(GTK_CONTAINER(edit_window), widgets->description_entry);
    gtk_widget_show_all(edit_window);
}

void on_save_edit_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *vbox = gtk_widget_get_parent(widget);
    GList *children = gtk_container_get_children(GTK_CONTAINER(vbox));

    GtkWidget *doctor_entry = GTK_WIDGET(g_list_nth_data(children, 3));
    GtkWidget *name_entry = GTK_WIDGET(g_list_nth_data(children, 2));
    GtkWidget *date_entry = GTK_WIDGET(g_list_nth_data(children, 1));
    GtkWidget *description_entry = GTK_WIDGET(g_list_nth_data(children, 0));
    const gchar *doctor = gtk_entry_get_text(GTK_ENTRY(doctor_entry));
    const gchar *name = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const gchar *date = gtk_entry_get_text(GTK_ENTRY(date_entry));
    const gchar *description = gtk_entry_get_text(GTK_ENTRY(description_entry));

    g_list_free(children);

    FILE *file = fopen("../programari.txt", "r");
    FILE *temp_file = fopen("../temp.txt", "w");
    if (file == NULL || temp_file == NULL) {
        printf("Eroare la deschiderea fisierului.\n");
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        gchar stored_doctor[256];
        gchar stored_name[256];
        gchar stored_date[256];
        gchar stored_description[256];

        if (sscanf(line, "Doctor: %[^,]\nNume: %[^,]\nData: %[^,]\nDescriere: %[^\n]\n\n", stored_doctor, stored_name, stored_date, stored_description) == 4) {
            if (strcmp(stored_doctor, doctor) == 0 && strcmp(stored_name, name) == 0 && strcmp(stored_date, date) == 0 && strcmp(stored_description, description) == 0) {
                fprintf(temp_file, "Doctor: %s\nNume: %s\nData: %s\nDescriere: %s\n\n", doctor, name, date, description);
            } else {
                fprintf(temp_file, "%s", line);
            }
        } else {
            fprintf(temp_file, "%s", line);
        }
    }

    fclose(file);
    fclose(temp_file);

    remove("../programari.txt");
    rename("../temp.txt", "../programari.txt");
}

void on_edit_consult_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *hbox = gtk_widget_get_parent(widget);
    GList *children = gtk_container_get_children(GTK_CONTAINER(hbox));
    GtkWidget *label = GTK_WIDGET(children->data);
    const gchar *consult_text = gtk_label_get_text(GTK_LABEL(label));
    g_list_free(children);

    gchar **consult_details = g_strsplit(consult_text, "\n", -1);
    const gchar *doctor = consult_details[0];
    const gchar *name = consult_details[1];
    const gchar *date = consult_details[2];
    const gchar *description = consult_details[3];

    GtkWidget *edit_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(edit_window), "Edit Consultation");
    gtk_window_set_default_size(GTK_WINDOW(edit_window), 300, 200);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(edit_window), vbox);

    GtkWidget *doctor_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(doctor_entry), doctor);
    gtk_box_pack_start(GTK_BOX(vbox), doctor_entry, FALSE, FALSE, 0);

    GtkWidget *name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(name_entry), name);
    gtk_box_pack_start(GTK_BOX(vbox), name_entry, FALSE, FALSE, 0);

    GtkWidget *date_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(date_entry), date);
    gtk_box_pack_start(GTK_BOX(vbox), date_entry, FALSE, FALSE, 0);

    GtkWidget *description_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(description_entry), description);
    gtk_box_pack_start(GTK_BOX(vbox), description_entry, FALSE, FALSE, 0);

    // Add a "Save" button to the new window
    GtkWidget *save_button = gtk_button_new_with_label("Save");
    gtk_box_pack_start(GTK_BOX(vbox), save_button, FALSE, FALSE, 0);

    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_edit_button_clicked), widgets);

    gtk_widget_show_all(edit_window);

    g_strfreev(consult_details);
}

void on_consult_button_clicked(GtkWidget *widget, gpointer user_data) {
    const gchar *consult_text = gtk_button_get_label(GTK_BUTTON(widget));
    GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "You clicked on a consultation: %s", consult_text);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void update_consultation(const gchar *updated_consult_text) {
    FILE *file = fopen("../programari.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului  programari.txt\n");
        return;
    }

    FILE *temp_file = fopen("../temp.txt", "w");
    if (temp_file == NULL) {
        printf("Eroare la deschiderea fisierului  temp.txt\n");
        fclose(file);
        return;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, updated_consult_text) == 0) {
            continue;
        }

        fputs(line, temp_file);
    }

    fclose(file);
    fclose(temp_file);

    remove("../programari.txt");
    rename("../temp.txt", "../programari.txt");
}

void display_consults_in_new_window(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Medical Consults");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 400);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    FILE *file = fopen("../programari.txt", "r");
    if (file == NULL) {
        printf("Eroare la deschiderea fisierului programari.txt\n");
        return;
    }

    char line[256];
    GString *consult_text = g_string_new(NULL);
    while (fgets(line, sizeof(line), file)) {
        if (strcmp(line, "\n") != 0) {
            g_string_append(consult_text, line);
            if (strstr(line, "Descriere:") != NULL) {
                GtkWidget *frame = gtk_frame_new(NULL);
                gtk_box_pack_start(GTK_BOX(vbox), frame, FALSE, FALSE, 0);

                GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
                gtk_container_add(GTK_CONTAINER(frame), box);

                GtkWidget *label = gtk_label_new(consult_text->str);
                gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

                GtkWidget *update_button = gtk_button_new_with_label("Update");
                g_signal_connect(update_button, "clicked", G_CALLBACK(on_update_button_clicked), widgets);
                gtk_container_add(GTK_CONTAINER(frame), update_button);

                GtkWidget *edit_button = gtk_button_new_with_label("Edit");
                g_signal_connect(edit_button, "clicked", G_CALLBACK(on_edit_button_clicked), widgets);
                gtk_box_pack_start(GTK_BOX(box), edit_button, TRUE, TRUE, 0);

                GtkWidget *delete_button = gtk_button_new_with_label("Delete");
                g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_consult_button_clicked), widgets);
                gtk_box_pack_start(GTK_BOX(box), delete_button, TRUE, TRUE, 0);

                g_string_assign(consult_text, "");
            }
        }
    }
    if (consult_text->len > 0) {
        GtkWidget *frame = gtk_frame_new(NULL);
        gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

        GtkWidget *label = gtk_label_new(consult_text->str);
        gtk_container_add(GTK_CONTAINER(frame), label);

        GtkWidget *update_button = gtk_button_new_with_label("Update");
        g_signal_connect(update_button, "clicked", G_CALLBACK(on_update_button_clicked), widgets);
        gtk_container_add(GTK_CONTAINER(frame), update_button);
    }

    g_string_free(consult_text, TRUE);
    fclose(file);

    gtk_widget_show_all(window);
}

void on_update_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *frame = gtk_widget_get_parent(widget);
    GtkWidget *label = gtk_bin_get_child(GTK_BIN(frame));
    const gchar *consult_text = gtk_label_get_text(GTK_LABEL(label));

    GtkWidget *update_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(update_window), "Update Consultation");
    gtk_window_set_default_size(GTK_WINDOW(update_window), 300, 200);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(update_window), vbox);

    GtkWidget *entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry), consult_text);
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    GtkWidget *save_button = gtk_button_new_with_label("Save");
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_update_button_clicked), widgets);
    gtk_box_pack_start(GTK_BOX(vbox), save_button, FALSE, FALSE, 0);

    gtk_widget_show_all(update_window);
}

void on_save_update_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;

    GtkWidget *entry = gtk_widget_get_parent(widget);
    const gchar *updated_consult_text = gtk_entry_get_text(GTK_ENTRY(entry));

    update_consultation(updated_consult_text);
}

void activate(GtkApplication *app, gpointer user_data) {
    AppWidgets *widgets = g_slice_new(AppWidgets);
    widgets->app = app;
    GtkWidget *window;



    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;

    gtk_css_provider_load_from_path(provider, "C:\\books\\C programming\\untitled\\style.css", &error);

    if (error != NULL) {
        g_printerr("Eroare la incarcarea fisierului CSS %s\n", error->message);
        g_clear_error(&error);
        g_object_unref(provider);
        return;
    }

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    g_object_unref(provider);
    widgets->login_window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(widgets->login_window), "Login/Register");
    gtk_window_set_default_size(GTK_WINDOW(widgets->login_window), 200, 200);
    gtk_window_set_icon_from_file(GTK_WINDOW(widgets->login_window), "C:\\books\\C programming\\untitled\\icon_med.ico", NULL);

    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(widgets->login_window), grid);

    widgets->username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->username_entry), "Username");
    gtk_grid_attach(GTK_GRID(grid), widgets->username_entry, 0, 0, 1, 1);

    widgets->password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(widgets->password_entry), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(widgets->password_entry), FALSE); // Hide the password text
    gtk_grid_attach(GTK_GRID(grid), widgets->password_entry, 0, 1, 1, 1);



    GtkWidget *login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), login_button, 0, 2, 1, 1);

    GtkWidget *register_button = gtk_button_new_with_label("Register");
    g_signal_connect(register_button, "clicked", G_CALLBACK(on_register_button_clicked), widgets);
    gtk_grid_attach(GTK_GRID(grid), register_button, 0, 3, 1, 1);

    widgets->login_message_label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(grid), widgets->login_message_label, 0, 4, 1, 1);

    gtk_widget_show_all(widgets->login_window);
}
