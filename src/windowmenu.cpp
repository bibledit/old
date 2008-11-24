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

#include "libraries.h"
#include <glib.h>
#include "windowmenu.h"
#include "help.h"
#include "gui_features.h"

WindowMenu::WindowMenu(GtkAccelGroup *accelerator_group, bool startup) :
  WindowBase(widMenu, "Bibledit", accelerator_group, startup)
// Window showing the menu.
{
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox);
  gtk_container_add (GTK_CONTAINER (window), vbox);
  
  // Gui Features object.
  GuiFeatures guifeatures(0);

  menubar1 = gtk_menu_bar_new();
  gtk_widget_show(menubar1);
  gtk_box_pack_start(GTK_BOX (vbox), menubar1, FALSE, FALSE, 0);

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

    open1 = gtk_image_menu_item_new_from_stock("gtk-open", NULL); 
    gtk_widget_show(open1);
    gtk_container_add(GTK_CONTAINER (file_project_menu), open1);

    delete1 = gtk_image_menu_item_new_from_stock("gtk-delete", NULL);
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

  open_references1 = NULL;
  references_save_as = NULL;
  if (guifeatures.references_management()) {

    open_references1 = gtk_image_menu_item_new_with_mnemonic("_Open");
    gtk_widget_show(open_references1);
    gtk_container_add(GTK_CONTAINER (file_references_menu), open_references1);

    image466 = gtk_image_new_from_stock("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image466);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (open_references1), image466);

    references_save_as = gtk_image_menu_item_new_from_stock("gtk-save-as", NULL);
    gtk_widget_show(references_save_as);
    gtk_container_add(GTK_CONTAINER (file_references_menu), references_save_as);

  }

  close_references = NULL;
  delete_references = NULL;
  reference_hide = NULL;
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

    style_properties = gtk_image_menu_item_new_from_stock("gtk-properties", NULL);
    gtk_widget_show(style_properties);
    gtk_container_add(GTK_CONTAINER (style_menu), style_properties);

  }

  style_delete= NULL;
  if (guifeatures.styles_management()) {

    style_delete = gtk_image_menu_item_new_from_stock("gtk-delete", NULL);
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
  new_note = NULL;
  delete_note = NULL;
  import_notes = NULL;
  export_notes = NULL;
  if (guifeatures.project_notes_management()) {

    notes2 = gtk_image_menu_item_new_with_mnemonic("Project _notes");
    gtk_widget_show(notes2);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), notes2);

    image936 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image936);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (notes2), image936);

    notes2_menu = gtk_menu_new();
    gtk_menu_item_set_submenu(GTK_MENU_ITEM (notes2), notes2_menu);

    new_note = gtk_image_menu_item_new_from_stock("gtk-new", NULL);
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

    print = gtk_image_menu_item_new_from_stock("gtk-print", NULL);
    gtk_widget_show(print);
    gtk_container_add(GTK_CONTAINER (menuitem_file_menu), print);

  }

  quit1 = gtk_image_menu_item_new_from_stock("gtk-quit", NULL);
  gtk_widget_show(quit1);
  gtk_container_add(GTK_CONTAINER (menuitem_file_menu), quit1);

  menuitem_edit = gtk_menu_item_new_with_mnemonic("_Edit");
  gtk_widget_show(menuitem_edit);
  gtk_container_add(GTK_CONTAINER (menubar1), menuitem_edit);

  menuitem_edit_menu = gtk_menu_new();
  gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuitem_edit), menuitem_edit_menu);

  cut1 = gtk_image_menu_item_new_from_stock("gtk-cut", NULL);
  gtk_widget_show(cut1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), cut1);

  copy1 = gtk_image_menu_item_new_from_stock("gtk-copy", NULL);
  gtk_widget_show(copy1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), copy1);

  copy_without_formatting = gtk_image_menu_item_new_with_mnemonic("Copy _without formatting");
  gtk_widget_show(copy_without_formatting);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), copy_without_formatting);

  image18220 = gtk_image_new_from_stock("gtk-copy", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image18220);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (copy_without_formatting), image18220);

  paste1 = gtk_image_menu_item_new_from_stock("gtk-paste", NULL);
  gtk_widget_show(paste1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), paste1);

  separator2 = gtk_separator_menu_item_new();
  gtk_widget_show(separator2);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator2);
  gtk_widget_set_sensitive(separator2, FALSE);

  undo1 = gtk_image_menu_item_new_with_mnemonic("_Undo");
  gtk_widget_show(undo1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), undo1);

  image295 = gtk_image_new_from_stock("gtk-undo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image295);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (undo1), image295);

  redo1 = gtk_image_menu_item_new_with_mnemonic("_Redo");
  gtk_widget_show(redo1);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), redo1);

  image296 = gtk_image_new_from_stock("gtk-redo", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image296);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (redo1), image296);

  separator4 = gtk_separator_menu_item_new();
  gtk_widget_show(separator4);
  gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), separator4);
  gtk_widget_set_sensitive(separator4, FALSE);

  find1 = NULL;
  if (guifeatures.references_and_find()) {

    find1 = gtk_image_menu_item_new_from_stock("gtk-find", NULL); 
    gtk_widget_show(find1);
    gtk_container_add(GTK_CONTAINER (menuitem_edit_menu), find1);

  }

  find_and_replace1 = NULL;
  if (guifeatures.replace()) {

    find_and_replace1 = gtk_image_menu_item_new_from_stock("gtk-find-and-replace", NULL);
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

  viewnotes = NULL;
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

  /* Todo implement menu.

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

    image1963 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1963);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_1), image1963);

    standard_text_2 = gtk_image_menu_item_new_with_mnemonic("Standard text _2");
    gtk_widget_show(standard_text_2);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_2);

    image1964 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1964);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_2), image1964);

    standard_text_3 = gtk_image_menu_item_new_with_mnemonic("Standard text _3");
    gtk_widget_show(standard_text_3);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_3);

    image1965 = gtk_image_new_from_stock("gtk-paste", GTK_ICON_SIZE_MENU);
    gtk_widget_show(image1965);
    gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (standard_text_3), image1965);

    standard_text_4 = gtk_image_menu_item_new_with_mnemonic("Standard text _4");
    gtk_widget_show(standard_text_4);
    gtk_container_add(GTK_CONTAINER (insert1_menu), standard_text_4);

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

  image17520 = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image17520);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (help_main), image17520);

  help_context = gtk_image_menu_item_new_with_mnemonic("C_ontext");
  gtk_widget_show(help_context);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), help_context);

  image16053 = gtk_image_new_from_stock("gtk-help", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image16053);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (help_context), image16053);

  system_log1 = gtk_image_menu_item_new_with_mnemonic("_System log");
  gtk_widget_show(system_log1);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), system_log1);

  image4388 = gtk_image_new_from_stock("gtk-dialog-info", GTK_ICON_SIZE_MENU);
  gtk_widget_show(image4388);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM (system_log1), image4388);

  about1 = gtk_image_menu_item_new_from_stock("gtk-about", NULL);
  gtk_widget_show(about1);
  gtk_container_add(GTK_CONTAINER (menuitem_help_menu), about1);

  toolbar1 = gtk_toolbar_new();
  gtk_widget_show(toolbar1);
  gtk_box_pack_start(GTK_BOX (vbox), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style(GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);

  // Todo navigation.build(toolbar1);
  // g_signal_connect ((gpointer) navigation.reference_signal_delayed, "clicked", G_CALLBACK (on_navigation_new_reference_clicked), gpointer(this));

  hbox5 = gtk_hbox_new(FALSE, 0);
  gtk_widget_show(hbox5);
  gtk_box_pack_start(GTK_BOX (vbox), hbox5, FALSE, FALSE, 0);

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
  
  */
}

WindowMenu::~WindowMenu() {
}

