EditorsGUI::EditorsGUI(GtkWidget * notebook_vbox) {
  // Create signalling buttons.
  new_verse_button = gtk_button_new();
  new_styles_button = gtk_button_new();
  word_double_clicked_button = gtk_button_new();
  editor_reload_button = gtk_button_new();
  editor_changed_button = gtk_button_new();
  quick_references_button = gtk_button_new();
}

EditorsGUI::~EditorsGUI() {
  // Destroy signalling buttons.
  gtk_widget_destroy(new_verse_button);
  gtk_widget_destroy(new_styles_button);
  gtk_widget_destroy(word_double_clicked_button);
  gtk_widget_destroy(editor_reload_button);
  gtk_widget_destroy(editor_changed_button);
  gtk_widget_destroy(quick_references_button);
}

void EditorsGUI::go_to(const Reference& reference) {
  // Get the Editor. If none, bail out.
  Editor * editor = focused_editor();
  if (!editor)
    return;

  // Find out what needs to be changed: book, chapter and/or verse.
  bool new_book = (reference.book != editor->current_reference.book);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.book = reference.book;
  }
  bool new_chapter = (reference.chapter != editor->current_reference.chapter);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.chapter = reference.chapter;
  }
  bool new_verse = (reference.verse != editor->current_reference.verse);
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->current_reference.verse = reference.verse;
  }

  // Save the editors if need be.
  if (new_book || new_chapter) {
    save();
  }

  // With a new book, also load a new chapter.
  if (new_book) {
    new_chapter = true;
    for (unsigned int e = 0; e < editors.size(); e++) {
      editors[e]->book_set(reference.book);
    }
  }

  // Deal with a new chapter.
  if (new_chapter) {
    // Load chapter in all Editors, if need be.
    // Handle all Editors.
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Open chapter in editor.
      editors[e]->chapter_load(reference.chapter);
    }
    // When loading a new chapter, there is also a new verse.
    new_verse = true;
  }

  // New reference handling.  
  if (new_book || new_chapter || new_verse) {
    // Position the cursor properly.
    // The positioning will be done whenever Gtk is idle.
    // This is because sometimes Gtk is slow in loading a new chapter.
    // So if the cursor positioning is done straight after loading,
    // it will not work, as there is no text loaded yet.
    // But here we deal with that so that a delay is no longer needed. The trick:
    // Handle all pending events in GTK.
    while (gtk_events_pending())
      gtk_main_iteration();
    for (unsigned int e = 0; e < editors.size(); e++) {
      // Only focus the editor now showing.
      editors[e]->position_cursor_at_verse(reference.verse, editors[e] == editor);
    }
  }

  // Highlighting of searchwords.
  for (unsigned int e = 0; e < editors.size(); e++) {
    if (editors[e]->go_to_new_reference_highlight) {
      editors[e]->highlight_searchwords();
      editors[e]->go_to_new_reference_highlight = false;
    }
  }
}

void EditorsGUI::on_focus_signal(GtkButton *button)
// Handle the change of focus, that is, another editor gets focused.
{
  // The font and colour are tied to the project, 
  // but also stored in the general configuration.
  if (!project.empty()) {
    ProjectConfiguration * projectconfig = settings->projectconfig(project);
    settings->genconfig.text_editor_font_default_set(projectconfig->editor_font_default_get());
    settings->genconfig.text_editor_font_name_set(projectconfig->editor_font_name_get());
    settings->genconfig.text_editor_default_color_set(projectconfig->editor_default_color_get());
    settings->genconfig.text_editor_normal_text_color_set(projectconfig->editor_normal_text_color_get());
    settings->genconfig.text_editor_background_color_set(projectconfig->editor_background_color_get());
    settings->genconfig.text_editor_selected_text_color_set(projectconfig->editor_selected_text_color_get());
    settings->genconfig.text_editor_selection_color_set(projectconfig->editor_selection_color_get());

  }
}

void EditorsGUI::set_fonts() {
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->set_font();
    editors[e]->create_or_update_formatting_data();
  }
}

void EditorsGUI::reload_styles() {
  for (unsigned int e = 0; e < editors.size(); e++) {
    editors[e]->create_or_update_formatting_data();
  }
}

void EditorsGUI::on_editor_changed_clicked(GtkButton *button, gpointer user_data)
// This signal will be called if anything in any of the editors changed.
{
  ((EditorsGUI *) user_data)->on_editor_changed();
}

void EditorsGUI::on_editor_changed() {
  gtk_button_clicked(GTK_BUTTON (editor_changed_button));
}

