#include <gtk/gtk.h>

void on_open_file(GtkWidget *widget, gpointer data) {
    GtkWidget *dialog;
    GtkWindow *parent = GTK_WINDOW(data);

    dialog = gtk_file_chooser_dialog_new("Abrir archivo",
                                         parent,
                                         GTK_FILE_CHOOSER_ACTION_OPEN,
                                         "_Cancelar", GTK_RESPONSE_CANCEL,
                                         "_Abrir", GTK_RESPONSE_ACCEPT,
                                         NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename;
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        filename = gtk_file_chooser_get_filename(chooser);

        g_print("Archivo seleccionado: %s\n", filename);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title(GTK_WINDOW(window), "GTK Menu Hello");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // --- Barra vertical y menú ---
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Menú superior
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    GtkWidget *frame = gtk_frame_new("Video");
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    GtkWidget *video_area = gtk_drawing_area_new();
    gtk_container_add(GTK_CONTAINER(frame), video_area);



    // Menú Archivo
    GtkWidget *menu_file = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("Archivo");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), menu_file);

    // Opción "Abrir…"
    GtkWidget *open_item = gtk_menu_item_new_with_label("Abrir…");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), open_item);

    // Callback para abrir archivo
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), window);

    // Añadir menú a la barra
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // Añadir barra de menú al layout
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
    gtk_main();
    return 0;
}
