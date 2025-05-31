#include <gtk/gtk.h>
#include <glib.h>

typedef struct 
{
  GtkTreeStore *store;
  GtkTreeIter *parent_iter;
} TreeData;

void filesTree (const gchar *path, TreeData* data)
{
  GError *error = NULL;
  GDir *dir = g_dir_open(path, 0, &error);
  if (!dir)
  {
    g_printerr("Error opening directory %s: %s\n", path, error->message);
    g_error_free(error);
    return;
  }

  const gchar* name;
  while ((name = g_dir_read_name(dir)) != NULL)
  {
    g_autofree char* full_path = g_build_filename(path, name, NULL);
    GtkTreeIter iter;
    gtk_tree_store_append(data->store, &iter, data->parent_iter);
    gtk_tree_store_set(data->store, &iter,
                        0, name,
                        1, g_file_test(full_path, G_FILE_TEST_IS_DIR) ? "Directory" : "File",
                        -1);
    if (g_file_test(full_path, G_FILE_TEST_IS_DIR))
    {
      TreeData child_data = {data->store, &iter};
      filesTree(full_path, &child_data);
    }
  }
  g_dir_close(dir);
}

void buttonCallback (GtkWidget *widget, gpointer data)
{
  TreeData *tree_data = (TreeData*)data;
  gtk_tree_store_clear(tree_data->store);
  filesTree(".", tree_data);              // передаем текущую директорию
}

static void activate (GtkApplication *app, gpointer user_data)
{
  /* create a new window, and set its title */
  GtkWidget *window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW (window), "Tree directory");
/* Here we construct the container that is going pack our buttons */
  GtkWidget *grid = gtk_grid_new();
    /* Pack the container in the window */
  gtk_window_set_child (GTK_WINDOW (window), grid);
  GtkWidget *button;
  GtkTreeStore *store = gtk_tree_store_new(2, G_TYPE_STRING, G_TYPE_STRING);
  TreeData *data = g_new(TreeData, 1);
  data->store = store;
  data->parent_iter = NULL;

  button = gtk_button_new_with_label ("Get files tree");
  g_signal_connect (button, "clicked", G_CALLBACK (buttonCallback), data);

  /* Place the first button in the grid cell (0, 0), and make it fill
   * just 1 cell horizontally and vertically (ie no spanning)
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 0, 1, 1);

  button = gtk_button_new_with_label ("Quit");
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_destroy), window);

  /* Place the Quit button in the grid cell (0, 1), and make it
   * span 2 columns.
   */
  gtk_grid_attach (GTK_GRID (grid), button, 0, 1, 1, 1);

  GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *column = gtk_tree_view_column_new_with_attributes(
    "Tree", renderer, "text", 0, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), column);

  gtk_grid_attach(GTK_GRID(grid), tree_view, 1, 0, 1, 2);

  gtk_window_present (GTK_WINDOW (window));
}


int main (int argc, char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
