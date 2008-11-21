class EditorsGUI
{
  // Navigation.
public:
  void go_to(const Reference& reference);
  GtkWidget * new_verse_button;
  GtkWidget * new_styles_button;

  // Internal storage and flags.
public:
private:
  vector <Editor *> editors;
  bool pass_signals_on;

  // Editors per page.
public:
  vector <Editor *> visible_editors_get();
private:
  void projects_pages_get(vector <ustring>& projects, vector <int> * pages);
  void editors_pages_get(vector <Editor *>& editorpointers, vector <int> * pages);

  // Title bar.
public:
  void title_bar_display();
private:

  // Editor change.
public:
  GtkWidget * editor_changed_button;
private:
  static void on_editor_changed_clicked(GtkButton *button, gpointer user_data);
  void on_editor_changed();

};

#endif
