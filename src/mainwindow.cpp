/*
 ** Copyright (©) 2003-2008 Teus Benschop.
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
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 **  
 */

#include <config.h>
#include "mainwindow.h"
#include "libraries.h"
#include "dialoglistview.h"
#include "dialogshowscript.h"
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
#include "dialogimporttext.h"
#include "dialogreplacing.h"
#include <gdk/gdkkeysyms.h>
#include <glib.h>
#include "dialogsearchspecial.h"
#include "referenceutils.h"
#include "references.h"
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
#include "compareutils.h"
#include "dialogexportnotes.h"
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
#include "editor.h"
#include "layout.h"
#include "dialogbook.h"
#include "books.h"
#include "screen.h"
#include "dialoglinecutter.h"
#include "dialogoutpost.h"
#include "dialognotestransfer.h"
#include "dialogchapternumber.h"
#include "versification.h"
#include "dialogmychecks.h"
#include "help.h"
#include "dialogoriginreferences.h"
#include "dialogtidy.h"
#include "dialognotesupdate.h"
#include "dialogbackup.h"
#include "backup.h"
#include "dialogviewgit.h"
#include "dialoggitsetup.h"
#include "dialogchanges.h"
#include "dialogrevert.h"
#include "resource_utils.h"
#include "dialogeditnote.h"
#include "dialogwordlist.h"
#include "dialogfontcolor.h"
#include "color.h"
#include "dialognewstylesheet.h"
#include "settings.h"
#include "ipc.h"
#include "dialogfeatures.h"
#include "password.h"
#include "gui_features.h"
#include "dialogprintreferences.h"
#include "print_parallel_references.h"
#include "dialogfixmarkers.h"
#include "dialogtextreplacement.h"
#include "temporal.h"
#include "parallel_passages.h"
#include "dialogpdfviewer.h"
#include "dialogviewusfm.h"
#include "dialoginserttable.h"
#include "tiny_utilities.h"
#include "hyphenate.h"
#include "dialogreportingsetup.h"
#include "dialogeditstatus.h"
#include "dialogviewstatus.h"
#include "planning.h"
#include "dialogplanningsetup.h"
#include "dialoginterfaceprefs.h"
#include "dialognewresource.h"
#include "shutdown.h"
#include "dialogfilters.h"
#include "dialogradiobutton.h"
#include "import.h"
#include "dialogimportrawtext.h"
#include "dialogxfernotes2text.h"
#include "htmlcolor.h"
#include "text2pdf.h"
#include "windows.h"
#include "unixwrappers.h"
#include "accelerators.h"

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

MainWindow::MainWindow(unsigned long xembed) :
  navigation(0), bibletime(true), httpd(0) {
  // Set some pointers to NULL.  
  // To save memory, we only create the object when actually needed.
  window_screen_layout = NULL;
  window_show_keyterms = NULL;
  window_show_quick_references = NULL;
  window_merge = NULL;
  window_outline = NULL;
  window_check_keyterms = NULL;
  window_styles = NULL;
  window_notes = NULL;
  window_references = NULL;

  // Initialize some variables.
  git_reopen_project = false;
  windows_startup_pointer = 0;
  now_focused_signal_button = NULL;
  last_focused_signal_button = NULL;

  // Gui Features object.
  GuiFeatures guifeatures(0);
  project_notes_enabled = guifeatures.project_notes();

  // Accelerators.
  accel_group = gtk_accel_group_new();
  accelerator_group = gtk_accel_group_new();
  gtk_accel_group_connect(accelerator_group, GDK_X, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_cut_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_C, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_copy_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_V, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_paste_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Z, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_undo_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Z, GdkModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_redo_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_1, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_standard_text_1_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_2, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_standard_text_2_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_3, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_standard_text_3_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_4, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_standard_text_4_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_N, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_new_project_note_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Down, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_verse_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Up, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_verse_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Page_Down, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_chapter_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Page_Up, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_chapter_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Page_Down, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_book_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Page_Up, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_book_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Right, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_reference_in_history_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Left, GDK_MOD1_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_reference_in_history_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_G, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_go_to_reference_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_W, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_close_window_callback), gpointer(this), NULL));
  if (guifeatures.styles()) {
    gtk_accel_group_connect(accelerator_group, GDK_S, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_goto_styles_area_callback), gpointer(this), NULL));
  }
  gtk_accel_group_connect(accelerator_group, GDK_Q, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_quit_program_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_F5, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_text_area_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_F5, GDK_SHIFT_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_tools_area_callback), gpointer(this), NULL));
  if (guifeatures.project_notes()) {
    gtk_accel_group_connect(accelerator_group, GDK_F5, GDK_CONTROL_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_activate_notes_area_callback), gpointer(this), NULL));
  }
  if (guifeatures.references_and_find()) {
    gtk_accel_group_connect(accelerator_group, GDK_F6, GdkModifierType(0), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_reference_in_reference_area_callback), gpointer(this), NULL));
    gtk_accel_group_connect(accelerator_group, GDK_F6, GDK_SHIFT_MASK, GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_reference_in_reference_area_callback), gpointer(this), NULL));
  }
  gtk_accel_group_connect(accelerator_group, GDK_Right, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_next_project_callback), gpointer(this), NULL));
  gtk_accel_group_connect(accelerator_group, GDK_Left, (GdkModifierType) (GDK_CONTROL_MASK | GDK_MOD1_MASK), GtkAccelFlags(0), g_cclosure_new_swap(G_CALLBACK(accelerator_previous_project_callback), gpointer(this), NULL));

  // GUI build.
  if (xembed) {
    mainwindow = gtk_plug_new(GdkNativeWindow(xembed));
  } else {
    mainwindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  }
  gtk_window_set_default_icon_from_file(gw_build_filename (directories_get_package_data (), "bibledit.xpm").c_str(), NULL);
  gtk_window_set_gravity(GTK_WINDOW (mainwindow), GDK_GRAVITY_STATIC);

  // Pointer to the settings.
  extern Settings * settings;

  g_set_application_name("Bibledit");

  // Start Outpost.
  windowsoutpost = new WindowsOutpost (true);
  if (settings->genconfig.use_outpost_get())
    windowsoutpost->Start();

  vbox1 = gtk_vbox_new(FALSE, 0);
  gtk_widget_show(vbox1);
  gtk_container_add(GTK_CONTAINER (mainwindow), vbox1);

  menubar1 = gtk_menu_bar_new();
  gtk_widget_show(menubar1);
  gtk_box_pack_start(GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  menuitem_file = gtk_menu_item_new_with_mnemonic("_File");
  gtk_widget_show(menuitem_file);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_file);

  menuitem_file_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_file), menuitem_file_menu);

  file_project = gtk_image_menu_item_new_with_mnemonic("Pr_oject");
  gtk_widget_show(file_project);
  gtk_container_add(GTK_CONTAINER (menuitem_file_menu), file_project);

  image463 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image463);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_project), image463);

  file_project_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (file_project), file_project_menu);

  new1 = NULL;
  open1 = NULL;
  delete1 = NULL;
  if (guifeatures.project_management()) {

    new1 = gtk_image_menu_item_new_with_mnemonic("_New");
    gtk_widget_show(new1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), new1);

    image903 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image903);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (new1), image903);

    open1 = gtk_image_menu_item_new_from_stock("gtk-open", accel_group);
    gtk_widget_show(open1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), open1);

    delete1 = gtk_image_menu_item_new_from_stock("gtk-delete", accel_group);
    gtk_widget_show(delete1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), delete1);

  }

  properties1 = NULL;
  import1 = NULL;
  export_project = NULL;
  export_project_menu = NULL;
  export_usfm_files = NULL;
  export_zipped_unified_standard_format_markers1 = NULL;
  to_bibleworks_version_database_compiler = NULL;
  export_to_sword_module = NULL;
  export_opendocument = NULL;
  copy_project_to = NULL;
  compare_with1 = NULL;
  if (guifeatures.project_management()) {

    properties1 = gtk_image_menu_item_new_with_mnemonic("P_roperties");
    gtk_widget_show(properties1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), properties1);

    image4995 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image4995);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (properties1), image4995);

    import1 = gtk_image_menu_item_new_with_mnemonic("_Import");
    gtk_widget_show(import1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), import1);

    image464 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image464);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (import1), image464);

    export_project = gtk_image_menu_item_new_with_mnemonic("_Export");
    gtk_widget_show(export_project);
    gtk_container_add(GTK_CONTAINER (file_project_menu), export_project);

    image3298 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3298);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_project), image3298);

    export_project_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (export_project), export_project_menu);

    export_usfm_files = gtk_image_menu_item_new_with_mnemonic("_Unified Standard Format Markers files");
    gtk_widget_show(export_usfm_files);
    gtk_container_add(GTK_CONTAINER (export_project_menu), export_usfm_files);

    image12814 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image12814);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_usfm_files), image12814);

    export_zipped_unified_standard_format_markers1 = gtk_image_menu_item_new_with_mnemonic("_Zipped Unified Standard Format Markers");
    gtk_widget_show(export_zipped_unified_standard_format_markers1);
    gtk_container_add(GTK_CONTAINER (export_project_menu), export_zipped_unified_standard_format_markers1);

    image17639 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image17639);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_zipped_unified_standard_format_markers1), image17639);

    to_bibleworks_version_database_compiler = gtk_image_menu_item_new_with_mnemonic("_BibleWorks Version Database Compiler");
    gtk_widget_show(to_bibleworks_version_database_compiler);
    gtk_container_add(GTK_CONTAINER (export_project_menu), to_bibleworks_version_database_compiler);

    image3299 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3299);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (to_bibleworks_version_database_compiler), image3299);

    export_to_sword_module = gtk_image_menu_item_new_with_mnemonic("_SWORD module");
    gtk_widget_show(export_to_sword_module);
    gtk_container_add(GTK_CONTAINER (export_project_menu), export_to_sword_module);

    image11392 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image11392);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_to_sword_module), image11392);

    export_opendocument = gtk_image_menu_item_new_with_mnemonic("_OpenDocument");
    gtk_widget_show(export_opendocument);
    gtk_container_add(GTK_CONTAINER (export_project_menu), export_opendocument);

    image15162 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image15162);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_opendocument), image15162);

    copy_project_to = gtk_image_menu_item_new_with_mnemonic("Cop_y to");
    gtk_widget_show(copy_project_to);
    gtk_container_add(GTK_CONTAINER (file_project_menu), copy_project_to);

    image2688 = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2688);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (copy_project_to), image2688);

    compare_with1 = gtk_image_menu_item_new_with_mnemonic("Co_mpare with");
    gtk_widget_show(compare_with1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), compare_with1);

    image2764 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2764);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (compare_with1), image2764);

  }

  project_backup = gtk_image_menu_item_new_with_mnemonic("_Backup");
  gtk_widget_show(project_backup);
  gtk_container_add(GTK_CONTAINER (file_project_menu), project_backup);

  image18535 = gtk_image_new_from_stock("gtk-floppy", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18535);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (project_backup), image18535);

  project_backup_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (project_backup), project_backup_menu);

  project_backup_incremental = gtk_image_menu_item_new_with_mnemonic("_Incremental");
  gtk_widget_show(project_backup_incremental);
  gtk_container_add(GTK_CONTAINER (project_backup_menu), project_backup_incremental);

  image18536 = gtk_image_new_from_stock("gtk-floppy", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18536);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (project_backup_incremental), image18536);

  project_backup_flexible = gtk_image_menu_item_new_with_mnemonic("_Flexible");
  gtk_widget_show(project_backup_flexible);
  gtk_container_add(GTK_CONTAINER (project_backup_menu), project_backup_flexible);

  image18537 = gtk_image_new_from_stock("gtk-cdrom", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18537);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (project_backup_flexible), image18537);

  project_changes = NULL;
  if (guifeatures.project_management()) {

    project_changes = gtk_image_menu_item_new_with_mnemonic("C_hanges");
    gtk_widget_show(project_changes);
    gtk_container_add(GTK_CONTAINER (file_project_menu), project_changes);

    image19115 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image19115);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (project_changes), image19115);

  }

  file_projects_merge = gtk_check_menu_item_new_with_mnemonic("Mer_ge");
  gtk_widget_show(file_projects_merge);
  gtk_container_add(GTK_CONTAINER (file_project_menu), file_projects_merge);

  file_references = NULL;
  if (guifeatures.references_management() || guifeatures.printing()) {

    file_references = gtk_image_menu_item_new_with_mnemonic("_References");
    gtk_widget_show(file_references);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), file_references);

    image465 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image465);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_references), image465);

    file_references_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (file_references), file_references_menu);

  }

  if (guifeatures.references_management()) {

    open_references1 = gtk_image_menu_item_new_with_mnemonic("_Open");
    gtk_widget_show(open_references1);
    gtk_container_add(GTK_CONTAINER (file_references_menu), open_references1);

    image466 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image466);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (open_references1), image466);

    references_save_as = gtk_image_menu_item_new_from_stock("gtk-save-as", accel_group);
    gtk_widget_show(references_save_as);
    gtk_container_add(GTK_CONTAINER (file_references_menu), references_save_as);

  }

  if (guifeatures.references_management()) {

    close_references = gtk_image_menu_item_new_with_mnemonic("D_ismiss all");
    gtk_widget_show(close_references);
    gtk_container_add(GTK_CONTAINER (file_references_menu), close_references);

    image468 = gtk_image_new_from_stock("gtk-close", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image468);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (close_references), image468);

    delete_references = gtk_image_menu_item_new_with_mnemonic("_Dismiss");
    gtk_widget_show(delete_references);
    gtk_container_add(GTK_CONTAINER (file_references_menu), delete_references);

    image469 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image469);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (delete_references), image469);

    reference_hide = gtk_image_menu_item_new_with_mnemonic("_Hide from now on");
    gtk_widget_show(reference_hide);
    gtk_container_add(GTK_CONTAINER (file_references_menu), reference_hide);

    image6483 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6483);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (reference_hide), image6483);

  }

  style = NULL;
  if (guifeatures.styles()) {

    style = gtk_image_menu_item_new_with_mnemonic("_Styles");
    gtk_widget_show(style);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), style);

    image10735 = gtk_image_new_from_stock("gtk-print-preview", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10735);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (style), image10735);

  }

  style_menu= NULL;
  if (guifeatures.styles()) {

    style_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (style), style_menu);

  }

  stylesheets_expand_all= NULL;
  if (guifeatures.styles()) {

    stylesheets_expand_all = gtk_image_menu_item_new_with_mnemonic("_Expand all");
    gtk_widget_show(stylesheets_expand_all);
    gtk_container_add(GTK_CONTAINER (style_menu), stylesheets_expand_all);

    GtkWidget *image10736;
    image10736 = gtk_image_new_from_stock("gtk-add", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10736);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_expand_all), image10736);

  }

  stylesheets_collapse_all= NULL;
  if (guifeatures.styles()) {

    stylesheets_collapse_all = gtk_image_menu_item_new_with_mnemonic("_Collapse all");
    gtk_widget_show(stylesheets_collapse_all);
    gtk_container_add(GTK_CONTAINER (style_menu), stylesheets_collapse_all);

    GtkWidget *image10737;
    image10737 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10737);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_collapse_all), image10737);

  }

  style_insert= NULL;
  if (guifeatures.styles()) {

    style_insert = gtk_image_menu_item_new_with_mnemonic("_Insert");
    gtk_widget_show(style_insert);
    gtk_container_add(GTK_CONTAINER (style_menu), style_insert);

    GtkWidget *image10738;
    image10738 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10738);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (style_insert), image10738);

  }

  stylesheet_edit_mode= NULL;
  if (guifeatures.styles_management()) {

    stylesheet_edit_mode = gtk_check_menu_item_new_with_mnemonic("Edit _mode");
    gtk_widget_show(stylesheet_edit_mode);
    gtk_container_add(GTK_CONTAINER (style_menu), stylesheet_edit_mode);

  }

  style_new= NULL;
  if (guifeatures.styles_management()) {

    style_new = gtk_image_menu_item_new_with_mnemonic("_New");
    gtk_widget_show(style_new);
    gtk_container_add(GTK_CONTAINER (style_menu), style_new);

    GtkWidget *image10739;
    image10739 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10739);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (style_new), image10739);

  }

  style_properties= NULL;
  if (guifeatures.styles_management()) {

    style_properties = gtk_image_menu_item_new_from_stock("gtk-properties", accel_group);
    gtk_widget_show(style_properties);
    gtk_container_add(GTK_CONTAINER (style_menu), style_properties);

  }

  style_delete= NULL;
  if (guifeatures.styles_management()) {

    style_delete = gtk_image_menu_item_new_from_stock("gtk-delete", accel_group);
    gtk_widget_show(style_delete);
    gtk_container_add(GTK_CONTAINER (style_menu), style_delete);

  }

  menu_stylesheet= NULL;
  if (guifeatures.styles_management()) {

    menu_stylesheet = gtk_image_menu_item_new_with_mnemonic("_Stylesheet");
    gtk_widget_show(menu_stylesheet);
    gtk_container_add(GTK_CONTAINER (style_menu), menu_stylesheet);

    GtkWidget *image10740;
    image10740 = gtk_image_new_from_stock("gtk-print-preview", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10740);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (menu_stylesheet), image10740);

  }

  menu_stylesheet_menu= NULL;
  if (guifeatures.styles_management()) {

    menu_stylesheet_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (menu_stylesheet), menu_stylesheet_menu);

  }

  stylesheet_switch= NULL;
  if (guifeatures.styles_management()) {

    stylesheet_switch = gtk_image_menu_item_new_with_mnemonic("_Switch");
    gtk_widget_show(stylesheet_switch);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheet_switch);

    GtkWidget *image10741;
    image10741 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10741);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheet_switch), image10741);

  }

  stylesheets_new= NULL;
  if (guifeatures.styles_management()) {

    stylesheets_new = gtk_image_menu_item_new_with_mnemonic("_New");
    gtk_widget_show(stylesheets_new);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheets_new);

    GtkWidget *image10742;
    image10742 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10742);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_new), image10742);

  }

  stylesheets_delete= NULL;
  if (guifeatures.styles_management()) {

    stylesheets_delete = gtk_image_menu_item_new_with_mnemonic("_Delete");
    gtk_widget_show(stylesheets_delete);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheets_delete);

    GtkWidget *image10743;
    image10743 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10743);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_delete), image10743);

  }

  stylesheets_rename= NULL;
  if (guifeatures.styles_management()) {

    stylesheets_rename = gtk_image_menu_item_new_with_mnemonic("_Rename");
    gtk_widget_show(stylesheets_rename);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheets_rename);

    GtkWidget *image10744;
    image10744 = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10744);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_rename), image10744);

  }

  stylesheets_import= NULL;
  if (guifeatures.styles_management()) {

    stylesheets_import = gtk_image_menu_item_new_with_mnemonic("_Import");
    gtk_widget_show(stylesheets_import);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheets_import);

    GtkWidget *image10745;
    image10745 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10745);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_import), image10745);

  }

  stylesheets_export= NULL;
  if (guifeatures.styles_management()) {

    stylesheets_export = gtk_image_menu_item_new_with_mnemonic("_Export");
    gtk_widget_show(stylesheets_export);
    gtk_container_add(GTK_CONTAINER (menu_stylesheet_menu), stylesheets_export);

    GtkWidget *image10746;
    image10746 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image10746);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (stylesheets_export), image10746);

  }

  notes2 = NULL;
  if (guifeatures.project_notes_management()) {

    notes2 = gtk_image_menu_item_new_with_mnemonic("Project _notes");
    gtk_widget_show(notes2);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), notes2);

    image936 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image936);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (notes2), image936);

    notes2_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (notes2), notes2_menu);

    new_note = gtk_image_menu_item_new_from_stock("gtk-new", accel_group);
    gtk_widget_show(new_note);
    gtk_container_add(GTK_CONTAINER (notes2_menu), new_note);

    delete_note = gtk_image_menu_item_new_with_mnemonic("_Delete");
    gtk_widget_show(delete_note);
    gtk_container_add(GTK_CONTAINER (notes2_menu), delete_note);

    image963 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image963);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (delete_note), image963);

    import_notes = gtk_image_menu_item_new_with_mnemonic("_Import");
    gtk_widget_show(import_notes);
    gtk_container_add(GTK_CONTAINER (notes2_menu), import_notes);

    image1455 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1455);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (import_notes), image1455);

    export_notes = gtk_image_menu_item_new_with_mnemonic("_Export");
    gtk_widget_show(export_notes);
    gtk_container_add(GTK_CONTAINER (notes2_menu), export_notes);

    image4068 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image4068);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (export_notes), image4068);

  }

  file_resources = gtk_image_menu_item_new_with_mnemonic("R_esources");
  gtk_widget_show(file_resources);
  gtk_container_add(GTK_CONTAINER (menuitem_file_menu), file_resources);

  image27365 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27365);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources), image27365);

  file_resources_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (file_resources), file_resources_menu);

  file_resources_open = gtk_image_menu_item_new_with_mnemonic("_Open");
  gtk_widget_show(file_resources_open);
  gtk_container_add(GTK_CONTAINER (file_resources_menu), file_resources_open);

  image27366 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27366);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources_open), image27366);

  file_resources_close = gtk_image_menu_item_new_with_mnemonic("_Close");
  gtk_widget_show(file_resources_close);
  gtk_container_add(GTK_CONTAINER (file_resources_menu), file_resources_close);

  image27367 = gtk_image_new_from_stock("gtk-close", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27367);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources_close), image27367);

  file_resources_new = gtk_image_menu_item_new_with_mnemonic("_New");
  gtk_widget_show(file_resources_new);
  gtk_container_add(GTK_CONTAINER (file_resources_menu), file_resources_new);

  image27514 = gtk_image_new_from_stock("gtk-new", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27514);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources_new), image27514);

  file_resources_edit = gtk_image_menu_item_new_with_mnemonic("_Edit");
  gtk_widget_show(file_resources_edit);
  gtk_container_add(GTK_CONTAINER (file_resources_menu), file_resources_edit);

  image27515 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27515);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources_edit), image27515);

  file_resources_delete = gtk_image_menu_item_new_with_mnemonic("_Delete");
  gtk_widget_show(file_resources_delete);
  gtk_container_add(GTK_CONTAINER (file_resources_menu), file_resources_delete);

  image27664 = gtk_image_new_from_stock("gtk-delete", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image27664);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (file_resources_delete), image27664);

  print = NULL;

  if (guifeatures.printing()) {

    print = gtk_image_menu_item_new_from_stock("gtk-print", accel_group);
    gtk_widget_show(print);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), print);
    gtk_widget_add_accelerator(print, "activate", accel_group, 
    GDK_P, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  }

  quit1 = gtk_image_menu_item_new_from_stock("gtk-quit", accel_group);
  gtk_widget_show(quit1);
  gtk_container_add(GTK_CONTAINER (menuitem_file_menu), quit1);

  menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
  gtk_widget_show(menuitem_edit);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_edit);

  menuitem_edit_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_edit), menuitem_edit_menu);

  cut1 = gtk_image_menu_item_new_from_stock("gtk-cut", accel_group);
  gtk_widget_show(cut1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), cut1);

  copy1 = gtk_image_menu_item_new_from_stock("gtk-copy", accel_group);
  gtk_widget_show(copy1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), copy1);

  copy_without_formatting = gtk_image_menu_item_new_with_mnemonic("Copy _without formatting");
  gtk_widget_show(copy_without_formatting);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), copy_without_formatting);
  gtk_widget_add_accelerator(copy_without_formatting, "activate", accel_group, 
  GDK_C, (GdkModifierType) (GDK_CONTROL_MASK | GDK_SHIFT_MASK), GTK_ACCEL_VISIBLE);

  image18220 = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18220);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (copy_without_formatting), image18220);

  paste1 = gtk_image_menu_item_new_from_stock("gtk-paste", accel_group);
  gtk_widget_show(paste1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), paste1);

  separator2 = gtk_separator_menu_item_new();
  gtk_widget_show(separator2);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator2);
  gtk_widget_set_sensitive(separator2, FALSE);

  undo1 = gtk_image_menu_item_new_with_mnemonic("_Undo");
  gtk_widget_show(undo1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), undo1);
  gtk_widget_add_accelerator(undo1, "activate", accel_group, GDK_Z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  image295 = gtk_image_new_from_stock("gtk-undo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image295);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (undo1), image295);

  redo1 = gtk_image_menu_item_new_with_mnemonic("_Redo");
  gtk_widget_show(redo1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), redo1);
  gtk_widget_add_accelerator(redo1, "activate", accel_group, GDK_Z, GdkModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GTK_ACCEL_VISIBLE);

  image296 = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image296);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (redo1), image296);

  separator4 = gtk_separator_menu_item_new();
  gtk_widget_show(separator4);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator4);
  gtk_widget_set_sensitive(separator4, FALSE);

  if (guifeatures.references_and_find()) {

    find1 = gtk_image_menu_item_new_from_stock("gtk-find", accel_group);
    gtk_widget_show(find1);
    gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), find1);

  }

  if (guifeatures.replace()) {

    find_and_replace1 = gtk_image_menu_item_new_from_stock("gtk-find-and-replace", accel_group);
    gtk_widget_show(find_and_replace1);
    gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), find_and_replace1);

  }

  find_in_notes1 = gtk_image_menu_item_new_with_mnemonic("Find in Project _notes");
  gtk_widget_show(find_in_notes1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), find_in_notes1);

  image1430 = gtk_image_new_from_stock("gtk-find", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image1430);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (find_in_notes1), image1430);

  separator7 = gtk_separator_menu_item_new();
  gtk_widget_show(separator7);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator7);
  gtk_widget_set_sensitive(separator7, FALSE);

  get_references_from_note = gtk_image_menu_item_new_with_mnemonic("_Get references from project note");
  gtk_widget_show(get_references_from_note);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), get_references_from_note);

  image3158 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image3158);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (get_references_from_note), image3158);

  separator15 = gtk_separator_menu_item_new();
  gtk_widget_show(separator15);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator15);
  gtk_widget_set_sensitive(separator15, FALSE);

  edit_revert = gtk_image_menu_item_new_with_mnemonic("Re_vert");
  gtk_widget_show(edit_revert);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), edit_revert);

  image19262 = gtk_image_new_from_stock("gtk-go-back", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image19262);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (edit_revert), image19262);

  separator17 = gtk_separator_menu_item_new();
  gtk_widget_show(separator17);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator17);
  gtk_widget_set_sensitive(separator17, FALSE);

  edit_bible_note = gtk_image_menu_item_new_with_mnemonic("_Bible note");
  gtk_widget_show(edit_bible_note);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), edit_bible_note);

  image20483 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20483);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (edit_bible_note), image20483);

  separator21 = gtk_separator_menu_item_new();
  gtk_widget_show(separator21);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator21);
  gtk_widget_set_sensitive(separator21, FALSE);

  editstatus = gtk_image_menu_item_new_with_mnemonic("St_atus");
  gtk_widget_show(editstatus);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), editstatus);

  image25815 = gtk_image_new_from_stock("gtk-about", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image25815);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (editstatus), image25815);

  edit_planning = gtk_image_menu_item_new_with_mnemonic("P_lanning");
  gtk_widget_show(edit_planning);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), edit_planning);

  image26801 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image26801);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (edit_planning), image26801);

  menuitem_view = gtk_menu_item_new_with_mnemonic("_View");
  gtk_widget_show(menuitem_view);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_view);

  menuitem_view_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_view), menuitem_view_menu);

  view_font = gtk_image_menu_item_new_with_mnemonic("_Font & Color");
  gtk_widget_show(view_font);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_font);

  image20234 = gtk_image_new_from_stock("gtk-select-font", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20234);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_font), image20234);

  view_font_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (view_font), view_font_menu);

  view_text_font = gtk_image_menu_item_new_with_mnemonic("_Text");
  gtk_widget_show(view_text_font);
  gtk_container_add(GTK_CONTAINER (view_font_menu), view_text_font);

  image20235 = gtk_image_new_from_stock("gtk-select-font", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20235);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_text_font), image20235);

  /* Because of switching to GtkHtml for displaying and editing project notes, the fonts no longer can be set in the menu.
   view_notes_font = gtk_image_menu_item_new_with_mnemonic("_Notes");
   gtk_widget_show(view_notes_font);
   gtk_container_add(GTK_CONTAINER (view_font_menu), view_notes_font);

   image20236 = gtk_image_new_from_stock("gtk-select-font", GTK_ICON_SIZE_MENU);
   gtk_widget_show(image20236);
   gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_notes_font), image20236);
   */

  if (guifeatures.project_notes()) {

    viewnotes = gtk_image_menu_item_new_with_mnemonic("Project _notes");
    gtk_widget_show(viewnotes);
    gtk_container_add(GTK_CONTAINER (menuitem_view_menu), viewnotes);

    image2627 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2627);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (viewnotes), image2627);

  }

  view_git_tasks = gtk_image_menu_item_new_with_mnemonic("_Git tasks");
  gtk_widget_show(view_git_tasks);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_git_tasks);

  image18685 = gtk_image_new_from_stock("gtk-connect", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18685);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_git_tasks), image18685);

  parallel_passages1 = gtk_check_menu_item_new_with_mnemonic("_Parallel passages");
  gtk_widget_show(parallel_passages1);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), parallel_passages1);

  view_usfm_code = gtk_image_menu_item_new_with_mnemonic("USFM _code");
  gtk_widget_show(view_usfm_code);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_usfm_code);

  image25006 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image25006);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_usfm_code), image25006);

  view_status = gtk_image_menu_item_new_with_mnemonic("S_tatus");
  gtk_widget_show(view_status);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_status);

  image25963 = gtk_image_new_from_stock("gtk-apply", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image25963);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_status), image25963);

  view_planning = gtk_image_menu_item_new_with_mnemonic("Pl_anning");
  gtk_widget_show(view_planning);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_planning);

  image26812 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image26812);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (view_planning), image26812);

  view_screen_layout = gtk_check_menu_item_new_with_mnemonic("_Screen layout");
  gtk_widget_show(view_screen_layout);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_screen_layout);

  view_keyterms = gtk_check_menu_item_new_with_mnemonic("_Keyterms in verse");
  gtk_widget_show(view_keyterms);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_keyterms);

  view_quick_references = gtk_check_menu_item_new_with_mnemonic("_Quick references");
  gtk_widget_show(view_quick_references);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_quick_references);

  view_outline = gtk_check_menu_item_new_with_mnemonic("_Outline");
  gtk_widget_show(view_outline);
  gtk_container_add(GTK_CONTAINER (menuitem_view_menu), view_outline);

  insert1 = gtk_menu_item_new_with_mnemonic("_Insert");
  gtk_widget_show(insert1);
  gtk_container_add(GTK_CONTAINER (menubar1), insert1);

  insert1_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (insert1), insert1_menu);

  standard_text_1 = NULL;
  standard_text_2 = NULL;
  standard_text_3 = NULL;
  standard_text_4 = NULL;
  current_reference1 = NULL;

  if (guifeatures.project_notes()) {

    standard_text_1 = gtk_image_menu_item_new_with_mnemonic("Standard text _1");
    gtk_widget_show(standard_text_1);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_1);
    gtk_widget_add_accelerator(standard_text_1, "activate", accel_group, 
    GDK_1, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    image1963 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1963);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_1), image1963);

    standard_text_2 = gtk_image_menu_item_new_with_mnemonic("Standard text _2");
    gtk_widget_show(standard_text_2);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_2);
    gtk_widget_add_accelerator(standard_text_2, "activate", accel_group, 
    GDK_2, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    image1964 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1964);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_2), image1964);

    standard_text_3 = gtk_image_menu_item_new_with_mnemonic("Standard text _3");
    gtk_widget_show(standard_text_3);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_3);
    gtk_widget_add_accelerator(standard_text_3, "activate", accel_group, 
    GDK_3, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    image1965 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1965);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_3), image1965);

    standard_text_4 = gtk_image_menu_item_new_with_mnemonic("Standard text _4");
    gtk_widget_show(standard_text_4);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_4);
    gtk_widget_add_accelerator(standard_text_4, "activate", accel_group, 
    GDK_4, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

    image1966 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1966);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_4), image1966);

    separator9 = gtk_separator_menu_item_new();
    gtk_widget_show(separator9);
    gtk_container_add(GTK_CONTAINER (insert1_menu), separator9);
    gtk_widget_set_sensitive(separator9, FALSE);

    current_reference1 = gtk_image_menu_item_new_with_mnemonic("_Current reference");
    gtk_widget_show(current_reference1);
    gtk_container_add(GTK_CONTAINER (insert1_menu), current_reference1);

    image3797 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3797);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (current_reference1), image3797);

    separator20 = gtk_separator_menu_item_new();
    gtk_widget_show(separator20);
    gtk_container_add(GTK_CONTAINER (insert1_menu), separator20);
    gtk_widget_set_sensitive(separator20, FALSE);

  }

  insert_special_character = gtk_image_menu_item_new_with_mnemonic("_Special character");
  gtk_widget_show(insert_special_character);
  gtk_container_add(GTK_CONTAINER (insert1_menu), insert_special_character);

  image25281 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image25281);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (insert_special_character), image25281);

  menuitem_goto = gtk_menu_item_new_with_mnemonic("_Goto");
  gtk_widget_show(menuitem_goto);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_goto);

  menuitem_goto_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_goto), menuitem_goto_menu);

  synchronize_other_programs2 = gtk_image_menu_item_new_with_mnemonic("_Synchronize other programs");
  gtk_widget_show(synchronize_other_programs2);
  gtk_container_add(GTK_CONTAINER (menuitem_goto_menu), synchronize_other_programs2);

  image4931 = gtk_image_new_from_stock("gtk-execute", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image4931);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (synchronize_other_programs2), image4931);

  check1 = NULL;
  if (guifeatures.checks()) {

    check1 = gtk_menu_item_new_with_mnemonic("Chec_k");
    gtk_widget_show(check1);
    gtk_container_add(GTK_CONTAINER (menubar1), check1);

    check1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (check1), check1_menu);

    chapters_and_verses1 = gtk_image_menu_item_new_with_mnemonic("_Chapters and verses");
    gtk_widget_show(chapters_and_verses1);
    gtk_container_add(GTK_CONTAINER (check1_menu), chapters_and_verses1);

    image5580 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5580);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (chapters_and_verses1), image5580);

    markers1 = gtk_image_menu_item_new_with_mnemonic("_Markers");
    gtk_widget_show(markers1);
    gtk_container_add(GTK_CONTAINER (check1_menu), markers1);

    image5578 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5578);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (markers1), image5578);

    markers1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (markers1), markers1_menu);

    validate_usfms1 = gtk_image_menu_item_new_with_mnemonic("_Validate");
    gtk_widget_show(validate_usfms1);
    gtk_container_add(GTK_CONTAINER (markers1_menu), validate_usfms1);

    image5579 = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5579);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (validate_usfms1), image5579);

    count_usfms1 = gtk_image_menu_item_new_with_mnemonic("_Count");
    gtk_widget_show(count_usfms1);
    gtk_container_add(GTK_CONTAINER (markers1_menu), count_usfms1);

    image6239 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6239);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (count_usfms1), image6239);

    compare_usfm1 = gtk_image_menu_item_new_with_mnemonic("C_ompare");
    gtk_widget_show(compare_usfm1);
    gtk_container_add(GTK_CONTAINER (markers1_menu), compare_usfm1);

    image6748 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6748);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (compare_usfm1), image6748);

    check_markers_spacing = gtk_image_menu_item_new_with_mnemonic("_Spacing");
    gtk_widget_show(check_markers_spacing);
    gtk_container_add(GTK_CONTAINER (markers1_menu), check_markers_spacing);

    image17930 = gtk_image_new_from_stock("gtk-media-pause", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image17930);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_markers_spacing), image17930);

    characters1 = gtk_image_menu_item_new_with_mnemonic("C_haracters");
    gtk_widget_show(characters1);
    gtk_container_add(GTK_CONTAINER (check1_menu), characters1);

    image6867 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6867);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (characters1), image6867);

    characters1_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (characters1), characters1_menu);

    count_characters = gtk_image_menu_item_new_with_mnemonic("_Inventory");
    gtk_widget_show(count_characters);
    gtk_container_add(GTK_CONTAINER (characters1_menu), count_characters);

    image6868 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6868);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (count_characters), image6868);

    unwanted_patterns = gtk_image_menu_item_new_with_mnemonic("_Unwanted patterns");
    gtk_widget_show(unwanted_patterns);
    gtk_container_add(GTK_CONTAINER (characters1_menu), unwanted_patterns);

    image7494 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7494);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (unwanted_patterns), image7494);

    check_words = gtk_image_menu_item_new_with_mnemonic("_Words");
    gtk_widget_show(check_words);
    gtk_container_add(GTK_CONTAINER (check1_menu), check_words);

    image7111 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7111);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_words), image7111);

    check_words_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (check_words), check_words_menu);

    check_capitalization = gtk_image_menu_item_new_with_mnemonic("_Capitalization");
    gtk_widget_show(check_capitalization);
    gtk_container_add(GTK_CONTAINER (check_words_menu), check_capitalization);

    image7112 = gtk_image_new_from_stock("gtk-ok", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7112);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_capitalization), image7112);

    check_repetition = gtk_image_menu_item_new_with_mnemonic("_Repetition");
    gtk_widget_show(check_repetition);
    gtk_container_add(GTK_CONTAINER (check_words_menu), check_repetition);

    image7238 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7238);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_repetition), image7238);

    unwanted_words = gtk_image_menu_item_new_with_mnemonic("_Unwanted");
    gtk_widget_show(unwanted_words);
    gtk_container_add(GTK_CONTAINER (check_words_menu), unwanted_words);

    image7631 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7631);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (unwanted_words), image7631);

    word_count_inventory = gtk_image_menu_item_new_with_mnemonic("_Inventory");
    gtk_widget_show(word_count_inventory);
    gtk_container_add(GTK_CONTAINER (check_words_menu), word_count_inventory);

    image13715 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image13715);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (word_count_inventory), image13715);

    check_punctuation = gtk_image_menu_item_new_with_mnemonic("_Punctuation");
    gtk_widget_show(check_punctuation);
    gtk_container_add(GTK_CONTAINER (check1_menu), check_punctuation);

    image7366 = gtk_image_new_from_stock("gtk-zoom-100", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7366);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_punctuation), image7366);

    check_punctuation_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (check_punctuation), check_punctuation_menu);

    check_matching_pairs = gtk_image_menu_item_new_with_mnemonic("_Matching pairs");
    gtk_widget_show(check_matching_pairs);
    gtk_container_add(GTK_CONTAINER (check_punctuation_menu), check_matching_pairs);

    image7367 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image7367);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_matching_pairs), image7367);

    check_sentence_structure = gtk_image_menu_item_new_with_mnemonic("_Sentence structure");
    gtk_widget_show(check_sentence_structure);
    gtk_container_add(GTK_CONTAINER (check_punctuation_menu), check_sentence_structure);

    image28475 = gtk_image_new_from_stock("gtk-justify-fill", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image28475);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_sentence_structure), image28475);

    check_references = gtk_image_menu_item_new_with_mnemonic("_References");
    gtk_widget_show(check_references);
    gtk_container_add(GTK_CONTAINER (check1_menu), check_references);

    image21826 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21826);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_references), image21826);

    check_references_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (check_references), check_references_menu);

    check_references_inventory = gtk_image_menu_item_new_with_mnemonic("_Inventory");
    gtk_widget_show(check_references_inventory);
    gtk_container_add(GTK_CONTAINER (check_references_menu), check_references_inventory);

    image21827 = gtk_image_new_from_stock("gtk-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21827);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_references_inventory), image21827);

    check_references_validate = gtk_image_menu_item_new_with_mnemonic("_Validate");
    gtk_widget_show(check_references_validate);
    gtk_container_add(GTK_CONTAINER (check_references_menu), check_references_validate);

    image21828 = gtk_image_new_from_stock("gtk-spell-check", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21828);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_references_validate), image21828);

    checks_passages = gtk_image_menu_item_new_with_mnemonic("P_assages");
    gtk_widget_show(checks_passages);
    gtk_container_add(GTK_CONTAINER (check1_menu), checks_passages);

    image24103 = gtk_image_new_from_stock("gtk-justify-fill", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24103);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (checks_passages), image24103);

    checks_passages_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (checks_passages), checks_passages_menu);

    check_nt_quotations_from_the_ot = gtk_image_menu_item_new_with_mnemonic("_NT quotations from the OT");
    gtk_widget_show(check_nt_quotations_from_the_ot);
    gtk_container_add(GTK_CONTAINER (checks_passages_menu), check_nt_quotations_from_the_ot);

    image24104 = gtk_image_new_from_stock("gtk-edit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24104);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (check_nt_quotations_from_the_ot), image24104);

    synoptic_parallel_passages_from_the_nt = gtk_image_menu_item_new_with_mnemonic("_Synoptic parallels from the NT");
    gtk_widget_show(synoptic_parallel_passages_from_the_nt);
    gtk_container_add(GTK_CONTAINER (checks_passages_menu), synoptic_parallel_passages_from_the_nt);

    image24105 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24105);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (synoptic_parallel_passages_from_the_nt), image24105);

    parallels_from_the_ot = gtk_image_menu_item_new_with_mnemonic("_Parallels from the OT");
    gtk_widget_show(parallels_from_the_ot);
    gtk_container_add(GTK_CONTAINER (checks_passages_menu), parallels_from_the_ot);

    image24106 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24106);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (parallels_from_the_ot), image24106);

    check_key_terms = gtk_check_menu_item_new_with_mnemonic("_Key terms");
    gtk_widget_show(check_key_terms);
    gtk_container_add(GTK_CONTAINER (check1_menu), check_key_terms);

    my_checks = gtk_image_menu_item_new_with_mnemonic("M_y checks");
    gtk_widget_show(my_checks);
    gtk_container_add(GTK_CONTAINER (check1_menu), my_checks);

    image15438 = gtk_image_new_from_stock("gtk-home", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image15438);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (my_checks), image15438);

  }

  menutools = NULL;
  if (guifeatures.tools()) {

    menutools = gtk_menu_item_new_with_mnemonic("_Tools");
    gtk_widget_show(menutools);
    gtk_container_add(GTK_CONTAINER (menubar1), menutools);

    menutools_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (menutools), menutools_menu);

    line_cutter_for_hebrew_text1 = gtk_image_menu_item_new_with_mnemonic("_Line cutter for Hebrew text");
    gtk_widget_show(line_cutter_for_hebrew_text1);
    gtk_container_add(GTK_CONTAINER (menutools_menu), line_cutter_for_hebrew_text1);

    image13532 = gtk_image_new_from_stock("gtk-cut", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image13532);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (line_cutter_for_hebrew_text1), image13532);

    notes_transfer = gtk_image_menu_item_new_with_mnemonic("_Transfer text to project notes");
    gtk_widget_show(notes_transfer);
    gtk_container_add(GTK_CONTAINER (menutools_menu), notes_transfer);

    image14659 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image14659);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (notes_transfer), image14659);

    tool_origin_references_in_bible_notes = gtk_image_menu_item_new_with_mnemonic("_Bible notes mass update");
    gtk_widget_show(tool_origin_references_in_bible_notes);
    gtk_container_add(GTK_CONTAINER (menutools_menu), tool_origin_references_in_bible_notes);

    image16248 = gtk_image_new_from_stock("gtk-find-and-replace", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image16248);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (tool_origin_references_in_bible_notes), image16248);

    tool_project_notes_mass_update1 = gtk_image_menu_item_new_with_mnemonic("_Project notes mass update");
    gtk_widget_show(tool_project_notes_mass_update1);
    gtk_container_add(GTK_CONTAINER (menutools_menu), tool_project_notes_mass_update1);

    image17187 = gtk_image_new_from_stock("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image17187);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (tool_project_notes_mass_update1), image17187);

    tool_generate_word_lists = gtk_image_menu_item_new_with_mnemonic("_Generate word lists");
    gtk_widget_show(tool_generate_word_lists);
    gtk_container_add(GTK_CONTAINER (menutools_menu), tool_generate_word_lists);

    image20671 = gtk_image_new_from_stock("gtk-justify-fill", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image20671);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (tool_generate_word_lists), image20671);

    tool_simple_text_corrections = gtk_image_menu_item_new_with_mnemonic("_Simple text corrections");
    gtk_widget_show(tool_simple_text_corrections);
    gtk_container_add(GTK_CONTAINER (menutools_menu), tool_simple_text_corrections);

    image21054 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image21054);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (tool_simple_text_corrections), image21054);

    tool_transfer_project_notes_to_text = gtk_image_menu_item_new_with_mnemonic("Tr_ansfer project notes to text");
    gtk_widget_show(tool_transfer_project_notes_to_text);
    gtk_container_add(GTK_CONTAINER (menutools_menu), tool_transfer_project_notes_to_text);

    image29089 = gtk_image_new_from_stock("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image29089);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (tool_transfer_project_notes_to_text), image29089);

  }

  menuitem_preferences = gtk_menu_item_new_with_mnemonic("P_references");
  // At first the Alt-P was the shortcut key. On the XO machine, this key is 
  // in use already: 
  // Alt-N goes to the Next Activity and
  // Alt-P goes to the Previous one. 
  // For that reason the Alt-R is now the shortcut key.
  gtk_widget_show(menuitem_preferences);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_preferences);

  menuitem_preferences_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_preferences), menuitem_preferences_menu);

  preferences_remote_git_repository = NULL;
  if (guifeatures.preferences()) {

    notes_preferences = gtk_image_menu_item_new_with_mnemonic("Project _notes");
    gtk_widget_show(notes_preferences);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), notes_preferences);

    image2116 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image2116);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (notes_preferences), image2116);

    printingprefs = gtk_image_menu_item_new_with_mnemonic("_Printing");
    gtk_widget_show(printingprefs);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), printingprefs);

    image3493 = gtk_image_new_from_stock("gtk-print", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image3493);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (printingprefs), image3493);

    reference_exchange1 = gtk_image_menu_item_new_with_mnemonic("_Reference exchange");
    gtk_widget_show(reference_exchange1);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), reference_exchange1);

    image5972 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image5972);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (reference_exchange1), image5972);

    ignored_references1 = gtk_image_menu_item_new_with_mnemonic("_Ignored references");
    gtk_widget_show(ignored_references1);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), ignored_references1);

    image6467 = gtk_image_new_from_stock("gtk-remove", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image6467);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (ignored_references1), image6467);

    prefs_books = gtk_image_menu_item_new_with_mnemonic("_Books");
    gtk_widget_show(prefs_books);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), prefs_books);

    image12167 = gtk_image_new_from_stock("gtk-index", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image12167);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (prefs_books), image12167);

    preferences_windows_outpost = gtk_image_menu_item_new_with_mnemonic("_Windows Outpost");
    gtk_widget_show(preferences_windows_outpost);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_windows_outpost);

    image14287 = gtk_image_new_from_stock("gtk-network", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image14287);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_windows_outpost), image14287);

    preferences_tidy_text = gtk_image_menu_item_new_with_mnemonic("_Tidy text");
    gtk_widget_show(preferences_tidy_text);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_tidy_text);

    image16359 = gtk_image_new_from_stock("gtk-clear", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image16359);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_tidy_text), image16359);

    preferences_remote_git_repository = gtk_image_menu_item_new_with_mnemonic("_Git repository");
    gtk_widget_show(preferences_remote_git_repository);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_remote_git_repository);

    image18977 = gtk_image_new_from_stock("gtk-connect", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image18977);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_remote_git_repository), image18977);

  }

  preferences_features = gtk_image_menu_item_new_with_mnemonic("F_eatures");
  gtk_widget_show(preferences_features);
  gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_features);

  image20936 = gtk_image_new_from_stock("gtk-properties", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image20936);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_features), image20936);

  preferences_planning = NULL;
  if (guifeatures.preferences()) {

    preferences_password = gtk_image_menu_item_new_with_mnemonic("P_assword");
    gtk_widget_show(preferences_password);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_password);

    image20937 = gtk_image_new_from_stock("gtk-dialog-authentication", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image20937);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_password), image20937);

    preferences_text_replacement = gtk_image_menu_item_new_with_mnemonic("Te_xt replacement");
    gtk_widget_show(preferences_text_replacement);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_text_replacement);

    image23181 = gtk_image_new_from_stock("gtk-find-and-replace", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image23181);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_text_replacement), image23181);

    pdf_viewer1 = gtk_image_menu_item_new_with_mnemonic("P_DF Viewer");
    gtk_widget_show(pdf_viewer1);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), pdf_viewer1);

    image24540 = gtk_image_new_from_stock("gtk-zoom-fit", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image24540);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (pdf_viewer1), image24540);

    preferences_reporting = gtk_image_menu_item_new_with_mnemonic("Rep_orting");
    gtk_widget_show(preferences_reporting);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_reporting);

    image25623 = gtk_image_new_from_stock("gtk-sort-ascending", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image25623);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_reporting), image25623);

    preferences_planning = gtk_image_menu_item_new_with_mnemonic("P_lanning");
    gtk_widget_show(preferences_planning);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_planning);

    image26888 = gtk_image_new_from_stock("gtk-preferences", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image26888);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_planning), image26888);

    preferences_graphical_interface = gtk_image_menu_item_new_with_mnemonic("Grap_hical Interface");
    gtk_widget_show(preferences_graphical_interface);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_graphical_interface);

    image27031 = gtk_image_new_from_stock("gtk-fullscreen", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image27031);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_graphical_interface), image27031);

    preferences_filters = gtk_image_menu_item_new_with_mnemonic("Filter_s");
    gtk_widget_show(preferences_filters);
    gtk_container_add(GTK_CONTAINER (menuitem_preferences_menu), preferences_filters);

    image28360 = gtk_image_new_from_stock("gtk-convert", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image28360);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (preferences_filters), image28360);

  }

  menuitem_help = gtk_menu_item_new_with_mnemonic("_Help");
  gtk_widget_show(menuitem_help);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_help);

  menuitem_help_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_help), menuitem_help_menu);

  help_main = gtk_image_menu_item_new_with_mnemonic("_Contents");
  gtk_widget_show(help_main);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), help_main);
  gtk_widget_add_accelerator(help_main, "activate", accel_group, 
  GDK_F1, (GdkModifierType) 0, GTK_ACCEL_VISIBLE);

  image17520 = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image17520);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (help_main), image17520);

  help_context = gtk_image_menu_item_new_with_mnemonic("C_ontext");
  gtk_widget_show(help_context);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), help_context);
  gtk_widget_add_accelerator(help_context, "activate", accel_group, 
  GDK_H, (GdkModifierType) GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  image16053 = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image16053);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (help_context), image16053);

  system_log1 = gtk_image_menu_item_new_with_mnemonic("_System log");
  gtk_widget_show(system_log1);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), system_log1);

  image4388 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image4388);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (system_log1), image4388);

  about1 = gtk_image_menu_item_new_from_stock("gtk-about", accel_group);
  gtk_widget_show(about1);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), about1);

  toolbar1 = gtk_toolbar_new();
  gtk_widget_show(toolbar1);
  gtk_box_pack_start(GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

  navigation.build(toolbar1);
  g_signal_connect ((gpointer) navigation.reference_signal_delayed, "clicked", G_CALLBACK (on_navigation_new_reference_clicked), gpointer(this));

  // Store project of last session because it gets affected when the editors build.
  focused_project_last_session = settings->genconfig.project_get();

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox5);
  gtk_box_pack_start(GTK_BOX (vbox1), hbox5, FALSE, FALSE, 0);

  hbox7 = gtk_hbox_new(FALSE, 8);
  gtk_widget_show(hbox7);
  gtk_box_pack_start(GTK_BOX (hbox5), hbox7, TRUE, TRUE, 0);

  statuslabel_style = gtk_label_new("");
  gtk_widget_show(statuslabel_style);
  gtk_box_pack_start(GTK_BOX (hbox7), statuslabel_style, FALSE, FALSE, 0);

  // This label could display a lot of styles so running out of space, and 
  // therefore needs to show an ellipsis if there is too much text.
  gtk_label_set_ellipsize(GTK_LABEL (statuslabel_style), PANGO_ELLIPSIZE_MIDDLE);
  gtk_label_set_max_width_chars(GTK_LABEL (statuslabel_style), 50);

  label_git = gtk_label_new("");
  gtk_widget_show(label_git);
  gtk_box_pack_start(GTK_BOX (hbox7), label_git, FALSE, FALSE, 0);

  statusbar1 = gtk_statusbar_new();
  gtk_widget_show(statusbar1);
  gtk_box_pack_start(GTK_BOX (hbox5), statusbar1, FALSE, TRUE, 0);
  gtk_widget_set_size_request(statusbar1, 25, -1);

  // Appearance of text in editor.
  set_fonts();

  // Size and position of window and screen layout.
  {
    ScreenLayoutDimensions dimensions(mainwindow);
    dimensions.verify();
  }

  g_signal_connect ((gpointer) mainwindow, "destroy", G_CALLBACK (gtk_main_quit), gpointer(this));
  g_signal_connect ((gpointer) mainwindow, "delete_event", G_CALLBACK (on_mainwindow_delete_event), gpointer(this));
  g_signal_connect ((gpointer) mainwindow, "focus_in_event", G_CALLBACK (on_mainwindow_focus_in_event), gpointer(this));
  g_signal_connect ((gpointer) mainwindow, "window_state_event", G_CALLBACK (on_mainwindow_window_state_event), gpointer(this));
  if (guifeatures.project_management()) {
    g_signal_connect ((gpointer) new1, "activate", G_CALLBACK (on_new1_activate), gpointer(this));
    g_signal_connect ((gpointer) open1, "activate", G_CALLBACK (on_open1_activate), gpointer(this));
    g_signal_connect ((gpointer) delete1, "activate", G_CALLBACK (on_delete1_activate), gpointer(this));
  }
  if (guifeatures.project_management()) {
    g_signal_connect ((gpointer) properties1, "activate", G_CALLBACK (on_properties1_activate), gpointer(this));
    g_signal_connect ((gpointer) import1, "activate", G_CALLBACK (on_import1_activate), gpointer(this));
    g_signal_connect ((gpointer) export_usfm_files, "activate", G_CALLBACK (on_export_usfm_files_activate), gpointer(this));
    g_signal_connect ((gpointer) export_zipped_unified_standard_format_markers1, "activate", G_CALLBACK (on_export_zipped_unified_standard_format_markers1_activate), gpointer(this));
    g_signal_connect ((gpointer) to_bibleworks_version_database_compiler, "activate", G_CALLBACK (on_to_bibleworks_version_compiler_activate), gpointer(this));
    g_signal_connect ((gpointer) export_to_sword_module, "activate", G_CALLBACK (on_export_to_sword_module_activate), gpointer(this));
    g_signal_connect ((gpointer) export_opendocument, "activate", G_CALLBACK (on_export_opendocument_activate), gpointer(this));
    g_signal_connect ((gpointer) copy_project_to, "activate", G_CALLBACK (on_copy_project_to_activate), gpointer(this));
    g_signal_connect ((gpointer) compare_with1, "activate", G_CALLBACK (on_compare_with1_activate), gpointer(this));
  }
  g_signal_connect ((gpointer) project_backup_incremental, "activate", G_CALLBACK (on_project_backup_incremental_activate), gpointer(this));
  g_signal_connect ((gpointer) project_backup_flexible, "activate", G_CALLBACK (on_project_backup_flexible_activate), gpointer(this));
  if (guifeatures.project_management()) {
    g_signal_connect ((gpointer) project_changes, "activate", G_CALLBACK (on_project_changes_activate), gpointer(this));
  }
  g_signal_connect ((gpointer) file_projects_merge, "activate", G_CALLBACK (on_file_projects_merge_activate), gpointer(this));
  if (guifeatures.references_management()) {
    g_signal_connect ((gpointer) open_references1, "activate", G_CALLBACK (on_open_references1_activate), gpointer(this));
    g_signal_connect ((gpointer) references_save_as, "activate", G_CALLBACK (on_references_save_as_activate), gpointer(this));
  }
  if (guifeatures.references_management()) {
    g_signal_connect ((gpointer) close_references, "activate", G_CALLBACK (on_close_references_activate), gpointer (this));
    g_signal_connect ((gpointer) delete_references, "activate", G_CALLBACK (on_delete_references_activate), gpointer (this));
    g_signal_connect ((gpointer) reference_hide, "activate", G_CALLBACK (on_reference_hide_activate), gpointer (this));
  }
  if (guifeatures.project_notes_management()) {
    g_signal_connect ((gpointer) new_note, "activate", G_CALLBACK (on_new_note_activate), gpointer(this));
    g_signal_connect ((gpointer) delete_note, "activate", G_CALLBACK (on_delete_note_activate), gpointer(this));
    g_signal_connect ((gpointer) import_notes, "activate", G_CALLBACK (on_import_notes_activate), gpointer(this));
    g_signal_connect ((gpointer) export_notes, "activate", G_CALLBACK (on_export_notes_activate), gpointer(this));
  }
  if (style)
    g_signal_connect ((gpointer) style, "activate", G_CALLBACK (on_file_styles_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources, "activate", G_CALLBACK (on_file_resources_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources_open, "activate", G_CALLBACK (on_file_resources_open_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources_close, "activate", G_CALLBACK (on_file_resources_close_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources_new, "activate", G_CALLBACK (on_file_resources_new_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources_edit, "activate", G_CALLBACK (on_file_resources_edit_activate), gpointer(this));
  g_signal_connect ((gpointer) file_resources_delete, "activate", G_CALLBACK (on_file_resources_delete_activate), gpointer(this));
  if (print)
    g_signal_connect ((gpointer) print, "activate", G_CALLBACK (on_print_activate), gpointer(this));
  g_signal_connect ((gpointer) quit1, "activate", G_CALLBACK (on_quit1_activate), gpointer(this));
  g_signal_connect ((gpointer) menuitem_edit, "activate", G_CALLBACK (on_edit1_activate), gpointer(this));
  g_signal_connect ((gpointer) cut1, "activate", G_CALLBACK (on_cut1_activate), gpointer(this));
  g_signal_connect ((gpointer) copy1, "activate", G_CALLBACK (on_copy1_activate), gpointer(this));
  g_signal_connect ((gpointer) copy_without_formatting, "activate", G_CALLBACK (on_copy_without_formatting_activate), gpointer(this));
  g_signal_connect ((gpointer) paste1, "activate", G_CALLBACK (on_paste1_activate), gpointer(this));
  g_signal_connect ((gpointer) undo1, "activate", G_CALLBACK (on_undo1_activate), gpointer(this));
  g_signal_connect ((gpointer) redo1, "activate", G_CALLBACK (on_redo1_activate), gpointer(this));
  if (guifeatures.references_and_find()) {
    g_signal_connect ((gpointer) find1, "activate", G_CALLBACK (on_findspecial1_activate), gpointer(this));
  }
  if (guifeatures.replace()) {
    g_signal_connect ((gpointer) find_and_replace1, "activate", G_CALLBACK (on_find_and_replace1_activate), gpointer (this));
  }
  g_signal_connect ((gpointer) find_in_notes1, "activate", G_CALLBACK (on_find_in_notes1_activate), gpointer(this));
  g_signal_connect ((gpointer) get_references_from_note, "activate", G_CALLBACK (on_get_references_from_note_activate), gpointer(this));
  g_signal_connect ((gpointer) edit_revert, "activate", G_CALLBACK (on_edit_revert_activate), gpointer(this));
  g_signal_connect ((gpointer) edit_bible_note, "activate", G_CALLBACK (on_edit_bible_note_activate), gpointer(this));
  g_signal_connect ((gpointer) editstatus, "activate", G_CALLBACK (on_editstatus_activate), gpointer(this));
  g_signal_connect ((gpointer) edit_planning, "activate", G_CALLBACK (on_edit_planning_activate), gpointer(this));
  g_signal_connect ((gpointer) menuitem_view, "activate", G_CALLBACK (on_menuitem_view_activate), gpointer(this));
  g_signal_connect ((gpointer) view_text_font, "activate", G_CALLBACK (on_view_text_font_activate), gpointer(this));
  /* Because of switching to GtkHtml for displaying and editing project notes, the fonts no longer can be set in the menu.
   g_signal_connect ((gpointer) view_notes_font, "activate", G_CALLBACK (on_view_notes_font_activate), gpointer(this));
   */
  if (guifeatures.project_notes())
    g_signal_connect ((gpointer) viewnotes, "activate", G_CALLBACK (on_viewnotes_activate), gpointer(this));
  g_signal_connect ((gpointer) view_git_tasks, "activate", G_CALLBACK (on_view_git_tasks_activate), gpointer(this));
  g_signal_connect ((gpointer) parallel_passages1, "activate", G_CALLBACK (on_parallel_passages1_activate), gpointer(this));
  g_signal_connect ((gpointer) view_usfm_code, "activate", G_CALLBACK (on_view_usfm_code_activate), gpointer(this));
  g_signal_connect ((gpointer) view_status, "activate", G_CALLBACK (on_view_status_activate), gpointer(this));
  g_signal_connect ((gpointer) view_planning, "activate", G_CALLBACK (on_view_planning_activate), gpointer(this));
  g_signal_connect ((gpointer) view_screen_layout, "activate", G_CALLBACK (on_view_screen_layout_activate), gpointer(this));
  g_signal_connect ((gpointer) view_keyterms, "activate", G_CALLBACK (on_view_keyterms_activate), gpointer(this));
  g_signal_connect ((gpointer) view_quick_references, "activate", G_CALLBACK (on_view_quick_references_activate), gpointer(this));
  g_signal_connect ((gpointer) view_outline, "activate", G_CALLBACK (on_view_outline_activate), gpointer(this));

  g_signal_connect ((gpointer) insert1, "activate", G_CALLBACK (on_insert1_activate), gpointer(this));
  if (guifeatures.project_notes()) {
    g_signal_connect ((gpointer) standard_text_1, "activate", G_CALLBACK (on_standard_text_1_activate), gpointer (this));
    g_signal_connect ((gpointer) standard_text_2, "activate", G_CALLBACK (on_standard_text_2_activate), gpointer (this));
    g_signal_connect ((gpointer) standard_text_3, "activate", G_CALLBACK (on_standard_text_3_activate), gpointer (this));
    g_signal_connect ((gpointer) standard_text_4, "activate", G_CALLBACK (on_standard_text_4_activate), gpointer (this));
    g_signal_connect ((gpointer) current_reference1, "activate", G_CALLBACK (on_current_reference1_activate), gpointer (this));
  }
  g_signal_connect ((gpointer) insert_special_character, "activate", G_CALLBACK (on_insert_special_character_activate), gpointer (this));
  g_signal_connect ((gpointer) synchronize_other_programs2, "activate", G_CALLBACK (on_synchronize_other_programs2_activate), gpointer(this));
  if (guifeatures.checks()) {
    g_signal_connect ((gpointer) check1, "activate", G_CALLBACK (on_check1_activate), gpointer(this));
    g_signal_connect ((gpointer) markers1, "activate", G_CALLBACK (on_markers1_activate), gpointer(this));
    g_signal_connect ((gpointer) validate_usfms1, "activate", G_CALLBACK (on_validate_usfms1_activate), gpointer(this));
    g_signal_connect ((gpointer) count_usfms1, "activate", G_CALLBACK (on_count_usfms1_activate), gpointer(this));
    g_signal_connect ((gpointer) compare_usfm1, "activate", G_CALLBACK (on_compare_usfm1_activate), gpointer(this));
    g_signal_connect ((gpointer) check_markers_spacing, "activate", G_CALLBACK (on_check_markers_spacing_activate), gpointer(this));
    g_signal_connect ((gpointer) chapters_and_verses1, "activate", G_CALLBACK (on_chapters_and_verses1_activate), gpointer(this));
    g_signal_connect ((gpointer) count_characters, "activate", G_CALLBACK (on_count_characters_activate), gpointer(this));
    g_signal_connect ((gpointer) unwanted_patterns, "activate", G_CALLBACK (on_unwanted_patterns_activate), gpointer(this));
    g_signal_connect ((gpointer) check_capitalization, "activate", G_CALLBACK (on_check_capitalization_activate), gpointer(this));
    g_signal_connect ((gpointer) check_repetition, "activate", G_CALLBACK (on_check_repetition_activate), gpointer(this));
    g_signal_connect ((gpointer) unwanted_words, "activate", G_CALLBACK (on_unwanted_words_activate), gpointer(this));
    g_signal_connect ((gpointer) check_matching_pairs, "activate", G_CALLBACK (on_check_matching_pairs_activate), gpointer(this));
    g_signal_connect ((gpointer) check_sentence_structure, "activate", G_CALLBACK (on_check_sentence_structure_activate), gpointer(this));
    g_signal_connect ((gpointer) word_count_inventory, "activate", G_CALLBACK (on_word_count_inventory_activate), gpointer(this));
    g_signal_connect ((gpointer) check_references_inventory, "activate", G_CALLBACK (on_check_references_inventory_activate), gpointer(this));
    g_signal_connect ((gpointer) check_references_validate, "activate", G_CALLBACK (on_check_references_validate_activate), gpointer(this));
    g_signal_connect ((gpointer) check_nt_quotations_from_the_ot, "activate", G_CALLBACK (on_check_nt_quotations_from_the_ot_activate), gpointer(this));
    g_signal_connect ((gpointer) synoptic_parallel_passages_from_the_nt, "activate", G_CALLBACK (on_synoptic_parallel_passages_from_the_nt_activate), gpointer(this));
    g_signal_connect ((gpointer) parallels_from_the_ot, "activate", G_CALLBACK (on_parallels_from_the_ot_activate), gpointer(this));
    g_signal_connect ((gpointer) check_key_terms, "activate", G_CALLBACK (on_check_key_terms_activate), gpointer(this));
    g_signal_connect ((gpointer) my_checks, "activate", G_CALLBACK (on_my_checks_activate), gpointer(this));
  }
  if (guifeatures.tools()) {
    g_signal_connect ((gpointer) menutools, "activate", G_CALLBACK (on_menutools_activate), gpointer(this));
    g_signal_connect ((gpointer) line_cutter_for_hebrew_text1, "activate", G_CALLBACK (on_line_cutter_for_hebrew_text1_activate), gpointer(this));
    g_signal_connect ((gpointer) notes_transfer, "activate", G_CALLBACK (on_notes_transfer_activate), gpointer(this));
    g_signal_connect ((gpointer) tool_origin_references_in_bible_notes, "activate", G_CALLBACK (on_tool_origin_references_in_bible_notes_activate), gpointer(this));
    g_signal_connect ((gpointer) tool_project_notes_mass_update1, "activate", G_CALLBACK (on_tool_project_notes_mass_update1_activate), gpointer(this));
    g_signal_connect ((gpointer) tool_generate_word_lists, "activate", G_CALLBACK (on_tool_generate_word_lists_activate), gpointer(this));
    g_signal_connect ((gpointer) tool_simple_text_corrections, "activate", G_CALLBACK (on_tool_simple_text_corrections_activate), gpointer(this));
    g_signal_connect ((gpointer) tool_transfer_project_notes_to_text, "activate", G_CALLBACK (on_tool_transfer_project_notes_to_text_activate), gpointer(this));
  }
  if (guifeatures.preferences()) {
    g_signal_connect ((gpointer) notes_preferences, "activate", G_CALLBACK (on_notes_preferences_activate), gpointer(this));
    g_signal_connect ((gpointer) printingprefs, "activate", G_CALLBACK (on_printingprefs_activate), gpointer(this));
    g_signal_connect ((gpointer) reference_exchange1, "activate", G_CALLBACK (on_reference_exchange1_activate), gpointer(this));
    g_signal_connect ((gpointer) ignored_references1, "activate", G_CALLBACK (on_ignored_references1_activate), gpointer(this));
    g_signal_connect ((gpointer) prefs_books, "activate", G_CALLBACK (on_prefs_books_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_windows_outpost, "activate", G_CALLBACK (on_preferences_windows_outpost_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_tidy_text, "activate", G_CALLBACK (on_preferences_tidy_text_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_remote_git_repository, "activate", G_CALLBACK (on_preferences_remote_git_repository_activate), gpointer(this));
  }
  g_signal_connect ((gpointer) preferences_features, "activate", G_CALLBACK (on_preferences_features_activate), gpointer(this));
  if (guifeatures.preferences()) {
    g_signal_connect ((gpointer) preferences_password, "activate", G_CALLBACK (on_preferences_password_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_text_replacement, "activate", G_CALLBACK (on_preferences_text_replacement_activate), gpointer(this));
    g_signal_connect ((gpointer) pdf_viewer1, "activate", G_CALLBACK (on_pdf_viewer1_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_reporting, "activate", G_CALLBACK (on_preferences_reporting_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_planning, "activate", G_CALLBACK (on_preferences_planning_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_graphical_interface, "activate", G_CALLBACK (on_preferences_graphical_interface_activate), gpointer(this));
    g_signal_connect ((gpointer) preferences_filters, "activate", G_CALLBACK (on_preferences_filters_activate), gpointer(this));
  }
  g_signal_connect ((gpointer) help_context, "activate", G_CALLBACK (on_help_context_activate), gpointer(this));
  g_signal_connect ((gpointer) help_main, "activate", G_CALLBACK (on_help_main_activate), gpointer(this));
  g_signal_connect ((gpointer) system_log1, "activate", G_CALLBACK (on_system_log1_activate), gpointer(this));
  g_signal_connect ((gpointer) about1, "activate", G_CALLBACK (on_about1_activate), gpointer(this));

  gtk_window_add_accel_group(GTK_WINDOW (mainwindow), accel_group);

  // Communication with BibleTime
  got_new_bt_reference = 0;
  g_timeout_add(100, GSourceFunc(mainwindow_on_external_programs_timeout), gpointer(this));

#ifndef WIN32
  // Signal handling.
  // Block the signal of a pipe error that otherwise would kill bibledit.
  signal(SIGPIPE, SIG_IGN);
  // USR1, ignore it.
  signal(SIGUSR1, SIG_IGN);
#endif

  // Display project notes.
  notes_redisplay();

  // Start the GUI updater.
  g_timeout_add(100, GSourceFunc(on_gui_timeout), gpointer(this));

  // Start bibledit http responder.
  g_timeout_add(300, GSourceFunc(on_check_httpd_timeout), gpointer(this));

  // Initialize content manager subsystem.
  git_initialize_subsystem();
  git_update_intervals_initialize();

  // Show main window.
  gtk_widget_show(mainwindow);
  // Present it because some window managers do not present it when started from 
  // a terminal.
  gtk_window_present(GTK_WINDOW (mainwindow));

  // Interprocess communications.
  extern InterprocessCommunication * ipc;
  ipc->methodcall_add_signal(ipcctGitJobDescription);
  ipc->methodcall_add_signal(ipcctGitTaskDone);
  g_signal_connect ((gpointer) ipc->method_called_signal, "clicked", G_CALLBACK (on_ipc_method_called), gpointer(this));

  // Show open windows.
  g_timeout_add(300, GSourceFunc(on_windows_startup_timeout), gpointer(this));

}

MainWindow::~MainWindow() {
  // Destroy the accelerator system.
  g_object_unref(G_OBJECT (accelerator_group));

  // Shut the separate windows down.
  shutdown_windows();

  // Hide bibledit. 
  gtk_widget_hide(mainwindow);

  // No ipc signals anymore.
  extern InterprocessCommunication * ipc;
  ipc->methodcall_remove_all_signals();

  // Destroy the Outpost
  delete windowsoutpost;
  // Finalize content manager subsystem.
  git_finalize_subsystem();
  // Do shutdown actions.
  shutdown_actions();
  // Destroying the window is done by gtk itself.
}

int MainWindow::run() {
  return gtk_dialog_run(GTK_DIALOG (mainwindow));
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

void MainWindow::enable_or_disable_widgets(bool enable) {
  // Set some widgets (in)sensitive depending on whether a project is open.
  if (properties1)
    gtk_widget_set_sensitive(properties1, enable);
  if (import1)
    gtk_widget_set_sensitive(import1, enable);
  if (notes2)
    gtk_widget_set_sensitive(notes2, enable);
  if (menuitem_edit)
    gtk_widget_set_sensitive(menuitem_edit, enable);
  if (file_references)
    gtk_widget_set_sensitive(file_references, enable);
  if (export_project)
    gtk_widget_set_sensitive(export_project, enable);
  if (print)
    gtk_widget_set_sensitive(print, enable);
  if (project_changes)
    gtk_widget_set_sensitive(project_changes, enable);
  if (menuitem_view)
    gtk_widget_set_sensitive(menuitem_view, enable);
  if (menuitem_goto)
    gtk_widget_set_sensitive(menuitem_goto, enable);
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
  if (preferences_remote_git_repository)
    gtk_widget_set_sensitive(preferences_remote_git_repository, enable);
  if (preferences_planning)
    gtk_widget_set_sensitive(preferences_planning, enable);
  if (project_backup)
    gtk_widget_set_sensitive(project_backup, enable);
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
 Menu callbacks
 |
 |
 |
 |
 |
 */

void MainWindow::on_open1_activate(GtkMenuItem * menuitem, gpointer user_data) {
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
  on_file_project_open(newproject);
}

void MainWindow::on_new1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->newproject();
}

void MainWindow::newproject() {
  git_command_pause(true);
  ProjectDialog projectdialog(true);
  if (projectdialog.run() == GTK_RESPONSE_OK) {
    on_file_project_open(projectdialog.newprojectname);
  }
  git_command_pause(false);
}

void MainWindow::on_properties1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->editproject();
}

void MainWindow::editproject() {
  git_command_pause(true);
  save_editors();
  // Show project dialog.
  ProjectDialog projectdialog(false);
  if (projectdialog.run() == GTK_RESPONSE_OK) {
    // Get focused project window.
    WindowEditor * editor_window = last_focused_editor_window();
    if (editor_window) {
      // Reload dictionaries.
      editor_window->editor->load_dictionaries();
    }
    // As anything could have been changed to the project, reopen it.
    reload_project();
  }
  git_command_pause(false);
}

void MainWindow::on_delete1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->deleteproject();
}

void MainWindow::deleteproject() {
  // Get all projects, leave the current one and the non-editable ones out.
  extern Settings * settings;
  vector <ustring> all_projects = projects_get_all();
  vector<ustring> projects;
  for (unsigned int i = 0; i < all_projects.size(); i++) {
    bool include = true;
    if (all_projects[i] == settings->genconfig.project_get())
      include = false;
    ProjectConfiguration * projectconfig = settings->projectconfig(all_projects[i]);
    if (!projectconfig->editable_get())
      include = false;
    if (include)
      projects.push_back(all_projects[i]);
  }
  // User interface.
  ListviewDialog dialog("Delete project", projects, "", true, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    int result;
    result = gtkw_dialog_question(mainwindow, "Are you sure you want to delete project " + dialog.focus + "?");
    if (result == GTK_RESPONSE_YES) {
      result = gtkw_dialog_question(mainwindow, "Are you really sure to delete project " + dialog.focus + ", something worth perhaps years of work?");
    }
    if (result == GTK_RESPONSE_YES) {
      project_delete(dialog.focus);
    }
  }
}

void MainWindow::on_quit1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  gtk_main_quit();
}

void MainWindow::on_system_log1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->viewlog();
}

void MainWindow::viewlog() {
  ShowScriptDialog showscript(0);
  showscript.run();
}

void MainWindow::on_help_context_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_help_context();
}

void MainWindow::on_help_context() {
  help_open(NULL, gpointer ("none"));
}

void MainWindow::on_help_main_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_help_main();
}

void MainWindow::on_help_main() {
  htmlbrowser("localhost:51516", true);
}

void MainWindow::on_about1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->showabout();
}

void MainWindow::showabout() {
  gtk_show_about_dialog(GTK_WINDOW (mainwindow),
  "version", PACKAGE_VERSION,
  "website", PACKAGE_BUGREPORT,
  "copyright", "Copyright (©) 2003-2008 Teus Benschop",
  "license", "This program is free software; you can redistribute it and/or modify\n"
  "it under the terms of the GNU General Public License as published by\n"
  "the Free Software Foundation; either version 3 of the License, or\n"
  "(at your option) any later version.\n"
  "\n"
  "This program is distributed in the hope that it will be useful,\n"
  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
  "GNU General Public License for more details.\n"
  "\n"
  "You should have received a copy of the GNU General Public License\n"
  "along with this program; if not, write to the Free Software\n"
  "Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.\n",
  NULL);
}

void MainWindow::on_undo1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_undo();
}

void MainWindow::menu_undo()
// Called for undo.
{
  /* 
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   if (editor->has_focus()) {
   editor->undo();
   }
   }
   */
  if (window_notes && (now_focused_signal_button == NULL) && (last_focused_signal_button == window_notes->focus_in_signal_button)) {
    //window_notes->undo();
  }
}

void MainWindow::on_redo1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_redo();
}

void MainWindow::menu_redo()
// Called for redo.
{
  /*
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   if (editor->has_focus()) {
   editor->redo();
   }
   }
   */
  if (window_notes && (now_focused_signal_button == NULL) && (last_focused_signal_button == window_notes->focus_in_signal_button)) {
    //window_notes->redo();
  }
}

void MainWindow::on_edit1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_edit();
}

void MainWindow::menu_edit() {
  // Set the sensitivity of some items in the Edit menu.
  /* 
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   gtk_widget_set_sensitive(copy_without_formatting, editor->has_focus());
   //gtk_widget_set_sensitive(undo1, editor->can_undo());
   //gtk_widget_set_sensitive(redo1, editor->can_redo());
   } else {
   gtk_widget_set_sensitive(undo1, true);
   gtk_widget_set_sensitive(redo1, true);
   }
   */

  // Sensitivity of the clipboard operations.
  // There is also the "owner-change" signal of the clipboard, but this is not
  // a reliable indicator for pastable content.
  bool cut = true;
  bool copy = true;
  bool paste = true;
  gtk_widget_set_sensitive(cut1, cut);
  gtk_widget_set_sensitive(copy1, copy);
  gtk_widget_set_sensitive(paste1, paste);

  // Enable/disable based on whether we're editing a note.
  bool enable = (window_notes && window_notes->note_being_edited());
  // References can only be taken from a note when it is opened.
  gtk_widget_set_sensitive(get_references_from_note, enable);

  // The Bible notes can only be edited when the cursor is in a note text.
  enable = false;
  WindowEditor * editor_window = last_focused_editor_window();
  if (editor_window)
    if (editor_window->editor->last_focused_type() == etvtNote)
      enable = true;
  gtk_widget_set_sensitive(edit_bible_note, enable);
}

void MainWindow::on_find_and_replace1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_replace();
}

void MainWindow::menu_replace() {
  // Before finding, save the current file.
  save_editors();
  // Display references.
  show_references_window();
  // Start find/replace dialog.
  vector <Reference> results;
  {
    ReplaceDialog replacedialog(0);
    if (replacedialog.run() == GTK_RESPONSE_OK) {
      results.assign(replacedialog.results.begin(), replacedialog.results.end());
      if (window_references) {
        window_references->display(replacedialog.results);
      }
    } else {
      return;
    }
  }
  // Replace text.
  if (results.size()) {
    ReplacingDialog replacedialog(results);
    replacedialog.run();
    reload_project();
  } else {
    gtkw_dialog_info(mainwindow, "There was nothing to replace");
  }
}

void MainWindow::on_findspecial1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_findspecial();
}

void MainWindow::menu_findspecial() {
  // Before finding, save the current file.
  save_editors();
  // Display the references window.
  show_references_window();
  // Start dialog.
  {
    SearchSpecialDialog dialog(&bibletime);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
  }
  // Carry out the search. 
  search_string(window_references->liststore, window_references->treeview, window_references->treecolumn, &bibletime);
}

void MainWindow::on_import1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->menu_import();
}

void MainWindow::menu_import() {
  bool structured, raw;
  import_dialog_selector(structured, raw);
  if (structured) {
    ImportTextDialog dialog(0);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
  }
  if (raw) {
    ImportRawTextDialog dialog(0);
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
  }
  reload_project();
}

gboolean MainWindow::on_mainwindow_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  /*
   This solves a bug:
   When quitting the program though the menu, all went fine.
   But when quitting the program by clicking on the cross at the top of the program,
   a segmentation fault occurred.
   This segmentation occurred in the destructor of the object MainWindow.
   In the destructor properties of GTK were accessed, which obviously were 
   already in the process of being destroyed.
   This is the solution:
   When the window receives the delete_event (somebody clicks on the cross to
   close the program), the window is not closed, but instead a timeout is 
   installed that calls qtk_main_quit after a short while.
   The destructor of the object MainWindow can not access the properties of GTK
   without a segmentation fault.
   */
  g_timeout_add(10, GSourceFunc(gtk_main_quit), user_data);
  return true;
}

void MainWindow::on_insert1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_insert();
}

void MainWindow::on_menu_insert()
// Sets the labels of the underlying menu items right.
{
  // Get the focused editor window.
  WindowEditor * editor_window = last_focused_editor_window();
  // Write the proper labels.
  extern Settings * settings;
  ustring std_txt = "Standard text ";
  ustring label;
  label = std_txt + "_1: " + settings->genconfig.edit_note_standard_text_one_get();
  if (standard_text_1)
    gtk_label_set_text_with_mnemonic(GTK_LABEL (gtk_bin_get_child (GTK_BIN (standard_text_1))), label.c_str());
  label = std_txt + "_2: " + settings->genconfig.edit_note_standard_text_two_get();
  if (standard_text_2)
    gtk_label_set_text_with_mnemonic(GTK_LABEL (gtk_bin_get_child (GTK_BIN (standard_text_2))), label.c_str());
  label = std_txt + "_3: " + settings->genconfig.edit_note_standard_text_three_get();
  if (standard_text_3)
    gtk_label_set_text_with_mnemonic(GTK_LABEL (gtk_bin_get_child (GTK_BIN (standard_text_3))), label.c_str());
  label = std_txt + "_4: " + settings->genconfig.edit_note_standard_text_four_get();
  if (standard_text_4)
    gtk_label_set_text_with_mnemonic(GTK_LABEL (gtk_bin_get_child (GTK_BIN (standard_text_4))), label.c_str());
  // Enable or disable depending on situation.
  bool enable = (window_notes && window_notes->note_being_edited());
  if (standard_text_1)
    gtk_widget_set_sensitive(standard_text_1, enable);
  if (standard_text_2)
    gtk_widget_set_sensitive(standard_text_2, enable);
  if (standard_text_3)
    gtk_widget_set_sensitive(standard_text_3, enable);
  if (standard_text_4)
    gtk_widget_set_sensitive(standard_text_4, enable);

  // Allow inserting reference when we edit a note and the reference is different 
  // from any of the references loaded already.
  enable = (window_notes && window_notes->note_being_edited());
  if (enable) {
    // Get all references from the note.
    vector<Reference> references;
    vector<ustring> messages;
    window_notes->get_references_from_note(references, messages);
    // See whether the current reference is already in it.
    bool already_in = false;
    for (unsigned int i = 0; i < references.size(); i++) {
      if (editor_window)
        if (references[i].equals(editor_window->editor->current_reference))
          already_in = true;
    }
    // If the reference is not yet in the note's references, enable menu, so user can add it.
    enable = !already_in;
  }
  // Update menu.
  ProjectConfiguration * projectconfig = settings->projectconfig(settings->genconfig.project_get());
  if (editor_window) {
    label = "_Add " + editor_window->editor->current_reference.human_readable(projectconfig->language_get()) + " to project note";
  } else {
    enable = false;
  }

  if (current_reference1)
    gtk_label_set_text_with_mnemonic(GTK_LABEL (gtk_bin_get_child (GTK_BIN (current_reference1))), label.c_str());
  if (current_reference1)
    gtk_widget_set_sensitive(current_reference1, enable);

  // Inserting special character.
  gtk_widget_set_sensitive(insert_special_character, (editor_window && (now_focused_signal_button == editor_window->focus_in_signal_button)));
}

void MainWindow::on_menuitem_view_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menuitem_view();
}

void MainWindow::on_menuitem_view() {
}

void MainWindow::on_notes_preferences_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_notes_preferences();
}

void MainWindow::on_notes_preferences() {
  NotesDialog dialog(0);
  dialog.run();
}

void MainWindow::on_copy_project_to_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_copy_project_to();
}

void MainWindow::on_copy_project_to()
// Copy project to another one.
{
  save_editors();
  extern Settings * settings;
  EntryDialog dialog("New project name", "Enter a name of a non-existent project\nwhere this project will be copied to.", settings->genconfig.project_get());
  if (dialog.run() == GTK_RESPONSE_OK) {
    // Does the project exist?
    if ((project_exists(dialog.entered_value)) || (dialog.entered_value == "data")) {
      // Yes, give message that project exists.
      ustring error = "Project ";
      error.append(dialog.entered_value);
      error.append(" already exists.");
      error.append("\nIf you still intend to copy the project,");
      error.append("\ndelete project ");
      error.append(dialog.entered_value);
      error.append(" first.");
      gtkw_dialog_error(mainwindow, error);
    } else {
      // Ok, go ahead with the copy.
      project_copy(settings->genconfig.project_get(), dialog.entered_value);
      // Give message when through.
      ustring message;
      message.append("The project has been copied to a new project\n");
      message.append("named ");
      message.append(dialog.entered_value);
      message.append(".");
      gtkw_dialog_info(mainwindow, message);
    }
  }
}

void MainWindow::on_compare_with1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_compare_with();
}

void MainWindow::on_compare_with()
// Compare the current project with another one.
{
  save_editors();
  show_references_window();
  git_command_pause(true);
  References references(window_references->liststore, window_references->treeview, window_references->treecolumn);
  CompareDialog dialog(&references);
  dialog.run();
  git_command_pause(false);
}

void MainWindow::on_printingprefs_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_printing_preferences();
}

void MainWindow::on_printing_preferences() {
  PrintPreferencesDialog dialog(0);
  dialog.run();
}

void MainWindow::on_prefs_books_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_prefs_books();
}

void MainWindow::on_prefs_books() {
  extern Settings * settings;
  BookDialog dialog(settings->genconfig.project_get());
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_project();
  }
}

void MainWindow::on_preferences_tidy_text_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_tidy_text();
}

void MainWindow::on_preferences_tidy_text() {
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

void MainWindow::on_navigation_new_reference_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_navigation_new_reference();
}

void MainWindow::on_navigation_new_reference() {
  extern Settings * settings;
  settings->genconfig.book_set(navigation.reference.book);
  settings->genconfig.chapter_set(convert_to_string(navigation.reference.chapter));
  settings->genconfig.verse_set(navigation.reference.verse);
  go_to_new_reference();
  // Optionally display the parallel passages in the reference area.
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (parallel_passages1))) {
    show_references_window();
    parallel_passages_display(navigation.reference, window_references->liststore, window_references->treeview, window_references->treecolumn);
  }
  // Optional displaying keyterms in verse.
  if (window_show_keyterms) {
    window_show_keyterms->go_to(settings->genconfig.project_get(), navigation.reference);
  }
  // Optional resource windows.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    resource_windows[i]->go_to(navigation.reference);
  }
  // Optional outline window.
  if (window_outline) {
    window_outline->go_to(settings->genconfig.project_get(), navigation.reference);
  }
}

void MainWindow::goto_next_verse() {
  navigation.nextverse();
}

void MainWindow::goto_previous_verse() {
  navigation.previousverse();
}

void MainWindow::goto_next_chapter() {
  navigation.nextchapter();
}

void MainWindow::goto_previous_chapter() {
  navigation.previouschapter();
}

void MainWindow::goto_next_book() {
  navigation.nextbook();
}

void MainWindow::goto_previous_book() {
  navigation.previousbook();
}

void MainWindow::goto_reference_interactive() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (editor_window) {
    Editor * editor = editor_window->editor;
    GotoReferenceDialog dialog(editor->current_reference.book, editor->current_reference.chapter, editor->current_reference.verse);
    if (dialog.run() == GTK_RESPONSE_OK) {
      if (dialog.newreference) {
        // If the dialog closes, then another window will receive focus again.
        // This focusing causes the navigation to take the values as they are in the configuration.
        // This would frustrate the desire of the user to go somewhere else.
        // To fix the problem, the settings are updated here.
        extern Settings * settings;
        settings->genconfig.book_set(dialog.reference.book);
        settings->genconfig.chapter_set(convert_to_string(dialog.reference.chapter));
        settings->genconfig.verse_set(dialog.reference.verse);
        navigation.display(dialog.reference);
      }
    }
  }
}

void MainWindow::go_to_new_reference()
// This starts the procedure to carries out a requested change of reference.
{
  // Let the editor(s) show the right reference.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->go_to(navigation.reference);
  }

  // Create a reference for the external programs.
  // These do not take verses like 10a or 10-12, but only numbers like 10 or 12.
  Reference goto_reference(navigation.reference.book, navigation.reference.chapter, number_in_string(navigation.reference.verse));

  // Send it to the external programs.
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_bibleworks_get())
    windowsoutpost->BibleWorksReferenceSet(goto_reference);
  if (settings->genconfig.reference_exchange_send_to_santafefocus_get())
    windowsoutpost->SantaFeFocusReferenceSet(goto_reference);

  // Send to resources.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    resource_windows[i]->go_to(goto_reference);
  }
  // Update the notes view.
  notes_redisplay();
}

void MainWindow::on_new_verse_signalled(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_new_verse();
}

void MainWindow::on_new_verse()
/*
 When the cursor has moved, the navigation system needs to be updated
 so that it shows the right reference. If the user was, for example
 on Matthew 1:10, and the cursor moves, the move might have brought him
 to another reference, though this is not necessarily so. Therefore, as we 
 don't know where the user is now after the cursor moved, we need to find
 it out. The book is known, the chapter is known, because both stay the same.
 The only thing we don't know is the verse. 
 */
{
  WindowEditor * editor_window = last_focused_editor_window();
  if (editor_window) {
    Editor * editor = editor_window->editor;
    Reference reference(navigation.reference.book, navigation.reference.chapter, editor->current_verse_number);
    navigation.display(reference);
    if (window_outline)
      window_outline->go_to(editor->project, navigation.reference);
  }
}

void MainWindow::on_synchronize_other_programs2_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_synchronize_other_programs();
}

void MainWindow::on_synchronize_other_programs() {
  // Get the focused editor. If none, bail out.
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  // Send the reference.
  extern Settings * settings;
  if (settings->genconfig.reference_exchange_send_to_bibleworks_get()) {
    windowsoutpost->BibleWorksReferenceSet(editor_window->editor->current_reference);
  }
  if (settings->genconfig.reference_exchange_send_to_santafefocus_get()) {
    windowsoutpost->SantaFeFocusReferenceSet(editor_window->editor->current_reference);
  }
  if (settings->genconfig.reference_exchange_send_to_bibletime_get()) {
    bibletime.sendreference(editor_window->editor->current_reference);
  }
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    resource_windows[i]->go_to(editor_window->editor->current_reference);
  }
}

void MainWindow::on_text_area_activate() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (editor_window) {
    editor_window->present();
  }
}

void MainWindow::on_tools_area_activate() {
}

void MainWindow::on_notes_area_activate() {
  view_project_notes();
  notes_redisplay();
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

void MainWindow::on_cut1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_cut();
}

void MainWindow::on_cut() {
  /* 
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   if (editorsgui->has_focus()) {
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   gtk_clipboard_set_text(clipboard, editor->text_get_selection ().c_str(), -1);
   editor->text_erase_selection();
   }
   }
   if (window_notes && (now_focused_signal_button == NULL) && (last_focused_signal_button == window_notes->focus_in_signal_button)) {
   //window_notes->cut();
   }
   */
}

void MainWindow::on_copy1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_copy();
}

void MainWindow::on_copy() {
  /* 
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   if (editorsgui->has_focus()) {
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   // In case of the text editor, the USFM code is copied, not the plain text. 
   gtk_clipboard_set_text(clipboard, editor->text_get_selection ().c_str(), -1);
   }
   }
   if (window_notes && (now_focused_signal_button == NULL) && (last_focused_signal_button == window_notes->focus_in_signal_button)) {
   //window_notes->copy();
   }
   if (window_check_keyterms) {
   window_check_keyterms->copy_clipboard();
   }
   */
}

void MainWindow::on_copy_without_formatting_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_copy_without_formatting();
}

void MainWindow::on_copy_without_formatting() {
  /* 
   GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
   if (editorsgui->has_focus()) {
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   gtk_text_buffer_copy_clipboard(editor->last_focused_textbuffer(), clipboard);
   }
   }
   */
}

void MainWindow::on_paste1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_paste();
}

void MainWindow::on_paste() {
  // Get the clipboard.
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  // Bail out if no text is available.
  if (!gtk_clipboard_wait_is_text_available(clipboard))
    return;
  // Paste text in the focused textview.  
  /* 
   if (editorsgui->has_focus()) {
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   gchar * text = gtk_clipboard_wait_for_text(clipboard);
   if (text) {
   editor->text_insert(text);
   g_free(text);
   }
   }
   }
   if (window_notes && (now_focused_signal_button == NULL) && (last_focused_signal_button == window_notes->focus_in_signal_button)) {
   //window_notes->paste();
   }
   */
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

void MainWindow::show_references_window() {
  if (!window_references) {
    window_references = new WindowReferences (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_references->delete_signal_button, "clicked", G_CALLBACK (on_window_references_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_references->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_references->general_signal_button, "clicked", G_CALLBACK (on_window_references_general_signal_button_clicked), gpointer(this));
  }
}

void MainWindow::on_window_references_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_references_delete_button();
}

void MainWindow::on_window_references_delete_button() {
  if (window_references) {
    delete window_references;
    window_references = NULL;
  }
}

void MainWindow::on_window_references_general_signal_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_references_general_signal_button();
}

void MainWindow::on_window_references_general_signal_button()
// This routine is called when the reference window fires a signal that something has happened.
{
  switch (window_references->action)
  {
    case wratReferenceActivated:
    {
      on_list_goto();
      break;
    }
    case wratPopupMenu:
    {
      gtk_menu_popup(GTK_MENU (file_references_menu), NULL, NULL, NULL, NULL, window_references->popup_button, window_references->popup_event_time);
      break;
    }
    case wratReferencesSelected:
    {
      treeview_references_display_quick_reference();
      break;
    }
  }
}

void MainWindow::on_list_goto()
// Handler for when the user pressed Enter in the list and wants to go to a reference.
{
  // Get the editor window. If none, bail out.
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;

  // Bail out if there's no references window.
  if (!window_references)
    return;

  // Jump to the reference.
  navigation.display(window_references->reference);
  editor_window->editor->go_to_new_reference_highlight = true;
}

void MainWindow::on_open_references1_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_open_references();
}

void MainWindow::on_open_references() {
  show_references_window();
  window_references->open();
}

void MainWindow::on_references_save_as_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_save_references();
}

void MainWindow::on_save_references() {
  show_references_window();
  window_references->save();
}

void MainWindow::on_close_references_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_clear_references();
}

void MainWindow::on_clear_references() {
  show_references_window();
  window_references->clear();
}

void MainWindow::on_delete_references_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_delete_references();
}

void MainWindow::on_delete_references() {
  show_references_window();
  window_references->dismiss();
}

void MainWindow::on_next_reference()
// This goes to the next reference, if there is any.
// If no item has been selected it chooses the first, if it's there.
{
  // Display references.
  show_references_window();
  // Select next reference in the list.
  References references(window_references->liststore, window_references->treeview, window_references->treecolumn);
  references.goto_next();
  // Actually open the reference in the editor.
  window_references->activate();
}

void MainWindow::on_previous_reference()
// This goes to the previous reference, if there is any.
// If no item has been selected it chooses the first, if it's there.
{
  // Show references.
  show_references_window();
  // Select previous reference in the list.
  References references(window_references->liststore, window_references->treeview, window_references->treecolumn);
  references.goto_previous();
  // Actually open the reference in the editor.
  window_references->activate();
}

void MainWindow::on_ignored_references1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_ignored_references();
}

void MainWindow::on_ignored_references() {
  vector<ustring> hidden_references = references_hidden_ones_load();
  EditListDialog dialog( &hidden_references, "Hidden references", "of references and comments that will never be shown in the reference area.", true, false, true, false, false, false, false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK)
    references_hidden_ones_save(hidden_references);
}

void MainWindow::on_reference_hide_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_reference_hide();
}

void MainWindow::on_reference_hide()
// Deals with hiding references.
{
  show_references_window();
  window_references->hide();
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

bool MainWindow::mainwindow_on_external_programs_timeout(gpointer data) {
  return ((MainWindow *) data)->on_external_programs_timeout();
}

bool MainWindow::on_external_programs_timeout() {
  // Deal with exchanging references between Bibledit and BibleTime.
  // The trick of the trade here is that we look which of the two made a change.
  // If Bibledit made a change, then it sends its reference to BibleTime.
  // And vice versa.
  // This avoids race conditions, i.e. they keep sending
  // references to each other, and the reference keeps changing between the 
  // one Bibledit had and the one BibleTime had.

  extern Settings * settings;
  // A reference is taken from BibleTime only when it changed reference.
  if (settings->genconfig.reference_exchange_receive_from_bibletime_get()) {
    Reference btreference(0);
    if (bibletime.getreference(btreference)) {
      ustring new_reference = btreference.human_readable("");
      if (new_reference != bibletime_previous_reference) {
        bibletime_previous_reference = new_reference;
        // As BibleTime does not support chapter 0 or verse 0, let Bibledit not 
        // receive any reference from it if it is on such a chapter or verse.
        bool receive = true;
        if (navigation.reference.chapter == 0)
          receive = false;
        if (navigation.reference.verse == "0")
          receive = false;
        if (receive)
          navigation.display(btreference);
        got_new_bt_reference = 5;
      }
    }
  }

  // The reference is sent to BibleTime only after it changed.
  // But when we received a new reference, nothing will be sent, 
  // to avoid the race condition that both Bibledit and BibleTime keep 
  // alternating between two references on their own.
  if (got_new_bt_reference > 0)
    got_new_bt_reference--;
  if (got_new_bt_reference == 0) {
    WindowEditor * editor_window = last_focused_editor_window();
    if (editor_window) {
      if (settings->genconfig.reference_exchange_send_to_bibletime_get()) {
        ustring bibledit_bt_new_reference = convert_to_string(editor_window->editor->current_reference.book) + convert_to_string(editor_window->editor->current_reference.chapter)
            + editor_window->editor->current_reference.verse;
        if (bibledit_bt_new_reference != bibledit_bt_previous_reference) {
          bibledit_bt_previous_reference = bibledit_bt_new_reference;
          bibletime.sendreference(editor_window->editor->current_reference);
        }
      }
    }
  }

  return true;
}

void MainWindow::on_reference_exchange1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_reference_exchange();
}

void MainWindow::on_reference_exchange() {
  ReferenceExchangeDialog dialog(0);
  dialog.run();
}

void MainWindow::on_send_word_to_toolbox_signalled(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->send_word_to_toolbox();
}

void MainWindow::send_word_to_toolbox() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  ustring word = editor_window->editor->word_double_clicked_text;
  if (word.empty())
    return;
  gw_message("Sending to Toolbox: " + word);
  windowsoutpost->SantaFeFocusWordSet(word);
}

void MainWindow::on_preferences_windows_outpost_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_windows_outpost();
}

void MainWindow::on_preferences_windows_outpost() {
  // Dialog for making settings.
  OutpostDialog dialog(0);
  dialog.run();
  if (dialog.changed) {
    // Changes were made: destroy and recreate the object.
    delete windowsoutpost;
    windowsoutpost = new WindowsOutpost (true);
    // If required, start the Outpost.
    extern Settings * settings;
    if (settings->genconfig.use_outpost_get()) {
      // Delay a bit so wine debugger doesn't start.
      g_usleep(1000000);
      windowsoutpost->Start();
    }
  }
}

/*
 |
 |
 |
 |
 |
 Title bar and status bar
 |
 |
 |
 |
 |
 */

bool MainWindow::on_gui_timeout(gpointer data) {
  ((MainWindow *) data)->on_gui();
  return true;
}

void MainWindow::on_gui()
// Tasks related to the GUI.
{
  // The accelerators for undo and redo (Control(+Shift)-Z) stop working after
  // the widget has been set to insensitive and back to sensitive again.
  // As this may happen often, here we keep setting the accelerators.
  // But keeping setting these too often seems to disturb the mechanism, therefore 
  // they are set only when there was a change that makes it needed.
  {
    gint undo_redo_state = 0;
    if (GTK_WIDGET_SENSITIVE(undo1))
      undo_redo_state++;
    if (GTK_WIDGET_SENSITIVE(redo1))
      undo_redo_state++;
    if (undo_redo_state != editor_undo_redo_accelerator_state) {
      gtk_widget_remove_accelerator(undo1, accel_group, GDK_Z, GDK_CONTROL_MASK);
      gtk_widget_add_accelerator(undo1, "activate", accel_group, GDK_Z, GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);
      gtk_widget_remove_accelerator(redo1, accel_group, GDK_Z, GdkModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK));
      gtk_widget_add_accelerator(redo1, "activate", accel_group, GDK_Z, GdkModifierType(GDK_CONTROL_MASK | GDK_SHIFT_MASK), GTK_ACCEL_VISIBLE);
      editor_undo_redo_accelerator_state = undo_redo_state;
    }
  }

  // Display information about the number of Git tasks to be done if there are many of them.
  {
    ustring git;
    if (git_tasks_count() >= 100)
      git = "Git tasks pending: " + convert_to_string(git_tasks_count());
    ustring currenttext = gtk_label_get_text(GTK_LABEL (label_git));
    if (git != currenttext) {
      gtk_label_set_text(GTK_LABEL (label_git), git.c_str());
    }
  }

  // Check whether to reopen the project.
  on_git_reopen_project();

  // Care for possible restart.
  extern Settings * settings;
  if (settings->session.restart) {
    gtk_main_quit();
  }
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

void MainWindow::view_project_notes() {
  if (!project_notes_enabled)
    return;
  if (window_notes) {
    // If the window is there, present it to the user.
    window_notes->present();
  } else {
    window_notes = new WindowNotes (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_notes->delete_signal_button, "clicked", G_CALLBACK (on_window_notes_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_notes->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    treeview_references_display_quick_reference();
  }
}

void MainWindow::on_window_notes_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_notes_delete_button();
}

void MainWindow::on_window_notes_delete_button() {
  if (window_notes) {
    delete window_notes;
    window_notes = NULL;
  }
}

void MainWindow::on_new_note_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_new_note();
}

void MainWindow::on_new_note() {
  // Display notes.
  view_project_notes();
  // Create new note.
  if (window_notes)
    window_notes->new_note();
}

void MainWindow::on_delete_note_activate(GtkMenuItem *menuitem, gpointer user_data) {
  gtkw_dialog_info(((MainWindow *) user_data)->mainwindow, "A note can be deleted by clicking on the [delete] link in the notes view");
}

void MainWindow::on_viewnotes_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_notes();
}

void MainWindow::on_view_notes() {
  ShowNotesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    view_project_notes();
    notes_redisplay();
  }
}

void MainWindow::notes_redisplay() {
  if (window_notes) {
    window_notes->redisplay();
  }
}

void MainWindow::on_find_in_notes1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->find_in_notes();
}

void MainWindow::find_in_notes() {
  FindNoteDialog findnotedialog(0);
  if (findnotedialog.run() == GTK_RESPONSE_OK) {
    view_project_notes();
    if (window_notes) {
      window_notes->display(findnotedialog.ids);
    }
  }
}

void MainWindow::on_import_notes_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_import_notes();
}

void MainWindow::on_import_notes() {
  ImportNotesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_APPLY) {
    view_project_notes();
    notes_redisplay();
  }
}

void MainWindow::on_export_notes_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_export_notes();
}

void MainWindow::on_export_notes() {
  view_project_notes();
  int result;
  ustring filename;
  ExportNotesFormat format;
  bool save_all_notes;
  {
    ExportNotesDialog dialog(0);
    result = dialog.run();
    filename = dialog.filename;
    format = dialog.exportnotesformat;
    save_all_notes = dialog.save_all_notes;
  }
  if (result == GTK_RESPONSE_OK) {
    vector <unsigned int> ids_to_display;
    export_translation_notes(filename, format, ids_to_display, save_all_notes, mainwindow);
  }
}

void MainWindow::on_standard_text_1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_standard_text(menuitem);
}

void MainWindow::on_standard_text_2_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_standard_text(menuitem);
}

void MainWindow::on_standard_text_3_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_standard_text(menuitem);
}

void MainWindow::on_standard_text_4_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_standard_text(menuitem);
}

void MainWindow::on_insert_standard_text(GtkMenuItem *menuitem) {
  // Find out which standard text to insert, and where to insert it, and how.
  extern Settings * settings;
  ustring standardtext;
  unsigned int selector = 0;
  bool addspace = false;
  bool gtkhtml = false;
  if (menuitem == GTK_MENU_ITEM (standard_text_1)) {
    standardtext = settings->genconfig.edit_note_standard_text_one_get();
    selector = 0;
    addspace = true;
    gtkhtml = true;
  } else if (menuitem == GTK_MENU_ITEM (standard_text_2)) {
    standardtext = settings->genconfig.edit_note_standard_text_two_get();
    selector = 1;
    addspace = true;
    gtkhtml = true;
  } else if (menuitem == GTK_MENU_ITEM (standard_text_3)) {
    standardtext = settings->genconfig.edit_note_standard_text_three_get();
    selector = 2;
    addspace = true;
    gtkhtml = true;
  } else if (menuitem == GTK_MENU_ITEM (standard_text_4)) {
    standardtext = settings->genconfig.edit_note_standard_text_four_get();
    selector = 3;
    addspace = true;
    gtkhtml = true;
  } else if (menuitem == GTK_MENU_ITEM (current_reference1)) {
    WindowEditor * editor_window = last_focused_editor_window();
    if (editor_window) {
      Editor * editor = editor_window->editor;
      standardtext = books_id_to_english(editor->current_reference.book) + " " + convert_to_string(editor->current_reference.chapter) + ":" + editor->current_reference.verse;
      selector = 4;
      addspace = false;
      gtkhtml = false;
    }
  }

  // Insert the text.
  if (window_notes) {
    window_notes->insert_standard_text(selector);
  }
}

void MainWindow::on_current_reference1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_standard_text(menuitem);
}

void MainWindow::on_get_references_from_note_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_get_references_from_note();
}

void MainWindow::on_get_references_from_note() {
  // Store references.
  vector<Reference> references;
  // Store possible messages here, but they will be dumped.
  vector<ustring> messages;
  // Get all references from the editor.
  if (window_notes)
    window_notes->get_references_from_note(references, messages);
  // Sort the references so they appear nicely in the editor.
  sort_references(references);
  // Set none searchwords.
  extern Settings* settings;
  settings->session.highlights.clear();
  // Display the References
  show_references_window();
  // Set references.
  References references2(window_references->liststore, window_references->treeview, window_references->treecolumn);
  references2.set_references(references);
  ProjectConfiguration * projectconfig = settings->projectconfig(settings->genconfig.project_get());
  references2.fill_store(projectconfig->language_get());
}

void MainWindow::notes_get_references_from_id(gint id)
// Get the references from the note id
{
  // Store references we get.
  vector<Reference> references;

  // Fetch the references for the note from the database.
  sqlite3 *db;
  int rc;
  char *error= NULL;
  try
  {
    rc = sqlite3_open(notes_database_filename ().c_str (), &db);
    if (rc) throw runtime_error (sqlite3_errmsg(db));
    sqlite3_busy_timeout (db, 1000);
    SqliteReader sqlitereader (0);
    char * sql;
    sql = g_strdup_printf ("select ref_osis from %s where id = %d;", TABLE_NOTES, id);
    rc = sqlite3_exec(db, sql, sqlitereader.callback, &sqlitereader, &error);
    g_free (sql);
    if (rc != SQLITE_OK)
    throw runtime_error (error);
    if ((sqlitereader.ustring0.size()> 0))
    {
      ustring reference;
      reference = sqlitereader.ustring0[0];
      // Read the reference(s).
      Parse parse (reference, false);
      for (unsigned int i = 0; i < parse.words.size(); i++)
      {
        Reference ref (0);
        reference_discover (0, 0, "", parse.words[i], ref.book, ref.chapter, ref.verse);
        references.push_back (ref);
      }
    }
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Close connection.  
  sqlite3_close(db);

  // Display the References
  show_references_window();
  // Set references.
  References references2(window_references->liststore, window_references->treeview, window_references->treecolumn);
  references2.set_references(references);
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig(settings->genconfig.project_get());
  references2.fill_store(projectconfig->language_get());
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

void MainWindow::on_export_usfm_files_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_export_usfm_files(false);
}

void MainWindow::on_export_zipped_unified_standard_format_markers1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_export_usfm_files(true);
}

void MainWindow::on_export_usfm_files(bool zipped) {
  save_editors();
  export_to_usfm(mainwindow, zipped);
}

void MainWindow::on_to_bibleworks_version_compiler_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_to_bibleworks_version_compiler();
}

void MainWindow::on_to_bibleworks_version_compiler() {
  save_editors();
  export_to_bibleworks(mainwindow);
}

void MainWindow::on_export_to_sword_module_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_export_to_sword_module();
}

void MainWindow::on_export_to_sword_module() {
  save_editors();
  export_to_sword_interactive();
  bibletime.reloadmodules();
}

void MainWindow::on_export_opendocument_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_export_opendocument();
}

void MainWindow::on_export_opendocument() {
  save_editors();
  export_to_opendocument(mainwindow);
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

void MainWindow::on_check1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check();
}

void MainWindow::on_menu_check() {
  // Display the references.
  show_references_window();
}

void MainWindow::on_markers1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check_markers();
}

void MainWindow::on_menu_check_markers() {
}

void MainWindow::on_validate_usfms1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check_markers_validate();
}

void MainWindow::on_menu_check_markers_validate() {
  save_editors();
  show_references_window();
  scripture_checks_validate_usfms(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_count_usfms1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check_markers_count();
}

void MainWindow::on_menu_check_markers_count() {
  save_editors();
  scripture_checks_count_usfms(true);
}

void MainWindow::on_compare_usfm1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check_markers_compare();
}

void MainWindow::on_menu_check_markers_compare() {
  save_editors();
  show_references_window();
  scripture_checks_compare_usfms(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_chapters_and_verses1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_menu_check_chapters_and_verses();
}

void MainWindow::on_menu_check_chapters_and_verses() {
  save_editors();
  show_references_window();
  scripture_checks_chapters_verses(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_count_characters_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_count_characters();
}

void MainWindow::on_count_characters() {
  save_editors();
  scripture_checks_count_characters(true);
}

void MainWindow::on_unwanted_patterns_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_unwanted_patterns();
}

void MainWindow::on_unwanted_patterns() {
  save_editors();
  show_references_window();
  scripture_checks_unwanted_patterns(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_check_capitalization_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_capitalization();
}

void MainWindow::on_check_capitalization() {
  save_editors();
  show_references_window();
  scripture_checks_capitalization(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_check_repetition_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_repetition();
}

void MainWindow::on_check_repetition() {
  save_editors();
  show_references_window();
  scripture_checks_repetition(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_check_matching_pairs_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_matching_pairs();
}

void MainWindow::on_check_matching_pairs() {
  save_editors();
  show_references_window();
  scripture_checks_matching_pairs(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_unwanted_words_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_unwanted_words();
}

void MainWindow::on_unwanted_words() {
  save_editors();
  show_references_window();
  scripture_checks_unwanted_words(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_word_count_inventory_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_word_count_inventory();
}

void MainWindow::on_word_count_inventory() {
  save_editors();
  scripture_checks_word_inventory(true);
}

bool MainWindow::on_check_httpd_timeout(gpointer data) {
  ((MainWindow *) data)->on_check_httpd();
  return true;
}

void MainWindow::on_check_httpd() {
  // Does the httpd have a request for us?
  if (!httpd.search_whole_word.empty()) {
    // Bibledit presents itself and searches for the word.
    gtk_window_present(GTK_WINDOW (mainwindow));
    extern Settings * settings;
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
    SessionHighlights
        sessionhighlights(settings->session.searchword, settings->session.search_case_sensitive, settings->session.search_globbing, settings->session.search_start_word_match, settings->session.search_end_word_match, atRaw, false, false, false, false, false, false, false, false);
    settings->session.highlights.push_back(sessionhighlights);
    show_references_window();
    search_string(window_references->liststore, window_references->treeview, window_references->treecolumn, &bibletime);
  }
  // Did the browser request a url too difficult for it to handle?
  if (!httpd.difficult_url.empty()) {
    htmlbrowser(httpd.difficult_url, true, true);
    httpd.difficult_url.clear();
  }
}

void MainWindow::on_my_checks_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_my_checks();
}

void MainWindow::on_my_checks() {
  save_editors();
  show_references_window();
  MyChecksDialog dialog(window_references->liststore, window_references->treeview, window_references->treecolumn);
  dialog.run();
}

void MainWindow::on_check_markers_spacing_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_markers_spacing();
}

void MainWindow::on_check_markers_spacing() {
  save_editors();
  show_references_window();
  scripture_checks_usfm_spacing(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_check_references_inventory_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_references_inventory();
}

void MainWindow::on_check_references_inventory() {
  save_editors();
  scripture_checks_references_inventory(true);
}

void MainWindow::on_check_references_validate_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_references_validate();
}

void MainWindow::on_check_references_validate() {
  save_editors();
  show_references_window();
  scripture_checks_validate_references(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
}

void MainWindow::on_check_nt_quotations_from_the_ot_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_nt_quotations_from_the_ot();
}

void MainWindow::on_check_nt_quotations_from_the_ot() {
  save_editors();
  scripture_checks_nt_quotations_from_ot(true);
}

void MainWindow::on_synoptic_parallel_passages_from_the_nt_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_synoptic_parallel_passages_from_the_nt();
}

void MainWindow::on_synoptic_parallel_passages_from_the_nt() {
  save_editors();
  scripture_checks_synoptic_parallels_from_nt(true);
}

void MainWindow::on_parallels_from_the_ot_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_parallels_from_the_ot();
}

void MainWindow::on_parallels_from_the_ot() {
  save_editors();
  scripture_checks_parallels_from_ot(true);
}

void MainWindow::on_check_sentence_structure_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_sentence_structure();
}

void MainWindow::on_check_sentence_structure() {
  save_editors();
  show_references_window();
  scripture_checks_sentence_structure(window_references->liststore, window_references->treeview, window_references->treecolumn, NULL);
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

void MainWindow::on_goto_styles_area() {
  // Create or active the styles window.
  on_file_styles();
  // Focus the window to enable the user to start inserting the style using the keyboard.
  if (window_styles) {
    window_styles->present();
  }
}

void MainWindow::on_file_styles_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_styles();
}

void MainWindow::on_file_styles() {
  // Display the styles if needed.
  if (!window_styles) {
    window_styles = new WindowStyles (accelerator_group, windows_startup_pointer != G_MAXINT,
        style, style_menu,
        stylesheets_expand_all, stylesheets_collapse_all,
        style_insert, stylesheet_edit_mode, style_new,
        style_properties, style_delete,
        stylesheet_switch, stylesheets_new, stylesheets_delete,
        stylesheets_rename, stylesheets_import, stylesheets_export);
    g_signal_connect ((gpointer) window_styles->delete_signal_button, "clicked", G_CALLBACK (on_window_styles_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_styles->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_styles->apply_signal, "clicked", G_CALLBACK (on_style_button_apply_clicked), gpointer (this));
    g_signal_connect ((gpointer) window_styles->open_signal, "clicked", G_CALLBACK (on_style_button_open_clicked), gpointer (this));
    g_signal_connect ((gpointer) window_styles->edited_signal, "clicked", G_CALLBACK (on_style_edited), gpointer (this));
    extern Settings * settings;
    ustring stylesheet = settings->genconfig.stylesheet_get();
    stylesheet_open_named(stylesheet);
  }
}

void MainWindow::on_window_styles_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_styles_delete_button();
}

void MainWindow::on_window_styles_delete_button() {
  if (window_styles) {
    delete window_styles;
    window_styles = NULL;
  }
}

void MainWindow::stylesheet_open_named(const ustring& stylesheet) {
  if (window_styles) {
    window_styles->load(stylesheet);
  }
}

void MainWindow::on_style_button_open_clicked(GtkButton *button, gpointer user_data)
// This is activated by the GuiStyles object if another stylesheet should be opened.
{
  ((MainWindow *) user_data)->on_style_button_open();
}

void MainWindow::on_style_button_open() {
  if (window_styles) {
    // Save the name of the stylesheet in two locations.
    extern Settings * settings;
    settings->genconfig.stylesheet_set(window_styles->get_sheet());
    if (!settings->genconfig.project_get().empty()) {
      ProjectConfiguration * projectconfig = settings->projectconfig(settings->genconfig.project_get());
      projectconfig->stylesheet_set(window_styles->get_sheet());
    }
    // Actually open it.  
    stylesheet_open_named(window_styles->get_sheet());
  }
}

void MainWindow::on_style_button_apply_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_style_apply();
}

void MainWindow::on_style_apply() {
  // Get the focused Editor. If none, bail out.
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;

  // Bail out ifthere's no styles window.
  if (!window_styles)
    return;

  // Get the focused style(s).
  ustring selected_style = window_styles->get_focus();

  // Only proceed when a style has been selected.
  if (selected_style.empty())
    return;

  // Get the Style object.
  extern Settings * settings;
  Style style(settings->genconfig.stylesheet_get(), selected_style, false);

  // Whether and how the style is used.
  bool style_was_used = true;
  bool style_was_treated_specially = false;

  // Special treatment for the chapter style.
  if (style.type == stChapterNumber) {
    // Ask whether the user wishes to insert a new chapter.
    if (gtkw_dialog_question(mainwindow, "Would you like to insert a new chapter?", GTK_RESPONSE_YES) == GTK_RESPONSE_YES) {
      // Insert a new chapter.
      save_editors();
      ChapterNumberDialog dialog(true);
      if (dialog.run() == GTK_RESPONSE_OK) {
        reload_project();
      } else {
        style_was_used = false;
      }
      style_was_treated_specially = true;
    }
  }

  // Inserting footnote or endnote or crossreference.
  {
    Editor * editor = editor_window->editor;
    if (editor->last_focused_type() == etvtBody) {
      if (style.type == stFootEndNote) {
        if (style.subtype == fentFootnote) {
          InsertNoteDialog dialog(indtFootnote);
          if (dialog.run() == GTK_RESPONSE_OK) {
            editor->insert_note(style.marker, dialog.rawtext, 
            NULL, true);
          } else {
            style_was_used = false;
          }
          style_was_treated_specially = true;
        }
        if (style.subtype == fentEndnote) {
          InsertNoteDialog dialog(indtEndnote);
          if (dialog.run() == GTK_RESPONSE_OK) {
            editor->insert_note(style.marker, dialog.rawtext, 
            NULL, true);
          } else {
            style_was_used = false;
          }
          style_was_treated_specially = true;
        }
      }
      if (style.type == stCrossreference) {
        InsertNoteDialog dialog(indtCrossreference);
        if (dialog.run() == GTK_RESPONSE_OK) {
          editor->insert_note(style.marker, dialog.rawtext, NULL, true);
        } else {
          style_was_used = false;
        }
        style_was_treated_specially = true;
        // If the gui has been set so, display the references in the tools area.
        if (settings->genconfig.inserting_xref_shows_references_get()) {
          show_references_window();
          gtk_widget_grab_focus(editor->last_focused_textview());
        }
      }
    }
  }

  // Special treatment for a table style.
  {
    Editor * editor = editor_window->editor;
    if (editor->last_focused_type() == etvtBody) {
      if (style.type == stTableElement) {
        InsertTableDialog dialog(editor->project);
        if (dialog.run() == GTK_RESPONSE_OK) {
          editor->insert_table(dialog.rawtext, NULL);
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
    editor_window->editor->apply_style(selected_style);
  }

  // Take some actions if the style was used.
  if (style_was_used) {
    window_styles->use(selected_style);
  }
}

void MainWindow::on_editor_style_changed(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->editor_style_changed();
}

void MainWindow::editor_style_changed() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  Editor * editor = editor_window->editor;
  set <ustring> styles = editor->get_styles_at_cursor();
  vector <ustring> styles2(styles.begin(), styles.end());
  ustring text = "Style ";
  for (unsigned int i = 0; i < styles2.size(); i++) {
    if (i)
      text.append(", ");
    text.append(styles2[i]);
  }
  gtk_label_set_text(GTK_LABEL (statuslabel_style), text.c_str ());
}

void MainWindow::on_style_edited(GtkButton *button, gpointer user_data)
// This function is called when the properties of a style have been edited.
{
  ((MainWindow *) user_data)->reload_styles();
}

void MainWindow::reload_styles() {
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->editor->create_or_update_formatting_data();
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

void MainWindow::on_edit_bible_note_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_edit_bible_note();
}

void MainWindow::on_edit_bible_note() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (editor_window) {
    EditNoteDialog dialog(editor_window->editor);
    dialog.run();
  }
}

/*
 |
 |
 |
 |
 |
 Formatted view
 |
 |
 |
 |
 |
 */

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

void MainWindow::on_menutools_activate(GtkMenuItem *menuitem, gpointer user_data) {
}

void MainWindow::on_line_cutter_for_hebrew_text1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_line_cutter_for_hebrew_text();
}

void MainWindow::on_line_cutter_for_hebrew_text() {
  LineCutterDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_project();
  }
}

void MainWindow::on_notes_transfer_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_notes_transfer();
}

void MainWindow::on_notes_transfer() {
  save_editors();
  NotesTransferDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    notes_redisplay();
}

void MainWindow::on_tool_origin_references_in_bible_notes_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_tool_origin_references_in_bible_notes();
}

void MainWindow::on_tool_origin_references_in_bible_notes() {
  save_editors();
  OriginReferencesDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_project();
}

void MainWindow::on_tool_project_notes_mass_update1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_tool_project_notes_mass_update();
}

void MainWindow::on_tool_project_notes_mass_update() {
  NotesUpdateDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    notes_redisplay();
  }
}

void MainWindow::on_tool_generate_word_lists_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_tool_generate_word_lists();
}

void MainWindow::on_tool_generate_word_lists() {
  save_editors();
  WordlistDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_project();
}

void MainWindow::on_tool_transfer_project_notes_to_text_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_tool_transfer_project_notes_to_text();
}

void MainWindow::on_tool_transfer_project_notes_to_text()
// This transfers the currently visible project notes to the currently active project, 
// and does that for each verse.
{
  save_editors();
  XferNotes2TextDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_project();
  }
}

void MainWindow::on_preferences_features_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_features();
}

void MainWindow::on_preferences_features() {
  if (password_pass(mainwindow)) {
    FeaturesDialog dialog(0);
    dialog.run();
  }
}

void MainWindow::on_preferences_password_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_password();
}

void MainWindow::on_preferences_password() {
  password_edit(mainwindow);
}

void MainWindow::on_tool_simple_text_corrections_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_tool_simple_text_corrections();
}

void MainWindow::on_tool_simple_text_corrections() {
  save_editors();
  FixMarkersDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_project();
}

void MainWindow::on_preferences_text_replacement_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_text_replacement();
}

void MainWindow::on_preferences_text_replacement() {
  TextReplacementDialog dialog(0);
  dialog.run();
}

void MainWindow::on_parallel_passages1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_parallel_passages1();
}

void MainWindow::on_parallel_passages1() {
  // Act depending on whether to view the parallel passages.
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (parallel_passages1))) {
    // View them: show references tab, view passages.
    show_references_window();
    parallel_passages_display(navigation.reference, window_references->liststore, window_references->treeview, window_references->treecolumn);
  } else {
    // Don't view them: clear store.
    on_clear_references();
  }
}

void MainWindow::on_pdf_viewer1_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_pdf_viewer();
}

void MainWindow::on_pdf_viewer() {
  PDFViewerDialog dialog(0);
  dialog.run();
}

void MainWindow::on_view_usfm_code_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_usfm_code();
}

void MainWindow::on_view_usfm_code() {
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  Editor * editor = editor_window->editor;
  save_editors();
  ustring filename = project_data_filename_chapter(editor->project, editor->book, editor->chapter, false);
  ViewUSFMDialog dialog(filename);
  dialog.run();
  if (dialog.changed) {
    reload_project();
  }
}

void MainWindow::on_insert_special_character_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_insert_special_character();
}

void MainWindow::on_insert_special_character() {
  /* // Todo 
   Editor * editor = editorsgui->focused_editor();
   if (!editor)
   return;
   extern Settings * settings;
   vector <ustring> characters;
   vector <ustring> descriptions;
   characters.push_back("­");
   descriptions.push_back("Soft hyphen");
   characters.push_back(" ");
   descriptions.push_back("No-break space");
   characters.push_back("“");
   descriptions.push_back("Left double quotation mark");
   characters.push_back("”");
   descriptions.push_back("Right double quotation mark");
   characters.push_back("‘");
   descriptions.push_back("Left single quotation mark");
   characters.push_back("’");
   descriptions.push_back("Right single quotation mark");
   characters.push_back("«");
   descriptions.push_back("Left-pointing double angle quotation mark");
   characters.push_back("»");
   descriptions.push_back("Right-pointing double angle quotation mark");
   RadiobuttonDialog dialog("Insert character", "Insert special character", descriptions, settings->session.special_character_selection);
   if (dialog.run() != GTK_RESPONSE_OK)
   return;
   settings->session.special_character_selection = dialog.selection;
   editor->text_insert(characters[dialog.selection]);
   */
}

void MainWindow::on_preferences_graphical_interface_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_graphical_interface();
}

void MainWindow::on_preferences_graphical_interface() {
  InterfacePreferencesDialog dialog(0);
  dialog.run();
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

void MainWindow::on_check_key_terms_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_check_key_terms();
}

void MainWindow::on_check_key_terms() {
  on_window_check_keyterms_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (check_key_terms))) {
    window_check_keyterms = new WindowCheckKeyterms (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_check_keyterms->delete_signal_button, "clicked", G_CALLBACK (on_window_check_keyterms_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_check_keyterms->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_check_keyterms->signal, "clicked", G_CALLBACK (on_keyterms_new_reference), gpointer(this));
  }
}

void MainWindow::on_window_check_keyterms_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_check_keyterms_delete_button();
}

void MainWindow::on_window_check_keyterms_delete_button() {
  if (window_check_keyterms) {
    delete window_check_keyterms;
    window_check_keyterms = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (check_key_terms), false);
  }
}

void MainWindow::on_keyterms_new_reference(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->check_move_new_reference();
}

void MainWindow::check_move_new_reference() {
  Reference reference(window_check_keyterms->new_reference_showing->book, window_check_keyterms->new_reference_showing->chapter, window_check_keyterms->new_reference_showing->verse);
  navigation.display(reference);
}

void MainWindow::on_view_keyterms_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_keyterms();

}

void MainWindow::on_view_keyterms() {
  on_window_show_keyterms_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (view_keyterms))) {
    window_show_keyterms = new WindowShowKeyterms (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_show_keyterms->delete_signal_button, "clicked", G_CALLBACK (on_window_show_keyterms_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_show_keyterms->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    extern Settings * settings;
    window_show_keyterms->go_to(settings->genconfig.project_get(), navigation.reference);
  }
}

void MainWindow::on_window_show_keyterms_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_show_keyterms_delete_button();
}

void MainWindow::on_window_show_keyterms_delete_button() {
  if (window_show_keyterms) {
    delete window_show_keyterms;
    window_show_keyterms = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_keyterms), false);
  }
}

/*
 |
 |
 |
 |
 |
 Backup
 |
 |
 |
 |
 |
 */

void MainWindow::on_project_backup_incremental_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_project_backup_incremental();
}

void MainWindow::on_project_backup_incremental() {
  save_editors();
  git_command_pause(true);
  backup_make_incremental();
  git_command_pause(false);
}

void MainWindow::on_project_backup_flexible_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_project_backup_flexible();
}

void MainWindow::on_project_backup_flexible() {
  save_editors();
  git_command_pause(true);
  BackupDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK) {
    backup_make_flexible();
  }
  git_command_pause(false);
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

void MainWindow::on_view_git_tasks_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_git_tasks();
}

void MainWindow::on_view_git_tasks() {
  ViewGitDialog dialog(0);
  dialog.run();
}

void MainWindow::on_preferences_remote_git_repository_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_remote_git_repository();
}

void MainWindow::on_preferences_remote_git_repository() {
  save_editors();
  GitSetupDialog dialog(0);
  if (dialog.run() == GTK_RESPONSE_OK)
    reload_project();
}

void MainWindow::on_git_reopen_project() {
  if (git_reopen_project) {
    git_reopen_project = false; // Close flag before dialog shows, else the dialogs keep coming.
    reload_project();
  }
}

void MainWindow::on_project_changes_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_project_changes();
}

void MainWindow::on_project_changes() {
  // Save even the very latest changes.
  save_editors();
  // The changes checker will generate git tasks. Pause git.
  git_command_pause(true);
  // Do the actual changes dialog. 
  // It will delete the unwanted git tasks.
  show_references_window();
  References references(window_references->liststore, window_references->treeview, window_references->treecolumn);
  ChangesDialog dialog(&references);
  dialog.run();
  // Resume git operations.
  git_command_pause(false);
}

void MainWindow::on_edit_revert_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_edit_revert();
}

void MainWindow::on_edit_revert() {
  save_editors();
  RevertDialog dialog(&navigation.reference);
  if (dialog.run() == GTK_RESPONSE_OK) {
    reload_project();
  }
}

void MainWindow::git_update_intervals_initialize() {
  // Make containers with all projects to be updated, and their intervals.
  extern Settings * settings;
  vector <ustring> projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration * projectconfig = settings->projectconfig(projects[i]);
    if (projectconfig->git_use_remote_repository_get()) {
      git_update_intervals[projects[i]] = 0;
    }
  }
  // Start the timer.
  git_update_interval_event_id = g_timeout_add_full(G_PRIORITY_DEFAULT, 1000, GSourceFunc (on_git_update_timeout), gpointer(this), NULL);
}

bool MainWindow::on_git_update_timeout(gpointer user_data) {
  ((MainWindow *) user_data)->git_update_timeout();
  return true;
}

void MainWindow::git_update_timeout()
// Schedule project update tasks.
{
  // Bail out if git tasks are paused.
  extern Settings * settings;
  if (settings->session.git_pause)
    return;

  // Schedule a task for each relevant project.
  vector <ustring> projects = projects_get_all();
  for (unsigned int i = 0; i < projects.size(); i++) {
    ProjectConfiguration * projectconfig = settings->projectconfig(projects[i]);
    if (projectconfig->git_use_remote_repository_get()) {
      int interval = git_update_intervals[projects[i]];
      interval++;
      if (interval >= projectconfig->git_remote_update_interval_get()) {
        git_schedule(gttUpdateProject, projects[i], 0, 0, projectconfig->git_remote_repository_url_get());
        interval = 0;
      }
      git_update_intervals[projects[i]] = interval;
    }
  }
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

void MainWindow::on_view_text_font_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_text_font();
}

void MainWindow::on_text_font() {
  /* // Todo 
   // Get the font and colour settings, either from the project, if it is opened, 
   // or else from genconfig.
   extern Settings * settings;
   bool defaultfont = settings->genconfig.text_editor_font_default_get();
   ustring fontname = settings->genconfig.text_editor_font_name_get();
   unsigned int linespacing = 100;
   bool defaultcolour = settings->genconfig.text_editor_default_color_get();
   unsigned int normaltextcolour = settings->genconfig.text_editor_normal_text_color_get();
   unsigned int backgroundcolour = settings->genconfig.text_editor_background_color_get();
   unsigned int selectedtextcolour = settings->genconfig.text_editor_selected_text_color_get();
   unsigned int selectioncolour = settings->genconfig.text_editor_selection_color_get();
   Editor * editor = editorsgui->focused_editor();
   if (editor) {
   ProjectConfiguration * projectconfig = settings->projectconfig(editor->project);
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
   if (editor) {
   ProjectConfiguration * projectconfig = settings->projectconfig(editor->project);
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
   */
}

void MainWindow::on_view_notes_font_activate(GtkMenuItem * menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_notes_font();
}

void MainWindow::on_notes_font() {
  extern Settings * settings;
  FontColorDialog
      dialog(settings->genconfig.notes_editor_font_default_get(), settings->genconfig.notes_editor_font_name_get(), 100, settings->genconfig.notes_editor_default_color_get(), settings->genconfig.notes_editor_normal_text_color_get(), settings->genconfig.notes_editor_background_color_get(), settings->genconfig.notes_editor_selected_text_color_get(), settings->genconfig.notes_editor_selection_color_get());
  if (dialog.run() == GTK_RESPONSE_OK) {
    settings->genconfig.notes_editor_font_default_set(dialog.new_use_default_font);
    settings->genconfig.notes_editor_font_name_set(dialog.new_font);
    settings->genconfig.notes_editor_default_color_set(dialog.new_use_default_color);
    settings->genconfig.notes_editor_normal_text_color_set(dialog.new_normal_text_color);
    settings->genconfig.notes_editor_background_color_set(dialog.new_background_color);
    settings->genconfig.notes_editor_selected_text_color_set(dialog.new_selected_text_color);
    settings->genconfig.notes_editor_selection_color_set(dialog.new_selection_color);
    set_fonts();
  }
}

void MainWindow::set_fonts() {
  // Set font in the text editors. Set text direction too.
  // Todo editorsgui->set_fonts();

  /* Because of switching to GtkHtml for displaying and editing project notes, the fonts no longer can be set through the menu.
   // Set font for the translation notes editor.
   PangoFontDescription *font_desc= NULL;
   extern Settings * settings;
   if (!settings->genconfig.notes_editor_font_default_get()) {
   font_desc = pango_font_description_from_string(settings->genconfig.notes_editor_font_name_get ().c_str());
   }
   gtk_widget_modify_font(textview_notes, font_desc);
   gtk_widget_modify_font(textview_note, font_desc);
   if (font_desc)
   pango_font_description_free(font_desc);

   // Set the colors for the translation notes editor.
   if (settings->genconfig.notes_editor_default_color_get()) {
   color_widget_default(textview_notes);
   color_widget_default(textview_note);
   } else {
   color_widget_set(textview_notes, settings->genconfig.notes_editor_normal_text_color_get(), settings->genconfig.notes_editor_background_color_get(), settings->genconfig.notes_editor_selected_text_color_get(), settings->genconfig.notes_editor_selection_color_get());
   color_widget_set(textview_note, settings->genconfig.notes_editor_normal_text_color_get(), settings->genconfig.notes_editor_background_color_get(), settings->genconfig.notes_editor_selected_text_color_get(), settings->genconfig.notes_editor_selection_color_get());
   }
   */
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

void MainWindow::on_view_outline_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_outline();
}

void MainWindow::on_view_outline() {
  on_window_outline_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (view_outline))) {
    window_outline = new WindowOutline (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_outline->delete_signal_button, "clicked", G_CALLBACK (on_window_outline_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_outline->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_outline->outline->reference_changed_signal, "clicked", G_CALLBACK (on_button_outline_clicked), gpointer(this));
    extern Settings * settings;
    window_outline->go_to(settings->genconfig.project_get(), navigation.reference);
  }
}

void MainWindow::on_window_outline_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_outline_delete_button();
}

void MainWindow::on_window_outline_delete_button() {
  if (window_outline) {
    delete window_outline;
    window_outline = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_outline), false);
  }
}

void MainWindow::on_button_outline_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_button_outline();
}

void MainWindow::on_button_outline() {
  if (window_outline) {
    Reference reference(navigation.reference);
    reference.chapter = window_outline->outline->newchapter;
    reference.verse = convert_to_string(window_outline->outline->newverse);
    navigation.display(reference);
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

void MainWindow::on_ipc_method_called(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_ipc_method();
}

void MainWindow::on_ipc_method() {
  // Interprocess Communication pointer.
  extern InterprocessCommunication * ipc;

  // Settings pointer.
  extern Settings * settings;

  // Handle call for a new git job.
  if (ipc->method_called_type == ipcctGitJobDescription) {
    if (!settings->session.git_pause) {
      vector <ustring> task = git_get_next_task();
      if (!task.empty()) {
        ipc->send(ipcstBibleditGit, ipcctGitJobDescription, task);
        // The chapter state looks whether something changed in the chapter 
        // now opened while we pull changes from the remote repository.
        gitchapterstate = NULL;
        if (convert_to_int(task[0]) == gttUpdateProject) {
          if (task[1] == settings->genconfig.project_get()) {
            gitchapterstate = new GitChapterState (task[1], navigation.reference.book, navigation.reference.chapter);
          }
        }
      }
    }
  }

  // Handle a job done.
  else if (ipc->method_called_type == ipcctGitTaskDone) {
    // Get the errors given by this task.
    vector <ustring> error = ipc->get_payload(ipcctGitTaskDone);
    if (error.empty()) {
      // No errors: erase the task.
      git_erase_task_done();
    } else {
      // Errors: Resolve any conflicts in the project the latest task refers to.  
      vector <ustring> task = git_get_next_task();
      if (!task.empty()) {
        git_resolve_conflicts(task[1], error);
      }
      // Task failed: re-queue it.
      git_fail_task_done();
    }
    // Set a flag if the state of the currently opened chapter changed.
    // (This must be done after conflicting merges have been resolved, as that
    //  could affect the chapter now opened).
    if (gitchapterstate) {
      if (gitchapterstate->changed()) {
        git_reopen_project = true;
      }
      delete gitchapterstate;
      gitchapterstate = NULL;
    }
  }
}

/*
 |
 |
 |
 |
 |
 Reporting and Planning
 |
 |
 |
 |
 |
 */

void MainWindow::on_preferences_reporting_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_reporting();
}

void MainWindow::on_preferences_reporting() {
  ReportingSetupDialog dialog(0);
  dialog.run();
}

void MainWindow::on_editstatus_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_editstatus();
}

void MainWindow::on_editstatus()
// Edits the project's status.
{
  extern Settings * settings;
  EditStatusDialog dialog(settings->genconfig.project_get(), navigation.reference.book, navigation.reference.chapter);
  dialog.run();
}

void MainWindow::on_view_status_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_status();
}

void MainWindow::on_view_status() {
  ViewStatusDialog dialog(0);
  dialog.run();
}

void MainWindow::on_edit_planning_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_edit_planning();
}

void MainWindow::on_edit_planning() {
  extern Settings * settings;
  planning_edit(settings->genconfig.project_get());
}

void MainWindow::on_view_planning_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_planning();
}

void MainWindow::on_view_planning() {
  extern Settings * settings;
  planning_produce_report(settings->genconfig.project_get());
}

void MainWindow::on_preferences_planning_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_planning();
}

void MainWindow::on_preferences_planning() {
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

void MainWindow::on_window_resource_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_resource_delete_button(button);
}

void MainWindow::on_window_resource_delete_button(GtkButton *button) {
  GtkWidget * widget= GTK_WIDGET (button);
  vector <WindowResource *>::iterator iterator = resource_windows.begin();
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (widget == resource_windows[i]->delete_signal_button) {
      delete resource_windows[i];
      resource_windows.erase(iterator);
      break;
    }
    iterator++;
  }
}

void MainWindow::on_file_resources_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources();
}

WindowResource * MainWindow::last_focused_resource_window()
// Get the focused resource window, or NULL if there's none.
{
  WindowResource * resource_window= NULL;
  time_t most_recent_time = 0;
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (resource_windows[i]->focused_time > most_recent_time) {
      most_recent_time = resource_windows[i]->focused_time;
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

void MainWindow::on_file_resources_open_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources_open("");
}

void MainWindow::on_file_resources_open(ustring resource)
// Opens a resource.
{
  // Find data about the resource, and whether it exists.
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  quick_sort(resources, filenames, 0, resources.size());
  if (resource.empty()) {
    ListviewDialog dialog("Open resource", resources, "", false, NULL);
    if (dialog.run() == GTK_RESPONSE_OK) {
      resource = dialog.focus;
    }
  }
  if (resource.empty())
    return;
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
  WindowResource * resource_window = new WindowResource (resource, accelerator_group, false);
  g_signal_connect ((gpointer) resource_window->delete_signal_button, "clicked", G_CALLBACK (on_window_resource_delete_button_clicked), gpointer(this));
  g_signal_connect ((gpointer) resource_window->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
  resource_window->go_to(navigation.reference);
  resource_windows.push_back(resource_window);
}

void MainWindow::on_file_resources_close_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources_close();
}

void MainWindow::on_file_resources_close()
// Closes the focused resource.
{
  WindowResource * focused_window = last_focused_resource_window();
  if (focused_window) {
    on_window_resource_delete_button(GTK_BUTTON (focused_window->delete_signal_button));
  }
}

void MainWindow::on_file_resources_new_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources_new();
}

void MainWindow::on_file_resources_new() {
  NewResourceDialog dialog("");
  dialog.run();
}

void MainWindow::on_file_resources_edit_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources_edit();
}

void MainWindow::on_file_resources_edit() {
  WindowResource * focused_resource_window = last_focused_resource_window();
  if (focused_resource_window) {
    ustring templatefile = focused_resource_window->resource->template_get();
    NewResourceDialog dialog(templatefile);
    if (dialog.run() == GTK_RESPONSE_OK) {
      focused_resource_window->resource->open(dialog.edited_template_file);
    }
  }
}

void MainWindow::on_file_resources_delete_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_resources_delete();
}

void MainWindow::on_file_resources_delete()
// Delete a resource.
{
  vector <ustring> filenames;
  vector <ustring> resources = resource_get_resources(filenames, false);
  ListviewDialog dialog("Delete resource", resources, "", false, NULL);
  if (dialog.run() == GTK_RESPONSE_OK) {
    int result = gtkw_dialog_question(NULL, "Are you sure you want to delete resource " + dialog.focus + "?");
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
 Text Editors Todo working here
 |
 |
 |
 |
 |
 */

void MainWindow::on_window_editor_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_editor_delete_button(button);
}

void MainWindow::on_window_editor_delete_button(GtkButton *button) {
  GtkWidget * widget= GTK_WIDGET (button);
  vector <WindowEditor *>::iterator iterator = editor_windows.begin();
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (widget == editor_windows[i]->delete_signal_button) {
      delete editor_windows[i];
      editor_windows.erase(iterator);
      break;
    }
    iterator++;
  }
  handle_editor_focus();
}

WindowEditor * MainWindow::last_focused_editor_window()
// Get the focused editor window, or NULL if there's none.
{
  WindowEditor * editor_window= NULL;
  time_t most_recent_time = 0;
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (editor_windows[i]->focused_time > most_recent_time) {
      most_recent_time = editor_windows[i]->focused_time;
      editor_window = editor_windows[i];
    }
  }
  return editor_window;
}

void MainWindow::on_file_project_open(const ustring& project)
// Opens an editor.
{
  // If the editor already displays, present it and bail out.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (project == editor_windows[i]->window_data) {
      editor_windows[i]->present();
      return;
    }
  }

  // Display a new editor.
  WindowEditor * editor_window = new WindowEditor (project, accelerator_group, false);
  g_signal_connect ((gpointer) editor_window->delete_signal_button, "clicked", G_CALLBACK (on_window_editor_delete_button_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor_window->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->new_verse_signal, "clicked", G_CALLBACK (on_new_verse_signalled), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->new_styles_signal, "clicked", G_CALLBACK (on_editor_style_changed), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->quick_references_button, "clicked", G_CALLBACK (on_show_quick_references_signal_button_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->word_double_clicked_signal, "clicked", G_CALLBACK (on_send_word_to_toolbox_signalled), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->reload_signal, "clicked", G_CALLBACK (on_editor_reload_clicked), gpointer(this));
  g_signal_connect ((gpointer) editor_window->editor->changed_signal, "clicked", G_CALLBACK (on_editorsgui_changed_clicked), gpointer(this));
  editor_windows.push_back(editor_window);

  // After creation the window will generate a focus signal, 
  // and this signal in turn will cause further processing of the editor.
}

void MainWindow::on_editor_reload_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_editor_reload();
}

void MainWindow::on_editor_reload() {
  // Get the focused editor, if none, bail out.
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  Editor * editor = editor_window->editor;
  // Create the reference where to go to after the project has been reopened.
  // The reference should be obtained before closing the project,
  // so that the chapter number to go to is accessible.
  Reference reference(navigation.reference);
  reference.chapter = editor->reload_chapter_number;
  if (editor->reload_chapter_number == 0)
    reference.verse = "0";
  // Reopen.
  reload_project();
  // Go to the right reference.
  navigation.display(reference);
}

void MainWindow::handle_editor_focus() {
  // Get the focused editor and the project.
  WindowEditor * editor_window = last_focused_editor_window();
  ustring project;
  if (editor_window)
    project = editor_window->editor->project;

  // Set the focused project in the configuration.
  extern Settings * settings;
  settings->genconfig.project_set(project);

  // Enable or disable widgets depending on whether an editor window is focused.
  enable_or_disable_widgets(editor_window);

  // Inform the merge window, if it is there, about the editors.
  if (window_merge) {
    window_merge->set_focused_editor(editor_window->editor);
    vector <Editor *> visible_editors;
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      visible_editors.push_back(editor_windows[i]->editor);
    }
    window_merge->set_visible_editors(visible_editors);
  }

  // If we've no project bail out.
  if (project.empty())
    return;

  // Settings.
  extern Settings * settings;

  // Re-initialize Navigation.
  navigation.set_project(project, false);
  Reference reference(settings->genconfig.book_get(), convert_to_int(settings->genconfig.chapter_get()), settings->genconfig.verse_get());
  navigation.display(reference);
  // Some rumbling internal logic causes the verse to jump to 0 in several cases.
  // To resolve this in a rough manner, after a delay, that is, after the 
  // rumbling is over, cause the navigator to go to the desired verse.
  navigation.display_delayed(reference.verse);

  // Set the available books for search/replace functions.
  vector <unsigned int> books = project_get_books(project);
  set<unsigned int> selection(books.begin(), books.end());
  settings->session.selected_books = selection;

  // Redisplay the project notes.
  notes_redisplay();

  // Open the right stylesheet.
  ProjectConfiguration * projectconfig = settings->projectconfig(project);
  settings->genconfig.stylesheet_set(projectconfig->stylesheet_get());
  stylesheet_open_named(settings->genconfig.stylesheet_get());
}

void MainWindow::save_editors()
// Save all and any editors.
{
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->editor->chapter_save();
  }
}

void MainWindow::goto_next_previous_project(bool next) {
  // Bail out if there are not enough windows to switch.
  if (editor_windows.size() < 2)
    return;

  // Get the focused project window and its offset.
  WindowEditor * present_window = last_focused_editor_window();
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

  // Present the new window.
  editor_windows[offset]->present();
}

void MainWindow::on_editorsgui_changed_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_editorsgui_changed();
}

void MainWindow::on_editorsgui_changed() {
  if (window_merge) {
    window_merge->editors_changed();
  }
}

void MainWindow::reload_project()
// This function reloads the projects.
{
  // Project.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  if (project.empty())
    return;

  // Store the reference where to go to after the project has been reloaded.
  Reference reference(navigation.reference);

  // As anything could have happened to the data in the project, force a reload of the navigator.
  navigation.set_project(project, true);

  // Navigate to the old place.
  navigation.display(reference);

  // Reload all editors.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->editor->chapter_load(reference.chapter);
  }
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

void MainWindow::on_file_projects_merge_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_file_projects_merge();
}

void MainWindow::on_file_projects_merge() {
  on_window_merge_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (file_projects_merge))) {
    window_merge = new WindowMerge (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_merge->delete_signal_button, "clicked", G_CALLBACK (on_window_merge_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_merge->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_merge->editors_get_text_button, "clicked", G_CALLBACK (on_merge_window_get_text_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_merge->new_reference_button, "clicked", G_CALLBACK (on_merge_window_new_reference_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_merge->save_editors_button, "clicked", G_CALLBACK (on_merge_window_save_editors_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_merge->reload_editors_button, "clicked", G_CALLBACK (on_editor_reload_clicked), gpointer(this));
  }
}

void MainWindow::on_window_merge_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_merge_delete_button();
}

void MainWindow::on_window_merge_delete_button() {
  if (window_merge) {
    delete window_merge;
    window_merge = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (file_projects_merge), false);
  }
}

void MainWindow::on_merge_window_get_text_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_merge_window_get_text_button();
}

void MainWindow::on_merge_window_get_text_button() {
  if (window_merge) {
    for (unsigned int i = 0; i < editor_windows.size(); i++) {
      if (editor_windows[i]->window_data == window_merge->current_master_project) {
        window_merge->main_project_data = editor_windows[i]->editor->get_chapter();
      }
      if (editor_windows[i]->window_data == window_merge->current_edited_project) {
        window_merge->edited_project_data = editor_windows[i]->editor->get_chapter();
      }
    }
    window_merge->book = navigation.reference.book;
    window_merge->chapter = navigation.reference.chapter;
  }
}

void MainWindow::on_merge_window_new_reference_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_merge_window_new_reference_button();
}

void MainWindow::on_merge_window_new_reference_button() {
  if (window_merge) {
    Reference reference(window_merge->book, window_merge->chapter, "0");
    navigation.display(reference);
  }
}

void MainWindow::on_merge_window_save_editors_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_merge_window_save_editors_button();
}

void MainWindow::on_merge_window_save_editors_button() {
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

void MainWindow::on_preferences_filters_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_preferences_filters();
}

void MainWindow::on_preferences_filters() {
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

void MainWindow::on_print_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_print();
}

void MainWindow::on_print() {
  // Create the selection dialog using the saved selection.
  unsigned int selection;
  {
    vector <ustring> labels;
    labels.push_back("Project");
    labels.push_back("Parallel Bible");
    labels.push_back("References");
    //labels.push_back("Test usfm2pdf");
    extern Settings * settings;
    RadiobuttonDialog dialog("Print", "Select what to print", labels, settings->genconfig.print_job_get());
    if (dialog.run() != GTK_RESPONSE_OK)
      return;
    selection = dialog.selection;
    settings->genconfig.print_job_set(selection);
  }
  // Save the editors.
  save_editors();

  switch (selection)
  {
    case 0: // Project.
    {
      {
        PrintProjectDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      extern Settings * settings;
      ProjectMemory projectmemory(settings->genconfig.project_get(), true);
      PrintProject printproject(&projectmemory);
      printproject.print();
      break;
    }
    case 1: // Parallel Bible.
    {
      {
        ParallelBibleDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      view_parallel_bible_pdf();
      break;
    }
    case 2: // References.
    {
      // Activate references.
      show_references_window();
      // Show dialog.
      {
        PrintReferencesDialog dialog(0);
        if (dialog.run() != GTK_RESPONSE_OK)
          return;
      }
      // Load refs from the editor.
      show_references_window();
      References references(window_references->liststore, window_references->treeview, window_references->treecolumn);
      references.get_loaded();
      vector<Reference> refs;
      references.get_references(refs);
      if (refs.empty()) {
        gtkw_dialog_info(mainwindow, "There are no references to print");
      } else {
        // Run the function for printing the references.
        extern Settings * settings;
        vector <ustring> extra_projects = settings->genconfig.print_references_projects_get();
        ProjectMemory projectmemory(settings->genconfig.project_get(), true);
        view_parallel_references_pdf(projectmemory, &extra_projects, refs, true, NULL, true);
      }
      break;
    }
    case 3: // Tes
    {
      extern Settings * settings;
      Text2Pdf text2pdf(gw_build_filename(directories_get_temp(), "pdf.pdf"), settings->genconfig.print_engine_use_intermediate_text_get());

      text2pdf.page_size_set(21, 10);
      text2pdf.page_margins_set(2, 1, 2, 2);
      //text2pdf.print_date_in_header();
      //text2pdf.set_right_to_left(0);
      //text2pdf.suppress_header_this_page();
      text2pdf.set_running_header_left_page("Izihlabelelo");
      text2pdf.set_running_header_right_page("Izihlabelelo");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(16);
      text2pdf.paragraph_set_italic(1);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_column_count(1);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("UGWALO ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(20);
      text2pdf.paragraph_set_bold(1);
      text2pdf.paragraph_set_space_before(8);
      text2pdf.paragraph_set_space_after(4);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_column_count(1);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("LWEZIHLABELELO");
      text2pdf.set_running_chapter_number(1, 1);
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(15);
      text2pdf.paragraph_set_bold(1);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(1);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Isihlabelelo");
      text2pdf.add_text(" ");
      text2pdf.add_text("1");
      text2pdf.close_paragraph();
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("1");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" Uyabusiswa umuntu ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Ongahambi ngalo ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Icebo labo ababi ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Lenhlanganwen' yabo. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Ongem' endleleni yabo ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Abayizw' izoni, ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Engahlal' esihlalweni ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Esabagconayo. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("2");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" Kodw' ukujabula kwakhe ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Kukuwo umthetho ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("WeNkosi ewukhumbula ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Emini leb'suku. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("3");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" Uzaba njengesihlahla ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("'Simil' emfuleni ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("'Sithel' izithelo zaso ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Ngesikhathi saso. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Amahlamvu aso wona ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Awayikubuna, ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Konke akwenzayo yena ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Kuzaphumelela. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("4");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" AbangelaNkulunkulu ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Kabanjalo bona, ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Kodwa banjengamakhoba ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Amuka lomoya. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("5");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" Kant' ababi kabasoze ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Bem' ekwahluleni, ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Lezoni emhlanganweni ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Wabalungileyo. ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.inline_set_font_size_percentage(100);
      text2pdf.inline_set_italic(0);
      text2pdf.inline_set_bold(1);
      text2pdf.inline_set_underline(0);
      text2pdf.inline_set_small_caps(0);
      text2pdf.add_text("6");
      text2pdf.inline_clear_font_size_percentage();
      text2pdf.inline_clear_italic();
      text2pdf.inline_clear_bold();
      text2pdf.inline_clear_underline();
      text2pdf.inline_clear_small_caps();
      text2pdf.inline_clear_superscript();
      text2pdf.inline_clear_colour();
      text2pdf.add_text(" Ngob' iNkosi iyayazi ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Indlela yabo nje ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(0);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.paragraph_set_keep_with_next();
      text2pdf.add_text("Abalungileyo kodwa ");
      text2pdf.close_paragraph();
      text2pdf.open_paragraph();
      text2pdf.paragraph_set_font_size(12);
      text2pdf.paragraph_set_space_before(0);
      text2pdf.paragraph_set_space_after(2);
      text2pdf.paragraph_set_left_margin(0);
      text2pdf.paragraph_set_right_margin(0);
      text2pdf.paragraph_set_first_line_indent(0);
      text2pdf.add_text("Eyababi yofa.");
      text2pdf.close_paragraph();

      text2pdf.run();
      text2pdf.view();
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
 Windowing system
 |
 |
 |
 |
 |
 */

void MainWindow::on_view_screen_layout_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_screen_layout();
}

void MainWindow::on_view_screen_layout() {
  on_window_screen_layout_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (view_screen_layout))) {
    window_screen_layout = new WindowLayout (0);
    g_signal_connect ((gpointer) window_screen_layout->signal_button, "clicked", G_CALLBACK (on_window_screen_layout_button_clicked), gpointer(this));
  }
}

void MainWindow::on_window_screen_layout_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_screen_layout_button();
}

void MainWindow::on_window_screen_layout_button() {
  if (window_screen_layout) {
    delete window_screen_layout;
    window_screen_layout = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_screen_layout), false);
  }
}

bool MainWindow::on_windows_startup_timeout(gpointer data) {
  return ((MainWindow *) data)->on_windows_startup();
}

bool MainWindow::on_windows_startup() {
  // Get all window data.
  WindowData window_data(false);

  bool window_started = false;
  while ((windows_startup_pointer < window_data.ids.size()) && !window_started) {
    if (window_data.shows[windows_startup_pointer]) {
      WindowID id = WindowID(window_data.ids[windows_startup_pointer]);
      ustring data = window_data.datas[windows_startup_pointer];
      switch (id)
      {
        case widShowKeyterms:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_keyterms), true);
          break;
        }
        case widShowQuickReferences:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_quick_references), true);
          break;
        }
        case widMerge:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (file_projects_merge), true);
          break;
        }
        case widResource:
        {
          on_file_resources_open(data);
          break;
        }
        case widOutline:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_outline), true);
          break;
        }
        case widCheckKeyterms:
        {
          gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (check_key_terms), true);
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
          on_file_project_open(data);
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
      if (focused_project_last_session == editor_windows[i]->window_data) {
        editor_windows[i]->present();
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
  // Keyterms in verse. 
  if (window_show_keyterms) {
    window_show_keyterms->shutdown();
    delete window_show_keyterms;
    window_show_keyterms = NULL;
  }

  // Quick references.
  if (window_show_quick_references) {
    window_show_quick_references->shutdown();
    delete window_show_quick_references;
    window_show_quick_references = NULL;
  }

  // Merge
  if (window_merge) {
    window_merge->shutdown();
    delete window_merge;
    window_merge = NULL;
  }

  // Resources.
  while (!resource_windows.empty()) {
    WindowResource * resource_window = resource_windows[0];
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

  // Editor.
  while (!editor_windows.empty()) {
    WindowEditor * editor_window = editor_windows[0];
    editor_window->shutdown();
    delete editor_window;
    editor_windows.erase(editor_windows.begin());
  }

}

void MainWindow::on_window_focus_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_focus_button(button);
}

void MainWindow::on_window_focus_button(GtkButton *button)
// Called when a subwindow gets focused.
{
  // Store the focus if it is different from the currently stored values.
  GtkWidget * widget= GTK_WIDGET (button);
  if (widget != now_focused_signal_button) {
    last_focused_signal_button = now_focused_signal_button;
    now_focused_signal_button = widget;
  }

  // Present the windows.
  present_windows(widget);

  // Handle cases that an editor receives focus.
  handle_editor_focus();
}

void MainWindow::present_windows(GtkWidget * widget)
// If one window is focused, present them all.
{
  // Present all windows.
  if (window_show_quick_references)
    window_show_quick_references->present();
  if (window_show_keyterms)
    window_show_keyterms->present();
  if (window_merge)
    window_merge->present();
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    resource_windows[i]->present();
  }
  if (window_outline)
    window_outline->present();
  if (window_check_keyterms)
    window_check_keyterms->present();
  if (window_styles)
    window_styles->present();
  if (window_notes)
    window_notes->present();
  if (window_references)
    window_references->present();
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    editor_windows[i]->present();
  }

  // Present the calling window again so that it keeps the focus.
  if (window_show_quick_references) {
    if (widget == window_show_quick_references->focus_in_signal_button)
      window_show_quick_references->present();
  }
  if (window_show_keyterms) {
    if (widget == window_show_keyterms->focus_in_signal_button)
      window_show_keyterms->present();
  }
  if (window_merge) {
    if (widget == window_merge->focus_in_signal_button)
      window_merge->present();
  }
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    if (widget == resource_windows[i]->focus_in_signal_button)
      resource_windows[i]->present();
  }
  if (window_outline) {
    if (widget == window_outline->focus_in_signal_button)
      window_outline->present();
  }
  if (window_check_keyterms) {
    if (widget == window_check_keyterms->focus_in_signal_button)
      window_check_keyterms->present();
  }
  if (window_styles) {
    if (widget == window_styles->focus_in_signal_button)
      window_styles->present();
  }
  if (window_notes) {
    if (widget == window_notes->focus_in_signal_button)
      window_notes->present();
  }
  if (window_references) {
    if (widget == window_references->focus_in_signal_button)
      window_references->present();
  }
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (widget == editor_windows[i]->focus_in_signal_button)
      editor_windows[i]->present();
  }
}

gboolean MainWindow::on_mainwindow_focus_in_event(GtkWidget *widget, GdkEventFocus *event, gpointer user_data) {
  ((MainWindow *) user_data)->mainwindow_focus_in_event(event);
  return FALSE;
}

void MainWindow::mainwindow_focus_in_event(GdkEventFocus *event) {
  present_windows(NULL);
}

gboolean MainWindow::on_mainwindow_window_state_event(GtkWidget *widget, GdkEvent *event, gpointer user_data) {
  ((MainWindow *) user_data)->mainwindow_window_state_event(event);
  return FALSE;
}

void MainWindow::mainwindow_window_state_event(GdkEvent *event) {
}

/*
 |
 |
 |
 |
 |
 Quick references
 |
 |
 |
 |
 |
 */

void MainWindow::on_view_quick_references_activate(GtkMenuItem *menuitem, gpointer user_data) {
  ((MainWindow *) user_data)->on_view_quick_references();
}

void MainWindow::on_view_quick_references() {
  on_window_show_quick_references_delete_button();
  if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM (view_quick_references))) {
    window_show_quick_references = new WindowShowQuickReferences (accelerator_group, windows_startup_pointer != G_MAXINT);
    g_signal_connect ((gpointer) window_show_quick_references->delete_signal_button, "clicked", G_CALLBACK (on_window_show_quick_references_delete_button_clicked), gpointer(this));
    g_signal_connect ((gpointer) window_show_quick_references->focus_in_signal_button, "clicked", G_CALLBACK (on_window_focus_button_clicked), gpointer(this));
    treeview_references_display_quick_reference();
  }
}

void MainWindow::on_window_show_quick_references_delete_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_window_show_quick_references_delete_button();
}

void MainWindow::on_window_show_quick_references_delete_button() {
  if (window_show_quick_references) {
    delete window_show_quick_references;
    window_show_quick_references = NULL;
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM (view_quick_references), false);
  }
}

void MainWindow::on_show_quick_references_signal_button_clicked(GtkButton *button, gpointer user_data) {
  ((MainWindow *) user_data)->on_show_quick_references_signal_button(button);
}

void MainWindow::on_show_quick_references_signal_button(GtkButton *button) {
  if (!window_show_quick_references)
    return;
  WindowEditor * editor_window = last_focused_editor_window();
  if (!editor_window)
    return;
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  window_show_quick_references->go_to(project, editor_window->editor->quick_references);
}

void MainWindow::treeview_references_display_quick_reference()
// Display the quick references.
{
  // Bail out if there's no quick references window or references window..
  if (window_show_quick_references == NULL)
    return;
  if (window_references == NULL)
    return;

  // Display the verses.
  extern Settings * settings;
  ustring project = settings->genconfig.project_get();
  window_show_quick_references->go_to(project, window_references->references);
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

void MainWindow::accelerator_undo_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_undo();
}

void MainWindow::accelerator_undo() {
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (now_focused_signal_button == editor_windows[i]->focus_in_signal_button) {
      editor_windows[i]->editor->undo();
    }
  }
  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->undo();
    }
  }
}

void MainWindow::accelerator_redo_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_redo();
}

void MainWindow::accelerator_redo() {
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (now_focused_signal_button == editor_windows[i]->focus_in_signal_button) {
      editor_windows[i]->editor->redo();
    }
  }
  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->redo();
    }
  }
}

void MainWindow::accelerator_cut_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_cut();
}

void MainWindow::accelerator_cut() {
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (now_focused_signal_button == editor_windows[i]->focus_in_signal_button) {
      Editor * editor = editor_windows[i]->editor;
      gtk_clipboard_set_text(clipboard, editor->text_get_selection ().c_str(), -1);
      editor->text_erase_selection();
    }
  }

  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->cut();
    }
  }
}

void MainWindow::accelerator_copy_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_copy();
}

void MainWindow::accelerator_copy() {
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (now_focused_signal_button == editor_windows[i]->focus_in_signal_button) {
      // In case of the text editor, the USFM code is copied, not the plain text. 
      Editor * editor = editor_windows[i]->editor;
      gtk_clipboard_set_text(clipboard, editor->text_get_selection ().c_str(), -1);
    }
  }

  if (window_check_keyterms) {
    if (now_focused_signal_button == window_check_keyterms->focus_in_signal_button) {
      window_check_keyterms->copy_clipboard();
    }
  }

  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->copy();
    }
  }
}

void MainWindow::accelerator_paste_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_paste();
}

void MainWindow::accelerator_paste() {
  GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
  if (!gtk_clipboard_wait_is_text_available(clipboard))
    return;

  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    if (now_focused_signal_button == editor_windows[i]->focus_in_signal_button) {
      Editor * editor = editor_windows[i]->editor;
      gchar * text = gtk_clipboard_wait_for_text(clipboard);
      if (text) {
        editor->text_insert(text);
        g_free(text);
      }
    }
  }

  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->paste();
    }
  }
}

void MainWindow::accelerator_standard_text_1_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_standard_text_n(0);
}

void MainWindow::accelerator_standard_text_2_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_standard_text_n(1);
}

void MainWindow::accelerator_standard_text_3_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_standard_text_n(2);
}

void MainWindow::accelerator_standard_text_4_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_standard_text_n(3);
}

void MainWindow::accelerator_standard_text_n(unsigned int selector) {
  if (window_notes) {
    // Insert the text if the notes window has focus.
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      window_notes->insert_standard_text(selector);
    }
  }
}

void MainWindow::accelerator_new_project_note_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_new_note();
}

void MainWindow::accelerator_next_verse_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_next_verse();

}

void MainWindow::accelerator_previous_verse_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_previous_verse();
}

void MainWindow::accelerator_next_chapter_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_next_chapter();
}

void MainWindow::accelerator_previous_chapter_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_previous_chapter();
}

void MainWindow::accelerator_next_book_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_next_book();
}

void MainWindow::accelerator_previous_book_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_previous_book();
}

void MainWindow::accelerator_next_reference_in_history_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_next_reference_in_history();
}

void MainWindow::accelerator_next_reference_in_history() {
  navigation.on_forward();
}

void MainWindow::accelerator_previous_reference_in_history_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_previous_reference_in_history();
}

void MainWindow::accelerator_previous_reference_in_history() {
  navigation.on_back();
}

void MainWindow::accelerator_go_to_reference_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_reference_interactive();
}

void MainWindow::accelerator_close_window_callback(gpointer user_data) {
  ((MainWindow *) user_data)->accelerator_close_window();
}

void MainWindow::accelerator_close_window()
// Closes the focused window.
{
  // Keyterms in verse. 
  if (window_show_keyterms) {
    if (now_focused_signal_button == window_show_keyterms->focus_in_signal_button) {
      on_window_show_keyterms_delete_button();
    }
  }

  // Quick references.
  if (window_show_quick_references) {
    if (now_focused_signal_button == window_show_quick_references->focus_in_signal_button) {
      on_window_show_quick_references_delete_button();
    }
  }

  // Merge
  if (window_merge) {
    if (now_focused_signal_button == window_merge->focus_in_signal_button) {
      on_window_merge_delete_button();
    }
  }

  // Resources.
  for (unsigned int i = 0; i < resource_windows.size(); i++) {
    WindowResource * resource_window = resource_windows[i];
    if (now_focused_signal_button == resource_window->focus_in_signal_button) {
      on_window_resource_delete_button(GTK_BUTTON(resource_window->delete_signal_button));
      break;
    }
  }

  // Outline.
  if (window_outline) {
    if (now_focused_signal_button == window_outline->focus_in_signal_button) {
      on_window_outline_delete_button();
    }
  }

  // Check keyterms.
  if (window_check_keyterms) {
    if (now_focused_signal_button == window_check_keyterms->focus_in_signal_button) {
      on_window_check_keyterms_delete_button();
    }
  }

  // Styles.
  if (window_styles) {
    if (now_focused_signal_button == window_styles->focus_in_signal_button) {
      on_window_styles_delete_button();
    }
  }

  // Notes.
  if (window_notes) {
    if (now_focused_signal_button == window_notes->focus_in_signal_button) {
      on_window_notes_delete_button();
    }
  }

  // References.
  if (window_references) {
    if (now_focused_signal_button == window_references->focus_in_signal_button) {
      on_window_references_delete_button();
    }
  }

  // Editors.
  for (unsigned int i = 0; i < editor_windows.size(); i++) {
    WindowEditor * editor_window = editor_windows[i];
    if (now_focused_signal_button == editor_window->focus_in_signal_button) {
      on_window_editor_delete_button(GTK_BUTTON(editor_window->delete_signal_button));
      break;
    }
  }
  handle_editor_focus();
}

void MainWindow::accelerator_goto_styles_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_goto_styles_area();
}

void MainWindow::accelerator_quit_program_callback(gpointer user_data) {
  gtk_main_quit();
}

void MainWindow::accelerator_activate_text_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_text_area_activate();
}

void MainWindow::accelerator_activate_tools_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_tools_area_activate();
}

void MainWindow::accelerator_activate_notes_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_notes_area_activate();
}

void MainWindow::accelerator_next_reference_in_reference_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_next_reference();
}
void MainWindow::accelerator_previous_reference_in_reference_area_callback(gpointer user_data) {
  ((MainWindow *) user_data)->on_previous_reference();
}
void MainWindow::accelerator_next_project_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_next_previous_project(true);
}

void MainWindow::accelerator_previous_project_callback(gpointer user_data) {
  ((MainWindow *) user_data)->goto_next_previous_project(false);
}

/*

 Todo Improve the window layout system.

 The merge window should have its horizontal scrollbar set to automatic.
 
 We need to look at the "todo" entries in windownotes.h/cpp.

 Adding text to notes by accelerators, and by the menu.
 Adding the current reference to the note.
 If the notes window shows up on startup, it does now not display the relevant notes.

 Add all the accelerators in a helpfile, build the file while adding the accelerator.
 
 There is one menu window, which is the main one, and each function will get its own window.

 It is very important to make the program to "feel" as if it is one and the same window.
 This means that, e.g. the same shortcuts work in every window, and that the menu can be accessed
 from any window.
 
 To make a View / Tile menu. And a stack one. 
 When a new window is opened, for example, a new editor is opened, it then automatically finds a place
 within the existing editors, and tiles (or stacks) the windows.

 If the positions are reset, then all standard positions are put to zero, so that next time they show up,
 they will be allocated into the new position.
 
 If we'd like to present all windows when one window is focused, then the programs's icon in the taskbar will flash.
 This can be resolved in the following manner. Make the menu window to be like any of the other extra windows.
 And make the main window to be the main window, but it does not show. This main window has the icon in the taskbar,
 if never present()-ed, so the icon will never flash. If the menu window is deleted, then we might delete the 
 main window too, or else we can do it that we delete the main window only if all windows get deleted, or 
 if the quit menu is chosen. But the best I think is to quit when the menu quits.
 
 Eventually the menu will also become an independent window, and can be clicked away to disappear from the screen.
 But ideally we would like to stop the program if the menu is clicked away.
 We can resolved it this way.
 - If the menu is clicked away, then it asks whether to stop the program. If so, yes, if not, the menu goes away.
 - If the last window is clicked away, then the program stops too.
 - If Ctrl-Q is typed, it stops too, and this applies in any window.
 - If Ctrl-W is clicked in any window, it goes away, and if it is the last one, the program quits.

 When all the windows are done, then we need to check whether all menu entries work in each window,
 and whether all shortcuts in each relevant window.
 When all windows have been detached, we need to verify copy and paste through the menu and through shortcuts.
 
 All those places where it has "References references", it is to be looked into whether that cannot be moved
 into the references window itself, rather than cluttering the code in MainWindow.

 If the merge window opens while the two projects are opened already, it does not show them in the merge window.
 Does the merge window not have a horizontal scrollbar that can prevent the main window from getting too wide?
 The lines in the merge window should wrap, really.
 If Ctrl-N is pressed in an already existing notes window, the Ctrl-1/4 shortcuts don't work unless the window
 is clicked so as to focus it. No, even after clicking in the window, the shortcuts don't work.

 We have to put the footnotes in separate GtkTextViews. 
 So we can then use as many GtkTextViews as there are footnotes. 
 And then the automatic width routines go away.
 This is done in the same scrolled window, just by adding the required number of GtkTextViews to the vertical box.
 Each note consists of a horizontal box, with the caller in it as a label, and then a GtkTextView for the note.
 The up / down arrows keep working, so one can move from one GtkTextView to another, just as it is now.
 
 Adding the current reference to the project note does not work.

 Clicking on a project notes [references] should bring up the references in the window. It does not do that now. 
 
 When selecting the note category there's a lot of redundant focusing going around, and same applies to deleting a note.
 
 The following routines need attention to their code that has been commented out:
 void MainWindow::menu_redo()
 void MainWindow::menu_edit()
 void MainWindow::menu_undo() 
 void MainWindow::on_tools_area_activate() - This needs a routine that tracks the last focused tool, and this is
 the tool to be focused when the area is activated.
 void MainWindow::on_cut() 
 void MainWindow::on_copy()
 void MainWindow::on_copy_without_formatting()
 void MainWindow::on_paste() 
 
 */

