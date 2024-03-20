#include <gtk/gtk.h>

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
} AppWidgets;

static void adauga_medic(GtkWidget *widget, AppWidgets *widgets) {
    const gchar *nume = gtk_entry_get_text(GTK_ENTRY(widgets->nume_entry));
    const gchar *specialitate = gtk_entry_get_text(GTK_ENTRY(widgets->specialitate_entry));
    const gchar *loc_de_munca = gtk_entry_get_text(GTK_ENTRY(widgets->loc_de_munca_entry));
    gboolean lucreaza_cu_casa_de_asigurari = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(widgets->lucreaza_cu_casa_de_asigurari_checkbox));

    // Alocare memorie
    MedicInfo *medic_info = g_slice_new(MedicInfo);
    medic_info->nume = g_strdup(nume);
    medic_info->specialitate = g_strdup(specialitate);
    medic_info->loc_de_munca = g_strdup(loc_de_munca);
    medic_info->lucreaza_cu_casa_de_asigurari = lucreaza_cu_casa_de_asigurari;

    // Introdu un nou medic in catalog
    GtkTreeIter iter;
    gtk_list_store_append(widgets->doctor_store, &iter);
    gtk_list_store_set(widgets->doctor_store, &iter, 0, medic_info->nume, 1, medic_info->specialitate, 2, medic_info->loc_de_munca, 3, medic_info->lucreaza_cu_casa_de_asigurari, -1);
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *grid;
    AppWidgets *widgets = g_slice_new(AppWidgets);

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Aplicație de programare medicală");
    gtk_window_set_default_size(GTK_WINDOW(window), 600, 400);

    grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Lista pentru stocarea datelor despre medici
    widgets->doctor_store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_BOOLEAN);

    GtkWidget *doctor_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(widgets->doctor_store));
    gtk_grid_attach(GTK_GRID(grid), doctor_view, 0, 0, 3, 6);

    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;

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

    // Entry fields and labels for adding a doctor manually
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