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


#include <config.h>
#include "mainwindow.h"
#include "libraries.h"
#include "dialoglistview.h"
#include "dialogsystemlog.h"
#include "constants.h"
#include "utilities.h"
#include "htmlbrowser.h"
#include "usfmtools.h"
#include "dialogreplace.h"
#include "dialoggotoreference.h"
#include <signal.h>
#include "bible.h"
#include "projectutils.h"
#include "dialogproject.h"
#include "usfm.h"
#include "dialogreplacing.h"
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include "dialogsearchspecial.h"
#include "referenceutils.h"
#include "pdfviewer.h"
#include "notes_utils.h"
#include "dialogfindnote.h"
#include "dialogimportnotes.h"
#include "date_time_utils.h"
#include "dialognotes.h"
#include "dialogentry.h"
#include "projectutils.h"
#include "directories.h"
#include "dialogcompare.h"
#include "export_utils.h"
#include "dialogprintprefs.h"
#include "dialogprintproject.h"
#include "printproject.h"
#include "printproject2.h"
#include "compareutils.h"
#include "dialogshownotes.h"
#include "dialogentry3.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "search_utils.h"
#include "combobox.h"
#include "scripturechecks.h"
#include "dialogrefexchange.h"
#include "dialogeditlist.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "highlight.h"
#include "stylesheetutils.h"
#include "keyboard.h"
#include "dialoginsertnote.h"
#include "dialogparallelbible.h"
#include "print_parallel_bible.h"
#include "layout.h"
#include "dialogbook.h"
#include "books.h"
#include "screen.h"
#include "dialogoutpost.h"
#include "dialognotestransfer.h"
#include "dialogchapternumber.h"
#include "versification.h"
#include "dialogmychecks.h"
#include "help.h"
#include "dialogoriginreferences.h"
#include "dialogtidy.h"
#include "dialognotesupdate.h"
#include "backup.h"
#include "dialogrevert.h"
#include "resource_utils.h"
#include "dialogeditnote.h"
#include "dialogwordlist.h"
#include "dialogfontcolor.h"
#include "color.h"
#include "dialognewstylesheet.h"
#include "dialoggui.h"
#include "password.h"
#include "gui_features.h"
#include "dialogprintreferences.h"
#include "print_parallel_references.h"
#include "dialogfixmarkers.h"
#include "dialogtextreplacement.h"
#include "temporal.h"
#include "parallel_passages.h"
#include "dialogpdfviewer.h"
#include "dialoginserttable.h"
#include "tiny_utilities.h"
#include "hyphenate.h"
#include "dialogviewplanning.h"
#include "planning.h"
#include "dialogplanningsetup.h"
#include "maintenance.h"
#include "dialogfilters.h"
#include "dialogradiobutton.h"
#include "import.h"
#include "dialogxfernotes2text.h"
#include "text2pdf.h"
#include "floatingwindow.h"
#include "unixwrappers.h"
#include "dialogcompareprefs.h"
#include "windowtimednotifier.h"
#include "dialogbulkspelling.h"
#include "dialogplanningedit.h"
#include "dialogmaintenance.h"
#include "kjv.h"
#include "dialogyesnoalways.h"
#include "dialogxetex.h"
#include "vcs.h"
#include <glib/gi18n.h>
#include "chapterview.h"
#include "debug.h"

/*
 |
 |
 |
 |
 |
 Construction and destruction
 |
 |
 |
 |
 |
 */


MainWindow::MainWindow(unsigned long xembed, GtkAccelGroup * _accelerator_group, Settings *_settings, URLTransport *_urltransport, VCS *_vcs):
navigation(0), httpd(0)
{
  // Store pointers to the settings, etc. so we don't have to rely on "extern" declarations of them
  accelerator_group = _accelerator_group;
  settings = _settings;
  urltransport = _urltransport;
  vcs = _vcs;

  // Set some window pointers to NULL.
  // To save memory, we only create the object when actually needed.
  window_show_related_verses = NULL;
  window_merge = NULL;
  window_outline = NULL;
  window_check_keyterms = NULL;
  window_styles = NULL;
  window_notes = NULL;
  window_references = NULL;
  import_keyterms_assistant = NULL;
  delete_keyterms_assistant = NULL;
  changes_assistant = NULL;
  window_check_usfm = NULL;
  remote_repository_assistant = NULL;
  resource_assistant = NULL;
  backup_assistant = NULL;
  restore_assistant = NULL;
  export_assistant = NULL;
  import_assistant = NULL;
  
  // Initialize some variables.
  windows_startup_pointer = 0;
  focused_editor_button = NULL;
  focused_resource_button = NULL;
  focused_tool_button = NULL;
  shutting_down = false;
  check_spelling_at_start = false;
  check_spelling_at_end = false;
  interprocess_communications_initiate_listener_event_id = 0;
  interprocess_communications_initiate_listener_message_id = 0;
  
  // Application name.
  g_set_application_name("Bibledit-Gtk");

  // Gui Features object.
  GuiFeatures guifeatures(0);
  project_notes_enabled = guifeatures.project_notes();
  project_notes_management_enabled = guifeatures.project_notes_management();
  references_management_enabled = guifeatures.references_management();

  // Accelerators.
  gtk_accel_group_connect(accelerator_group, GDK_KEY_X, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_cut_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_C, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_copy_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_V, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_paste_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Z, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_undo_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Z, GdkModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_redo_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_N, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_new_project_note_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Down, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_verse_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Up, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_verse_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Page_Down, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_chapter_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Page_Up, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_chapter_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Page_Down, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_book_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Page_Up, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_book_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Right, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_reference_in_history_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Left, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_reference_in_history_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_G, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_go_to_reference_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_W, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_close_window_callback), gpointer(this), NULL));
  if (guifeatures.styles()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_S, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_goto_styles_area_callback), gpointer(this), NULL));
  }
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Q, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_quit_program_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_F5, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_text_area_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_F5, GDK_SHIFT_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_tools_area_callback), gpointer(this), NULL));
  if (guifeatures.project_notes()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_F5, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_notes_area_callback), gpointer(this), NULL));
  }
  if (guifeatures.references_and_find()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_F6, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_reference_in_reference_area_callback), gpointer(this), NULL));
    gtk_accel_group_connect(accelerator_group, GDK_KEY_F6, GDK_SHIFT_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_reference_in_reference_area_callback), gpointer(this), NULL));
  }
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Right, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_project_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_Left, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_project_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_O, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_open_project_callback), gpointer(this), NULL));
  if (guifeatures.printing()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_P, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_print_callback), gpointer(this), NULL));
  }
  if (guifeatures.references_and_find()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_F, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_find_callback), gpointer(this), NULL));
  }
  if (guifeatures.replace()) {
    gtk_accel_group_connect(accelerator_group, GDK_KEY_R, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_replace_callback), gpointer(this), NULL));
  }
  gtk_accel_group_connect(accelerator_group, GDK_KEY_F1, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_main_help_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_KEY_F2, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_main_jumpto_callback), gpointer(this), NULL));
  // For USFM view, we first had defined the Ctrl-U for that. The U is the first letter of USFM,
  // and also in for example Firefox, the Ctrl-U views the source code.
  // But since the Ctrl-U clashed with the Ctrl-U for underline in the note editor,
  // the accelerator key for viewing USFM code was changed to Ctrl-\.
  // The backslash is the first character of the USFM code.
  gtk_accel_group_connect(accelerator_group, GDK_KEY_backslash, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_view_usfm_code), gpointer(this), NULL));

  // GUI build.

  // Main window, xembed is used on the XO machine.
  if (xembed) {
    window_main = gtk_plug_new(GdkNativeWindow(xembed));
	// In 64-bit compilation, we get this error at xembed:
	// mainwindow.cpp: In constructor 'MainWindow::MainWindow(long unsigned int, GtkAccelGroup*, Settings*, URLTransport*, VCS*)':
    // mainwindow.cpp:251:54: warning: cast to pointer from integer of different size [-Wint-to-pointer-cast]
    // window_main = gtk_plug_new(GdkNativeWindow(xembed));

  } else {
    window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  }

  // Size and position of window and screen layout.
  ScreenLayoutDimensions * dimensions = new ScreenLayoutDimensions (window_main);
  dimensions->verify();
  gtk_widget_show (window_main);
  g_signal_connect ((gpointer) window_main, "delete_event", G_CALLBACK(on_window_delete_event), gpointer(this));

  // Use accelerators.
  gtk_window_add_accel_group(GTK_WINDOW(window_main), accelerator_group);

  vbox_main = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox_main);
  gtk_container_add (GTK_CONTAINER (window_main), vbox_main);

  menubar1 = gtk_menu_bar_new();
  gtk_widget_show(menubar1);
  gtk_box_pack_start(GTK_BOX(vbox_main), menubar1, FALSE, FALSE, 0);

  menuitem_file = gtk_menu_item_new_with_mnemonic(_("_File"));
  gtk_widget_show(menuitem_file);
  gtk_container_add(GTK_CONTAINER(menubar1), menuitem_file);

  menuitem_file_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_file), menuitem_file_menu);

  file_project = gtk_image_menu_item_new_with_mnemonic(_("Pr_oject"));
  gtk_widget_show(file_project);
  gtk_container_add(GTK_CONTAINER(menuitem_file_menu), file_project);

  image463 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image463);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_project), image463);

  file_project_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_project), file_project_menu);

  new1 = NULL;
  open1 = NULL;
  delete1 = NULL;
  if (guifeatures.project_management()) {

    new1 = gtk_image_menu_item_new_with_mnemonic(_("_New"));
    gtk_widget_show(new1);
    gtk_container_add(GTK_CONTAINER(file_project_menu), new1);

    image903 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image903);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(new1), image903);

    open1 = gtk_image_menu_item_new_from_stock("gtk-open", NULL);
    gtk_widget_show(open1);
    gtk_container_add(GTK_CONTAINER(file_project_menu), open1);

    delete1 = gtk_image_menu_item_new_from_stock("gtk-delete", NULL);
    gtk_widget_show(delete1);
    gtk_container_add(GTK_CONTAINER(file_project_menu), delete1);

  }

  properties1 = NULL;
  copy_project_to = NULL;
  compare_with1 = NULL;
  if (guifeatures.project_management()) {

    properties1 = gtk_image_menu_item_new_with_mnemonic(_("P_roperties"));
    gtk_widget_show(properties1);
    gtk_container_add(GTK_CONTAINER(file_project_menu), properties1);

    image4995 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image4995);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(properties1), image4995);

    copy_project_to = gtk_image_menu_item_new_with_mnemonic(_("Cop_y to"));
    gtk_widget_show(copy_project_to);
    gtk_container_add(GTK_CONTAINER(file_project_menu), copy_project_to);

    image2688 = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2688);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(copy_project_to), image2688);

    compare_with1 = gtk_image_menu_item_new_with_mnemonic(_("Co_mpare with"));
    gtk_widget_show(compare_with1);
    gtk_container_add(GTK_CONTAINER(file_project_menu), compare_with1);

    image2764 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2764);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(compare_with1), image2764);

  }

  project_changes = NULL;
  if (guifeatures.project_management()) {

    project_changes = gtk_image_menu_item_new_with_mnemonic(_("C_hanges"));
    gtk_widget_show(project_changes);
    gtk_container_add(GTK_CONTAINER(file_project_menu), project_changes);

    image19115 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image19115);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(project_changes), image19115);

  }

  file_projects_merge = gtk_check_menu_item_new_with_mnemonic(_("Mer_ge"));
  gtk_widget_show(file_projects_merge);
  gtk_container_add(GTK_CONTAINER(file_project_menu), file_projects_merge);

  projects_send_receive1 = gtk_image_menu_item_new_with_mnemonic (_("_Send / receive"));
  gtk_widget_show (projects_send_receive1);
  gtk_container_add (GTK_CONTAINER (file_project_menu), projects_send_receive1);

  image34440 = gtk_image_new_from_stock ("gtk-network", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image34440);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (projects_send_receive1), image34440);

  style = NULL;
  if (guifeatures.styles()) {

    style = gtk_image_menu_item_new_with_mnemonic(_("_Styles"));
    gtk_widget_show(style);
    gtk_container_add(GTK_CONTAINER(menuitem_file_menu), style);

    image10735 = gtk_image_new_from_stock("gtk-print-preview", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10735);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(style), image10735);

  }

  style_menu = NULL;
  if (guifeatures.styles()) {

    style_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(style), style_menu);

  }

  stylesheet_open = NULL;
  if (guifeatures.styles()) {

    stylesheet_open = gtk_image_menu_item_new_with_mnemonic(_("_Open"));
    gtk_widget_show(stylesheet_open);
    gtk_container_add(GTK_CONTAINER(style_menu), stylesheet_open);

    image31346 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image31346);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheet_open), image31346);

  }

  stylesheets_expand_all = NULL;
  if (guifeatures.styles()) {

    stylesheets_expand_all = gtk_image_menu_item_new_with_mnemonic(_("_Expand all"));
    //gtk_widget_show(stylesheets_expand_all);
    gtk_container_add(GTK_CONTAINER(style_menu), stylesheets_expand_all);

    GtkWidget *image10736;
    image10736 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10736);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheets_expand_all), image10736);

  }

  stylesheets_collapse_all = NULL;
  if (guifeatures.styles()) {

    stylesheets_collapse_all = gtk_image_menu_item_new_with_mnemonic(_("_Collapse all"));
    //gtk_widget_show(stylesheets_collapse_all);
    gtk_container_add(GTK_CONTAINER(style_menu), stylesheets_collapse_all);

    GtkWidget *image10737;
    image10737 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10737);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheets_collapse_all), image10737);

  }

  style_insert = NULL;
  if (guifeatures.styles()) {

    style_insert = gtk_image_menu_item_new_with_mnemonic("_Insert");
    //gtk_widget_show(style_insert);
    gtk_container_add(GTK_CONTAINER(style_menu), style_insert);

    GtkWidget *image10738;
    image10738 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10738);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(style_insert), image10738);

  }

  stylesheet_edit_mode = NULL;
  if (guifeatures.styles_management()) {

    stylesheet_edit_mode = gtk_check_menu_item_new_with_mnemonic(_("Edit _mode"));
    //gtk_widget_show(stylesheet_edit_mode);
    gtk_container_add(GTK_CONTAINER(style_menu), stylesheet_edit_mode);

  }

  style_new = NULL;
  if (guifeatures.styles_management()) {

    style_new = gtk_image_menu_item_new_with_mnemonic("_New");
    //gtk_widget_show(style_new);
    gtk_container_add(GTK_CONTAINER(style_menu), style_new);

    GtkWidget *image10739;
    image10739 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10739);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(style_new), image10739);

  }

  style_properties = NULL;
  if (guifeatures.styles_management()) {

    style_properties = gtk_image_menu_item_new_from_stock("gtk-properties", NULL);
    //gtk_widget_show(style_properties);
    gtk_container_add(GTK_CONTAINER(style_menu), style_properties);

  }

  style_delete = NULL;
  if (guifeatures.styles_management()) {

    style_delete = gtk_image_menu_item_new_from_stock("gtk-delete", NULL);
    //gtk_widget_show(style_delete);
    gtk_container_add(GTK_CONTAINER(style_menu), style_delete);

  }

  menu_stylesheet = NULL;
  if (guifeatures.styles_management()) {

    menu_stylesheet = gtk_image_menu_item_new_with_mnemonic(_("_Stylesheet"));
    //gtk_widget_show(menu_stylesheet);
    gtk_container_add(GTK_CONTAINER(style_menu), menu_stylesheet);

    GtkWidget *image10740;
    image10740 = gtk_image_new_from_stock("gtk-print-preview", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10740);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_stylesheet), image10740);

  }

  menu_stylesheet_menu = NULL;
  if (guifeatures.styles_management()) {

    menu_stylesheet_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(menu_stylesheet), menu_stylesheet_menu);

  }

  stylesheet_switch = NULL;
  if (guifeatures.styles_management()) {

    stylesheet_switch = gtk_image_menu_item_new_with_mnemonic(_("_Switch"));
    gtk_widget_show(stylesheet_switch);
    gtk_container_add(GTK_CONTAINER(menu_stylesheet_menu), stylesheet_switch);

    GtkWidget *image10741;
    image10741 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10741);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheet_switch), image10741);

  }

  stylesheets_new = NULL;
  if (guifeatures.styles_management()) {

    stylesheets_new = gtk_image_menu_item_new_with_mnemonic(_("_New"));
    gtk_widget_show(stylesheets_new);
    gtk_container_add(GTK_CONTAINER(menu_stylesheet_menu), stylesheets_new);

    GtkWidget *image10742;
    image10742 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10742);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheets_new), image10742);

  }

  stylesheets_delete = NULL;
  if (guifeatures.styles_management()) {

    stylesheets_delete = gtk_image_menu_item_new_with_mnemonic(_("_Delete"));
    gtk_widget_show(stylesheets_delete);
    gtk_container_add(GTK_CONTAINER(menu_stylesheet_menu), stylesheets_delete);

    GtkWidget *image10743;
    image10743 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10743);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheets_delete), image10743);

  }

  stylesheets_rename = NULL;
  if (guifeatures.styles_management()) {

    stylesheets_rename = gtk_image_menu_item_new_with_mnemonic(_("_Rename"));
    gtk_widget_show(stylesheets_rename);
    gtk_container_add(GTK_CONTAINER(menu_stylesheet_menu), stylesheets_rename);

    GtkWidget *image10744;
    image10744 = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10744);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(stylesheets_rename), image10744);

  }

  // Whether style management is enabled.
  style_management_enabled = guifeatures.styles_management();

  notes2 = NULL;
  new_note = NULL;
  delete_note = NULL;
  consultation_notes_send_receive = NULL;
  if (guifeatures.project_notes_management()) {

    notes2 = gtk_image_menu_item_new_with_mnemonic(_("Project _notes"));
    gtk_widget_show(notes2);
    gtk_container_add(GTK_CONTAINER(menuitem_file_menu), notes2);

    image936 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image936);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(notes2), image936);

    notes2_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(notes2), notes2_menu);

    new_note = gtk_image_menu_item_new_from_stock("gtk-new", NULL);
    gtk_widget_show(new_note);
    gtk_container_add(GTK_CONTAINER(notes2_menu), new_note);

    delete_note = gtk_image_menu_item_new_with_mnemonic(_("_Delete"));
    gtk_widget_show(delete_note);
    gtk_container_add(GTK_CONTAINER(notes2_menu), delete_note);

    image963 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image963);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(delete_note), image963);

    consultation_notes_send_receive = gtk_image_menu_item_new_with_mnemonic(_("_Send / Receive"));
    gtk_widget_show(consultation_notes_send_receive);
    gtk_container_add(GTK_CONTAINER(notes2_menu), consultation_notes_send_receive);
  }

  file_resources = gtk_image_menu_item_new_with_mnemonic(_("R_esources"));
  //gtk_widget_show(file_resources);
  gtk_container_add(GTK_CONTAINER(menuitem_file_menu), file_resources);

  image27365 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27365);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources), image27365);

  file_resources_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_resources), file_resources_menu);

  file_resources_open = gtk_image_menu_item_new_with_mnemonic(_("_Open"));
  //gtk_widget_show(file_resources_open);
  gtk_container_add(GTK_CONTAINER(file_resources_menu), file_resources_open);

  image27366 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27366);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources_open), image27366);

  file_resources_close = gtk_image_menu_item_new_with_mnemonic(_("_Close"));
  //gtk_widget_show(file_resources_close);
  gtk_container_add(GTK_CONTAINER(file_resources_menu), file_resources_close);

  image27367 = gtk_image_new_from_stock("gtk-close", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27367);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources_close), image27367);

  file_resources_new = gtk_image_menu_item_new_with_mnemonic(_("_New"));
  //gtk_widget_show(file_resources_new);
  gtk_container_add(GTK_CONTAINER(file_resources_menu), file_resources_new);

  image27514 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27514);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources_new), image27514);

  file_resources_edit = gtk_image_menu_item_new_with_mnemonic(_("_Edit"));
  //gtk_widget_show(file_resources_edit);
  gtk_container_add(GTK_CONTAINER(file_resources_menu), file_resources_edit);

  image27515 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27515);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources_edit), image27515);

  file_resources_delete = gtk_image_menu_item_new_with_mnemonic(_("_Delete"));
  //gtk_widget_show(file_resources_delete);
  gtk_container_add(GTK_CONTAINER(file_resources_menu), file_resources_delete);

  image27664 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
  //gtk_widget_show(image27664);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(file_resources_delete), image27664);

  file_keyterms = gtk_image_menu_item_new_with_mnemonic (_("_Keyterms"));
  gtk_widget_show (file_keyterms);
  gtk_container_add (GTK_CONTAINER (menuitem_file_menu), file_keyterms);

  image32231 = gtk_image_new_from_stock ("gtk-select-font", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image32231);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (file_keyterms), image32231);

  file_keyterms_menu = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (file_keyterms), file_keyterms_menu);

  keyterms_delete = gtk_image_menu_item_new_from_stock ("gtk-delete", NULL);
  gtk_widget_show (keyterms_delete);
  gtk_container_add (GTK_CONTAINER (file_keyterms_menu), keyterms_delete);

  print = NULL;

  if (guifeatures.printing()) {

    print = gtk_image_menu_item_new_from_stock("gtk-print", NULL);
    gtk_widget_show(print);
    gtk_container_add(GTK_CONTAINER(menuitem_file_menu), print);

  }

  file_import = gtk_image_menu_item_new_with_mnemonic (_("_Import"));
  gtk_widget_show (file_import);
  gtk_container_add (GTK_CONTAINER (menuitem_file_menu), file_import);

  image36797 = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image36797);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (file_import), image36797);

  file_export = gtk_image_menu_item_new_with_mnemonic (_("E_xport"));
  gtk_widget_show (file_export);
  gtk_container_add (GTK_CONTAINER (menuitem_file_menu), file_export);

  image35236 = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image35236);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (file_export), image35236);

  file_backup = gtk_image_menu_item_new_with_mnemonic (_("_Backup"));
  gtk_widget_show (file_backup);
  gtk_container_add (GTK_CONTAINER (menuitem_file_menu), file_backup);

  image34724 = gtk_image_new_from_stock ("gtk-copy", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image34724);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (file_backup), image34724);

  file_restore = gtk_image_menu_item_new_with_mnemonic (_("Res_tore"));
  gtk_widget_show (file_restore);
  gtk_container_add (GTK_CONTAINER (menuitem_file_menu), file_restore);

  image34980 = gtk_image_new_from_stock ("gtk-home", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image34980);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (file_restore), image34980);

  close1 = gtk_image_menu_item_new_from_stock("gtk-close", NULL);
  gtk_widget_show(close1);
  gtk_container_add(GTK_CONTAINER(menuitem_file_menu), close1);

  quit1 = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
  gtk_widget_show(quit1);
  gtk_container_add(GTK_CONTAINER(menuitem_file_menu), quit1);

  menuitem_edit = gtk_menu_item_new_with_mnemonic(_("_Edit"));
  gtk_widget_show(menuitem_edit);
  gtk_container_add(GTK_CONTAINER(menubar1), menuitem_edit);

  menuitem_edit_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_edit), menuitem_edit_menu);

  cut1 = gtk_image_menu_item_new_from_stock("gtk-cut", NULL);
  gtk_widget_show(cut1);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), cut1);

  copy1 = gtk_image_menu_item_new_from_stock("gtk-copy", NULL);
  gtk_widget_show(copy1);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), copy1);

  copy_with_formatting = gtk_image_menu_item_new_with_mnemonic(_("Copy _with formatting"));
  gtk_widget_show(copy_with_formatting);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), copy_with_formatting);

  image18220 = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18220);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(copy_with_formatting), image18220);

  paste1 = gtk_image_menu_item_new_from_stock("gtk-paste", NULL);
  gtk_widget_show(paste1);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), paste1);

  separator2 = gtk_separator_menu_item_new();
  gtk_widget_show(separator2);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator2);
  gtk_widget_set_sensitive(separator2, FALSE);

  undo1 = gtk_image_menu_item_new_with_mnemonic(_("_Undo"));
  gtk_widget_show(undo1);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), undo1);

  image295 = gtk_image_new_from_stock("gtk-undo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image295);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(undo1), image295);

  redo1 = gtk_image_menu_item_new_with_mnemonic(_("_Redo"));
  gtk_widget_show(redo1);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), redo1);

  image296 = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image296);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(redo1), image296);

  separator4 = gtk_separator_menu_item_new();
  gtk_widget_show(separator4);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator4);
  gtk_widget_set_sensitive(separator4, FALSE);

  find1 = NULL;
  if (guifeatures.references_and_find()) {

    find1 = gtk_image_menu_item_new_from_stock("gtk-find", NULL);
    gtk_widget_show(find1);
    gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), find1);

  }

  find_and_replace1 = NULL;
  if (guifeatures.replace()) {

    find_and_replace1 = gtk_image_menu_item_new_from_stock("gtk-find-and-replace", NULL);
    gtk_widget_show(find_and_replace1);
    gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), find_and_replace1);

  }

  find_in_notes1 = NULL;
  if (guifeatures.project_notes()) {

    find_in_notes1 = gtk_image_menu_item_new_with_mnemonic(_("Find in Project _notes"));
    gtk_widget_show(find_in_notes1);
    gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), find_in_notes1);

    image1430 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1430);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(find_in_notes1), image1430);

  }
  separator7 = gtk_separator_menu_item_new();
  gtk_widget_show(separator7);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator7);
  gtk_widget_set_sensitive(separator7, FALSE);

  get_references_from_note = NULL;
  if (guifeatures.project_notes()) {

    get_references_from_note = gtk_image_menu_item_new_with_mnemonic(_("_Get references from project note"));
    gtk_widget_show(get_references_from_note);
    gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), get_references_from_note);

    image3158 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3158);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(get_references_from_note), image3158);

  }

  separator15 = gtk_separator_menu_item_new();
  gtk_widget_show(separator15);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator15);
  gtk_widget_set_sensitive(separator15, FALSE);

  edit_revert = gtk_image_menu_item_new_with_mnemonic(_("Re_vert"));
  gtk_widget_show(edit_revert);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), edit_revert);

  image19262 = gtk_image_new_from_stock("gtk-go-back", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image19262);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(edit_revert), image19262);

  separator17 = gtk_separator_menu_item_new();
  gtk_widget_show(separator17);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator17);
  gtk_widget_set_sensitive(separator17, FALSE);

  edit_bible_note = gtk_image_menu_item_new_with_mnemonic(_("_Bible note"));
  gtk_widget_show(edit_bible_note);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), edit_bible_note);

  image20483 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20483);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(edit_bible_note), image20483);

  separator21 = gtk_separator_menu_item_new();
  gtk_widget_show(separator21);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), separator21);
  gtk_widget_set_sensitive(separator21, FALSE);

  edit_planning = gtk_image_menu_item_new_with_mnemonic(_("P_lanning"));
  gtk_widget_show(edit_planning);
  gtk_container_add(GTK_CONTAINER(menuitem_edit_menu), edit_planning);

  image26801 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image26801);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(edit_planning), image26801);

  menuitem_view = gtk_menu_item_new_with_mnemonic(_("_View"));
  gtk_widget_show(menuitem_view);
  gtk_container_add(GTK_CONTAINER(menubar1), menuitem_view);

  menuitem_view_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_view), menuitem_view_menu);

  view_font = gtk_image_menu_item_new_with_mnemonic(_("_Font & Color"));
  gtk_widget_show(view_font);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_font);

  image20234 = gtk_image_new_from_stock("gtk-select-font", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20234);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(view_font), image20234);

  view_font_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_font), view_font_menu);

  view_text_font = gtk_image_menu_item_new_with_mnemonic(_("_Text"));
  gtk_widget_show(view_text_font);
  gtk_container_add(GTK_CONTAINER(view_font_menu), view_text_font);

  image20235 = gtk_image_new_from_stock("gtk-select-font", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20235);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(view_text_font), image20235);

  viewnotes = NULL;
  if (guifeatures.project_notes()) {

    viewnotes = gtk_image_menu_item_new_with_mnemonic(_("Project _notes"));
    gtk_widget_show(viewnotes);
    gtk_container_add(GTK_CONTAINER(menuitem_view_menu), viewnotes);

    image2627 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2627);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(viewnotes), image2627);

  }

  //-----------------------------------------------------------------------------
  // View | Chapter as =>  _Formatted, _USFM code, or _Experimental
  view_chapter_item = gtk_menu_item_new_with_mnemonic(_("Chapter as"));
  gtk_widget_show(view_chapter_item);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_chapter_item);

  view_chapter_submenu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_chapter_item), view_chapter_submenu);
  // was gtk_check_menu...
  chapterViewGroup = NULL;
  view_formatted = (GtkRadioMenuItem*)gtk_radio_menu_item_new_with_mnemonic (chapterViewGroup, _("_Formatted"));
  //gtk3 gtk_radio_menu_item_join_group(view_formatted, NULL);
  chapterViewGroup = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (view_formatted));
  gtk_widget_show((GtkWidget*)view_formatted);
  gtk_container_add(GTK_CONTAINER(view_chapter_submenu), (GtkWidget*)view_formatted);
  gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_formatted), TRUE); // default

  view_usfm_code = (GtkRadioMenuItem*)gtk_radio_menu_item_new_with_mnemonic (chapterViewGroup, _("_USFM code"));
  //gtk3 gtk_radio_menu_item_join_group(view_usfm_code, view_formatted);
  chapterViewGroup = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (view_usfm_code));
  gtk_widget_show((GtkWidget*)view_usfm_code);
  gtk_container_add(GTK_CONTAINER(view_chapter_submenu), (GtkWidget*)view_usfm_code);

  view_experimental = (GtkRadioMenuItem*)gtk_radio_menu_item_new_with_mnemonic (chapterViewGroup, _("_Experimental"));
  //gtk3 gtk_radio_menu_item_join_group(view_experimental, view_usfm_code);
  chapterViewGroup = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (view_experimental));
  gtk_widget_show((GtkWidget*)view_experimental);
  gtk_container_add(GTK_CONTAINER(view_chapter_submenu), (GtkWidget*)view_experimental);
  //-----------------------------------------------------------------------------

  view_planning = gtk_image_menu_item_new_with_mnemonic(_("Pl_anning"));
  gtk_widget_show(view_planning);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_planning);

  image26812 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image26812);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(view_planning), image26812);

  view_references = gtk_check_menu_item_new_with_mnemonic (_("R_eferences"));
  gtk_widget_show (view_references);
  gtk_container_add (GTK_CONTAINER (menuitem_view_menu), view_references);

  view_related_verses = gtk_check_menu_item_new_with_mnemonic(_("_Related verses"));
  gtk_widget_show(view_related_verses);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_related_verses);

  view_outline = gtk_check_menu_item_new_with_mnemonic(_("_Outline"));
  gtk_widget_show(view_outline);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_outline);

  view_tile_windows = gtk_image_menu_item_new_with_mnemonic (_("_Tile Windows"));
  gtk_widget_show(view_tile_windows);
  gtk_container_add(GTK_CONTAINER(menuitem_view_menu), view_tile_windows);

  insert1 = gtk_menu_item_new_with_mnemonic(_("_Insert"));
  gtk_widget_show(insert1);
  gtk_container_add(GTK_CONTAINER(menubar1), insert1);

  insert1_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(insert1), insert1_menu);

  insert_special_character = gtk_image_menu_item_new_with_mnemonic(_("_Special character"));
  gtk_widget_show(insert_special_character);
  gtk_container_add(GTK_CONTAINER(insert1_menu), insert_special_character);

  GtkWidget *image25281;
  image25281 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image25281);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(insert_special_character), image25281);

  insert_footnote = gtk_image_menu_item_new_with_mnemonic(_("_Footnote"));
  gtk_widget_show(insert_footnote);
  gtk_container_add(GTK_CONTAINER(insert1_menu), insert_footnote);

  GtkWidget *imagefn = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  gtk_widget_show(imagefn);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(insert_footnote), imagefn);

  insert_xref = gtk_image_menu_item_new_with_mnemonic(_("_Crossreference"));
  gtk_widget_show(insert_xref);
  gtk_container_add(GTK_CONTAINER(insert1_menu), insert_xref);

  GtkWidget *imagexref = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  gtk_widget_show(imagexref);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(insert_xref), imagexref);

  check1 = NULL;
  chapters_and_verses1 = NULL;
  markers1 = NULL;
  markers1_menu = NULL;
  validate_usfms1 = NULL;
  count_usfms1 = NULL;
  compare_usfm1 = NULL;
  check_markers_spacing = NULL;
  characters1 = NULL;
  characters1_menu = NULL;
  count_characters = NULL;
  unwanted_patterns = NULL;
  check_words = NULL;
  check_words_menu = NULL;
  check_capitalization = NULL;
  check_repetition = NULL;
  unwanted_words = NULL;
  word_count_inventory = NULL;
  check_punctuation = NULL;
  check_punctuation_menu = NULL;
  check_matching_pairs = NULL;
  check_references = NULL;
  check_references_menu = NULL;
  check_references_inventory = NULL;
  check_references_validate = NULL;
  checks_passages = NULL;
  checks_passages_menu = NULL;
  check_nt_quotations_from_the_ot = NULL;
  synoptic_parallel_passages_from_the_nt = NULL;
  parallels_from_the_ot = NULL;
  check_key_terms = NULL;
  my_checks = NULL;
  check_usfm = NULL;
  check_spelling_error_next = NULL;
  check_spelling_error_previous = NULL;
  check_spelling_bulk = NULL;
  if (guifeatures.checks()) {

    check1 = gtk_menu_item_new_with_mnemonic(_("Chec_k"));
    gtk_widget_show(check1);
    gtk_container_add(GTK_CONTAINER(menubar1), check1);

    check1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(check1), check1_menu);

    chapters_and_verses1 = gtk_image_menu_item_new_with_mnemonic(_("_Chapters and verses"));
    gtk_widget_show(chapters_and_verses1);
    gtk_container_add(GTK_CONTAINER(check1_menu), chapters_and_verses1);

    image5580 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5580);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(chapters_and_verses1), image5580);

    markers1 = gtk_image_menu_item_new_with_mnemonic(_("_Markers"));
    gtk_widget_show(markers1);
    gtk_container_add(GTK_CONTAINER(check1_menu), markers1);

    image5578 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5578);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(markers1), image5578);

    markers1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(markers1), markers1_menu);

    validate_usfms1 = gtk_image_menu_item_new_with_mnemonic(_("_Validate"));
    gtk_widget_show(validate_usfms1);
    gtk_container_add(GTK_CONTAINER(markers1_menu), validate_usfms1);

    image5579 = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5579);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(validate_usfms1), image5579);

    count_usfms1 = gtk_image_menu_item_new_with_mnemonic(_("_Count"));
    gtk_widget_show(count_usfms1);
    gtk_container_add(GTK_CONTAINER(markers1_menu), count_usfms1);

    image6239 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6239);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(count_usfms1), image6239);

    compare_usfm1 = gtk_image_menu_item_new_with_mnemonic(_("C_ompare"));
    gtk_widget_show(compare_usfm1);
    gtk_container_add(GTK_CONTAINER(markers1_menu), compare_usfm1);

    image6748 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6748);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(compare_usfm1), image6748);

    check_markers_spacing = gtk_image_menu_item_new_with_mnemonic(_("_Spacing"));
    gtk_widget_show(check_markers_spacing);
    gtk_container_add(GTK_CONTAINER(markers1_menu), check_markers_spacing);

    image17930 = gtk_image_new_from_stock("gtk-media-pause", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image17930);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_markers_spacing), image17930);

    characters1 = gtk_image_menu_item_new_with_mnemonic(_("C_haracters"));
    gtk_widget_show(characters1);
    gtk_container_add(GTK_CONTAINER(check1_menu), characters1);

    image6867 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6867);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(characters1), image6867);

    characters1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(characters1), characters1_menu);

    count_characters = gtk_image_menu_item_new_with_mnemonic(_("_Inventory"));
    gtk_widget_show(count_characters);
    gtk_container_add(GTK_CONTAINER(characters1_menu), count_characters);

    image6868 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6868);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(count_characters), image6868);

    unwanted_patterns = gtk_image_menu_item_new_with_mnemonic(_("_Unwanted patterns"));
    gtk_widget_show(unwanted_patterns);
    gtk_container_add(GTK_CONTAINER(characters1_menu), unwanted_patterns);

    image7494 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7494);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(unwanted_patterns), image7494);

    check_words = gtk_image_menu_item_new_with_mnemonic(_("_Words"));
    gtk_widget_show(check_words);
    gtk_container_add(GTK_CONTAINER(check1_menu), check_words);

    image7111 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7111);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_words), image7111);

    check_words_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(check_words), check_words_menu);

    check_capitalization = gtk_image_menu_item_new_with_mnemonic(_("_Capitalization"));
    gtk_widget_show(check_capitalization);
    gtk_container_add(GTK_CONTAINER(check_words_menu), check_capitalization);

    image7112 = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7112);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_capitalization), image7112);

    check_repetition = gtk_image_menu_item_new_with_mnemonic(_("_Repetition"));
    gtk_widget_show(check_repetition);
    gtk_container_add(GTK_CONTAINER(check_words_menu), check_repetition);

    image7238 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7238);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_repetition), image7238);

    unwanted_words = gtk_image_menu_item_new_with_mnemonic(_("_Unwanted"));
    gtk_widget_show(unwanted_words);
    gtk_container_add(GTK_CONTAINER(check_words_menu), unwanted_words);

    image7631 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7631);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(unwanted_words), image7631);

    word_count_inventory = gtk_image_menu_item_new_with_mnemonic(_("_Inventory"));
    gtk_widget_show(word_count_inventory);
    gtk_container_add(GTK_CONTAINER(check_words_menu), word_count_inventory);

    image13715 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image13715);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(word_count_inventory), image13715);

    check_punctuation = gtk_image_menu_item_new_with_mnemonic(_("_Punctuation"));
    gtk_widget_show(check_punctuation);
    gtk_container_add(GTK_CONTAINER(check1_menu), check_punctuation);

    image7366 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7366);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_punctuation), image7366);

    check_punctuation_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(check_punctuation), check_punctuation_menu);

    check_matching_pairs = gtk_image_menu_item_new_with_mnemonic(_("_Matching pairs"));
    gtk_widget_show(check_matching_pairs);
    gtk_container_add(GTK_CONTAINER(check_punctuation_menu), check_matching_pairs);

    image7367 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7367);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_matching_pairs), image7367);

    check_references = gtk_image_menu_item_new_with_mnemonic(_("_References"));
    gtk_widget_show(check_references);
    gtk_container_add(GTK_CONTAINER(check1_menu), check_references);

    image21826 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21826);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_references), image21826);

    check_references_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(check_references), check_references_menu);

    check_references_inventory = gtk_image_menu_item_new_with_mnemonic(_("_Inventory"));
    gtk_widget_show(check_references_inventory);
    gtk_container_add(GTK_CONTAINER(check_references_menu), check_references_inventory);

    image21827 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21827);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_references_inventory), image21827);

    check_references_validate = gtk_image_menu_item_new_with_mnemonic(_("_Validate"));
    gtk_widget_show(check_references_validate);
    gtk_container_add(GTK_CONTAINER(check_references_menu), check_references_validate);

    image21828 = gtk_image_new_from_stock("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21828);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_references_validate), image21828);

    checks_passages = gtk_image_menu_item_new_with_mnemonic(_("P_assages"));
    gtk_widget_show(checks_passages);
    gtk_container_add(GTK_CONTAINER(check1_menu), checks_passages);

    image24103 = gtk_image_new_from_stock("gtk-justify-fill", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24103);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(checks_passages), image24103);

    checks_passages_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(checks_passages), checks_passages_menu);

    check_nt_quotations_from_the_ot = gtk_image_menu_item_new_with_mnemonic(_("_OT quotations in the NT"));
    gtk_widget_show(check_nt_quotations_from_the_ot);
    gtk_container_add(GTK_CONTAINER(checks_passages_menu), check_nt_quotations_from_the_ot);

    image24104 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24104);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(check_nt_quotations_from_the_ot), image24104);

    synoptic_parallel_passages_from_the_nt = gtk_image_menu_item_new_with_mnemonic(_("_Synoptic parallels in the NT"));
    gtk_widget_show(synoptic_parallel_passages_from_the_nt);
    gtk_container_add(GTK_CONTAINER(checks_passages_menu), synoptic_parallel_passages_from_the_nt);

    image24105 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24105);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(synoptic_parallel_passages_from_the_nt), image24105);

    parallels_from_the_ot = gtk_image_menu_item_new_with_mnemonic(_("_Parallels in the OT"));
    gtk_widget_show(parallels_from_the_ot);
    gtk_container_add(GTK_CONTAINER(checks_passages_menu), parallels_from_the_ot);

    image24106 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24106);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(parallels_from_the_ot), image24106);

    check_key_terms = gtk_check_menu_item_new_with_mnemonic(_("_Key terms"));
    gtk_widget_show(check_key_terms);
    gtk_container_add(GTK_CONTAINER(check1_menu), check_key_terms);

    my_checks = gtk_image_menu_item_new_with_mnemonic(_("M_y checks"));
    gtk_widget_show(my_checks);
    gtk_container_add(GTK_CONTAINER(check1_menu), my_checks);

    image15438 = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image15438);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(my_checks), image15438);

    check_usfm = gtk_check_menu_item_new_with_mnemonic (_("_USFM"));
    gtk_widget_show (check_usfm);
    gtk_container_add (GTK_CONTAINER (check1_menu), check_usfm);

    check_spelling_error = gtk_image_menu_item_new_with_mnemonic (_("_Spelling error"));
    gtk_widget_show (check_spelling_error);
    gtk_container_add (GTK_CONTAINER (check1_menu), check_spelling_error);

    image34138 = gtk_image_new_from_stock ("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image34138);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (check_spelling_error), image34138);

    check_spelling_error_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (check_spelling_error), check_spelling_error_menu);

    check_spelling_error_next = gtk_image_menu_item_new_with_mnemonic (_("_Next"));
    gtk_widget_show (check_spelling_error_next);
    gtk_container_add (GTK_CONTAINER (check_spelling_error_menu), check_spelling_error_next);

    image34139 = gtk_image_new_from_stock ("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image34139);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (check_spelling_error_next), image34139);

    check_spelling_error_previous = gtk_image_menu_item_new_with_mnemonic (_("_Previous"));
    gtk_widget_show (check_spelling_error_previous);
    gtk_container_add (GTK_CONTAINER (check_spelling_error_menu), check_spelling_error_previous);

    image34140 = gtk_image_new_from_stock ("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image34140);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (check_spelling_error_previous), image34140);

    check_spelling_bulk = gtk_image_menu_item_new_with_mnemonic (_("_Bulk"));
    gtk_widget_show (check_spelling_bulk);
    gtk_container_add (GTK_CONTAINER (check_spelling_error_menu), check_spelling_bulk);

    image35887 = gtk_image_new_from_stock ("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image35887);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (check_spelling_bulk), image35887);

  }

  menutools = gtk_menu_item_new_with_mnemonic(_("_Tools"));
  gtk_widget_show(menutools);
  gtk_container_add(GTK_CONTAINER(menubar1), menutools);

  menutools_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menutools), menutools_menu);

  notes_transfer = NULL;
  tool_origin_references_in_bible_notes = NULL;
  tool_project_notes_mass_update1 = NULL;
  tool_generate_word_lists = NULL;
  tool_simple_text_corrections = NULL;
  tool_transfer_project_notes_to_text = NULL;
  if (guifeatures.tools()) {

    notes_transfer = gtk_image_menu_item_new_with_mnemonic(_("_Transfer text to project notes"));
    gtk_widget_show(notes_transfer);
    gtk_container_add(GTK_CONTAINER(menutools_menu), notes_transfer);

    image14659 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image14659);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(notes_transfer), image14659);

    tool_origin_references_in_bible_notes = gtk_image_menu_item_new_with_mnemonic(_("_Bible notes mass update"));
    gtk_widget_show(tool_origin_references_in_bible_notes);
    gtk_container_add(GTK_CONTAINER(menutools_menu), tool_origin_references_in_bible_notes);

    image16248 = gtk_image_new_from_stock("gtk-find-and-replace", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image16248);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(tool_origin_references_in_bible_notes), image16248);

    tool_project_notes_mass_update1 = gtk_image_menu_item_new_with_mnemonic(_("_Project notes mass update"));
    gtk_widget_show(tool_project_notes_mass_update1);
    gtk_container_add(GTK_CONTAINER(menutools_menu), tool_project_notes_mass_update1);

    image17187 = gtk_image_new_from_stock("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image17187);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(tool_project_notes_mass_update1), image17187);

    tool_generate_word_lists = gtk_image_menu_item_new_with_mnemonic(_("_Generate word lists"));
    gtk_widget_show(tool_generate_word_lists);
    gtk_container_add(GTK_CONTAINER(menutools_menu), tool_generate_word_lists);

    image20671 = gtk_image_new_from_stock("gtk-justify-fill", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image20671);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(tool_generate_word_lists), image20671);

    tool_simple_text_corrections = gtk_image_menu_item_new_with_mnemonic(_("_Simple text corrections"));
    gtk_widget_show(tool_simple_text_corrections);
    gtk_container_add(GTK_CONTAINER(menutools_menu), tool_simple_text_corrections);

    image21054 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21054);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(tool_simple_text_corrections), image21054);

    tool_transfer_project_notes_to_text = gtk_image_menu_item_new_with_mnemonic(_("Tr_ansfer project notes to text"));
    gtk_widget_show(tool_transfer_project_notes_to_text);
    gtk_container_add(GTK_CONTAINER(menutools_menu), tool_transfer_project_notes_to_text);

    image29089 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image29089);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(tool_transfer_project_notes_to_text), image29089);

  }

  tool_go_to_reference = gtk_image_menu_item_new_with_mnemonic (_("G_o to reference (Ctrl-G)"));
  gtk_widget_show (tool_go_to_reference);
  gtk_container_add (GTK_CONTAINER (menutools_menu), tool_go_to_reference);

  image36137 = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image36137);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tool_go_to_reference), image36137);

  tools_maintenance = gtk_image_menu_item_new_with_mnemonic (_("_Maintenance"));
  gtk_widget_show (tools_maintenance);
  gtk_container_add (GTK_CONTAINER (menutools_menu), tools_maintenance);

  image36259 = gtk_image_new_from_stock ("gtk-preferences", GTK_ICON_SIZE_MENU);
  gtk_widget_show (image36259);
  gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tools_maintenance), image36259);

  menuitem_preferences = gtk_menu_item_new_with_mnemonic(_("P_references"));
  // At first the Alt-P was the accelerator. On the XO machine, this key is 
  // in use already: 
  // Alt-N goes to the Next Activity and
  // Alt-P goes to the Previous one. 
  // For that reason the Alt-R is now the accelerator.
  gtk_widget_show(menuitem_preferences);
  gtk_container_add(GTK_CONTAINER(menubar1), menuitem_preferences);

  menuitem_preferences_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_preferences), menuitem_preferences_menu);

  notes_preferences = NULL;
  printingprefs = NULL;
  reference_exchange1 = NULL;
  prefs_books = NULL;
  preferences_windows_outpost = NULL;
  preferences_tidy_text = NULL;
  preferences_remote_repository = NULL;
  if (guifeatures.preferences()) {

    notes_preferences = gtk_image_menu_item_new_with_mnemonic(_("Project _notes"));
    gtk_widget_show(notes_preferences);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), notes_preferences);

    image2116 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2116);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(notes_preferences), image2116);

    printingprefs = gtk_image_menu_item_new_with_mnemonic(_("_Printing"));
    gtk_widget_show(printingprefs);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), printingprefs);

    image3493 = gtk_image_new_from_stock("gtk-print", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3493);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(printingprefs), image3493);

    reference_exchange1 = gtk_image_menu_item_new_with_mnemonic(_("_Reference exchange"));
    gtk_widget_show(reference_exchange1);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), reference_exchange1);

    image5972 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5972);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(reference_exchange1), image5972);

    prefs_books = gtk_image_menu_item_new_with_mnemonic(_("_Books"));
    gtk_widget_show(prefs_books);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), prefs_books);

    image12167 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image12167);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(prefs_books), image12167);

    preferences_windows_outpost = gtk_image_menu_item_new_with_mnemonic(_("_Windows Outpost"));
    gtk_widget_show(preferences_windows_outpost);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_windows_outpost);

    image14287 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image14287);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_windows_outpost), image14287);

    preferences_tidy_text = gtk_image_menu_item_new_with_mnemonic(_("_Tidy text"));
    gtk_widget_show(preferences_tidy_text);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_tidy_text);

    image16359 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image16359);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_tidy_text), image16359);

    preferences_remote_repository = gtk_image_menu_item_new_with_mnemonic (_("R_emote repository"));
    gtk_widget_show (preferences_remote_repository);
    gtk_container_add (GTK_CONTAINER (menuitem_preferences_menu), preferences_remote_repository);

    image33654 = gtk_image_new_from_stock ("gtk-connect", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image33654);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (preferences_remote_repository), image33654);

  }

  preferences_gui = gtk_image_menu_item_new_with_mnemonic(_("_User interface"));
  gtk_widget_show(preferences_gui);
  gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_gui);

  image20936 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20936);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_gui), image20936);

  preferences_planning = NULL;
  preferences_password = NULL;
  preferences_text_replacement = NULL;
  pdf_viewer1 = NULL;
  preferences_planning = NULL;
  preferences_filters = NULL;
  preferences_compare = NULL;
  if (guifeatures.preferences()) {

    preferences_password = gtk_image_menu_item_new_with_mnemonic(_("P_assword"));
    gtk_widget_show(preferences_password);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_password);

    image20937 = gtk_image_new_from_stock("gtk-dialog-authentication", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image20937);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_password), image20937);

    preferences_text_replacement = gtk_image_menu_item_new_with_mnemonic(_("Te_xt replacement"));
    gtk_widget_show(preferences_text_replacement);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_text_replacement);

    image23181 = gtk_image_new_from_stock("gtk-find-and-replace", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image23181);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_text_replacement), image23181);

    pdf_viewer1 = gtk_image_menu_item_new_with_mnemonic(_("P_DF Viewer"));
    gtk_widget_show(pdf_viewer1);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), pdf_viewer1);

    image24540 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24540);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(pdf_viewer1), image24540);

    preferences_planning = gtk_image_menu_item_new_with_mnemonic(_("P_lanning"));
    gtk_widget_show(preferences_planning);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_planning);

    image26888 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image26888);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_planning), image26888);

    preferences_filters = gtk_image_menu_item_new_with_mnemonic(_("Filter_s"));
    gtk_widget_show(preferences_filters);
    gtk_container_add(GTK_CONTAINER(menuitem_preferences_menu), preferences_filters);

    image28360 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image28360);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(preferences_filters), image28360);

    preferences_compare = gtk_image_menu_item_new_with_mnemonic (_("_Compare"));
    gtk_widget_show (preferences_compare);
    gtk_container_add (GTK_CONTAINER (menuitem_preferences_menu), preferences_compare);

    image32676 = gtk_image_new_from_stock ("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image32676);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (preferences_compare), image32676);

  }

  menuitem_help = gtk_menu_item_new_with_mnemonic(_("_Help"));
  gtk_widget_show(menuitem_help);
  gtk_container_add(GTK_CONTAINER(menubar1), menuitem_help);

  menuitem_help_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(menuitem_help), menuitem_help_menu);

  help_main = gtk_image_menu_item_new_with_mnemonic(_("_Contents"));
  gtk_widget_show(help_main);
  gtk_container_add(GTK_CONTAINER(menuitem_help_menu), help_main);

  image17520 = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image17520);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(help_main), image17520);

  syslogdialog = NULL;
  system_log1 = gtk_image_menu_item_new_with_mnemonic(_("_System log"));
  gtk_widget_show(system_log1);
  gtk_container_add(GTK_CONTAINER(menuitem_help_menu), system_log1);

  image4388 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image4388);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(system_log1), image4388);

  about1 = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
  gtk_widget_show(about1);
  gtk_container_add(GTK_CONTAINER(menuitem_help_menu), about1);

  toolbar = gtk_toolbar_new();
  gtk_widget_show(toolbar);
  gtk_box_pack_start(GTK_BOX(vbox_main), toolbar, FALSE, FALSE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR(toolbar), GTK_TOOLBAR_BOTH_HORIZ);

  scrolledwindow_layout = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow_layout);
  gtk_box_pack_start (GTK_BOX (vbox_main), scrolledwindow_layout, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow_layout), GTK_POLICY_NEVER, GTK_POLICY_NEVER);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow_layout), GTK_SHADOW_IN);

  layout = gtk_layout_new (NULL, NULL);
  gtk_widget_show (layout);
  gtk_container_add (GTK_CONTAINER (scrolledwindow_layout), layout);
  // The real size of the layout will be set once space has been allocated to the parent scrolled window
  gtk_layout_set_size (GTK_LAYOUT (layout), 10, 10);
  GtkAdjustment * hadjustment = gtk_layout_get_hadjustment (GTK_LAYOUT (layout));
  gtk_adjustment_set_step_increment (hadjustment, 10);
  GtkAdjustment * vadjustment = gtk_layout_get_vadjustment (GTK_LAYOUT (layout));
  gtk_adjustment_set_step_increment (vadjustment, 10);

  // Menu callbacks.
  if (new1)
    g_signal_connect((gpointer) new1, "activate", G_CALLBACK(on_new1_activate), gpointer(this));
  if (open1)
    g_signal_connect((gpointer) open1, "activate", G_CALLBACK(on_open1_activate), gpointer(this));
  if (delete1)
    g_signal_connect((gpointer) delete1, "activate", G_CALLBACK(on_delete1_activate), gpointer(this));
  if (properties1)
    g_signal_connect((gpointer) properties1, "activate", G_CALLBACK(on_properties1_activate), gpointer(this));
  if (copy_project_to)
    g_signal_connect((gpointer) copy_project_to, "activate", G_CALLBACK(on_copy_project_to_activate), gpointer(this));
  if (compare_with1)
    g_signal_connect((gpointer) compare_with1, "activate", G_CALLBACK(on_compare_with1_activate), gpointer(this));
  if (project_changes)
    g_signal_connect((gpointer) project_changes, "activate", G_CALLBACK(on_project_changes_activate), gpointer(this));
  if (file_projects_merge)
    g_signal_connect((gpointer) file_projects_merge, "activate", G_CALLBACK(on_file_projects_merge_activate), gpointer(this));
  g_signal_connect ((gpointer) projects_send_receive1, "activate", G_CALLBACK (on_projects_send_receive1_activate), gpointer(this));
  if (stylesheet_open)
    g_signal_connect((gpointer) stylesheet_open, "activate", G_CALLBACK(on_stylesheet_open_activate), gpointer(this));
  if (new_note)
    g_signal_connect((gpointer) new_note, "activate", G_CALLBACK(on_new_note_activate), gpointer(this));
  if (delete_note)
    g_signal_connect((gpointer) delete_note, "activate", G_CALLBACK(on_delete_note_activate), gpointer(this));
  if (consultation_notes_send_receive)
    g_signal_connect((gpointer) consultation_notes_send_receive, "activate", G_CALLBACK(on_consultant_notes_send_receive_activate), gpointer(this));
  if (file_resources)
    g_signal_connect((gpointer) file_resources, "activate", G_CALLBACK(on_file_resources_activate), gpointer(this));
  if (file_resources_open)
    g_signal_connect((gpointer) file_resources_open, "activate", G_CALLBACK(on_file_resources_open_activate), gpointer(this));
  if (file_resources_close)
    g_signal_connect((gpointer) file_resources_close, "activate", G_CALLBACK(on_file_resources_close_activate), gpointer(this));
  if (file_resources_new)
    g_signal_connect((gpointer) file_resources_new, "activate", G_CALLBACK(on_file_resources_new_activate), gpointer(this));
  if (file_resources_edit)
    g_signal_connect((gpointer) file_resources_edit, "activate", G_CALLBACK(on_file_resources_edit_activate), gpointer(this));
  if (file_resources_delete)
    g_signal_connect((gpointer) file_resources_delete, "activate", G_CALLBACK(on_file_resources_delete_activate), gpointer(this));
  if (keyterms_delete)
    g_signal_connect((gpointer) keyterms_delete, "activate", G_CALLBACK(on_keyterms_delete_activate), gpointer(this));
  if (print)
    g_signal_connect((gpointer) print, "activate", G_CALLBACK(on_print_activate), gpointer(this));
  g_signal_connect ((gpointer) file_import, "activate", G_CALLBACK (on_file_import_activate), gpointer(this));
  g_signal_connect ((gpointer) file_export, "activate", G_CALLBACK (on_file_export_activate), gpointer(this));
  g_signal_connect ((gpointer) file_backup, "activate", G_CALLBACK (on_file_backup_activate), gpointer(this));
  g_signal_connect ((gpointer) file_restore, "activate", G_CALLBACK (on_file_restore_activate), gpointer(this));
  if (close1)
    g_signal_connect((gpointer) close1, "activate", G_CALLBACK(on_close1_activate), gpointer(this));
  if (quit1)
    g_signal_connect((gpointer) quit1, "activate", G_CALLBACK(on_quit1_activate), gpointer(this));
  if (menuitem_edit)
    g_signal_connect((gpointer) menuitem_edit, "activate", G_CALLBACK(on_edit1_activate), gpointer(this));
  if (cut1)
    g_signal_connect((gpointer) cut1, "activate", G_CALLBACK(on_cut1_activate), gpointer(this));
  if (copy1)
    g_signal_connect((gpointer) copy1, "activate", G_CALLBACK(on_copy1_activate), gpointer(this));
  if (copy_with_formatting)
    g_signal_connect((gpointer) copy_with_formatting, "activate", G_CALLBACK(on_copy_with_formatting_activate), gpointer(this));
  if (paste1)
    g_signal_connect((gpointer) paste1, "activate", G_CALLBACK(on_paste1_activate), gpointer(this));
  if (undo1)
    g_signal_connect((gpointer) undo1, "activate", G_CALLBACK(on_undo1_activate), gpointer(this));
  if (redo1)
    g_signal_connect((gpointer) redo1, "activate", G_CALLBACK(on_redo1_activate), gpointer(this));
  if (find1)
    g_signal_connect((gpointer) find1, "activate", G_CALLBACK(on_findspecial1_activate), gpointer(this));
  if (find_and_replace1)
    g_signal_connect((gpointer) find_and_replace1, "activate", G_CALLBACK(on_find_and_replace1_activate), gpointer(this));
  if (find_in_notes1)
    g_signal_connect((gpointer) find_in_notes1, "activate", G_CALLBACK(on_find_in_notes1_activate), gpointer(this));
  if (get_references_from_note)
    g_signal_connect((gpointer) get_references_from_note, "activate", G_CALLBACK(on_get_references_from_note_activate), gpointer(this));
  if (edit_revert)
    g_signal_connect((gpointer) edit_revert, "activate", G_CALLBACK(on_edit_revert_activate), gpointer(this));
  if (edit_bible_note)
    g_signal_connect((gpointer) edit_bible_note, "activate", G_CALLBACK(on_edit_bible_note_activate), gpointer(this));
  if (edit_planning)
    g_signal_connect((gpointer) edit_planning, "activate", G_CALLBACK(on_edit_planning_activate), gpointer(this));
  if (menuitem_view)
    g_signal_connect((gpointer) menuitem_view, "activate", G_CALLBACK(on_menuitem_view_activate), gpointer(this));
  if (view_text_font)
    g_signal_connect((gpointer) view_text_font, "activate", G_CALLBACK(on_view_text_font_activate), gpointer(this));
  if (viewnotes)
    g_signal_connect((gpointer) viewnotes, "activate", G_CALLBACK(on_viewnotes_activate), gpointer(this));
  if (view_formatted) {
    g_signal_connect((gpointer) view_formatted, "activate", G_CALLBACK(on_view_chapteras_activate), gpointer(this));
    g_signal_connect((gpointer) view_formatted, "toggled", G_CALLBACK(on_view_formatted_toggled), gpointer(this));
  }
  if (view_usfm_code) {
    g_signal_connect((gpointer) view_usfm_code, "activate", G_CALLBACK(on_view_chapteras_activate), gpointer(this));
    g_signal_connect((gpointer) view_usfm_code, "toggled", G_CALLBACK(on_view_usfm_code_toggled), gpointer(this));
  }
  if (view_experimental) {
    g_signal_connect((gpointer) view_experimental, "activate", G_CALLBACK(on_view_chapteras_activate), gpointer(this));
    g_signal_connect((gpointer) view_experimental, "toggled", G_CALLBACK(on_view_experimental_toggled), gpointer(this));
  }

  if (view_planning)
    g_signal_connect((gpointer) view_planning, "activate", G_CALLBACK(on_view_planning_activate), gpointer(this));
  if (view_related_verses)
    g_signal_connect((gpointer) view_related_verses, "activate", G_CALLBACK(on_view_related_verses_activate), gpointer(this));
  g_signal_connect ((gpointer) view_references, "activate", G_CALLBACK (on_view_references_activate), gpointer(this));
  if (view_outline)
    g_signal_connect((gpointer) view_outline, "activate", G_CALLBACK(on_view_outline_activate), gpointer(this));
  g_signal_connect((gpointer) view_tile_windows, "activate", G_CALLBACK(on_view_tile_windows_activate), gpointer (this));
  if (insert1)
    g_signal_connect((gpointer) insert1, "activate", G_CALLBACK(on_insert1_activate), gpointer(this));
  if (insert_special_character)
    g_signal_connect((gpointer) insert_special_character, "activate", G_CALLBACK(on_insert_special_character_activate), gpointer(this));
  if (insert_footnote)
    g_signal_connect((gpointer) insert_footnote, "activate", G_CALLBACK(on_insert_footnote_activate), gpointer(this));
  if (insert_xref)
    g_signal_connect((gpointer) insert_xref, "activate", G_CALLBACK(on_insert_xref_activate), gpointer(this));
  if (validate_usfms1)
    g_signal_connect((gpointer) validate_usfms1, "activate", G_CALLBACK(on_validate_usfms1_activate), gpointer(this));
  if (count_usfms1)
    g_signal_connect((gpointer) count_usfms1, "activate", G_CALLBACK(on_count_usfms1_activate), gpointer(this));
  if (compare_usfm1)
    g_signal_connect((gpointer) compare_usfm1, "activate", G_CALLBACK(on_compare_usfm1_activate), gpointer(this));
  if (check_markers_spacing)
    g_signal_connect((gpointer) check_markers_spacing, "activate", G_CALLBACK(on_check_markers_spacing_activate), gpointer(this));
  if (chapters_and_verses1)
    g_signal_connect((gpointer) chapters_and_verses1, "activate", G_CALLBACK(on_chapters_and_verses1_activate), gpointer(this));
  if (count_characters)
    g_signal_connect((gpointer) count_characters, "activate", G_CALLBACK(on_count_characters_activate), gpointer(this));
  if (unwanted_patterns)
    g_signal_connect((gpointer) unwanted_patterns, "activate", G_CALLBACK(on_unwanted_patterns_activate), gpointer(this));
  if (check_capitalization)
    g_signal_connect((gpointer) check_capitalization, "activate", G_CALLBACK(on_check_capitalization_activate), gpointer(this));
  if (check_repetition)
    g_signal_connect((gpointer) check_repetition, "activate", G_CALLBACK(on_check_repetition_activate), gpointer(this));
  if (unwanted_words)
    g_signal_connect((gpointer) unwanted_words, "activate", G_CALLBACK(on_unwanted_words_activate), gpointer(this));
  if (check_matching_pairs)
    g_signal_connect((gpointer) check_matching_pairs, "activate", G_CALLBACK(on_check_matching_pairs_activate), gpointer(this));
  if (word_count_inventory)
    g_signal_connect((gpointer) word_count_inventory, "activate", G_CALLBACK(on_word_count_inventory_activate), gpointer(this));
  if (check_references_inventory)
    g_signal_connect((gpointer) check_references_inventory, "activate", G_CALLBACK(on_check_references_inventory_activate), gpointer(this));
  if (check_references_validate)
    g_signal_connect((gpointer) check_references_validate, "activate", G_CALLBACK(on_check_references_validate_activate), gpointer(this));
  if (check_nt_quotations_from_the_ot)
    g_signal_connect((gpointer) check_nt_quotations_from_the_ot, "activate", G_CALLBACK(on_check_nt_quotations_from_the_ot_activate), gpointer(this));
  if (synoptic_parallel_passages_from_the_nt)
    g_signal_connect((gpointer) synoptic_parallel_passages_from_the_nt, "activate", G_CALLBACK(on_synoptic_parallel_passages_from_the_nt_activate), gpointer(this));
  if (parallels_from_the_ot)
    g_signal_connect((gpointer) parallels_from_the_ot, "activate", G_CALLBACK(on_parallels_from_the_ot_activate), gpointer(this));
  if (check_key_terms)
    g_signal_connect((gpointer) check_key_terms, "activate", G_CALLBACK(on_check_key_terms_activate), gpointer(this));
  if (my_checks)
    g_signal_connect((gpointer) my_checks, "activate", G_CALLBACK(on_my_checks_activate), gpointer(this));
  if (check_usfm)
    g_signal_connect ((gpointer) check_usfm, "activate", G_CALLBACK (on_check_usfm_activate), gpointer(this));
  if (check_spelling_error_next)
    g_signal_connect ((gpointer) check_spelling_error_next, "activate", G_CALLBACK (on_check_spelling_error_next_activate), gpointer(this));
  if (check_spelling_error_previous)
    g_signal_connect ((gpointer) check_spelling_error_previous, "activate", G_CALLBACK (on_check_spelling_error_previous_activate), gpointer(this));
  if (check_spelling_bulk)
    g_signal_connect ((gpointer) check_spelling_bulk, "activate", G_CALLBACK (on_check_spelling_bulk_activate), gpointer(this));
  if (menutools)
    g_signal_connect((gpointer) menutools, "activate", G_CALLBACK(on_menutools_activate), gpointer(this));
  if (notes_transfer)
    g_signal_connect((gpointer) notes_transfer, "activate", G_CALLBACK(on_notes_transfer_activate), gpointer(this));
  if (tool_origin_references_in_bible_notes)
    g_signal_connect((gpointer) tool_origin_references_in_bible_notes, "activate", G_CALLBACK(on_tool_origin_references_in_bible_notes_activate), gpointer(this));
  if (tool_project_notes_mass_update1)
    g_signal_connect((gpointer) tool_project_notes_mass_update1, "activate", G_CALLBACK(on_tool_project_notes_mass_update1_activate), gpointer(this));
  if (tool_generate_word_lists)
    g_signal_connect((gpointer) tool_generate_word_lists, "activate", G_CALLBACK(on_tool_generate_word_lists_activate), gpointer(this));
  if (tool_simple_text_corrections)
    g_signal_connect((gpointer) tool_simple_text_corrections, "activate", G_CALLBACK(on_tool_simple_text_corrections_activate), gpointer(this));
  if (tool_transfer_project_notes_to_text)
    g_signal_connect((gpointer) tool_transfer_project_notes_to_text, "activate", G_CALLBACK(on_tool_transfer_project_notes_to_text_activate), gpointer(this));
  g_signal_connect ((gpointer) tool_go_to_reference, "activate", G_CALLBACK (on_tool_go_to_reference_activate), gpointer (this));
  g_signal_connect ((gpointer) tools_maintenance, "activate", G_CALLBACK (on_tools_maintenance_activate), gpointer (this));
  if (notes_preferences)
    g_signal_connect((gpointer) notes_preferences, "activate", G_CALLBACK(on_notes_preferences_activate), gpointer(this));
  if (printingprefs)
    g_signal_connect((gpointer) printingprefs, "activate", G_CALLBACK(on_printingprefs_activate), gpointer(this));
  if (reference_exchange1)
    g_signal_connect((gpointer) reference_exchange1, "activate", G_CALLBACK(on_reference_exchange1_activate), gpointer(this));
  if (prefs_books)
    g_signal_connect((gpointer) prefs_books, "activate", G_CALLBACK(on_prefs_books_activate), gpointer(this));
  if (preferences_windows_outpost)
    g_signal_connect((gpointer) preferences_windows_outpost, "activate", G_CALLBACK(on_preferences_windows_outpost_activate), gpointer(this));
  if (preferences_tidy_text)
    g_signal_connect((gpointer) preferences_tidy_text, "activate", G_CALLBACK(on_preferences_tidy_text_activate), gpointer(this));
  if (preferences_remote_repository)
    g_signal_connect((gpointer) preferences_remote_repository, "activate", G_CALLBACK(on_preferences_remote_repository_activate), gpointer(this));
  if (preferences_gui)
    g_signal_connect((gpointer) preferences_gui, "activate", G_CALLBACK(on_preferences_gui_activate), gpointer(this));
  if (preferences_password)
    g_signal_connect((gpointer) preferences_password, "activate", G_CALLBACK(on_preferences_password_activate), gpointer(this));
  if (preferences_text_replacement)
    g_signal_connect((gpointer) preferences_text_replacement, "activate", G_CALLBACK(on_preferences_text_replacement_activate), gpointer(this));
  if (pdf_viewer1)
    g_signal_connect((gpointer) pdf_viewer1, "activate", G_CALLBACK(on_pdf_viewer1_activate), gpointer(this));
  if (preferences_planning)
    g_signal_connect((gpointer) preferences_planning, "activate", G_CALLBACK(on_preferences_planning_activate), gpointer(this));
  if (preferences_filters)
    g_signal_connect((gpointer) preferences_filters, "activate", G_CALLBACK(on_preferences_filters_activate), gpointer(this));
  if (preferences_compare)
    g_signal_connect((gpointer) preferences_compare, "activate", G_CALLBACK(on_preferences_compare_activate), gpointer(this));
  if (help_main)
    g_signal_connect((gpointer) help_main, "activate", G_CALLBACK(on_help_main_activate), gpointer(this));
  if (system_log1)
    g_signal_connect((gpointer) system_log1, "activate", G_CALLBACK(on_system_log1_activate), gpointer(this));
  if (about1)
    g_signal_connect((gpointer) about1, "activate", G_CALLBACK(on_about1_activate), gpointer(this));
  g_signal_connect ((gpointer) scrolledwindow_layout, "size_allocate",  G_CALLBACK (on_scrolledwindow_layout_size_allocate), gpointer (this));
  navigation.build(toolbar);
  g_signal_connect((gpointer) navigation.new_reference_signal, "clicked", G_CALLBACK(on_navigation_new_reference_clicked), gpointer(this));

  // Start Outpost.
  windowsoutpost = new WindowsOutpost(true);
#ifdef WIN32  
  if (settings->genconfig.use_outpost_get())
    windowsoutpost->Start();
#endif

  // Store project of last session because it gets affected when the editors build.
  focused_project_last_session = settings->genconfig.project_get();

  // Appearance of text in editor.
  set_fonts();



  // If the windows are attached, apply the dimensions with a delay.
  // This delay will also take care of object destruction.
  dimensions->apply();

#ifndef WIN32
  // Signal handling.
  // Block the signal of a pipe error that otherwise would kill bibledit.
  signal(SIGPIPE, SIG_IGN);
  // USR1, ignore it.
  signal(SIGUSR1, SIG_IGN);
#endif

  // Display project notes.
  notes_redisplay ();

  // Start bibledit http responder.
  g_timeout_add (300, GSourceFunc(on_check_httpd_timeout), gpointer(this));

  // Show open windows.
  g_timeout_add (300, GSourceFunc(on_windows_startup_timeout), gpointer(this));
  
  // Start listening to messages directed to us.
  interprocess_communications_initiate_listener ();
}


MainWindow::~MainWindow()
{
  // Destroy any pending listener restart.
  gw_destroy_source (interprocess_communications_initiate_listener_event_id);

  // Store main window dimensions if windows are attached.
  ScreenLayoutDimensions dimensions (window_main);
  dimensions.save();
  
  // Shut down the various windows.
  shutdown_windows();

  // Destroy the Outpost
  delete windowsoutpost;
  
  // The possibly always-on Help | System log dialog box
  if (syslogdialog) { delete syslogdialog; }
  
  // Do shutdown actions.
  shutdown_actions();
  // Destroying the window is done by Gtk itself.
}


/*
 |
 |
 |
 |
 |
 Initialization
 |
 |
 |
 |
 |
 */

void MainWindow::enable_or_disable_widgets(bool enable)
{
  // Set some widgets (in)sensitive depending on whether a project is open.
  if (properties1)
    gtk_widget_set_sensitive(properties1, enable);
  if (notes2)
    gtk_widget_set_sensitive(notes2, enable);
  if (menuitem_edit)
    gtk_widget_set_sensitive(menuitem_edit, enable);
  if (print)
    gtk_widget_set_sensitive(print, enable);
  if (project_changes)
    gtk_widget_set_sensitive(project_changes, enable);
  if (menuitem_view)
    gtk_widget_set_sensitive(menuitem_view, enable);
  if (compare_with1)
    gtk_widget_set_sensitive(compare_with1, enable);
  if (copy_project_to)
    gtk_widget_set_sensitive(copy_project_to, enable);
  if (insert1)
    gtk_widget_set_sensitive(insert1, enable);
  if (check1)
    gtk_widget_set_sensitive(check1, enable);
  if (menutools)
    gtk_widget_set_sensitive(menutools, enable);
  if (preferences_remote_repository)
    gtk_widget_set_sensitive(preferences_remote_repository, enable);
  if (preferences_planning)
    gtk_widget_set_sensitive(preferences_planning, enable);
  if (file_projects_merge)
    gtk_widget_set_sensitive(file_projects_merge, enable);
  navigation.sensitive(enable);
}

/*
 |
 |
 |
 |
 |
 Menu window
 |
 |
 |
 |
 |
 */


void MainWindow::on_open1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->open();
}


void MainWindow::open()
// Do the logic for opening a project.
{
  // Get new project, bail out if none.
  ustring newproject;
  if (!project_select(newproject))
    return;
  // Open editor.
  on_file_project_open(newproject, false);
}


void MainWindow::on_new1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->newproject();
}


void MainWindow::newproject ()
{
  ProjectDialog projectdialog(true);
  if (projectdialog.run() == GTK_RESPONSE_OK) {
    on_file_project_open(projectdialog.newprojectname, false);
    // Focus the desired book.
    navigation.display (Reference (projectdialog.focusbook, 1, "1"));
  }
}


void MainWindow::on_properties1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->editproject();
}


void MainWindow::editproject ()
{
  save_editors();
  // Show project dialog.
  ProjectDialog projectdialog(false);
  if (projectdialog.run() == GTK_RESPONSE_OK) {
    // Get focused project window.
    WindowEditor *editor_window = last_focused_editor_window();
    if (editor_window) {
      // Reload dictionaries.
      editor_window->load_dictionaries();
    }
    if (projectdialog.isChanged) {
      // Postiff: have tried to ensure that isChanged is set true when/
      // it needs to be. If I'm wrong, we could see some strange bugs. But
      // without having that flag, it is very annoying to have the current
      // editing window changed out from under you just for looking into the
      // project properties dialog.
      // As anything could have been changed to the project, reopen it.
      reload_all_editors (false);
      // Focus the desired book...the last one added in Bible order?
      navigation.display (Reference (projectdialog.focusbook, 1, "1"));
    }
  }
}


void MainWindow::on_delete1_activate (GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->deleteproject ();
}


void MainWindow::deleteproject ()
{
  // Get all projects, leave the current one and the non-editable ones out.
  vector < ustring > all_projects = projects_get_all();
  vector < ustring > projects;
  for (unsigned int i = 0; i < all_projects.size(); i++) {
    bool include = true;
    if (all_projects[i] == settings->genconfig.project_get())
      include = false;
    ProjectConfiguration *projectconfig = settings->projectconfig(all_projects[i]);
    if (!projectconfig->editable_get())
      include = false;
    if (include)
      projects.push_back(all_projects[i]);
  }
  // User interface.
  ListviewDialog dialog(_("Delete project"), projects, "", true, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    int result;
    result = gtkw_dialog_question(window_main, _("Are you sure you want to delete project ") + dialog.focus + "?");
    if (result == GTK_RESPONSE_YES) {
      result = gtkw_dialog_question(window_main, _("Are you really sure to delete project ") + dialog.focus + _(", something worth perhaps years of work?"));
    }
    if (result == GTK_RESPONSE_YES) {
      project_delete(dialog.focus);
    }
  }
}

void MainWindow::on_close1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->accelerator_close_window();
}

void MainWindow::on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->initiate_shutdown();
}

void MainWindow::on_system_log1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->viewlog();
}

void MainWindow::viewlog()
{
  if (!syslogdialog) {
	syslogdialog = new SystemlogDialog(0);
  }
  // This window can float while the user does other things, so the syslogdialog has some 
  // persistent state to manage this feature. It takes care of killing the window, restarting a 
  // new one, etc. so we don't have to worry about that here.
  syslogdialog->run();
}

void MainWindow::on_help_main_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_help_main();
}


void MainWindow::on_help_main()
{
  gtkw_show_uri (html_server_url ("site/home.html"), false);
}

// For F2 accelerator
void MainWindow::on_jumpto_main()
{
  gtk_widget_grab_focus (navigation.get_entry_free());
}


void MainWindow::on_about1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->showabout();
}

void MainWindow::showabout()
{
  gtk_show_about_dialog(GTK_WINDOW(window_main),
                        "version", PACKAGE_VERSION,
                        "website", PACKAGE_BUGREPORT,
                        "copyright", _("Copyright (c) 2003-2013 Teus Benschop"),
			"translator-credits", _("translator-credits"),
                        "license",
			_("This program is free software; you can redistribute it and/or modify\n"
                        "it under the terms of the GNU General Public License as published by\n"
                        "the Free Software Foundation; either version 3 of the License, or\n"
                        "(at your option) any later version.\n"
                        "\n"
			  "This program is distributed in the hope that it will be useful,\n" "but WITHOUT ANY WARRANTY; without even the implied warranty of\n" "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n" "GNU General Public License for more details.\n" "\n" "You should have received a copy of the GNU General Public License\n" "along with this program; if not, write to the Free Software\n" "Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n"), NULL);
}

void MainWindow::on_undo1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->menu_accelerator_undo();
}

void MainWindow::menu_accelerator_undo()
// Called for undo.
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused) {
      editor_windows[i]->undo();
    }
  }
  if (window_notes) {
    if (window_notes->focused) {
      window_notes->undo();
    }
  }
}

void MainWindow::on_redo1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->menu_accelerator_redo();
}

void MainWindow::menu_accelerator_redo()
// Called for redo.
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused) {
      editor_windows[i]->redo();
    }
  }
  if (window_notes) {
    if (window_notes->focused) {
      window_notes->redo();
    }
  }
}

void MainWindow::on_edit1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->menu_edit();
}

void MainWindow::menu_edit()
{
  // Set the sensitivity of some items in the Edit menu.
  WindowEditor *editor_window = last_focused_editor_window();

  bool undo = true;
  bool redo = true;
  if (editor_window) {
    undo = editor_window->can_undo();
    redo = editor_window->can_redo();
  }
  gtk_widget_set_sensitive(undo1, undo);
  gtk_widget_set_sensitive(redo1, redo);

  // Sensitivity of the clipboard operations.
  // The "owner-change" signal of the clipboard is not a reliable indicator for pastable content.
  bool cut = true;
  bool copy = true;
  bool paste = true;
  gtk_widget_set_sensitive(cut1, cut);
  gtk_widget_set_sensitive(copy1, copy);
  gtk_widget_set_sensitive(copy_with_formatting, copy);
  gtk_widget_set_sensitive(paste1, paste);

  // Enable/disable based on whether we're editing a note.
  bool enable = (window_notes && window_notes->note_being_edited());
  // References can only be taken from a note when it is opened.
  if (get_references_from_note)
    gtk_widget_set_sensitive(get_references_from_note, enable);

  // The Bible notes can only be edited when the cursor is in a note text.
  enable = false;
  if (editor_window)
    if (editor_window->last_focused_type() == etvtNote)
      enable = true;
  gtk_widget_set_sensitive(edit_bible_note, enable);
}

void MainWindow::on_find_and_replace1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->menu_replace();
}

void MainWindow::menu_replace()
{
  // Before finding, save the current file.
  save_editors();
  // Display references.
  show_references_window();
  // Start find/replace dialog.
  vector < Reference > results;
  {
    ReplaceDialog replacedialog(0);
    if (replacedialog.run() == GTK_RESPONSE_OK) {
      results.assign(replacedialog.results.begin(), replacedialog.results.end());
      if (window_references) {
        window_references->set(replacedialog.results, settings->genconfig.project_get(), NULL);
      }
    } else {
      return;
    }
  }
  // Replace text.
  if (results.size()) {
    ReplacingDialog replacedialog(results);
    replacedialog.run();
    reload_all_editors(false);
  } else {
    gtkw_dialog_info(window_main, _("There was nothing to replace"));
  }
}

void MainWindow::on_findspecial1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->menu_findspecial();
}

void MainWindow::menu_findspecial()
{
  DEBUG("Called")
  // Before finding, save the current file.
  save_editors();
  DEBUG("Saved editors...now show_references_window")
  // Display the references window.
  show_references_window();
  // Start dialog.
  {
	DEBUG("Now run search special dialog")
    SearchSpecialDialog dialog(0);
    if (dialog.run() != GTK_RESPONSE_OK) { return; }
  }
  // Carry out the search. 
  DEBUG("Now do the search")
  search_string(window_references);
}


void MainWindow::on_insert1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menu_insert();
}


void MainWindow::on_menu_insert()
// Sets the labels of the underlying menu items right.
{
  // Get the focused editor window.
  WindowEditor *editor_window = last_focused_editor_window();

  // Inserting special character.
  gtk_widget_set_sensitive(insert_special_character, (editor_window && (editor_window->focused)));
  
  // Inserting footnote and xref.
  gtk_widget_set_sensitive(insert_footnote, (editor_window && (editor_window->focused)));
  gtk_widget_set_sensitive(insert_xref, (editor_window && (editor_window->focused)));
}

void MainWindow::on_menuitem_view_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menuitem_view();
}

void MainWindow::on_menuitem_view()
{
}

void MainWindow::on_notes_preferences_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_notes_preferences();
}

void MainWindow::on_notes_preferences()
{
  NotesDialog dialog(0);
  dialog.run();
}

void MainWindow::on_copy_project_to_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_copy_project_to();
}

void MainWindow::on_copy_project_to()
// Copy project to another one.
{
  save_editors();
  EntryDialog dialog(_("New project name"), _("Enter a name of a non-existent project\nwhere this project will be copied to."), settings->genconfig.project_get());
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Does the project exist?
    if ((project_exists(dialog.entered_value)) || (dialog.entered_value == "data")) {
      // Yes, give message that project exists.
      ustring error;
      error.append(dialog.entered_value);
      error.append(_(": This project already exists.\nIf you still intend to copy the project,\ndelete it first."));
      gtkw_dialog_error(window_main, error);
    } else {
      // Ok, go ahead with the copy.
      project_copy(settings->genconfig.project_get(), dialog.entered_value);
      // Give message when through.
      ustring message;
      message.append(_("The project has been copied to a new project named\n"));
      message.append(dialog.entered_value);
      message.append(".");
      gtkw_dialog_info(window_main, message);
    }
  }
}


void MainWindow::on_compare_with1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_compare_with();
}


void MainWindow::on_compare_with()
// Compare the current project with another one.
{
  save_editors();
  show_references_window();
  CompareDialog dialog(window_references);
  dialog.run();
}


void MainWindow::on_printingprefs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_printing_preferences();
}

void MainWindow::on_printing_preferences()
{
  PrintPreferencesDialog dialog(0);
  dialog.run();
}

void MainWindow::on_prefs_books_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_prefs_books();
}

void MainWindow::on_prefs_books()
{
  BookDialog dialog(settings->genconfig.project_get());
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_all_editors(false);
  }
}

void MainWindow::on_preferences_tidy_text_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_tidy_text();
}

void MainWindow::on_preferences_tidy_text()
{
  TidyDialog dialog(0);
  dialog.run();
}

/*
 |
 |
 |
 |
 |
 Navigation
 |
 |
 |
 |
 |
 */


void MainWindow::on_navigation_new_reference_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_navigation_new_reference();
}


void MainWindow::on_navigation_new_reference()
// This function is called when the navigation object goes to another reference.
{
  // Store the new reference in the configuration.
  settings->genconfig.book_set(navigation.reference.book_get());
  settings->genconfig.chapter_set(convert_to_string(navigation.reference.chapter_get()));
  settings->genconfig.verse_set(navigation.reference.verse_get());

  // Let the editor(s) show the reference.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->go_to(navigation.reference);
  }

  // Get last focused editor.
  WindowEditor *last_focused_editor = last_focused_editor_window();

  // Send the focus to the interprocess communications focus system.
  on_tool_send_reference ();

  // Send to resources.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    resource_windows[i]->go_to(navigation.reference);
  }

  // Update the notes view.
  notes_redisplay();

  // Create a reference for the related windows.
  // These may not take verses like 10a or 10-12, but only numbers like 10 or 12.
  Reference goto_reference(navigation.reference.book_get(),
			   navigation.reference.chapter_get(),
			   number_in_string(navigation.reference.verse_get()));

  // Send it to the outline window.  
  if (window_outline) {
    if (last_focused_editor) {
      window_outline->go_to(last_focused_editor->project(), goto_reference);
    }
  }

  // Optional displaying related verses.
  if (window_show_related_verses) {
    window_show_related_verses->go_to(settings->genconfig.project_get(), navigation.reference);
  }
  
}


void MainWindow::goto_next_verse()
{
  navigation.nextverse();
}


void MainWindow::goto_previous_verse()
{
  navigation.previousverse();
}


void MainWindow::goto_next_chapter()
{
  navigation.nextchapter();
}


void MainWindow::goto_previous_chapter()
{
  navigation.previouschapter();
}


void MainWindow::goto_next_book()
{
  navigation.nextbook();
}


void MainWindow::goto_previous_book()
{
  navigation.previousbook();
}


void MainWindow::goto_reference_interactive()
// Opens a dialog to ask the user to which reference to go.
{
  bool go_back = false;
  bool go_forward = false;
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window) {
    GotoReferenceDialog dialog(editor_window->current_reference().book_get(),
			       editor_window->current_reference().chapter_get(),
			       editor_window->current_reference().verse_get());
    if (dialog.run() == GTK_RESPONSE_OK) {
      if (dialog.newreference) {
        // If the dialog closes, then another window will receive focus again.
        // This focusing causes the navigation to take the values as they are in the configuration.
        // This would frustrate the desire of the user to go somewhere else.
        // To fix the problem, the settings are updated here.
        settings->genconfig.book_set(dialog.reference.book_get());
        settings->genconfig.chapter_set(convert_to_string(dialog.reference.chapter_get()));
        settings->genconfig.verse_set(dialog.reference.verse_get());
        navigation.display(dialog.reference);
      }
      go_back = dialog.go_back;
      go_forward = dialog.go_forward;
    }
  }
  if (go_back) {
    navigation.on_list_back ();
  }
  if (go_forward) {
    navigation.on_list_forward ();
  }
}


void MainWindow::on_new_verse_signalled(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_editor_another_verse();
}


void MainWindow::on_editor_another_verse()
// This one is called when an editor signals that the cursor is now on another verse.
{
  WindowEditor *last_focused_editor = last_focused_editor_window();
  if (last_focused_editor) {
    Reference reference(navigation.reference.book_get(),
			navigation.reference.chapter_get(),
			last_focused_editor->current_verse_number());
    navigation.display(reference);
  }
}


void MainWindow::on_text_area_activate()
{
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window == NULL) {
    if (!editor_windows.empty()) {
      editor_window = editor_windows[0];
    }
  }
  if (editor_window) {
    editor_window->focus_set();
  }
}


void MainWindow::on_tools_area_activate()
{
  if (window_show_related_verses) {
    if (focused_tool_button == window_show_related_verses->focus_in_signal_button) {
      window_show_related_verses->focus_set ();
    }
  }
  if (window_merge) {
    if (focused_tool_button == window_merge->focus_in_signal_button) {
      window_merge->focus_set ();
    }
  }
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (focused_tool_button == resource_windows[i]->focus_in_signal_button) {
      resource_windows[i]->focus_set ();
    }
  } 
  if (window_outline) {
    if (focused_tool_button == window_outline->focus_in_signal_button) {
      window_outline->focus_set ();
    }
  }
  if (window_check_keyterms) {
    if (focused_tool_button == window_check_keyterms->focus_in_signal_button) {
      window_check_keyterms->focus_set ();
    }
  }
  if (window_styles) {
    if (focused_tool_button == window_styles->focus_in_signal_button) {
      window_styles->focus_set ();
    }
  }
  // Skip project notes window.
  if (window_references) {
    if (focused_tool_button == window_references->focus_in_signal_button) {
      window_references->focus_set ();
    }
  }
  // Skip editor windows.
  if (window_check_usfm) {
    if (focused_tool_button == window_check_usfm->focus_in_signal_button) {
      window_check_usfm->focus_set ();
    }
  }
}


void MainWindow::on_notes_area_activate()
{
  view_project_notes();
  notes_redisplay();
}


void MainWindow::on_tool_go_to_reference_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->goto_reference_interactive();
}


/*
 |
 |
 |
 |
 |
 Clipboard
 |
 |
 |
 |
 |
 */


void MainWindow::on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_cut();
}


void MainWindow::on_cut()
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused) {
      editor_windows[i]->cut();
    }
  }
  if (window_notes) {
    if (window_notes->focused) {
      window_notes->cut();
    }
  }
}


void MainWindow::on_copy1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_copy();
}


void MainWindow::on_copy()
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused) {
      editor_windows[i]->copy();
    }
  }
  if (window_check_keyterms) {
    if (window_check_keyterms->focused) {
      window_check_keyterms->copy_clipboard();
    }
  }
  if (window_notes) {
    if (window_notes->focused) {
      window_notes->copy();
    }
  }
  if (window_references) {
    if (window_references->focused) {
      window_references->copy();
    }
  }
}


void MainWindow::on_copy_with_formatting_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_copy_with_formatting();
}


void MainWindow::on_copy_with_formatting()
{
  // Using the accelerator in the GtkTextView give weird results. The accelerator has been removed.
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window) {
    if (editor_window->focused) {
      // In case of the text editor, the USFM code is copied, not the plain text. 
      GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
      gtk_clipboard_set_text(clipboard, editor_window->text_get_selection().c_str(), -1);
    }
  }
}


void MainWindow::on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_paste();
}


void MainWindow::on_paste()
{
  // Bail out if no text is available.
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  if (!gtk_clipboard_wait_is_text_available(clipboard))
    return;
  // Do the paste operation.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused) {
      editor_windows[i]->paste();
    }
  }
  if (window_notes) {
    if (window_notes->focused) {
      window_notes->paste();
    }
  }
}


/*
 |
 |
 |
 |
 |
 References
 |
 |
 |
 |
 |
 */


void MainWindow::on_view_references_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_references();
}


void MainWindow::on_view_references ()
{
  DEBUG("Called")
  on_window_references_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_references))) {
	DEBUG("Create new references window")
    window_references = new WindowReferences(layout, accelerator_group, windows_startup_pointer != G_MAXINT, references_management_enabled);
    g_signal_connect((gpointer) window_references->delete_signal_button, "clicked", G_CALLBACK(on_window_references_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_references->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_references->signal_button, "clicked", G_CALLBACK(on_window_references_signal_button_clicked), gpointer(this));
  }
}


void MainWindow::show_references_window()
{
  DEBUG("Called")
  gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_references), TRUE);
  window_references->focus_set ();
}


void MainWindow::on_window_references_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_references_delete_button();
}


void MainWindow::on_window_references_delete_button()
{
  DEBUG("Called")
  if (window_references) {
  DEBUG("Delete old references window")
  delete window_references;
    window_references = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_references), FALSE);
  }
}


void MainWindow::on_window_references_signal_button_clicked(GtkButton * button, gpointer user_data)
// This routine is called when the reference window fires a signal that something has happened.
{
  ((MainWindow *) user_data)->on_window_references_signal_button();
}


void MainWindow::on_window_references_signal_button()
// Handler for when the user clicked a reference in the references window so as to go to that reference.
{
  // Get the editor window. If none, bail out.
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  
  // Bail out if there's no references window.
  if (!window_references)
    return;

  // Focus the editor.
  editor_window->focus_set ();

  // Jump to the reference.
  navigation.display(window_references->reference);
  editor_window->go_to_new_reference_highlight_set();
}


void MainWindow::on_next_reference()
// Go to the next reference in the references window.
{
  show_references_window();
  window_references->goto_next();
}


void MainWindow::on_previous_reference()
// Go to the previous reference in the references window.
{
  show_references_window();
  window_references->goto_previous();
}


void MainWindow::on_show_quick_references_signal_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_show_quick_references_signal_button(button);
}


void MainWindow::on_show_quick_references_signal_button(GtkButton * button)
{
  if (!window_references)
    return;
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  // Ask whether the references should be made available.
  if (yes_no_always_dialog (_("This note has references.\nWould you like to load these in the references list?"), ynadtLoadReferences, false, true)) {
    vector <Reference> references = editor_window->quick_references();
    window_references->set(references, editor_window->project(), NULL);
  }
}


/*
 |
 |
 |
 |
 |
 Bibledit Windows Outpost
 BibleTime
 |
 |
 |
 |
 |
 */


void MainWindow::on_reference_exchange1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_reference_exchange();
}


void MainWindow::on_reference_exchange()
{
  ReferenceExchangeDialog dialog(0);
  dialog.run();
}


void MainWindow::on_send_word_to_toolbox_signalled(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->send_word_to_toolbox();
}


void MainWindow::send_word_to_toolbox()
{
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  ustring word = editor_window->word_double_clicked_text();
  if (word.empty())
    return;
  gw_message(_("Sending to Toolbox: ") + word);
  windowsoutpost->SantaFeFocusWordSet(word);
}


void MainWindow::on_preferences_windows_outpost_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_windows_outpost();
}


void MainWindow::on_preferences_windows_outpost()
{
  // Dialog for making settings.
  OutpostDialog dialog(0);
  dialog.run();
  if (dialog.changed) {
    // Changes were made: destroy and recreate the object.
    delete windowsoutpost;
    windowsoutpost = new WindowsOutpost(true);
    // If required, start the Outpost.
    if (settings->genconfig.use_outpost_get()) {
      // Delay a bit so wine debugger doesn't start.
      g_usleep(1000000);
      windowsoutpost->Start();
    }
  }
}


void MainWindow::on_tool_send_reference ()
{
  // Send the focus to Bibledit-Web.
  ustring payload = convert_to_string (navigation.reference.book_get());
  payload.append (".");
  payload.append (convert_to_string (navigation.reference.chapter_get()));
  payload.append (".");
  payload.append (navigation.reference.verse_get());
  ustring url = settings->genconfig.bibledit_web_url_get();
  ustring user = settings->genconfig.bibledit_web_user_get();
  url.append ("/ipc/setmessage.php?user=" + user + "&subject=focus&message=");
  url.append (payload);
  urltransport->send_message (url);
}


/*
 |
 |
 |
 |
 |
 Project notes
 |
 |
 |
 |
 |
 */


void MainWindow::view_project_notes()
{
  if (!project_notes_enabled)
    return;
  if (window_notes) {
    // If the window is there, focus it for the user.
    window_notes->focus_set();
  } else {
    // New notes window.
    window_notes = new WindowNotes(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_notes->delete_signal_button, "clicked", G_CALLBACK(on_window_notes_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_notes->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_notes->references_available_signal_button, "clicked", G_CALLBACK(on_window_notes_references_available_button_clicked), gpointer(this));
    // The window should focus, but it doesn't, hence focus it here.
    gtk_button_clicked(GTK_BUTTON(window_notes->focus_in_signal_button));
  }
}

void MainWindow::on_window_notes_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_notes_delete_button();
}


void MainWindow::on_window_notes_delete_button()
{
  if (window_notes) {
    delete window_notes;
    window_notes = NULL;
  }
}


void MainWindow::on_new_note_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_new_note();
}


void MainWindow::on_new_note()
{
  // Display notes.
  view_project_notes();
  // Create new note.
  if (window_notes)
    window_notes->new_note();
}


void MainWindow::on_delete_note_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  gtkw_dialog_info(((MainWindow *) user_data)->window_main, _("A note can be deleted by clicking on the [delete] link in the notes view"));
}


void MainWindow::on_consultant_notes_send_receive_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_consultant_notes_send_receive();
}


void MainWindow::on_consultant_notes_send_receive ()
{
  if (settings->genconfig.consultation_notes_git_use_remote_repository_get()) {
    vcs->schedule(notes_shared_storage_folder ());
    maintenance_register_git_repository (notes_shared_storage_folder ());
  }
}


void MainWindow::on_viewnotes_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_notes();
}


void MainWindow::on_view_notes()
{
  ShowNotesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    view_project_notes();
    notes_redisplay();
  }
}

void MainWindow::notes_redisplay()
{
  if (window_notes) {
    window_notes->redisplay();
  }
}

void MainWindow::on_find_in_notes1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->find_in_notes();
}

void MainWindow::find_in_notes()
{
  FindNoteDialog findnotedialog(0);
  if (findnotedialog.run() == GTK_RESPONSE_OK) {
    view_project_notes();
    if (window_notes) {
      window_notes->display(findnotedialog.ids);
    }
  }
}

void MainWindow::on_import_notes()
{
  ImportNotesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_APPLY) {
    view_project_notes();
    notes_redisplay();
  }
}


void MainWindow::on_get_references_from_note_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_get_references_from_note();
}


void MainWindow::on_get_references_from_note()
// This one is called through the Edit menu.
{
  // Clear the searchwords.
  settings->session.highlights.clear();

  // Get all references from the editor.
  vector <Reference> references;
  vector <ustring> dummy;
  if (window_notes)
    window_notes->get_references_from_note(references, dummy);

  // Sort the references so they appear nicely in the editor.
  sort_references(references);

  // Load the references in the window
  show_references_window();
  window_references->set (references, settings->genconfig.project_get(), NULL);
}


void MainWindow::on_window_notes_references_available_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_notes_references_available_button();
}


void MainWindow::on_window_notes_references_available_button()
{
  show_references_window();
  if (window_notes) {
    vector <Reference> references = window_notes->available_references;
    sort_references(references);
    window_references->set (references, settings->genconfig.project_get(), NULL);
  }
}


/*
 |
 |
 |
 |
 |
 Export
 |
 |
 |
 |
 |
 */

void MainWindow::on_file_export_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_export();
}

void MainWindow::on_file_export ()
{
  save_editors();
  export_assistant = new ExportAssistant (window_references, window_styles, window_check_keyterms);
  g_signal_connect ((gpointer) export_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}


/*
 |
 |
 |
 |
 |
 Checks
 |
 |
 |
 |
 |
 */


void MainWindow::on_validate_usfms1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menu_check_markers_validate();
}


void MainWindow::on_menu_check_markers_validate()
{
  save_editors();
  show_references_window();
  scripture_checks_validate_usfms(window_references, NULL);
}


void MainWindow::on_count_usfms1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menu_check_markers_count();
}


void MainWindow::on_menu_check_markers_count()
{
  save_editors();
  scripture_checks_count_usfms(true);
}

void MainWindow::on_compare_usfm1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menu_check_markers_compare();
}

void MainWindow::on_menu_check_markers_compare()
{
  save_editors();
  show_references_window();
  scripture_checks_compare_usfms(window_references, NULL);
}

void MainWindow::on_chapters_and_verses1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_menu_check_chapters_and_verses();
}


void MainWindow::on_menu_check_chapters_and_verses()
{
  save_editors();
  show_references_window();
  scripture_checks_chapters_verses(window_references, NULL);
}


void MainWindow::on_count_characters_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_count_characters();
}


void MainWindow::on_count_characters()
{
  save_editors();
  scripture_checks_count_characters(true);
}


void MainWindow::on_unwanted_patterns_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_unwanted_patterns();
}


void MainWindow::on_unwanted_patterns()
{
  save_editors();
  show_references_window();
  scripture_checks_unwanted_patterns(window_references, NULL);
}


void MainWindow::on_check_capitalization_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_capitalization();
}


void MainWindow::on_check_capitalization()
{
  save_editors();
  show_references_window();
  scripture_checks_capitalization(window_references, NULL);
}


void MainWindow::on_check_repetition_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_repetition();
}


void MainWindow::on_check_repetition()
{
  save_editors();
  show_references_window();
  scripture_checks_repetition(window_references, NULL);
}


void MainWindow::on_check_matching_pairs_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_matching_pairs();
}


void MainWindow::on_check_matching_pairs()
{
  save_editors();
  show_references_window();
  scripture_checks_matching_pairs(window_references, NULL);
}


void MainWindow::on_unwanted_words_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_unwanted_words();
}


void MainWindow::on_unwanted_words()
{
  save_editors();
  show_references_window();
  scripture_checks_unwanted_words(window_references, NULL);
}


void MainWindow::on_word_count_inventory_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_word_count_inventory();
}


void MainWindow::on_word_count_inventory()
{
  save_editors();
  scripture_checks_word_inventory(true);
}


bool MainWindow::on_check_httpd_timeout(gpointer data)
{
  ((MainWindow *) data)->on_check_httpd();
  return true;
}


void MainWindow::on_check_httpd()
{
  // Does the httpd have a request for us?
  if (!httpd.search_whole_word.empty()) {
    // Bibledit presents itself and any detached editors.
    gtk_window_present(GTK_WINDOW(window_main));
    // Bibledit searches for the word.
    settings->session.searchword = httpd.search_whole_word;
    httpd.search_whole_word.clear();
    settings->session.search_case_sensitive = true;
    settings->session.search_current_book = false;
    settings->session.search_current_chapter = false;
    settings->session.search_globbing = false;
    settings->session.search_start_word_match = true;
    settings->session.search_end_word_match = true;
    settings->session.search_page = 1;
    settings->session.searchresultstype = sstLoad;
    settings->session.highlights.clear();
    SessionHighlights sessionhighlights(settings->session.searchword, settings->session.search_case_sensitive, settings->session.search_globbing, settings->session.search_start_word_match, settings->session.search_end_word_match, atRaw, false, false, false, false, false, false, false, false);
    settings->session.highlights.push_back(sessionhighlights);
    show_references_window();
    search_string(window_references);
  }
  // Did the browser request a url too difficult for it to handle?
  if (!httpd.difficult_url.empty()) {
    htmlbrowser(httpd.difficult_url, true, true);
    httpd.difficult_url.clear();
  }
}


void MainWindow::on_my_checks_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_my_checks();
}


void MainWindow::on_my_checks()
{
  save_editors();
  show_references_window();
  MyChecksDialog dialog(window_references);
  dialog.run();
}


void MainWindow::on_check_markers_spacing_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_markers_spacing();
}


void MainWindow::on_check_markers_spacing()
{
  save_editors();
  show_references_window();
  scripture_checks_usfm_spacing(window_references, NULL);
}


void MainWindow::on_check_references_inventory_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_references_inventory();
}

void MainWindow::on_check_references_inventory()
{
  save_editors();
  scripture_checks_references_inventory(true);
}


void MainWindow::on_check_references_validate_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_references_validate();
}


void MainWindow::on_check_references_validate()
{
  save_editors();
  show_references_window();
  scripture_checks_validate_references(window_references, NULL);
}


void MainWindow::on_check_nt_quotations_from_the_ot_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_nt_quotations_from_the_ot();
}

void MainWindow::on_check_nt_quotations_from_the_ot()
{
  save_editors();
  show_references_window();
  scripture_checks_nt_quotations_from_ot(window_references);
}

void MainWindow::on_synoptic_parallel_passages_from_the_nt_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_synoptic_parallel_passages_from_the_nt();
}

void MainWindow::on_synoptic_parallel_passages_from_the_nt()
{
  save_editors();
  show_references_window();
  scripture_checks_synoptic_parallels_from_nt(window_references);
}

void MainWindow::on_parallels_from_the_ot_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_parallels_from_the_ot();
}

void MainWindow::on_parallels_from_the_ot()
{
  save_editors();
  show_references_window();
  scripture_checks_parallels_from_ot(window_references);
}


void MainWindow::on_check_spelling_error_next_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_spelling_error(true, false);
}


void MainWindow::on_check_spelling_error_previous_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_spelling_error(false, false);
}


void MainWindow::on_check_spelling_error(bool next, bool extremity)
// Moves the cursor to another spelling mistake.
// next: if true goes to next error, and if false goes to the previous one.
// extremity: start checking at the extremity of the chapter that is open.
{
  // Get the editor window, if not, bail out.
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;

  // If the project has spelling switched off, bail out.
  ustring project = editor_window->project();
  ProjectConfiguration * projectconfig = settings->projectconfig (project);
  if (!projectconfig->spelling_check_get()) {
    gtkw_dialog_info (window_main, _("To make this work, enable spelling checking in the project"));
    return;
  }
    
  // Go to the next (or previous) spelling error. If it's there, bail out.
  if (editor_window->move_cursor_to_spelling_error (next, extremity))
    return;
    
  // No next (or previous) error in the current chapter. Ask whether the user wishes to go to other chapter.
  ustring msg = _("There are no more spelling errors in this chapter\nWould you like to go to the");
  if (next)
    msg.append (_("next"));
  else
    msg.append (_("previous"));
  msg.append (_(" chapter or chapters?"));
  if (gtkw_dialog_question (NULL, msg) != GTK_RESPONSE_YES)
    return;
  if (next) {
    navigation.nextchapter();
    check_spelling_at_start = true;
  } else {
    navigation.previouschapter();
    check_spelling_at_end = true;
  }
}


void MainWindow::on_editor_spelling_checked_button_clicked(GtkButton *button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_editor_spelling_checked_button();
}


void MainWindow::on_editor_spelling_checked_button()
// This one is called when the spelling of the document has been checked.
{
  // Handle cases that we're going to another spelling error.
  if (check_spelling_at_start) {
    check_spelling_at_start = false;
    on_check_spelling_error(true, true);
  }
  if (check_spelling_at_end) {
    check_spelling_at_end = false;
    on_check_spelling_error(false, true);
  }
}


void MainWindow::on_check_spelling_bulk_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_spelling_bulk();
}


void MainWindow::on_check_spelling_bulk()
{
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window) {
    gtkw_dialog_info (NULL, _("There is nothing to be checked"));
    return;
  }
  BulkSpellingDialog dialog (editor_window->spelling_get_misspelled());
  if (dialog.run () == GTK_RESPONSE_OK) {
    editor_window->spelling_approve (dialog.approved);
  }
}


/*
 |
 |
 |
 |
 |
 Styles
 |
 |
 |
 |
 |
 */

void MainWindow::on_stylesheet_open_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_stylesheet_open();
}

void MainWindow::on_stylesheet_open()
{
  display_window_styles();
}

void MainWindow::on_goto_styles_area()
{
  // Create or active the styles window.
  display_window_styles();
  // Focus the window to enable the user to start inserting the style using the keyboard.
  if (window_styles) {
    window_styles->focus_set();
  }
}

void MainWindow::display_window_styles()
{
  // Display the styles if needed.
  if (!window_styles) {
    // The visibility of widgets depends on whether a stylesheet shows.
    gtk_widget_hide(stylesheet_open);
    gtk_widget_show(stylesheets_expand_all);
    gtk_widget_show(stylesheets_collapse_all);
    gtk_widget_show(style_insert);
    gtk_widget_show(stylesheet_edit_mode);
    gtk_widget_show(style_new);
    gtk_widget_show(style_properties);
    gtk_widget_show(style_delete);
    gtk_widget_show(menu_stylesheet);
    // Open the window.
    window_styles = new WindowStyles(layout, accelerator_group, windows_startup_pointer != G_MAXINT, style, style_menu, stylesheets_expand_all, stylesheets_collapse_all, style_insert, stylesheet_edit_mode, style_new, style_properties, style_delete, stylesheet_switch, stylesheets_new, stylesheets_delete, stylesheets_rename);
    g_signal_connect((gpointer) window_styles->delete_signal_button, "clicked", G_CALLBACK(on_window_styles_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_styles->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_styles->apply_signal, "clicked", G_CALLBACK(on_style_button_apply_clicked), gpointer(this));
    g_signal_connect((gpointer) window_styles->open_signal, "clicked", G_CALLBACK(on_style_button_open_clicked), gpointer(this));
    g_signal_connect((gpointer) window_styles->edited_signal, "clicked", G_CALLBACK(on_style_edited), gpointer(this));
    stylesheet_open_named(stylesheet_get_actual());
  }
}

void MainWindow::on_window_styles_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_styles_delete_button();
}

void MainWindow::on_window_styles_delete_button()
{
  if (window_styles) {
    delete window_styles;
    window_styles = NULL;
  }
  gtk_widget_show(stylesheet_open);
  gtk_widget_hide(stylesheets_expand_all);
  gtk_widget_hide(stylesheets_collapse_all);
  gtk_widget_hide(style_insert);
  gtk_widget_hide(stylesheet_edit_mode);
  gtk_widget_hide(style_new);
  gtk_widget_hide(style_properties);
  gtk_widget_hide(style_delete);
  gtk_widget_hide(menu_stylesheet);
}

void MainWindow::stylesheet_open_named(const ustring & stylesheet)
{
  if (window_styles) {
    window_styles->load(stylesheet);
  }
}

void MainWindow::on_style_button_open_clicked(GtkButton * button, gpointer user_data)
// This is activated by the GuiStyles object if another stylesheet should be opened.
{
  ((MainWindow *) user_data)->on_style_button_open();
}

void MainWindow::on_style_button_open()
{
  if (window_styles) {
    // Save the name of the stylesheet.
    settings->genconfig.stylesheet_set(window_styles->get_sheet());
    // Actually open it.  
    stylesheet_open_named(window_styles->get_sheet());
  }
}


void MainWindow::on_style_button_apply_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_style_apply();
}


void MainWindow::on_style_apply (ustring marker)
{
  // Variable 'marker' may be empty, in which case it is called from the Styles window.
  
  // Get the last focused Editor. If none, bail out.
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;

  // Focus the editor.
  editor_window->focus_set ();
  
  // Bail out if the editor is not editable.
  if (!editor_window->editable())
    return;

  // Bail out if there's no styles window.
  if (marker == "")
    if (!window_styles)
      return;

  // Get the focused style(s).
  ustring selected_style;
  if (marker == "") selected_style = window_styles->get_focus();
  else selected_style = marker;

  // Only proceed when a style has been selected.
  if (selected_style.empty())
    return;

  // Get the Style object.
  Style style(settings->genconfig.stylesheet_get(), selected_style, false);

  // Whether and how the style is used.
  bool style_was_used = true;
  bool style_was_treated_specially = false;

  // Special treatment for the chapter style.
  if (style.type == stChapterNumber) {
    // Ask whether the user wishes to insert a new chapter.
    if (gtkw_dialog_question(window_main, _("Would you like to insert a new chapter?"), GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      // Insert a new chapter.
      save_editors();
      ChapterNumberDialog dialog(true);
      if (dialog.run() == GTK_RESPONSE_OK) {
        reload_all_editors(false);
      } else {
        style_was_used = false;
      }
      style_was_treated_specially = true;
    }
  }
  // Inserting footnote or endnote or crossreference.
  {
    if (editor_window->last_focused_type() == etvtBody) {
      if (style.type == stFootEndNote) {
        if (style.subtype == fentFootnote) {
          InsertNoteDialog dialog(indtFootnote);
          if (dialog.run() == GTK_RESPONSE_OK) {
            editor_window->insert_note(style.marker, dialog.rawtext);
          } else {
            style_was_used = false;
          }
          style_was_treated_specially = true;
        }
        if (style.subtype == fentEndnote) {
          InsertNoteDialog dialog(indtEndnote);
          if (dialog.run() == GTK_RESPONSE_OK) {
            editor_window->insert_note(style.marker, dialog.rawtext);
          } else {
            style_was_used = false;
          }
          style_was_treated_specially = true;
        }
      }
      if (style.type == stCrossreference) {
        InsertNoteDialog dialog(indtCrossreference);
        if (dialog.run() == GTK_RESPONSE_OK) {
          editor_window->insert_note(style.marker, dialog.rawtext);
        } else {
          style_was_used = false;
        }
        style_was_treated_specially = true;
      }
    }
  }

  // Special treatment for a table style.
  {
    if (editor_window->last_focused_type() == etvtBody) {
      if (style.type == stTableElement) {
        InsertTableDialog dialog(editor_window->project());
        if (dialog.run() == GTK_RESPONSE_OK) {
          editor_window->insert_table(dialog.rawtext);
        } else {
          style_was_used = false;
        }
        style_was_treated_specially = true;
      }
    }
  }

  // Normal treatment of the style if it was not handled specially.
  if (!style_was_treated_specially) {
    // Normal treatment of the marker: apply it.
    editor_window->apply_style(selected_style);
  }
  // Take some actions if the style was used.
  if (marker == "") {
    if (style_was_used) {
      window_styles->use(selected_style);
    }
  }
}


void MainWindow::on_editor_style_changed(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->editor_style_changed();
}


void MainWindow::editor_style_changed()
{
}


void MainWindow::on_style_edited(GtkButton * button, gpointer user_data)
// This function is called when the properties of a style have been edited.
{
  ((MainWindow *) user_data)->reload_styles();
}


void MainWindow::reload_styles()
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->create_or_update_formatting_data();
  }
}


/*
 |
 |
 |
 |
 |
 Footnotes, endnotes, crossreferences
 |
 |
 |
 |
 |
 */


void MainWindow::on_edit_bible_note_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_edit_bible_note();
}


void MainWindow::on_edit_bible_note()
{
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window) {
    Editor2 * editor = editor_window->editor_get();
    if (editor) {
      EditNoteDialog dialog(editor);
      dialog.run();
    }
  }
}


void MainWindow::on_insert_footnote_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_style_apply("f");
}


void MainWindow::on_insert_xref_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_style_apply("x");
}


/*
 |
 |
 |
 |
 |
 Tools
 |
 |
 |
 |
 |
 */


void MainWindow::on_menutools_activate(GtkMenuItem * menuitem, gpointer user_data)
{
}


void MainWindow::on_notes_transfer_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_notes_transfer();
}


void MainWindow::on_notes_transfer()
{
  save_editors();
  NotesTransferDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    notes_redisplay();
}


void MainWindow::on_tool_origin_references_in_bible_notes_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tool_origin_references_in_bible_notes();
}


void MainWindow::on_tool_origin_references_in_bible_notes()
{
  save_editors();
  OriginReferencesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_all_editors(false);
}


void MainWindow::on_tool_project_notes_mass_update1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tool_project_notes_mass_update();
}


void MainWindow::on_tool_project_notes_mass_update()
{
  NotesUpdateDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    notes_redisplay();
  }
}


void MainWindow::on_tool_generate_word_lists_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tool_generate_word_lists();
}


void MainWindow::on_tool_generate_word_lists()
{
  save_editors();
  WordlistDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_all_editors(false);
}


void MainWindow::on_tool_transfer_project_notes_to_text_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tool_transfer_project_notes_to_text();
}


void MainWindow::on_tool_transfer_project_notes_to_text()
// This transfers the currently visible project notes to the currently active project, 
// and does that for each verse.
{
  save_editors();
  XferNotes2TextDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_all_editors(false);
  }
}


void MainWindow::on_tools_maintenance_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tools_maintenance();
}


void MainWindow::on_tools_maintenance ()
{
  MaintenanceDialog dialog (0);
  dialog.run();
}


void MainWindow::on_preferences_gui_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_gui();
}


void MainWindow::on_preferences_gui()
{
  if (password_pass(window_main)) {
    GuiDialog dialog(0);
    dialog.run();
  }
}


void MainWindow::on_preferences_password_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_password();
}


void MainWindow::on_preferences_password()
{
  password_edit(window_main);
}


void MainWindow::on_tool_simple_text_corrections_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_tool_simple_text_corrections();
}


void MainWindow::on_tool_simple_text_corrections()
{
  save_editors();
  FixMarkersDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_all_editors(false);
}


void MainWindow::on_preferences_text_replacement_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_text_replacement();
}


void MainWindow::on_preferences_text_replacement()
{
  TextReplacementDialog dialog(0);
  dialog.run();
}


void MainWindow::on_pdf_viewer1_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_pdf_viewer();
}


void MainWindow::on_pdf_viewer()
{
  PDFViewerDialog dialog(0);
  dialog.run();
}


void MainWindow::on_insert_special_character_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_insert_special_character();
}


void MainWindow::on_insert_special_character()
{
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  vector < ustring > characters;
  vector < ustring > descriptions;
  characters.push_back("­");
  descriptions.push_back(_("Soft hyphen"));
  characters.push_back(" ");
  descriptions.push_back(_("No-break space"));
  characters.push_back("“");
  descriptions.push_back(_("Left double quotation mark"));
  characters.push_back("”");
  descriptions.push_back(_("Right double quotation mark"));
  characters.push_back("‘");
  descriptions.push_back(_("Left single quotation mark"));
  characters.push_back("’");
  descriptions.push_back(_("Right single quotation mark"));
  characters.push_back("«");
  descriptions.push_back(_("Left-pointing double angle quotation mark"));
  characters.push_back("»");
  descriptions.push_back(_("Right-pointing double angle quotation mark"));
  RadiobuttonDialog dialog(_("Insert character"), _("Insert special character"), descriptions, settings->session.special_character_selection, false);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  settings->session.special_character_selection = dialog.selection;
  editor_window->insert_text(characters[dialog.selection]);
}


void MainWindow::on_preferences_compare_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_compare();
}


void MainWindow::on_preferences_compare()
{
  ComparePreferencesDialog dialog (0);
  dialog.run ();
}


/*
 |
 |
 |
 |
 |
 Keyterms
 |
 |
 |
 |
 |
 */

void MainWindow::on_check_key_terms_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_key_terms();
}

void MainWindow::on_check_key_terms()
{
  on_window_check_keyterms_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_key_terms))) {
    window_check_keyterms = new WindowCheckKeyterms(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_check_keyterms->delete_signal_button, "clicked", G_CALLBACK(on_window_check_keyterms_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_check_keyterms->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_check_keyterms->signal, "clicked", G_CALLBACK(on_keyterms_signal_button_clicked), gpointer(this));
  }
}

void MainWindow::on_window_check_keyterms_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_check_keyterms_delete_button();
}

void MainWindow::on_window_check_keyterms_delete_button()
{
  if (window_check_keyterms) {
    delete window_check_keyterms;
    window_check_keyterms = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check_key_terms), FALSE);
  }
}


void MainWindow::on_keyterms_signal_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->keyterms_check_action();
}


void MainWindow::keyterms_check_action()
// This is called when the keyterms checking window requests action to be taken.
{
  if (window_check_keyterms->new_reference_showing) {
    // Go to another reference.
    Reference reference(window_check_keyterms->new_reference_showing->book_get(),
			window_check_keyterms->new_reference_showing->chapter_get(),
			window_check_keyterms->new_reference_showing->verse_get());
    navigation.display(reference);
  } else {
    // Transfer references to the references window.
    show_references_window();
    window_references->set (window_check_keyterms->references, settings->genconfig.project_get(), NULL);
  }
}


void MainWindow::on_view_related_verses_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_related_verses();
}


void MainWindow::on_view_related_verses()
{
  on_window_show_related_verses_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_related_verses))) {
    window_show_related_verses = new WindowShowRelatedVerses(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_show_related_verses->delete_signal_button, "clicked", G_CALLBACK(on_window_show_related_verses_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_show_related_verses->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_show_related_verses->button_item, "clicked", G_CALLBACK(on_window_show_related_verses_item_button_clicked), gpointer(this));
    window_show_related_verses->go_to(settings->genconfig.project_get(), navigation.reference);
  }
}


void MainWindow::on_window_show_related_verses_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_show_related_verses_delete_button();
}


void MainWindow::on_window_show_related_verses_delete_button()
{
  if (window_show_related_verses) {
    delete window_show_related_verses;
    window_show_related_verses = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_related_verses), FALSE);
  }
}


void MainWindow::on_window_show_related_verses_item_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_show_related_verses_item_button();
}


void MainWindow::on_window_show_related_verses_item_button()
{
  switch (window_show_related_verses->item_type) {
    case ritNone:
    {
      break;
    }
    case ritKeytermId:
    {
      gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check_key_terms), TRUE);
      window_check_keyterms->go_to_term (convert_to_int (window_show_related_verses->item_id));
      break;
    }
    case ritStrongNumber:
    {
      // There may be more than one strong's number.
      Parse parse (window_show_related_verses->item_id);
      vector <Reference> references;
      for (unsigned int i = 0; i < parse.words.size(); i++) {
        // Get references for this strong's number, and add them to the main store.
        vector <Reference> refs = kjv_get_strongs_verses (parse.words[i]);
        for (unsigned int i = 0; i < refs.size(); i++) {
          references.push_back (refs[i]);
        }
      }
      // Show results.
      show_references_window();
      window_references->set (references, settings->genconfig.project_get(), NULL);
      break;
    }
    case ritParallels:
    {
      vector <Reference> references;
      vector <ustring> comments;
      parallel_passages_retrieve (navigation.reference, references, comments);
      show_references_window();
      window_references->set (references, settings->genconfig.project_get(), &comments);
      break;
    }
  }
}


void MainWindow::on_keyterms_import()
{
  import_keyterms_assistant = new ImportKeytermsAssistant (0);
  g_signal_connect ((gpointer) import_keyterms_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}


void MainWindow::on_keyterms_delete_activate(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_keyterms_delete();
}

void MainWindow::on_keyterms_delete()
{
  delete_keyterms_assistant = new DeleteKeytermsAssistant (0);
  g_signal_connect ((gpointer) delete_keyterms_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}



/*
 |
 |
 |
 |
 |
 Backup and restore
 |
 |
 |
 |
 |
 */

void MainWindow::on_file_backup_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_backup();
}

void MainWindow::on_file_backup()
{
  save_editors();
  backup_assistant = new BackupAssistant (0);
  g_signal_connect ((gpointer) backup_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}

void MainWindow::on_file_restore_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_restore();
}

void MainWindow::on_file_restore()
{
  restore_assistant = new RestoreAssistant (0);
  g_signal_connect ((gpointer) restore_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}



/*
 |
 |
 |
 |
 |
 Git
 |
 |
 |
 |
 |
 */

void MainWindow::on_preferences_remote_repository_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_remote_repository();
}

void MainWindow::on_preferences_remote_repository()
{
  save_editors();
  remote_repository_assistant = new RemoteRepositoryAssistant (0);
  g_signal_connect ((gpointer) remote_repository_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}


void MainWindow::on_project_changes_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_project_changes();
}


void MainWindow::on_project_changes()
{
  // Save even the very latest changes.
  save_editors();
  // Do the actual changes dialog. 
  show_references_window();
  changes_assistant = new ChangesAssistant (window_references);
  g_signal_connect ((gpointer) changes_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}


void MainWindow::on_edit_revert_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_edit_revert();
}


void MainWindow::on_edit_revert()
{
  save_editors();
  RevertDialog dialog(&navigation.reference);
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_all_editors(false);
  }
}


void MainWindow::on_projects_send_receive1_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_projects_send_receive();
}


void MainWindow::on_projects_send_receive ()
{

  // Save all editors.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->chapter_save ();
  }

  GwSpawn spawn ("bibledit-git");
  spawn.async ();
  
  // Schedule send/receive for the focused project first.
  // The reason for this is so that the translators can decide which one to do first,
  // since doing all of them could take time.
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window) {
    ustring project = editor_window->project ();
    ProjectConfiguration *projectconfig = settings->projectconfig(project);
    if (projectconfig->git_use_remote_repository_get()) {
      // Schedule an update.
      ustring folder = project_data_directory_project(project);
      spawn.arg (folder);
    }
  }

  // Schedule a push and pull task for each relevant project.
  vector < ustring > projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration *projectconfig = settings->projectconfig(projects[i]);
    if (projectconfig->git_use_remote_repository_get()) {
      // Schedule an update.
      ustring folder = project_data_directory_project(projects[i]);
      spawn.arg (folder);
      maintenance_register_git_repository (folder);
    }
  }
  
  // Run the lot.
  spawn.run ();
}


/*
 |
 |
 |
 |
 |
 Fonts
 |
 |
 |
 |
 |
 */


void MainWindow::on_view_text_font_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_text_font();
}


void MainWindow::on_text_font()
{
  // Get the font and colour settings, either from the project, if it is opened, 
  // or else from genconfig.
  bool defaultfont = settings->genconfig.text_editor_font_default_get();
  ustring fontname = settings->genconfig.text_editor_font_name_get();
  unsigned int linespacing = 100;
  bool defaultcolour = settings->genconfig.text_editor_default_color_get();
  unsigned int normaltextcolour = settings->genconfig.text_editor_normal_text_color_get();
  unsigned int backgroundcolour = settings->genconfig.text_editor_background_color_get();
  unsigned int selectedtextcolour = settings->genconfig.text_editor_selected_text_color_get();
  unsigned int selectioncolour = settings->genconfig.text_editor_selection_color_get();
  WindowEditor *editor_window = last_focused_editor_window();
  if (editor_window) {
    ProjectConfiguration *projectconfig = settings->projectconfig(editor_window->project());
    defaultfont = projectconfig->editor_font_default_get();
    fontname = projectconfig->editor_font_name_get();
    linespacing = projectconfig->text_line_height_get();
    defaultcolour = settings->genconfig.text_editor_default_color_get();
    normaltextcolour = projectconfig->editor_normal_text_color_get();
    backgroundcolour = projectconfig->editor_background_color_get();
    selectedtextcolour = projectconfig->editor_selected_text_color_get();
    selectioncolour = projectconfig->editor_selection_color_get();
  }
  // Display font selection dialog. 
  FontColorDialog dialog(defaultfont, fontname, linespacing, defaultcolour, normaltextcolour, backgroundcolour, selectedtextcolour, selectioncolour);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;

  // Save font, and set it.
  settings->genconfig.text_editor_font_default_set(dialog.new_use_default_font);
  settings->genconfig.text_editor_font_name_set(dialog.new_font);
  settings->genconfig.text_editor_default_color_set(dialog.new_use_default_color);
  settings->genconfig.text_editor_normal_text_color_set(dialog.new_normal_text_color);
  settings->genconfig.text_editor_background_color_set(dialog.new_background_color);
  settings->genconfig.text_editor_selected_text_color_set(dialog.new_selected_text_color);
  settings->genconfig.text_editor_selection_color_set(dialog.new_selection_color);
  if (editor_window) {
    ProjectConfiguration *projectconfig = settings->projectconfig(editor_window->project());
    projectconfig->editor_font_default_set(dialog.new_use_default_font);
    projectconfig->editor_font_name_set(dialog.new_font);
    projectconfig->text_line_height_set(dialog.new_line_spacing);
    projectconfig->editor_default_color_set(dialog.new_use_default_color);
    projectconfig->editor_normal_text_color_set(dialog.new_normal_text_color);
    projectconfig->editor_background_color_set(dialog.new_background_color);
    projectconfig->editor_selected_text_color_set(dialog.new_selected_text_color);
    projectconfig->editor_selection_color_set(dialog.new_selection_color);
  }
  set_fonts();
}


void MainWindow::set_fonts()
{
  // Set font in the text editors. Set text direction too.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->set_font();
    editor_windows[i]->create_or_update_formatting_data();
  }
  // Set fonts in the project notes window.
  if (window_notes) {
    window_notes->set_fonts();
  }
  // Set fonts in the references window.
  if (window_references) {
    window_references->set_fonts();
  }
  // Set fonts in the window for checking keyterms.
  if (window_check_keyterms) {
    window_check_keyterms->set_fonts();
  }
}


/*
 |
 |
 |
 |
 |
 Outline
 |
 |
 |
 |
 |
 */


void MainWindow::on_view_outline_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_outline();
}


void MainWindow::on_view_outline()
{
  on_window_outline_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_outline))) {
    window_outline = new WindowOutline(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_outline->delete_signal_button, "clicked", G_CALLBACK(on_window_outline_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_outline->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_outline->outline->reference_changed_signal, "clicked", G_CALLBACK(on_button_outline_clicked), gpointer(this));
    window_outline->go_to(settings->genconfig.project_get(), navigation.reference);
  }
}


void MainWindow::on_window_outline_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_outline_delete_button();
}

void MainWindow::on_window_outline_delete_button()
{
  if (window_outline) {
    delete window_outline;
    window_outline = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_outline), FALSE);
  }
}


void MainWindow::on_button_outline_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_button_outline();
}


void MainWindow::on_button_outline()
{
  if (window_outline) {
    Reference reference(navigation.reference);
    reference.chapter_set(window_outline->outline->newchapter);
    reference.verse_set(convert_to_string(window_outline->outline->newverse));
    navigation.display(reference);
  }
}


/*
 |
 |
 |
 |
 |
 Planning
 |
 |
 |
 |
 |
 */

void MainWindow::on_edit_planning_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_edit_planning();
}

void MainWindow::on_edit_planning()
{
  PlanningEditDialog dialog (navigation.reference.book_get(), navigation.reference.chapter_get());
  dialog.run ();
}

void MainWindow::on_view_planning_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_planning();
}

void MainWindow::on_view_planning()
{
  ViewPlanningDialog dialog(0);
  dialog.run();
}

void MainWindow::on_preferences_planning_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_planning();
}

void MainWindow::on_preferences_planning()
{
  PlanningSetupDialog dialog(0);
  dialog.run();
}

/*
 |
 |
 |
 |
 |
 Resources
 |
 |
 |
 |
 |
 */

void MainWindow::on_window_resource_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_resource_delete_button(button);
}

void MainWindow::on_window_resource_delete_button(GtkButton * button)
{
  GtkWidget *widget = GTK_WIDGET(button);
  vector < WindowResource * >::iterator iterator = resource_windows.begin();
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (widget == resource_windows[i]->delete_signal_button) {
      delete resource_windows[i];
      resource_windows.erase(iterator);
      break;
    }
    iterator++;
  }
}

void MainWindow::on_file_resources_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources();
}

WindowResource *MainWindow::last_focused_resource_window()
// Get the focused resource window, or NULL if there's none.
{
  WindowResource *resource_window = NULL;
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (resource_windows[i]->focus_in_signal_button == focused_resource_button) {
      resource_window = resource_windows[i];
    }
  }
  return resource_window;
}

void MainWindow::on_file_resources()
// Set some menu options sensitive if a resource is open.
{
  gtk_widget_set_sensitive(file_resources_close, !resource_windows.empty());
  gtk_widget_set_sensitive(file_resources_edit, !resource_windows.empty());
}

void MainWindow::on_file_resources_open_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources_open("", false);
}

void MainWindow::on_file_resources_open(ustring resource, bool startup)
// Opens a resource.
{
  // If no resource is given, select a new one.
  if (resource.empty ()) {
    resource = resource_select (NULL);
  }
  if (resource.empty())
    return;

  // Find data about the resource, and whether it exists.
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  ustring filename;
  for (unsigned int i = 0; i < resources.size(); i++) {
    if (resource == resources[i]) {
      filename = filenames[i];
    }
  }
  if (filename.empty())
    return;

  // If the resource already displays, bail out.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (resource == resource_windows[i]->name) {
      return;
    }
  }

  // Display a new resource.
  WindowResource *resource_window = new WindowResource(resource, layout, accelerator_group, startup);
  g_signal_connect((gpointer) resource_window->delete_signal_button, "clicked", G_CALLBACK(on_window_resource_delete_button_clicked), gpointer(this));
  g_signal_connect((gpointer) resource_window->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
  resource_window->go_to(navigation.reference);
  resource_windows.push_back(resource_window);
}

void MainWindow::on_file_resources_close_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources_close();
}

void MainWindow::on_file_resources_close()
// Closes the focused resource.
{
  WindowResource *focused_window = last_focused_resource_window();
  if (focused_window) {
    on_window_resource_delete_button(GTK_BUTTON(focused_window->delete_signal_button));
  }
}

void MainWindow::on_file_resources_new_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources_new();
}

void MainWindow::on_file_resources_new()
{
  // Start the assistant.
  resource_assistant = new ResourceAssistant ("");
  g_signal_connect ((gpointer) resource_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}

void MainWindow::on_file_resources_edit_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources_edit();
}

void MainWindow::on_file_resources_edit()
{
  WindowResource *focused_resource_window = last_focused_resource_window();
  if (focused_resource_window) {
    ustring templatefile = focused_resource_window->resource->template_get();
    resource_assistant = new ResourceAssistant (templatefile);
    g_signal_connect ((gpointer) resource_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
  }
}

void MainWindow::on_file_resources_delete_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_resources_delete();
}

void MainWindow::on_file_resources_delete()
// Delete a resource.
{
  vector < ustring > filenames;
  vector < ustring > resources = resource_get_resources(filenames, false);
  ListviewDialog dialog(_("Delete resource"), resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    int result = gtkw_dialog_question(NULL, _("Are you sure you want to delete resource ") + dialog.focus + "?");
    if (result == GTK_RESPONSE_YES) {
      ustring filename;
      for (unsigned int i = 0; i < resources.size(); i++) {
        if (dialog.focus == resources[i]) {
          filename = filenames[i];
        }
      }
      // There are two methods of deleting resources:
      if (resource_add_name_to_deleted_ones_if_standard_template(filename)) {
        // 1. A template that comes with bibledit: We can't delete this as we don't 
        // have write access to the folder where it is stored. Therefore 
        // we "delete" it by placing it in a file that lists deleted
        // resources.
      } else {
        // 2. A user-generated resource: Just delete it.
        ustring directory = gw_path_get_dirname(filename);
        unix_rmdir(directory);
      }
    }
  }
}

/*
 |
 |
 |
 |
 |
 Text Editors
 |
 |
 |
 |
 |
 */
void MainWindow::on_view_tile_windows_activate(GtkMenuItem * menuitem, gpointer user_data)
{
	((MainWindow *) user_data)->on_view_tile_windows();
}

void MainWindow::on_view_tile_windows()
// This function tiles all open windows
{
  guint width, height, x=0, y = 0;
  gtk_layout_get_size (GTK_LAYOUT (layout), &width, &height);

  // Resize each editor window
   for (unsigned int i=0;i<editor_windows.size();i++) {
     FloatingWindow *floating_window = (FloatingWindow *) editor_windows[i];
     GdkRectangle rectangle = floating_window->rectangle_get ();
     rectangle.height = height;
     rectangle.width = (width / editor_windows.size())*0.666;
     rectangle.x = x;
     rectangle.y = 0;
     floating_window->rectangle_set(rectangle);
     x += rectangle.width;
     y = rectangle.height;
   }

  // Reset the Y field, as it is no longer relevant to the remaining windows
  y = 0;

  // Resize the remaining windows
  for (unsigned int i=0;i<settings->session.open_floating_windows.size();i++) {
    FloatingWindow *floating_window = (FloatingWindow *) settings->session.open_floating_windows[i];
    if (floating_window->window_id != widEditor) {
      GdkRectangle rectangle = floating_window->rectangle_get ();
      rectangle.width = (settings->session.open_floating_windows.size()>editor_windows.size()) ? width * 0.333 : width;
      rectangle.height = height / (settings->session.open_floating_windows.size() - editor_windows.size());
      rectangle.x = x;
      rectangle.y = y;
      y += rectangle.height;
      floating_window->rectangle_set(rectangle);
   }
 }
}
void MainWindow::on_window_editor_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_editor_delete_button(button);
}


void MainWindow::on_window_editor_delete_button(GtkButton * button)
{
  GtkWidget *widget = GTK_WIDGET(button);
  vector < WindowEditor * >::iterator iterator = editor_windows.begin();
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (widget == editor_windows[i]->delete_signal_button) {
      delete editor_windows[i];
      editor_windows.erase(iterator);
      break;
    }
    iterator++;
  }
  // When one of two or more editor windows is closed,
  // a remaining one does not always focus. Focus one here.
  if (!editor_windows.empty()) {
    editor_windows[0]->focus_set();
  }
  handle_editor_focus();
}


WindowEditor *MainWindow::last_focused_editor_window()
// Get the focused editor window, or NULL if there's none.
{
  WindowEditor *editor_window = NULL;
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focus_in_signal_button == focused_editor_button) {
      editor_window = editor_windows[i];
    }
  }
  return editor_window;
}


void MainWindow::on_file_project_open(const ustring & project, bool startup)
// Opens an editor.
{
  // If the editor already displays, present it and bail out.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (project == editor_windows[i]->title) {
      editor_windows[i]->focus_set();
      return;
    }
  }

  // Display a new editor.
  WindowEditor *editor_window = new WindowEditor(project, layout, accelerator_group, startup);
  g_signal_connect((gpointer) editor_window->delete_signal_button, "clicked", G_CALLBACK(on_window_editor_delete_button_clicked), gpointer(this));
  g_signal_connect((gpointer) editor_window->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
  g_signal_connect((gpointer) editor_window->new_verse_signal, "clicked", G_CALLBACK(on_new_verse_signalled), gpointer(this));
  g_signal_connect((gpointer) editor_window->new_styles_signal, "clicked", G_CALLBACK(on_editor_style_changed), gpointer(this));
  g_signal_connect((gpointer) editor_window->quick_references_button, "clicked", G_CALLBACK(on_show_quick_references_signal_button_clicked), gpointer(this));
  g_signal_connect((gpointer) editor_window->word_double_clicked_signal, "clicked", G_CALLBACK(on_send_word_to_toolbox_signalled), gpointer(this));
  g_signal_connect((gpointer) editor_window->reload_signal, "clicked", G_CALLBACK(on_editor_reload_clicked), gpointer(this));
  g_signal_connect((gpointer) editor_window->changed_signal, "clicked", G_CALLBACK(on_editor_changed_clicked), gpointer(this));
  g_signal_connect((gpointer) editor_window->spelling_checked_signal, "clicked", G_CALLBACK(on_editor_spelling_checked_button_clicked), gpointer(this));
  editor_windows.push_back(editor_window);

  // After creation the window should generate a focus signal, 
  // and this signal in turn will cause further processing of the editor.
  on_window_focus_button(GTK_BUTTON(editor_window->focus_in_signal_button));
}

void MainWindow::on_editor_reload_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->reload_all_editors(true);
}

void MainWindow::debug_view(ustring txt, GtkRadioMenuItem *menuitem, viewType vt)
{
  ustring menu = "none";
  if (menuitem == (GtkRadioMenuItem*)view_formatted) { menu = "formatted"; }
  else if (menuitem == (GtkRadioMenuItem*)view_usfm_code) { menu = "view_usfm_code"; }
  else if (menuitem == (GtkRadioMenuItem*)view_experimental) { menu = "experimental"; }
  DEBUG(txt+":menuitem="+menu);
  bool formatted = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_formatted));
  bool usfm = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_usfm_code));
  bool experi = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(view_experimental));
  DEBUG("  formatted\t= "+std::to_string(formatted));
  DEBUG("  usfm\t\t= "+std::to_string(usfm));
  DEBUG("  experimental\t= "+std::to_string(experi));
  switch (vt) {
  case vtNone:
    break;
  case vtFormatted:
    if (!formatted) { DEBUG("vtFormatted but view_formatted not active!!") }
    break;
  case vtUSFM:
    if (!usfm) { DEBUG("vtUSFM but view_usfm_code not active!!") }
    break;
  case vtExperimental:
    if (!experi) { DEBUG("vtExperimental but view_experimental not active!!") }
    break;
  }
}

void MainWindow::handle_editor_focus()
{
  // Get the focused editor and the project.
  WindowEditor *editor_window = last_focused_editor_window();

  ustring project;
  if (editor_window) {
    project = editor_window->project();
  }

  // Bail out if there's no change in focused project.
  // In other words, if this routine is called because the user
  // clicked another project window, then keep on going. If it
  // is called because the view was changed on the same project,
  // then let the caller handle the change in state of the
  // radio buttons.
  if (project == previously_focused_project_name) {
    return;
  }
  previously_focused_project_name = project;

  // Set the focused project in the configuration.
  settings->genconfig.project_set(project);

  // Enable or disable widgets depending on whether an editor window is focused.
  enable_or_disable_widgets(editor_window);

  // Inform the merge window, if it is there, about the editors.
  if (window_merge) {
    window_merge->set_focused_editor();
    vector <ustring> open_projects;
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      open_projects.push_back(editor_windows[i]->project());
    }
    window_merge->set_open_projects(open_projects);
  }

  // Set the state for the radio-button menu group depending on what
  // view the current focused editor is in (formatted, usfm, etc.)
  viewType vt = vtNone;
  if (editor_window) {
    vt = editor_window->vt_get();
  }
  // Turn on one checkmark (and because they are 'grouped', the others
  // in the View|Chapter as| submenu will be turned off automagically).
  // Turn off signal handling for the submenu radio items while we do this...
  // g_signal_handlers_block_by_func(view_formatted, G_CALLBACK(on_view_chapteras_activate), gpointer(this));
  // g_signal_handlers_block_by_func(view_usfm_code, G_CALLBACK(on_view_chapteras_activate), gpointer(this));
  // g_signal_handlers_block_by_func(view_experimental, G_CALLBACK(on_view_chapteras_activate), gpointer(this));
  switch (vt) {
  case vtNone: // do nothing
    break;
  case vtFormatted:
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_formatted), TRUE);
    break;
  case vtUSFM:
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_usfm_code), TRUE);
    break;
  case vtExperimental:
    gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_experimental), TRUE);
    break;
  }
  // Turn signals back on for this submenu
  // g_signal_handlers_unblock_by_func(view_formatted, G_CALLBACK(on_view_chapteras_activate), gpointer(this));
  // g_signal_handlers_unblock_by_func(view_usfm_code, G_CALLBACK(on_view_chapteras_activate), gpointer(this));
  // g_signal_handlers_unblock_by_func(view_experimental, G_CALLBACK(on_view_chapteras_activate), gpointer(this));

  //debug_view("4", 0x0, vt);

  // Inform the check USFM window about the focused editor.
  check_usfm_window_ping ();
  
  // Set the title of the main window to include the project.
  ustring title = "Bibledit-Gtk";
  if (!project.empty()) {
    title.append (" - ");
    title.append (project);
  }
  gtk_window_set_title(GTK_WINDOW(window_main), title.c_str());

  // If we've no project bail out.
  if (project.empty())
    return;

  // Project configuration.
  ProjectConfiguration *projectconfig = settings->projectconfig(project);

  // The font and colour are tied to the project, 
  // but also stored in the general configuration.
  settings->genconfig.text_editor_font_default_set(projectconfig->editor_font_default_get());
  settings->genconfig.text_editor_font_name_set(projectconfig->editor_font_name_get());
  settings->genconfig.text_editor_default_color_set(projectconfig->editor_default_color_get());
  settings->genconfig.text_editor_normal_text_color_set(projectconfig->editor_normal_text_color_get());
  settings->genconfig.text_editor_background_color_set(projectconfig->editor_background_color_get());
  settings->genconfig.text_editor_selected_text_color_set(projectconfig->editor_selected_text_color_get());
  settings->genconfig.text_editor_selection_color_set(projectconfig->editor_selection_color_get());

  // Re-initialize Navigation.
  navigation.set_project(project, false);
  Reference reference(settings->genconfig.book_get(), convert_to_int(settings->genconfig.chapter_get()), settings->genconfig.verse_get());
  navigation.display(reference);

  // Set the available books for search/replace functions.
  vector < unsigned int >books = project_get_books(project);
  set < unsigned int >selection(books.begin(), books.end());
  settings->session.selected_books = selection;

  // Redisplay the project notes.
  notes_redisplay();
}

void MainWindow::save_editors()
// Save all and any editors.
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->chapter_save();
  }
}


void MainWindow::goto_next_previous_project(bool next)
{
  // Bail out if there are not enough windows to switch.
  if (editor_windows.size() < 2)
    return;

  // Get the focused project window and its offset.
  WindowEditor *present_window = last_focused_editor_window();
  int offset = 0;
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (present_window == editor_windows[i]) {
      offset = i;
    }
  }

  // Move offset to next (or previous) window.
  if (next) {
    offset++;
    if ((unsigned int)(offset) >= editor_windows.size())
      offset = 0;
  } else {
    offset--;
    if (offset < 0)
      offset = editor_windows.size() - 1;
  }

  // Focus the new window.
  editor_windows[offset]->focus_set();
}


void MainWindow::on_editor_changed_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_editor_changed();
}


void MainWindow::on_editor_changed()
{
  if (window_merge) {
    window_merge->editors_changed();
  }
  check_usfm_window_ping ();
  // Optionally signal the keyterms checker.
  if (window_check_keyterms) {
    WindowEditor *editor_window = last_focused_editor_window();
    if (editor_window) {
      Editor2 * editor = editor_window->editor_get();
      window_check_keyterms->text_changed(editor);
    }
  }
}

void MainWindow::on_view_formatted_toggled(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_formatted_togg();
}

void MainWindow::on_view_formatted_togg()
{
  //DEBUG("View|Formatted toggled")
}

void MainWindow::on_view_usfm_code_toggled(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_usfm_code_togg();
}

void MainWindow::on_view_usfm_code_togg()
{
  //DEBUG("View|USFM toggled")
}

void MainWindow::on_view_experimental_toggled(GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_experimental_togg();
}

void MainWindow::on_view_experimental_togg()
{
  //DEBUG("View|Experimental toggled")
}

void MainWindow::on_view_chapteras_activate(GtkRadioMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_view_chapteras(menuitem);
}

// Called when a view is toggled
void MainWindow::on_view_chapteras(GtkRadioMenuItem *menuitem)
{
  // When you select a view from the View|Chapter as submenu, this routine is 
  // called twice: once for the view that is on, to indicate it is "activated" off, 
  // and once for the new view to be turned on.

  bool radioButtonIsOn = gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(menuitem));

  viewType vt = vtNone;
  if (menuitem == (GtkRadioMenuItem *)view_formatted) {
    vt = vtFormatted;
  }
  else if (menuitem == (GtkRadioMenuItem *)view_usfm_code) {
    vt = vtUSFM;
  }
  else if (menuitem == (GtkRadioMenuItem *)view_experimental) {
    vt = vtExperimental;
  }
  //debug_view("1", menuitem, vt);
  WindowEditor *editor_window = last_focused_editor_window();
  if (radioButtonIsOn && editor_window) {
    editor_window->vt_set(vt);
    //debug_view("2", menuitem, vt);
    // There are objects that act on USFM view or formatted view only.
    // Inform these about a possible change.
    handle_editor_focus();
    //debug_view("3", menuitem, vt);
  }
}

void MainWindow::reload_all_editors(bool take_chapter_from_focused_editor)
{
  // Get the focused editor, if none, bail out.
  WindowEditor *editor_window = last_focused_editor_window();
  if (!editor_window)
    return;

  // Store the reference where to go to after the project has been reloaded.
  Reference reference(navigation.reference);

  // If needed take the chapter number from the currently focused project.
  if (take_chapter_from_focused_editor) {
    reference.chapter_set(editor_window->reload_chapter_number());
    if (editor_window->reload_chapter_number() == 0) {
      reference.verse_set("0");
    }
  }
  
  // Get the project.
  ustring project = editor_window->project();

  // As anything could have happened to the data in the project, force a reload of the navigator.
  navigation.set_project(project, true);

  // Reload all editors.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->chapter_load(reference.chapter_get());
  }
  
  // Go to the right reference.
  navigation.display(reference);
}


/*
 |
 |
 |
 |
 |
 Merge
 |
 |
 |
 |
 |
 */


void MainWindow::on_file_projects_merge_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_projects_merge();
}

void MainWindow::on_file_projects_merge()
{
  on_window_merge_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(file_projects_merge))) {
    window_merge = new WindowMerge(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_merge->delete_signal_button, "clicked", G_CALLBACK(on_window_merge_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_merge->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_merge->editors_get_text_button, "clicked", G_CALLBACK(on_merge_window_get_text_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_merge->new_reference_button, "clicked", G_CALLBACK(on_merge_window_new_reference_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_merge->save_editors_button, "clicked", G_CALLBACK(on_merge_window_save_editors_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_merge->reload_editors_button, "clicked", G_CALLBACK(on_editor_reload_clicked), gpointer(this));
    vector < ustring> open_projects;
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      open_projects.push_back(editor_windows[i]->project());
    }
    window_merge->set_open_projects(open_projects);
  }
}

void MainWindow::on_window_merge_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_merge_delete_button();
}

void MainWindow::on_window_merge_delete_button()
{
  if (window_merge) {
    delete window_merge;
    window_merge = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(file_projects_merge), FALSE);
  }
}

void MainWindow::on_merge_window_get_text_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_merge_window_get_text_button();
}

void MainWindow::on_merge_window_get_text_button()
{
  if (window_merge) {
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      if (editor_windows[i]->title == window_merge->current_master_project) {
        window_merge->main_project_data = editor_windows[i]->get_chapter();
      }
      if (editor_windows[i]->title == window_merge->current_edited_project) {
        window_merge->edited_project_data = editor_windows[i]->get_chapter();
      }
    }
    window_merge->book = navigation.reference.book_get();
    window_merge->chapter = navigation.reference.chapter_get();
  }
}

void MainWindow::on_merge_window_new_reference_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_merge_window_new_reference_button();
}

void MainWindow::on_merge_window_new_reference_button()
{
  if (window_merge) {
    Reference reference(window_merge->book, window_merge->chapter, "0");
    navigation.display(reference);
  }
}

void MainWindow::on_merge_window_save_editors_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_merge_window_save_editors_button();
}

void MainWindow::on_merge_window_save_editors_button()
{
  save_editors();
}

/*
 |
 |
 |
 |
 |
 Diglot
 |
 |
 |
 |
 |
 */

void MainWindow::on_preferences_filters_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_preferences_filters();
}

void MainWindow::on_preferences_filters()
{
  FiltersDialog dialog(0);
  dialog.run();
}

/*
 |
 |
 |
 |
 |
 Print
 |
 |
 |
 |
 |
 */

void MainWindow::on_print_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_print();
}

void MainWindow::on_print()
{
  // Bail out if no project is open.
  WindowEditor *editorwindow = last_focused_editor_window();
  if (!editorwindow)
    return;

  // Create the selection dialog using the saved selection.
  unsigned int selection;
  {
    vector <ustring> labels;
    labels.push_back(_("Project (through internal typesetter)"));
    labels.push_back(_("Project (through ptx2pdf typesetter)"));
    labels.push_back(_("Parallel Bible"));
    labels.push_back(_("References"));
    //labels.push_back("Test usfm2pdf");
    RadiobuttonDialog dialog(_("Print"), _("Select what to print"), labels, settings->genconfig.print_job_get(), false);
    if (dialog.run() != GTK_RESPONSE_OK) {
      return;
	}
    selection = dialog.selection;
    settings->genconfig.print_job_set(selection);
  }

  // Save the editors.
  save_editors();

  switch (selection) {
    case 0: // Project through internal typesetter.
    {
      {
        PrintProjectDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      ProjectMemory projectmemory(settings->genconfig.project_get(), true);
      PrintProject printproject(&projectmemory);
      printproject.print();
      break;
    }
    case 1: // Project through XeTeX
    {
      {
        XeTeXDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      ProjectMemory projectmemory(settings->genconfig.project_get(), true);
      PrintProject2 printproject2(&projectmemory);
      printproject2.print();
      break;
    }
    case 2: // Parallel Bible.
    {
      {
        ParallelBibleDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      view_parallel_bible_pdf();
      break;
    }
    case 3: // References.
    {
      // Activate references.
      show_references_window();
      // Show dialog.
      {
        PrintReferencesDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      // Load refs from the references window.
      show_references_window();
      vector <Reference> refs = window_references->get();
      if (refs.empty()) {
        gtkw_dialog_info(window_main, _("There are no references to print"));
      } else {
        // Run the function for printing the references.
        vector < ustring > extra_projects = settings->genconfig.print_references_projects_get();
        ProjectMemory projectmemory(settings->genconfig.project_get(), true);
        view_parallel_references_pdf(projectmemory, &extra_projects, refs, true, NULL, true);
      }
      break;
    }
  }
}


/*
 |
 |
 |
 |
 |
 Accelerators
 |
 |
 |
 |
 |
 */


void MainWindow::accelerator_undo_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->menu_accelerator_undo();
}

void MainWindow::accelerator_redo_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->menu_accelerator_redo();
}

void MainWindow::accelerator_cut_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_cut();
}

void MainWindow::accelerator_copy_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_copy();
}

void MainWindow::accelerator_paste_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_paste();
}

void MainWindow::accelerator_new_project_note_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_new_note();
}

void MainWindow::accelerator_next_verse_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_next_verse();

}

void MainWindow::accelerator_previous_verse_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_previous_verse();
}

void MainWindow::accelerator_next_chapter_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_next_chapter();
}

void MainWindow::accelerator_previous_chapter_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_previous_chapter();
}

void MainWindow::accelerator_next_book_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_next_book();
}

void MainWindow::accelerator_previous_book_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_previous_book();
}

void MainWindow::accelerator_next_reference_in_history_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->accelerator_next_reference_in_history();
}

void MainWindow::accelerator_next_reference_in_history()
{
  navigation.on_forward();
}

void MainWindow::accelerator_previous_reference_in_history_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->accelerator_previous_reference_in_history();
}

void MainWindow::accelerator_previous_reference_in_history()
{
  navigation.on_back();
}

void MainWindow::accelerator_go_to_reference_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_reference_interactive();
}

void MainWindow::accelerator_close_window_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->accelerator_close_window();
}


void MainWindow::accelerator_close_window()
// Closes the focused window.
{
  // Keyterms in verse. 
  if (window_show_related_verses) {
    if (window_show_related_verses->focused) {
      on_window_show_related_verses_delete_button();
    }
  }
  // Merge
  if (window_merge) {
    if (window_merge->focused) {
      on_window_merge_delete_button();
    }
  }
  // Resources.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    WindowResource *resource_window = resource_windows[i];
    if (resource_window->focused) {
      on_window_resource_delete_button(GTK_BUTTON(resource_window->delete_signal_button));
      break;
    }
  }
  // Outline.
  if (window_outline) {
    if (window_outline->focused) {
      on_window_outline_delete_button();
    }
  }
  // Check keyterms.
  if (window_check_keyterms) {
    if (window_check_keyterms->focused) {
      on_window_check_keyterms_delete_button();
    }
  }
  // Styles.
  if (window_styles) {
    if (window_styles->focused) {
      on_window_styles_delete_button();
    }
  }
  // Notes.
  if (window_notes) {
    if (window_notes->focused) {
      on_window_notes_delete_button();
    }
  }
  // References.
  if (window_references) {
    if (window_references->focused) {
      on_window_references_delete_button();
    }
  }
  // Editors.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    WindowEditor *editor_window = editor_windows[i];
    if (editor_window->focused) {
      on_window_editor_delete_button(GTK_BUTTON(editor_window->delete_signal_button));
      break;
    }
  }
  handle_editor_focus();

  // Check USFM.
  if (window_check_usfm) {
    if (window_check_usfm->focused) {
      on_window_check_usfm_delete_button();
    }
  }
}

void MainWindow::accelerator_goto_styles_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_goto_styles_area();
}

void MainWindow::accelerator_quit_program_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->initiate_shutdown();
}

void MainWindow::accelerator_activate_text_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_text_area_activate();
}

void MainWindow::accelerator_activate_tools_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_tools_area_activate();
}

void MainWindow::accelerator_activate_notes_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_notes_area_activate();
}

void MainWindow::accelerator_next_reference_in_reference_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_next_reference();
}

void MainWindow::accelerator_previous_reference_in_reference_area_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_previous_reference();
}

void MainWindow::accelerator_next_project_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_next_previous_project(true);
}

void MainWindow::accelerator_previous_project_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->goto_next_previous_project(false);
}


void MainWindow::accelerator_open_project_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->open();
}


void MainWindow::accelerator_print_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_print();
}


void MainWindow::accelerator_find_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->menu_findspecial();
}


void MainWindow::accelerator_replace_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->menu_replace();
}


void MainWindow::accelerator_main_help_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_help_main();
}

// For F2 accelerator
void MainWindow::accelerator_main_jumpto_callback(gpointer user_data)
{
  ((MainWindow *) user_data)->on_jumpto_main();
}


void MainWindow::accelerator_view_usfm_code(gpointer user_data)
{
  ((MainWindow *) user_data)->accelerator_view_usfm_code_toggle();
}

// Toggle between USFM and Formatted views. If in any other view, go to 
// USFM view.
void MainWindow::accelerator_view_usfm_code_toggle()
{
  bool active = gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (view_usfm_code));
  if (active) {
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_formatted), TRUE);
  }
  else {
	gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (view_usfm_code), TRUE);
  }
}


/*
 |
 |
 |
 |
 |
 Shutdown
 |
 |
 |
 |
 |
 */


bool MainWindow::on_window_delete_event(GtkWidget * widget, GdkEvent * event, gpointer user_data)
{
  ((MainWindow *) user_data)->initiate_shutdown();
  // Prevent the window from being deleted right now.
  // This prevents crashes.
  // The window will be deleted later on.
  return true;
}


void MainWindow::initiate_shutdown()
// Starts the shutdown sequence.
{
  // Bail out if we are already shutting down.
  if (shutting_down) { return; }

  shutting_down = true;

  // Shut down after a delay.
  g_timeout_add(10, GSourceFunc(gtk_main_quit), NULL);
}


/*
 |
 |
 |
 |
 |
 Assistants
 |
 |
 |
 |
 |
 */


void MainWindow::on_assistant_ready_signal (GtkButton *button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_assistant_ready();
}


void MainWindow::on_assistant_ready ()
// This handles the situation that any assistant is ready.
{
  // Importing keyterms.
  if (import_keyterms_assistant) {
    delete import_keyterms_assistant;
    import_keyterms_assistant = NULL;
    // Refresh window for checking keyterms.
    if (window_check_keyterms) {
      window_check_keyterms->reload_collections();
    }
  }

  // Deleting keyterms.
  if (delete_keyterms_assistant) {
    delete delete_keyterms_assistant;
    delete_keyterms_assistant = NULL;
    // Refresh window for checking keyterms.
    if (window_check_keyterms) {
      window_check_keyterms->reload_collections();
    }
  }

  // Changes.
  if (changes_assistant) {
    delete changes_assistant;
    changes_assistant = NULL;
  }
  
  // Remote repository setup.
  if (remote_repository_assistant) {
    delete remote_repository_assistant;
    reload_all_editors(false);
    remote_repository_assistant = NULL;
  }

  // Resource editor.
  if (resource_assistant) {
    ustring new_resource = resource_assistant->new_resource_get();
    if (!new_resource.empty()) {
      // The resource was created: open it.
      on_file_resources_open (new_resource, false);
    } else {
      // The resource was edited: refresh ito
      WindowResource *focused_resource_window = last_focused_resource_window();
      if (focused_resource_window) {
        focused_resource_window->resource->open(resource_assistant->edited_resource_get());
      }
    }
    delete resource_assistant;
    resource_assistant = NULL;
  }

  // Backup.
  if (backup_assistant) {
    delete backup_assistant;
    backup_assistant = NULL;
  }

  // Restore.
  if (restore_assistant) {
    delete restore_assistant;
    restore_assistant = NULL;
  }

  // Export.
  if (export_assistant) {
    delete export_assistant;
    export_assistant = NULL;
  }

  // Import.
  if (import_assistant) {
    reload_all_editors(false);
    bool import_notes = import_assistant->import_notes;
    bool import_keyterms = import_assistant->import_keyterms;
    delete import_assistant;
    import_assistant = NULL;
    if (import_notes) {
      on_import_notes ();
    }
    if (import_keyterms) {
      on_keyterms_import();
    }
  }
}


/*
 |
 |
 |
 |
 |
 Check USFM
 |
 |
 |
 |
 |
 */


void MainWindow::on_check_usfm_activate(GtkMenuItem * menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_check_usfm();
}

void MainWindow::on_check_usfm()
{
  on_window_check_usfm_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(check_usfm))) {
    window_check_usfm = new WindowCheckUSFM(layout, accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect((gpointer) window_check_usfm->delete_signal_button, "clicked", G_CALLBACK(on_window_check_usfm_delete_button_clicked), gpointer(this));
    g_signal_connect((gpointer) window_check_usfm->focus_in_signal_button, "clicked", G_CALLBACK(on_window_focus_button_clicked), gpointer(this));
    handle_editor_focus();
    on_editor_changed();
  }
}

void MainWindow::on_window_check_usfm_delete_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_check_usfm_delete_button();
}

void MainWindow::on_window_check_usfm_delete_button()
{
  if (window_check_usfm) {
    delete window_check_usfm;
    window_check_usfm = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check_usfm), FALSE);
  }
}

void MainWindow::check_usfm_window_ping()
// Inform the check USFM window about the focused editor.
{
  if (!window_check_usfm)
    return;
  WindowEditor *editor_window = last_focused_editor_window();
  ustring project;
  unsigned int book = 0;
  unsigned int chapter = 0;
  GtkTextBuffer * focused_textbuffer = NULL;
  if (editor_window) {  
    project = editor_window->project();
    focused_textbuffer = editor_window->edit_usfm_textbuffer();
    book = editor_window->book();
    chapter = editor_window->chapter();
  }  
  window_check_usfm->set_parameters(focused_textbuffer, project, book, chapter);
}


/*
 |
 |
 |
 |
 |
 Import
 |
 |
 |
 |
 |
 */


void MainWindow::on_file_import_activate (GtkMenuItem *menuitem, gpointer user_data)
{
  ((MainWindow *) user_data)->on_file_import();
}


void MainWindow::on_file_import ()
{
  save_editors();
  WindowStyles * styles_window = window_styles;
  if (!style_management_enabled) 
    styles_window = NULL;
  WindowReferences * references_window = window_references;
  if (!project_notes_management_enabled)
    references_window = NULL;
  import_assistant = new ImportAssistant (references_window, styles_window, window_check_keyterms, windowsoutpost);
  g_signal_connect ((gpointer) import_assistant->signal_button, "clicked", G_CALLBACK (on_assistant_ready_signal), gpointer (this));
}


/*
 |
 |
 |
 |
 |
 Floating windows
 |
 |
 |
 |
 |
 */


void MainWindow::on_scrolledwindow_layout_size_allocate (GtkWidget *widget, GdkRectangle *allocation, gpointer user_data)
{
  ((MainWindow *) user_data)->scrolledwindow_layout_size_allocate(allocation);
}


void MainWindow::scrolledwindow_layout_size_allocate (GdkRectangle *allocation)
// The size of the parent scrolled window determines the size of the layout.
{
  // Get current layout size.
  guint width, height;
  gtk_layout_get_size (GTK_LAYOUT (layout), &width, &height);
  // Only proceed if there's a change.
  if ((allocation->width != (gint) (width + 3)) || (allocation->height != (gint) (height + 3))) {
    // Set new size.
    width = allocation->width - 3;
    height = allocation->height - 3;
    gtk_layout_set_size (GTK_LAYOUT (layout), width, height);
  }
}


bool MainWindow::on_windows_startup_timeout(gpointer data)
{
  return ((MainWindow *) data)->on_windows_startup();
}


bool MainWindow::on_windows_startup()
{
  // Get all window data.
  WindowData window_data(false);

  bool window_started = false;
  while ((windows_startup_pointer < window_data.ids.size()) && !window_started) {
    if (window_data.shows[windows_startup_pointer]) {
      WindowID id = WindowID(window_data.ids[windows_startup_pointer]);
      ustring title = window_data.titles[windows_startup_pointer];
      switch (id) {
      case widShowRelatedVerses:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_related_verses), TRUE);
          break;
        }
      case widMerge:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(file_projects_merge), TRUE);
          break;
        }
      case widResource:
        {
          on_file_resources_open(title, true);
          break;
        }
      case widOutline:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(view_outline), TRUE);
          break;
        }
      case widCheckKeyterms:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check_key_terms), TRUE);
          break;
        }
      case widStyles:
        {
          on_goto_styles_area();
          break;
        }
      case widNotes:
        {
          view_project_notes();
          break;
        }
      case widReferences:
        {
          show_references_window();
          break;
        }
      case widEditor:
        {
          on_file_project_open(title, true);
          break;
        }
      case widMenu:
        {
          break;
        }
      case widShowVerses:
        {
          // The window was removed. Xiphos and similar programs provide this functionality much better
          break;
        }
      case widCheckUSFM:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check_usfm), TRUE);
          break;
        }
      case widSourceLanguages:
        {
          // The window was removed since the external applications provide source text and do it much better than Bibledit.
          break;
        }
      }
      window_started = true;
    }
    windows_startup_pointer++;
  }
  if (windows_startup_pointer < window_data.ids.size()) {
    return true;
  } else {
    windows_startup_pointer = G_MAXINT;
  }

  // At the end of all focus the right editor, the one that had focus last time on shutdown.
  if (focused_project_last_session.empty()) {
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      if (focused_project_last_session == editor_windows[i]->title) {
        editor_windows[i]->focus_set();
      }
    }
    focused_project_last_session.clear();
  }
  // We're through.
  return false;
}


void MainWindow::shutdown_windows()
// Shut any open windows down.
{
  // Related verses. 
  if (window_show_related_verses) {
    window_show_related_verses->shutdown();
    delete window_show_related_verses;
    window_show_related_verses = NULL;
  }
  // Merge
  if (window_merge) {
    window_merge->shutdown();
    delete window_merge;
    window_merge = NULL;
  }
  // Resources.
  while (!resource_windows.empty()) {
    WindowResource *resource_window = resource_windows[0];
    resource_window->shutdown();
    delete resource_window;
    resource_windows.erase(resource_windows.begin());
  }

  // Outline.
  if (window_outline) {
    window_outline->shutdown();
    delete window_outline;
    window_outline = NULL;
  }
  // Check keyterms.
  if (window_check_keyterms) {
    window_check_keyterms->shutdown();
    delete window_check_keyterms;
    window_check_keyterms = NULL;
  }
  // Styles.
  if (window_styles) {
    window_styles->shutdown();
    delete window_styles;
    window_styles = NULL;
  }
  // Notes.
  if (window_notes) {
    window_notes->shutdown();
    delete window_notes;
    window_notes = NULL;
  }
  // References.
  if (window_references) {
    window_references->shutdown();
    delete window_references;
    window_references = NULL;
  }
  // Editors.
  while (!editor_windows.empty()) {
    WindowEditor *editor_window = editor_windows[0];
    editor_window->shutdown();
    delete editor_window;
    editor_windows.erase(editor_windows.begin());
  }
  // Check USFM.
  if (window_check_usfm) {
    window_check_usfm->shutdown();
    delete window_check_usfm;
    window_check_usfm = NULL;
  }
}


void MainWindow::on_window_focus_button_clicked(GtkButton * button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_window_focus_button(button);
}


void MainWindow::on_window_focus_button(GtkButton * button)
// Called when a window gets focused.
{
  static GtkWidget * previously_focused_window_button = NULL;
  // Bail out if there's no change in the focus.
  GtkWidget *widget = GTK_WIDGET(button);
  if (widget == previously_focused_window_button)
    return;
  previously_focused_window_button = widget;

  // Save possible new focused resource.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (resource_windows[i]->focus_in_signal_button == widget) {
      if (widget != focused_resource_button) {
        focused_resource_button = widget;
      }
    }
  }

  // Save possible new focused editor.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focus_in_signal_button == widget) {
      if (widget != focused_editor_button) {
        focused_editor_button = widget;
        handle_editor_focus();
      }
    }
  }

  // Save possible new focused tool.
  store_last_focused_tool_button (button);
  
  // Focus the relevant window.
  if (window_show_related_verses)
    window_show_related_verses->focus_set (window_show_related_verses->focus_in_signal_button == widget);
  if (window_merge)
    window_merge->focus_set (window_merge->focus_in_signal_button == widget);
  for (unsigned int i = 0; i < resource_windows.size(); i++)
    resource_windows[i]->focus_set (resource_windows[i]->focus_in_signal_button == widget);
  if (window_outline)
    window_outline->focus_set (window_outline->focus_in_signal_button == widget);
  if (window_check_keyterms)
    window_check_keyterms->focus_set (window_check_keyterms->focus_in_signal_button == widget);
  if (window_styles)
    window_styles->focus_set (window_styles->focus_in_signal_button == widget);
  if (window_notes)
    window_notes->focus_set (window_notes->focus_in_signal_button == widget);
  if (window_references)
    window_references->focus_set (window_references->focus_in_signal_button == widget);
  for (unsigned int i = 0; i < editor_windows.size(); i++)
    editor_windows[i]->focus_set (editor_windows[i]->focus_in_signal_button == widget);
  if (window_check_usfm)
    window_check_usfm->focus_set (window_check_usfm->focus_in_signal_button == widget);
}


void MainWindow::store_last_focused_tool_button (GtkButton * button)
{
  GtkWidget * widget = GTK_WIDGET (button);
  if (window_show_related_verses) {
    if (widget == window_show_related_verses->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  if (window_merge) {
    if (widget == window_merge->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (widget == resource_windows[i]->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  } 
  if (window_outline) {
    if (widget == window_outline->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  if (window_check_keyterms) {
    if (widget == window_check_keyterms->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  if (window_styles) {
    if (widget == window_styles->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  // Skip project notes window.
  if (window_references) {
    if (widget == window_references->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
  // Skip editor windows.
  if (window_check_usfm) {
    if (widget == window_check_usfm->focus_in_signal_button) {
      focused_tool_button = widget;
    }
  }
}


/*
 |
 |
 |
 |
 |
 Interprocess communications
 |
 |
 |
 |
 |
 */


bool MainWindow::on_interprocess_communications_initiate_listener_timeout(gpointer data)
{
  ((MainWindow *) data)->interprocess_communications_initiate_listener();
  return false;
}


void MainWindow::interprocess_communications_initiate_listener ()
{
  interprocess_communications_initiate_listener_event_id = 0;
  ustring url = settings->genconfig.bibledit_web_url_get();
  ustring user = settings->genconfig.bibledit_web_user_get();
  url.append ("/ipc/getmessage.php?user=" + user + "&channel=bibleditgtk&id=");
  url.append (convert_to_string (interprocess_communications_initiate_listener_message_id));
  GtkWidget * button;
  button = urltransport->send_message_expect_reply (url);
  g_signal_connect((gpointer) button, "clicked", G_CALLBACK(on_interprocess_communications_listener_button_clicked), gpointer(this));
}


void MainWindow::on_interprocess_communications_listener_button_clicked(GtkButton *button, gpointer user_data)
{
  ((MainWindow *) user_data)->on_interprocess_communications_listener_button(button);
}


void MainWindow::on_interprocess_communications_listener_button(GtkButton *button)
{
  // Process the message if it looks good.
  if (urltransport->reply_is_ok) {
		ParseLine parseline (trim (urltransport->reply_body));
    if (!parseline.lines.empty()) {
      interprocess_communications_initiate_listener_message_id = convert_to_int (parseline.lines[0]);
      if (parseline.lines.size() > 2) {
        ustring subject = parseline.lines[1];
        // Handle "focus" command.
        if (subject == "focus") {
          ustring payload = parseline.lines[2];
          Parse parse (payload, false, ".");
          if (parse.words.size() >= 3) {
            Reference received_reference (convert_to_int (parse.words[0]), convert_to_int (parse.words[1]), parse.words[2]);
            navigation.display (received_reference);
          }
        }
      }
    }
  }

  // Since this listener is ready, it needs to start the next listener.
  // If all went well, this is done shortly after.
  // If there was an error, it waits a bit longer before restarting the listener.
  unsigned int milliseconds = 1000;
  if (urltransport->reply_is_ok) {
    milliseconds = 100;
  }
  interprocess_communications_initiate_listener_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, milliseconds, GSourceFunc(on_interprocess_communications_initiate_listener_timeout), gpointer(this), NULL);
}


