/*
 ** Copyright (©) 2003-2013 Teus Benschop.
 **  
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 3 of the License, or
 ** (at your option) any later version.
 **  
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **  
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 **  
 */

#include "outline.h"
#include "settings.h"
#include "projectutils.h"
#include "usfmtools.h"
#include "utilities.h"
#include "usfm.h"
#include "bible.h"
#include "keyboard.h"
#include "tiny_utilities.h"
#include "stylesheetutils.h"


enum { COLUMN_HEADING,
  COLUMN_CHAPTER_START, COLUMN_VERSE_START,
  COLUMN_CHAPTER_END, COLUMN_VERSE_END,
  NUM_COLUMNS
};

Outline::Outline(GtkWidget * vbox)
{
  // Initialize / save variables.

  // Build GUI.
  reference_changed_signal = gtk_button_new_with_mnemonic("");
  gtk_box_pack_start(GTK_BOX(vbox), reference_changed_signal, FALSE, FALSE, 0);
  gtk_widget_set_can_focus (reference_changed_signal, false);
  gtk_button_set_focus_on_click(GTK_BUTTON(reference_changed_signal), FALSE);

  scrolledwindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_widget_show(scrolledwindow);
  gtk_box_pack_start(GTK_BOX(vbox), scrolledwindow, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolledwindow), GTK_SHADOW_IN);

  treeview = gtk_tree_view_new();
  gtk_widget_show(treeview);
  gtk_container_add(GTK_CONTAINER(scrolledwindow), treeview);
  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(treeview), FALSE);

  // Build underlying structure.
  store = gtk_tree_store_new(NUM_COLUMNS, G_TYPE_STRING, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT, G_TYPE_INT);
  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview), GTK_TREE_MODEL(store));
  g_object_unref(store);
  GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
  GtkTreeViewColumn *column;
  column = gtk_tree_view_column_new_with_attributes("", renderer, "text", COLUMN_HEADING, NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(treeview), column);
  GtkTreeSelection *select = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  gtk_tree_selection_set_mode(select, GTK_SELECTION_SINGLE);
  g_signal_connect((gpointer) treeview, "button_press_event", G_CALLBACK(on_view_button_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "key_press_event", G_CALLBACK(on_view_key_press_event), gpointer(this));
  g_signal_connect((gpointer) treeview, "row_activated", G_CALLBACK(on_view_row_activated), gpointer(this));
  g_signal_connect((gpointer) treeview, "popup_menu", G_CALLBACK(on_view_popup_menu), gpointer(this));
}

Outline::~Outline()
{
}

void Outline::focus()
{
  gtk_widget_grab_focus(treeview);
}

bool Outline::focused()
{
  return gtk_widget_has_focus (treeview);
}

void Outline::goto_reference(const ustring & project, Reference & reference)
{
  // See what changed.
  bool changed = false;

  // Handle change of project.
  if (project != myproject) {
    changed = true;
    myproject = project;
    new_project_handling();
  }
  // Handle change of book.  
  if (reference.book != mybook) {
    changed = true;
    mybook = reference.book;
  }
  // Handle change of chapter.
  if (reference.chapter != mychapter) {
    changed = true;
    mychapter = reference.chapter;
  }
  // If no change, bail out after focusing for any verse change.
  if (!changed) {
    focus(reference);
    return;
  }
  // Get and process headers from the text.
  headers.clear();
  levels.clear();
  chapters_from.clear();
  verses_from.clear();
  chapters_to.clear();
  verses_to.clear();
  verses_last.clear();
  chapter_last = 0;
  vector < unsigned int >chapt = project_get_chapters(myproject, mybook);
  for (unsigned int ch = 0; ch < chapt.size(); ch++) {
    retrieve_headers(chapt[ch]);
  }
  chapters_verses();
  handle_chapter_zero_headers();

  // Load the values.
  load();

  // Focus the right heading.
  focus(reference);
}

void Outline::load()
// Load outline.
{
  // Clear store.
  gtk_tree_store_clear(store);
  // If there are no headers, bail out.
  if (headers.empty())
    return;
  // Iterator.
  GtkTreeIter iter;
  // Keep track of the last level that was added to the store.
  unsigned int last_level = 0;
  // Go through all headers.
  for (unsigned int i = 0; i < headers.size(); i++) {
    // Pad up the difference between this level and the last one.
    // Get the right iterator where to add this header.
    unsigned int current_level = levels[i];
    // Situation that the level is 1.
    if (current_level == 1) {
      gtk_tree_store_append(store, &iter, NULL);
    }
    // Situation that the level remains the same.
    else if (current_level == last_level) {
      GtkTreeIter child = iter;
      gtk_tree_model_iter_parent(GTK_TREE_MODEL(store), &iter, &child);
      gtk_tree_store_append(store, &iter, &iter);
    }
    // Situation that the level goes up.
    else if (current_level > last_level) {
      for (unsigned int i2 = last_level + 1; i2 <= current_level; i2++) {
        if (i2 == 1) {
          gtk_tree_store_append(store, &iter, NULL);
        } else {
          gtk_tree_store_append(store, &iter, &iter);
        }
        if (i2 < current_level) {
          ustring ch = "Chapter " + convert_to_string(chapters_from[i]);
          gtk_tree_store_set(store, &iter, COLUMN_HEADING, ch.c_str(), COLUMN_CHAPTER_START, chapters_from[i], COLUMN_VERSE_START, verses_from[i], COLUMN_CHAPTER_END, chapters_from[i], COLUMN_VERSE_END, verses_from[i], -1);
        }
      }
    }
    // Situation that the level goes down.
    else if (current_level < last_level) {
      for (unsigned int i2 = current_level; i2 <= last_level; i2++) {
        GtkTreeIter child = iter;
        gtk_tree_model_iter_parent(GTK_TREE_MODEL(store), &iter, &child);
      }
      gtk_tree_store_append(store, &iter, &iter);
    }
    // We now have the right iterator where to store the data. Store it.
    gtk_tree_store_set(store, &iter, COLUMN_HEADING, headers[i].c_str(), COLUMN_CHAPTER_START, chapters_from[i], COLUMN_VERSE_START, verses_from[i], COLUMN_CHAPTER_END, chapters_to[i], COLUMN_VERSE_END, verses_to[i], -1);
    // Save level.
    last_level = current_level;
  }
}

void Outline::retrieve_headers(unsigned int chapter)
{
  unsigned int verse = 0;
  vector < ustring > lines = project_retrieve_chapter(myproject, mybook, chapter);
  for (unsigned int i = 0; i < lines.size(); i++) {
    ustring marker = usfm_extract_marker(lines[i]);
    unsigned int level = 0;
    for (unsigned int i2 = 0; i2 < paragraphmarkers.size(); i2++) {
      if (paragraphmarkers[i2] == marker) {
        level = paragraphlevels[i2];
        break;
      }
    }
    if (level) {
      headers.push_back(lines[i]);
      levels.push_back(level);
      chapters_from.push_back(chapter);
      verses_from.push_back(verse + 1);
    }
    if (marker == versemarker) {
      // This also finds the last verse in a sequence or range, e.g.
      // verse 8 from \v 6-8.
      size_t position = lines[i].find(" ");
      position = CLAMP(position, 0, lines[i].length());
      ustring vs = lines[i].substr(0, position);
      vector < unsigned int >verses = verse_range_sequence(vs);
      if (verses.empty())
        verse = 0;
      else
        verse = verses[verses.size() - 1];
    }
  }
  // Store last verse of the chapter. It might be needed later.
  verses_last[chapter] = verse;
  // Store last chapter. Needed later on.
  chapter_last = chapter;
}

void Outline::chapters_verses()
/*
 Add chapter/verse information to the headers and appropriate containers.
 This is done using a certain algorithm to get it right.
 The header starts at the chapter:verse given, that is a simple issue.
 The more difficult thing is to find out where it ends.
 If the next header starts at verse 1 of a certain chapter, 
 it means that this header starts at the last verse of the previous chapter.
 But if the next header starts at any verse, not 1, of a certain chapter,
 that means that this header starts at that verse minus one.
 If the ending chapter is the same as the starting chapter, 
 it should not be named, e.g. 10:1-5.
 But if the ending chapters differs, it should be named, e,g. 10:1-11:6.
 For the last header, take the last chapter's last verse.
 */
{
  // Go through each header.
  for (unsigned int i = 0; i < headers.size(); i++) {
    // Do not add chapter:verse information for chapter zero.
    if (!chapters_from[i]) {
      verses_from[i] = 0;
      chapters_to.push_back(0);
      verses_to.push_back(0);
      continue;
    }
    // Add starting chapter:verse      
    headers[i].append(" (" + convert_to_string(chapters_from[i]) + ":" + convert_to_string(verses_from[i]));
    // To start off calculating the final chapter:verse, set it to the 
    // last chapter and the last verse.
    unsigned int ending_chapter = chapter_last;
    unsigned int ending_verse = verses_last[ending_chapter];
    // From here on we only can proceed calculating the last chapter:verse
    // if more headings follow. If not, the values are left as they are.
    unsigned int i2 = i + 1;
    bool chapter_verse_differ = true;
    if (i2 < headers.size()) {
      // Initialize the ending chapter:verse from where the next heading starts.
      unsigned int next_chapter = chapters_from[i2];
      unsigned int next_verse = verses_from[i2];
      // Find out whether the chapters:verses differ, the beginning and end.
      chapter_verse_differ = ((next_chapter != chapters_from[i]) || (next_verse != verses_from[i]));
      // If the verse is 1 then this heading ends at the previous chapter's last verse.
      if (next_verse == 1) {
        ending_chapter = next_chapter - 1;
        ending_verse = verses_last[ending_chapter];
      } else {
        ending_chapter = next_chapter;
        ending_verse = next_verse - 1;
      }
      // Ensure that the ending is not smaller than the starting.
      if (ending_chapter < chapters_from[i]) {
        ending_chapter = chapters_from[i];
        ending_verse = verses_from[i];
      }
    }
    // We've found the chapter:verse that this heading ends with.
    // If the chapter differs from the chapter it starts with, mention it.
    // Only add the ending bit, if the chapters:verses differ.
    if (chapter_verse_differ) {
      headers[i].append("-");
      if (ending_chapter != chapters_from[i]) {
        headers[i].append(convert_to_string(ending_chapter) + ":");
      }
      // Add ending verse.
      headers[i].append(convert_to_string(ending_verse));
    }
    // Add ending bracket.
    headers[i].append(")");
    // Store the ending chapters and verses.
    chapters_to.push_back(ending_chapter);
    verses_to.push_back(ending_verse);
  }
}

void Outline::handle_chapter_zero_headers()
// Pushes the level of all headers of chapter 0 up one step.
{
  for (unsigned int i = 0; i < levels.size(); i++) {
    if (chapters_from[i] == 0) {
      levels[i]++;
    }
  }
}

void Outline::new_project_handling()
// Does some operations when there is another project.
{
  // Create an USFM object of the relevant stylesheet.
  Usfm usfm(stylesheet_get_actual ());
  // Clear markers of the various levels of the paragraphs.
  paragraphmarkers.clear();
  paragraphlevels.clear();
  // Go through all the styles.
  for (unsigned int i = 0; i < usfm.styles.size(); i++) {
    ustring marker = usfm.styles[i].marker;
    // Store the markers that start the paragraph, and their levels.
    if (usfm.is_starting_paragraph(marker)) {
      ParagraphType paragraphtype = usfm.paragraph_get_subtype(marker);
      if (paragraphtype < ptNormalParagraph) {
        unsigned int level = 1;
        if (marker.find("2") != string::npos)
          level = 2;
        if (marker.find("3") != string::npos)
          level = 3;
        if (marker.find("4") != string::npos)
          level = 4;
        if (marker.find("5") != string::npos)
          level = 5;
        if (marker.find("6") != string::npos)
          level = 6;
        paragraphmarkers.push_back(marker);
        paragraphlevels.push_back(level);
      }
    }
    // Store the marker that starts the verse number.
    if (usfm.is_verse_number(marker)) {
      versemarker = marker;
    }
  }
}

gboolean Outline::on_view_button_press_event(GtkWidget * widget, GdkEventButton * event, gpointer user_data)
{
  return false;
}

gboolean Outline::on_view_key_press_event(GtkWidget * widget, GdkEventKey * event, gpointer user_data)
{
  // Collapse or expand an item.
  if (keyboard_left_arrow_pressed(event)) {
    return ((Outline *) user_data)->expand_collapse(false);
  }
  if (keyboard_right_arrow_pressed(event)) {
    return ((Outline *) user_data)->expand_collapse(true);
  }
  return false;
}

bool Outline::expand_collapse(bool expand)
// Expands or collapses the selected items in the treeview.
{
  bool expanded_collapsed = false;
  GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(treeview));
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  GtkTreeIter iter;
  if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    if (expand) {
      if (gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false))
        expanded_collapsed = true;
    } else {
      if (gtk_tree_view_collapse_row(GTK_TREE_VIEW(treeview), path))
        expanded_collapsed = true;
    }
    gtk_tree_path_free(path);
  }
  // Return whether we did something. 
  // This is helpful to let the keys do their natural function in case nothing
  // was expanded or collapsed.
  return expanded_collapsed;
}

void Outline::focus(Reference & reference)
// Focus the entry that has the reference.
{
  GtkTreeModel *model = GTK_TREE_MODEL(store);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  GtkTreeIter iter;
  gboolean valid;
  bool focused = false;
  unsigned int ref_number = reference_number(reference.chapter, convert_to_int(reference.verse));
  // Get the first iter in the store.
  valid = gtk_tree_model_get_iter_first(model, &iter);
  while (valid) {
    focus_internal_select(ref_number, iter, selection, focused);
    focus_internal_children(ref_number, iter, selection, focused);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}

void Outline::focus_internal_children(unsigned int reference, GtkTreeIter & parent, GtkTreeSelection * selection, bool & focused)
// Iterates through possible children. Re-entrant function.
{
  GtkTreeModel *model = GTK_TREE_MODEL(store);
  GtkTreeIter iter;
  gboolean valid;
  valid = gtk_tree_model_iter_children(model, &iter, &parent);
  while (valid) {
    focus_internal_select(reference, iter, selection, focused);
    focus_internal_children(reference, iter, selection, focused);
    valid = gtk_tree_model_iter_next(model, &iter);
  }
}

void Outline::focus_internal_select(unsigned int reference, GtkTreeIter & iter, GtkTreeSelection * selection, bool & focused)
// Selects and focuses the entry that corresponds to the reference.
{
  if (focused)
    return;
  GtkTreeModel *model = GTK_TREE_MODEL(store);
  guint chapter_start, verse_start, chapter_end, verse_end;
  gtk_tree_model_get(model, &iter, COLUMN_CHAPTER_START, &chapter_start, COLUMN_VERSE_START, &verse_start, COLUMN_CHAPTER_END, &chapter_end, COLUMN_VERSE_END, &verse_end, -1);
  unsigned int heading_start = reference_number(chapter_start, verse_start);
  unsigned int heading_end = reference_number(chapter_end, verse_end);
  if (reference >= heading_start) {
    if (reference <= heading_end) {
      while (gtk_events_pending())
        gtk_main_iteration();
      gtk_tree_selection_select_iter(selection, &iter);
      GtkTreePath *path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
      gtk_tree_view_expand_row(GTK_TREE_VIEW(treeview), path, false);
      gtk_tree_view_set_cursor(GTK_TREE_VIEW(treeview), path, NULL, false);
      gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(treeview), path, NULL, true, 0.5, 0.5);
      gtk_tree_path_free(path);
      focused = true;
    }
  }
}

void Outline::on_view_row_activated(GtkTreeView * treeview, GtkTreePath * path, GtkTreeViewColumn * column, gpointer user_data)
{
  ((Outline *) user_data)->on_view_row(path);
}

void Outline::on_view_row(GtkTreePath * path)
{
  // Get the chapter and verse Bibledit ought to go to.
  GtkTreeModel *model = GTK_TREE_MODEL(store);
  GtkTreeIter iter;
  if (!gtk_tree_model_get_iter(model, &iter, path))
    return;
  gtk_tree_model_get(model, &iter, COLUMN_CHAPTER_START, &newchapter, COLUMN_VERSE_START, &newverse, -1);
  // Signal that a new reference is available.
  gtk_button_clicked(GTK_BUTTON(reference_changed_signal));
}

gboolean Outline::on_view_popup_menu(GtkWidget * widget, gpointer user_data)
{
  return false;
}

unsigned int Outline::reference_number(unsigned int chapter, unsigned int verse)
{
  return (200 * chapter) + verse;
}
