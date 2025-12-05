#include <gtk/gtk.h>
#include <gst/gst.h>

typedef struct {
    GstElement *playbin;
} VideoData;

// Callback para abrir archivo de vídeo
void on_open_file(GtkWidget *widget, gpointer data) {
    VideoData *vd = (VideoData *)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("Abrir archivo",
                                                    NULL,
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_Cancelar", GTK_RESPONSE_CANCEL,
                                                    "_Abrir", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        g_print("Archivo seleccionado: %s\n", filename);

        // Detener reproducción si estaba corriendo
        gst_element_set_state(vd->playbin, GST_STATE_NULL);

        // Configurar el archivo de vídeo
        g_object_set(vd->playbin, "uri", g_strdup_printf("file://%s", filename), NULL);

        // Iniciar reproducción
        gst_element_set_state(vd->playbin, GST_STATE_PLAYING);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    // Crear ventana principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Video Player (gtksink)");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Contenedor vertical
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Barra de menú
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Frame para vídeo
    GtkWidget *frame = gtk_frame_new("Video");
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    // --- GStreamer playbin ---
    VideoData vd;
    vd.playbin = gst_element_factory_make("playbin", "playbin");

    // Crear sink gtksink
    GstElement *sink = gst_element_factory_make("gtksink", "videosink");
    g_object_set(vd.playbin, "video-sink", sink, NULL);

    // Obtener widget GTK que maneja gtksink
    GtkWidget *video_widget = NULL;
    g_object_get(sink, "widget", &video_widget, NULL);

    // Añadir widget al frame
    gtk_container_add(GTK_CONTAINER(frame), video_widget);

    // Menú Archivo -> Abrir
    GtkWidget *menu_file = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("Archivo");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), menu_file);

    GtkWidget *open_item = gtk_menu_item_new_with_label("Abrir…");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), open_item);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), &vd);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // Mostrar todo
    gtk_widget_show_all(window);

    gtk_main();

    // Limpiar GStreamer
    gst_element_set_state(vd.playbin, GST_STATE_NULL);
    gst_object_unref(vd.playbin);

    return 0;
}
