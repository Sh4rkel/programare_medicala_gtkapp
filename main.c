#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>
// TODO: ACESTE DATE TREBUIE MUTATE INTR-UN FISIER SEPARAT SI ENCAPSULATE!!!!!!!1!
GtkWidget *name_entry;
GtkWidget *data_entry;
GtkWidget *description_entry;

// Structura pentru stocarea datelor despre medici
typedef struct {
    gchar *nume;
    gchar *specialitate;
    gchar *loc_de_munca;
    gboolean lucreaza_cu_casa_de_asigurari;
} MedicInfo;

typedef struct {
    GtkWidget *nume_entry;
    GtkWidget *specialitate_entry;
    GtkWidget *loc_de_munca_entry;
    GtkWidget *lucreaza_cu_casa_de_asigurari_checkbox;
    GtkListStore *doctor_store;
    GtkWidget *display_label;
} AppWidgets;


static void on_save_button_clicked(GtkWidget *widget, gpointer user_data) {
    // Colectam datele introduse de utilizator
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(name_entry));
    const gchar *data = gtk_entry_get_text(GTK_ENTRY(data_entry));
    const gchar *description = gtk_entry_get_text(GTK_ENTRY(description_entry));

    gchar *display_text = g_strdup_printf("Nume: %s\nData: %s\nDescriere: %s\n", nume, data, description);
    gtk_label_set_text(GTK_LABEL(widgets->display_label), display_text);
    g_free(display_text);

    // TODO: Salvarea datelor intr-o baza de date pentru a putea fi accesate ulterior
}

// Functie apelata atunci cand se schimba selectia in lista de medici
static void on_selection_changed(GtkTreeSelection *selection, gpointer user_data) {
    GtkTreeModel *model;
    GtkTreeIter iter;
    AppWidgets *widgets = (AppWidgets *)user_data;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *nume;
        gchar *specialitate;
        gchar *loc_de_munca;
        gboolean lucreaza_cu_casa_de_asigurari;

        gtk_tree_model_get(model, &iter, 0, &nume, 1, &specialitate, 2, &loc_de_munca, 3, &lucreaza_cu_casa_de_asigurari, -1);

        // Creaza o fereastra noua pentru a introduce datele suplimentare
        GtkWidget *details_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(details_window), "Programeaza-te la medic");
        gtk_window_set_default_size(GTK_WINDOW(details_window), 500, 350);
        gtk_window_set_icon_from_file(GTK_WINDOW(details_window), "C:\\books\\C programming\\untitled\\icon_med.ico", NULL);

        GtkWidget *details_grid = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(details_window), details_grid);

        // Creaza label-uri si entry-uri pentru introducerea datelor suplimentare
        GtkWidget *name_label = gtk_label_new("Nume:");
        name_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(name_entry), "ex: Ion Popescu");

        GtkWidget *data_label = gtk_label_new("Data:");
        data_entry = gtk_entry_new();
        gtk_entry_set_placeholder_text(GTK_ENTRY(data_entry), "zi/luna/an, ex: 01/01/2021");

        GtkWidget *description_label = gtk_label_new("Motivul consultatiei dvs.:");
        description_entry = gtk_entry_new();
        gtk_entry_set_max_length(GTK_ENTRY(description_entry), 500);
        gtk_entry_set_placeholder_text(GTK_ENTRY(description_entry), "Maxim 500 de caractere");
        // Adauga label-urile si entry-urile in grid
        gtk_grid_attach(GTK_GRID(details_grid), name_label, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), name_entry, 1, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), data_label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), data_entry, 1, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), description_label, 0, 2, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), description_entry, 1, 2, 1, 1);

        // Creaza butonul de salvare
        GtkWidget *save_button = gtk_button_new_with_label("Salvează");
        gtk_grid_attach(GTK_GRID(details_grid), save_button, 0, 3, 2, 1);

        widgets->display_label = gtk_label_new("");
        gtk_grid_attach(GTK_GRID(details_grid), widgets->display_label, 0, 4, 2, 1);

        // Conecteaza semnalul de click la functia de salvare
        g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_button_clicked), widgets);

        // Arata toate elementele din fereastra
        gtk_widget_show_all(details_window);

        // Elibereaza memoria
        g_free(nume);
        g_free(specialitate);
        g_free(loc_de_munca);
    }
}

static void adauga_medic(AppWidgets *widgets, gchar *nume, gchar *specialitate, gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari) {
    // Alocare memorie
    MedicInfo *medic_info = g_slice_new(MedicInfo);
    medic_info->nume = g_strdup(nume);
    medic_info->specialitate = g_strdup(specialitate);
    medic_info->loc_de_munca = g_strdup(loc_de_munca);
    medic_info->lucreaza_cu_casa_de_asigurari = lucreaza_cu_casa_de_asigurari;

    // Introdu un nou medic în catalog
    GtkTreeIter iter;
    gtk_list_store_append(widgets->doctor_store, &iter);
    gtk_list_store_set(widgets->doctor_store, &iter, 0, medic_info->nume, 1, medic_info->specialitate, 2, medic_info->loc_de_munca, 3, medic_info->lucreaza_cu_casa_de_asigurari, -1);

    // Emiterea unui semnal pentru a actualiza afișajul
    GtkTreeModel *model = GTK_TREE_MODEL(widgets->doctor_store);
    gtk_tree_model_foreach(model, (GtkTreeModelForeachFunc)gtk_tree_model_row_changed, NULL);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    AppWidgets *widgets = g_slice_new(AppWidgets);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Aplicație de programare medicală");
    gtk_window_set_default_size(GTK_WINDOW(window), 700, 600);
    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Lista pentru stocarea datelor despre medici
    widgets->doctor_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkWidget *doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), doctor_view, 0, 0, 3, 6);

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

    // Open the file for reading
    FILE *file = fopen("../medici.txt", "r");
    if (file == NULL) {
        g_print("Eroare la deschiderea fisierului!\n");
        return;
    }
    g_print("File opened successfully.\n");
    char first_char = fgetc(file);
    g_print("First character of the file: %c\n", first_char);
    fseek(file, 0, SEEK_SET);  // Reset the file pointer to the beginning of the file
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        g_print("Line read from file: %s\n", line);
        gchar *nume = g_malloc(256);
        gchar *specialitate = g_malloc(256);
        gchar *loc_de_munca = g_malloc(256);
        gboolean lucreaza_cu_casa_de_asigurari;
        if (sscanf(line, "%[^,],%[^,], %[^\n]", nume, specialitate, loc_de_munca) == 3) {

            loc_de_munca[strlen(loc_de_munca) - 2] = '\0';

            lucreaza_cu_casa_de_asigurari = line[strlen(line) - 2] - '0';

            g_print("Data read from file: %s, %s, %s, %d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);

            adauga_medic(widgets, nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);

            g_print("Data added to doctor_store: %s, %s, %s, %d\n", nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
        } else {
            g_print("Eroare la citirea datelor din fisier!\n");
        }
        g_free(nume);
        g_free(specialitate);
        g_free(loc_de_munca);
    }
    fclose(file);

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

    gtk_widget_show_all(window);

    g_signal_connect(adauga_button, "clicked", G_CALLBACK(adauga_medic), widgets);
}


int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}