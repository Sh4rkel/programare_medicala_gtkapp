#include <gtk/gtk.h>
#include <stdio.h>
#include <unistd.h>

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
    GtkWidget *name_entry; // Adaugăm intrările pentru datele medicului aici
    GtkWidget *data_entry;
    GtkWidget *description_entry;
} AppWidgets;

static void on_save_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(widgets->nume_entry));
    const gchar *data = gtk_entry_get_text(GTK_ENTRY(widgets->data_entry));
    const gchar *description = gtk_entry_get_text(GTK_ENTRY(widgets->description_entry));

    gchar *display_text = g_strdup_printf("Nume: %s\nData: %s\nDescriere: %s\n", nume, data, description);
    gtk_label_set_text(GTK_LABEL(widgets->display_label), display_text);
    g_free(display_text);
}

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

        GtkWidget *details_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(details_window), "Programeaza-te la medic");
        gtk_window_set_default_size(GTK_WINDOW(details_window), 500, 350);

        GtkWidget *details_grid = gtk_grid_new();
        gtk_container_add(GTK_CONTAINER(details_window), details_grid);

        GtkWidget *name_label = gtk_label_new("Nume:");
        widgets->name_entry = gtk_entry_new();
        gtk_entry_set_text(GTK_ENTRY(widgets->name_entry), nume); // Setăm valoarea din listă
        gtk_grid_attach(GTK_GRID(details_grid), name_label, 0, 0, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), widgets->name_entry, 1, 0, 1, 1);

        GtkWidget *data_label = gtk_label_new("Data:");
        widgets->data_entry = gtk_entry_new();
        gtk_grid_attach(GTK_GRID(details_grid), data_label, 0, 1, 1, 1);
        gtk_grid_attach(GTK_GRID(details_grid), widgets->data_entry, 1, 1, 1, 1);

        GtkWidget *description_label = gtk_label_new("Motivul consultatiei dvs.:");
        widgets->description_entry = gtk_entry_new();
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
    }
}
static void adauga_medic(AppWidgets *widgets, gchar *nume, gchar *specialitate, gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari);
static void on_adauga_button_clicked(GtkWidget *widget, gpointer user_data) {
    AppWidgets *widgets = (AppWidgets *)user_data;
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(widgets->nume_entry));
    const gchar *specialitate = gtk_entry_get_text(GTK_ENTRY(widgets->specialitate_entry));
    const gchar *loc_de_munca = gtk_entry_get_text(GTK_ENTRY(widgets->loc_de_munca_entry));
    gboolean lucreaza_cu_casa_de_asigurari = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->lucreaza_cu_casa_de_asigurari_checkbox));

    adauga_medic(widgets, nume, specialitate, loc_de_munca, lucreaza_cu_casa_de_asigurari);
}

static void adauga_medic(AppWidgets *widgets, gchar *nume, gchar *specialitate, gchar *loc_de_munca, gboolean lucreaza_cu_casa_de_asigurari) {
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

static void activate(GtkApplication *app, gpointer user_data) {
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

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.example.app", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
