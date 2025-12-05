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

        gst_element_set_state(vd->playbin, GST_STATE_NULL);
        g_object_set(vd->playbin, "uri", g_strdup_printf("file://%s", filename), NULL);
        gst_element_set_state(vd->playbin, GST_STATE_PLAYING);

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Callbacks de control
void on_play(GtkWidget *widget, gpointer data) {
    gst_element_set_state(((VideoData *)data)->playbin, GST_STATE_PLAYING);
}

void on_pause(GtkWidget *widget, gpointer data) {
    gst_element_set_state(((VideoData *)data)->playbin, GST_STATE_PAUSED);
}

void on_stop(GtkWidget *widget, gpointer data) {
    gst_element_set_state(((VideoData *)data)->playbin, GST_STATE_NULL);
}

void on_restart(GtkWidget *widget, gpointer data) {
    VideoData *vd = (VideoData *)data;
    // Seek al inicio
    gst_element_seek_simple(vd->playbin,
                            GST_FORMAT_TIME,
                            (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT),
                            0);
    gst_element_set_state(vd->playbin, GST_STATE_PLAYING); // opcional: reproducir tras reiniciar
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);

    // Ventana principal
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "GTK Video Player (gtksink)");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Barra de menú
    GtkWidget *menu_bar = gtk_menu_bar_new();
    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);

    // Frame para vídeo
    GtkWidget *frame = gtk_frame_new("Video");
    gtk_box_pack_start(GTK_BOX(vbox), frame, TRUE, TRUE, 0);

    // Contenedor de botones
    GtkWidget *hbox_buttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_buttons, FALSE, FALSE, 5);

    // --- GStreamer playbin ---
    VideoData vd;
    vd.playbin = gst_element_factory_make("playbin", "playbin");

    GstElement *sink = gst_element_factory_make("gtksink", "videosink");
    if (!sink) {
        g_error("No se pudo crear gtksink. ¿Está instalado?");
    }
    g_object_set(vd.playbin, "video-sink", sink, NULL);

    GtkWidget *video_widget = nullptr;
    g_object_get(sink, "widget", &video_widget, nullptr);
    gtk_container_add(GTK_CONTAINER(frame), video_widget);

    // --- Botones ---
    GtkWidget *btn_play = gtk_button_new_with_label("Reproducir");
    GtkWidget *btn_pause = gtk_button_new_with_label("Pausar");
    GtkWidget *btn_stop = gtk_button_new_with_label("Detener");
    GtkWidget *btn_restart = gtk_button_new_with_label("Reiniciar");

    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_play, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_pause, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_stop, FALSE, FALSE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_buttons), btn_restart, FALSE, FALSE, 5);

    g_signal_connect(btn_play, "clicked", G_CALLBACK(on_play), &vd);
    g_signal_connect(btn_pause, "clicked", G_CALLBACK(on_pause), &vd);
    g_signal_connect(btn_stop, "clicked", G_CALLBACK(on_stop), &vd);
    g_signal_connect(btn_restart, "clicked", G_CALLBACK(on_restart), &vd);

    // --- Menú Archivo → Abrir ---
    GtkWidget *menu_file = gtk_menu_new();
    GtkWidget *file_item = gtk_menu_item_new_with_label("Archivo");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_item), menu_file);

    GtkWidget *open_item = gtk_menu_item_new_with_label("Abrir…");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_file), open_item);
    g_signal_connect(open_item, "activate", G_CALLBACK(on_open_file), &vd);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_item);

    // --- AccelGroup para shortcuts ---
    GtkAccelGroup *accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    gtk_widget_add_accelerator(open_item, "activate", accel_group,
                               GDK_KEY_o, (GdkModifierType)GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(btn_play, "clicked", accel_group,
                               GDK_KEY_p, (GdkModifierType)GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(btn_pause, "clicked", accel_group,
                               GDK_KEY_p, (GdkModifierType)(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(btn_stop, "clicked", accel_group,
                               GDK_KEY_s, (GdkModifierType)GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
    gtk_widget_add_accelerator(btn_restart, "clicked", accel_group,
                               GDK_KEY_r, (GdkModifierType)GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    gtk_widget_show_all(window);

    gtk_main();

    gst_element_set_state(vd.playbin, GST_STATE_NULL);
    gst_object_unref(vd.playbin);

    return 0;
}
