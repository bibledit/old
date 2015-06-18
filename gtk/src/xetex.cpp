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


#include "xetex.h"
#include "unixwrappers.h"
#include "gwrappers.h"
#include "directories.h"
#include "utilities.h"
#include "runtime.h"
#include "tiny_utilities.h"
#include "settings.h"
#include "books.h"
#include "stylesheetutils.h"
#include "styles.h"
#include <glib/gi18n.h>

// Tex editors: lyx, kile, texmaker


XeTeX::XeTeX(int dummy)
{
  create_work_area ();
  place_ptx2pdf_macros ();
  initialize_variables ();
}


XeTeX::~XeTeX()
{
}


void XeTeX::initialize_variables ()
{
}


void XeTeX::create_work_area ()
{
  working_directory = gw_build_filename (Directories->get_temp (), "xetex");
  unix_rmdir (working_directory);
  gw_mkdir_with_parents (working_directory);
}


void XeTeX::place_ptx2pdf_macros ()
{
  GwSpawn spawn ("tar");
  spawn.workingdirectory (working_directory);
  spawn.arg ("zxf");
  spawn.arg (gw_build_filename (Directories->get_package_data (), "ptx2pdf.tar.gz"));
  spawn.run ();
  ustring ptx2pdf_directory = "ptx2pdf";
  ReadFiles rf (gw_build_filename (working_directory, ptx2pdf_directory), "", "");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
    unix_mv (gw_build_filename (working_directory, ptx2pdf_directory, rf.files[i]), gw_build_filename (working_directory, rf.files[i]));
  }
  unix_rmdir (gw_build_filename (working_directory, ptx2pdf_directory));
}


void XeTeX::write_document_tex_file ()
{
  // Settings.
  extern Settings * settings;
  ProjectConfiguration *projectconfig = settings->projectconfig(settings->genconfig.project_get());
  
  // Style sheet.
  extern Styles * styles;
  Stylesheet * sheet = styles->stylesheet (stylesheet_get_actual ());
  
  document_tex.push_back (_("% Configuration file created by Bibledit-Gtk"));
  document_tex.push_back (_("% You can modify it to suit your needs"));
  document_tex.push_back (_("% After modification, run the following command in this directory:"));
  document_tex.push_back (_("%   xetex document.tex"));
  document_tex.push_back (_("% After that look carefully at the output"));
  document_tex.push_back (_("% If it says that a re-run is required, repeat this command"));

  document_tex.push_back ("");
  document_tex.push_back (_("% Include the ptx2pdf macros"));
  document_tex.push_back ("\\input paratext2.tex");

  document_tex.push_back ("");
  document_tex.push_back (_("% Paper size"));
  document_tex.push_back ("\\PaperWidth=" + convert_to_string (settings->genconfig.paper_width_get()) + "cm");
  document_tex.push_back ("\\PaperHeight=" + convert_to_string (settings->genconfig.paper_height_get()) + "cm");

  if (settings->session.print_crop_marks){
    document_tex.push_back ("");
    document_tex.push_back (_("% Crop marks"));
    document_tex.push_back ("\\CropMarkstrue");
  }

  document_tex.push_back ("");
  document_tex.push_back (_("% The basic unit for the margins; changing this will alter them all"));
  document_tex.push_back ("\\MarginUnit=1cm");

  document_tex.push_back ("");
  document_tex.push_back (_("% Relative sizes of margins, based on the unit above"));
  document_tex.push_back ("\\def\\TopMarginFactor{" + convert_to_string (settings->genconfig.paper_top_margin_get()) + "}");
  document_tex.push_back ("\\def\\BottomMarginFactor{" + convert_to_string (settings->genconfig.paper_bottom_margin_get()) + "}");
  document_tex.push_back ("\\def\\SideMarginFactor{" + convert_to_string (settings->genconfig.paper_outside_margin_get()) + "}");

  if (settings->genconfig.paper_inside_margin_get() != settings->genconfig.paper_outside_margin_get()) {
    document_tex.push_back ("");
    document_tex.push_back (_("% Extra margin for the gutter on the binding side"));
    document_tex.push_back ("\\BindingGuttertrue");
    document_tex.push_back ("\\BindingGutter=" + convert_to_string (settings->genconfig.paper_inside_margin_get() - settings->genconfig.paper_outside_margin_get()) + "cm");
    document_tex.push_back ("");
    document_tex.push_back (_("% Double sided printing"));
    document_tex.push_back ("\\DoubleSidedtrue");
  }

  if (!projectconfig->editor_font_default_get()) {
    PangoFontDescription *font_desc = pango_font_description_from_string(projectconfig->editor_font_name_get().c_str());
    if (font_desc){
      
      // Assemble the string for the font mapping.
      ustring font_mapping = projectconfig->xetex_font_mapping_file_get();
      if (!font_mapping.empty()) {
        if (g_str_has_suffix (font_mapping.c_str(), ".tec")) {
          font_mapping = gw_path_get_basename (font_mapping);
          // Remove the .tec suffix.
          font_mapping.erase (font_mapping.length() - 4, 4);
          // Insert the mapping command.
          font_mapping.insert (0, "mapping=");
        } else {
          gw_warning (_("Font mapping file ") + font_mapping +  _(" should have the .tec suffix - ignoring this file"));
          font_mapping.clear();
        }
      }

      // Assemble the string for the shaping engine.
      ustring shaping_engine;
      switch (XeTeXScriptingEngineType (projectconfig->xetex_shaping_engine_get())) {
        case xtxsetGeneric:                                 break;
        case xtxsetArab:    shaping_engine = "script=arab"; break;
      }

      // Assemble the addition to the font.
      ustring font_addition;
      if (!font_mapping.empty()) {
        if (font_addition.empty())
          font_addition.append (":");
        else 
          font_addition.append (";");
        font_addition.append (font_mapping);
      }
      if (!shaping_engine.empty()) {
        if (font_addition.empty())
          font_addition.append (":");
        else 
          font_addition.append (";");
        font_addition.append (shaping_engine);
      }

      ustring font_family = pango_font_description_get_family (font_desc);
      document_tex.push_back ("");
      document_tex.push_back (_("% Fonts to use for \"plain\", \"bold\", \"italic\", and \"bold italic\" from the stylesheet"));
      document_tex.push_back (_("% (they need not really be italic, etc.)"));
      document_tex.push_back (_("% Add e.g. \":mapping=farsidigits\" to get digits in Farsi, provided the farsidigits.tec TECkit mapping is available"));
      document_tex.push_back (_("% Add e.g. \":script=arab\" to use the arab shaping engine instead of the generic one"));
      document_tex.push_back ("\\def\\regular{\"" + font_family + font_addition + "\"}");
      document_tex.push_back ("\\def\\bold{\"" + font_family + "/B" + font_addition + "\"}");
      document_tex.push_back ("\\def\\italic{\"" + font_family + "/I" + font_addition +  "\"}");
      document_tex.push_back ("\\def\\bolditalic{\"" + font_family + "/BI" + font_addition +  + "\"}");
      pango_font_description_free(font_desc);
    }
  }

  if (projectconfig->right_to_left_get()) {
    document_tex.push_back ("");
    document_tex.push_back (_("% Right-to-left layout mode"));
    document_tex.push_back ("\\RTLtrue");
  }

  document_tex.push_back ("");
  document_tex.push_back (_("% The unit for font sizes in the stylesheet; changing this will scale all text proportionately"));
  document_tex.push_back ("\\FontSizeUnit=1pt");

  document_tex.push_back ("");
  document_tex.push_back (_("% Scaling factor used to adjust line spacing, relative to font size"));
  double line_spacing_factor = 1.0;
  double vertical_space_factor = 1.0;
  if (!projectconfig->editor_font_default_get()){
    line_spacing_factor = projectconfig->text_line_height_get() / 100;
    vertical_space_factor = projectconfig->text_line_height_get() / 100;
  }
  document_tex.push_back ("\\def\\LineSpacingFactor{" + convert_to_string (line_spacing_factor) + "}");
  document_tex.push_back ("\\def\\VerticalSpaceFactor{" + convert_to_string (vertical_space_factor) + "}");

  document_tex.push_back ("");
  document_tex.push_back (_("% Information to include in the running header (at top of pages, except first)"));
  document_tex.push_back (_("% We set the items to print at left/center/right of odd and even pages separately"));
  document_tex.push_back (_("% Possible contents:"));
  document_tex.push_back (_("%   \\rangeref = Scripture reference of the range of text on the page;"));
  document_tex.push_back (_("%   \\firstref = reference of the first verse on the page)"));
  document_tex.push_back (_("%   \\lastref = reference of the last verse on the page)"));
  document_tex.push_back (_("%   \\pagenumber = the page number"));
  document_tex.push_back (_("%   \\empty = print nothing in this position"));
  document_tex.push_back ("\\def\\RHoddleft{\\empty}");
  document_tex.push_back ("\\def\\RHoddcenter{\\empty}");
  document_tex.push_back ("\\def\\RHoddright{\\rangeref}");
  document_tex.push_back ("");
  document_tex.push_back ("\\def\\RHevenleft{\\rangeref}");
  document_tex.push_back ("\\def\\RHevencenter{\\empty}");
  document_tex.push_back ("\\def\\RHevenright{\\empty}");
  document_tex.push_back ("");
  document_tex.push_back ("\\def\\RHtitleleft{\\empty}");
  document_tex.push_back ("\\def\\RHtitlecenter{\\empty}");
  document_tex.push_back ("\\def\\RHtitleright{\\empty}");
  document_tex.push_back ("");
  document_tex.push_back ("\\def\\RFoddcenter{\\pagenumber}");
  document_tex.push_back ("\\def\\RFevencenter{\\pagenumber}");
  document_tex.push_back ("\\def\\RFtitlecenter{\\pagenumber}");

  document_tex.push_back ("");
  document_tex.push_back (_("% Whether to include verse number in running head, or only chapter"));
  document_tex.push_back ("\\VerseRefstrue");

  document_tex.push_back ("");
  document_tex.push_back (_("% Whether to skip printing verse number 1 at start of chapter"));
  document_tex.push_back ("\\OmitVerseNumberOnetrue");

  document_tex.push_back ("");
  document_tex.push_back (_("% Whether to use paragraph indent at drop-cap chapter numbers"));
  document_tex.push_back (_("% \\IndentAtChaptertrue"));

  // Go through the stylesheet looking for note markers.
  for (unsigned int i = 0; i < sheet->styles.size(); i++) {
    bool retrieve_note_data = false;
    StyleV2 * style = sheet->styles[i];
    if (style->type == stFootEndNote) {
      if ((style->subtype == fentFootnote) || (style->subtype == fentEndnote)) {
        retrieve_note_data = true;
      }
    }
    if ((style->type == stFootEndNote) || (style->type == stCrossreference)) {
      if (style->subtype == ctCrossreference) {
        retrieve_note_data = true;
      }
    }
    if (retrieve_note_data) {

      ustring marker = style->marker;
      document_tex.push_back ("");
      document_tex.push_back (_("% Reformat \\") + marker + " notes as a single paragraph");
      document_tex.push_back ("\\ParagraphedNotes{" + marker + "}");

      document_tex.push_back ("");
      NoteNumberingType note_numbering = NoteNumberingType (style->userint1);
      switch (note_numbering) {
        case nntNumerical:
          document_tex.push_back (_("% Numerical callers for \\") + marker + _(" notes"));
          document_tex.push_back ("\\NumericCallers{" + marker + "}");
          break;
        case nntAlphabetical:
          document_tex.push_back (_("% Alphabetical callers for \\") + marker + _(" notes"));
          document_tex.push_back ("\\AutoCallers{" + marker+ "}{a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z}");
          break;
        case nntUserDefined:
          ustring autocallers;
          for (unsigned int i = 0; i < style->userstring1.size(); i++) {
            if (i)
              autocallers.append (",");
            autocallers.append (style->userstring1.substr (i, 1));
          }
          if (!autocallers.empty()) {
            document_tex.push_back (_("% Special caller sequence for \\") + marker + " notes");
            document_tex.push_back ("\\AutoCallers{" + marker+ "}{" + autocallers + "}");
          }
          break;
      }

      document_tex.push_back ("");
      NoteNumberingRestartType note_restart = NoteNumberingRestartType (style->userint2);
      switch (note_restart) {
        case nnrtNever:
          break;
        case nnrtBook:
          break;
        case nnrtChapter:
          break;
      }
      document_tex.push_back (_("% Reset callers every page for \\") + marker + _(" notes"));
      document_tex.push_back ("\\PageResetCallers{" + marker + "}");

      document_tex.push_back ("");
      document_tex.push_back (_("% Omit callers in the note for \\") + marker + _(" notes"));
      document_tex.push_back (_("% \\OmitCallerInNote{") + marker + "}");

    }
  }

  document_tex.push_back ("");
  document_tex.push_back (_("% The number of columns"));
  document_tex.push_back ("\\TitleColumns=1");
  document_tex.push_back ("\\IntroColumns=1");
  document_tex.push_back ("\\BodyColumns=2");

  document_tex.push_back ("");
  document_tex.push_back (_("% The gutter between double cols, relative to font size"));
  document_tex.push_back ("\\def\\ColumnGutterFactor{15}");

  // Define the Paratext stylesheet to be used as a basis for formatting
  write_stylesheet ();

  // Write the data and add their filenames.
  for (unsigned int i = 0; i < book_ids.size(); i++) {
    ustring filename = convert_to_string (book_ids[i]) + " " + books_id_to_english(book_ids[i]) + ".usfm";
    replace_text (filename, " ", "_");
    write_lines (gw_build_filename (working_directory, filename), book_data[i]);
    document_tex.push_back ("\\ptxfile{" + filename + "}");
  }

  // End of document input.
  document_tex.push_back ("\\end");

  // Write document.text to file.
  write_lines (gw_build_filename (working_directory, "document.tex"), document_tex);
}


ustring XeTeX::run ()
{
  write_document_tex_file ();
  ustring pdf_file;
  if (runtime_check (rtXeTeX)) {
    pdf_file = gw_build_filename (working_directory, "document.pdf");
    bool re_run = false;
    unsigned int run_count = 0;
    do {
      run_count++;
      GwSpawn spawn (runtime_program (rtXeTeX));
      spawn.workingdirectory (working_directory);
      spawn.arg ("document.tex");
      spawn.read ();
      spawn.progress ("Formatting run " + convert_to_string (run_count), true);
      spawn.run ();
      re_run = false;
      for (unsigned int i = 0; i < spawn.standardout.size(); i++) {
        gw_message (spawn.standardout[i]);
        if (spawn.standardout[i].find ("re-run") != string::npos) 
        re_run = true;
      }
      for (unsigned int i = 0; i < spawn.standarderr.size(); i++) {
        gw_critical (spawn.standarderr[i]);
        if (spawn.standarderr[i].find ("re-run") != string::npos) 
          re_run = true;
      }
      // If the formatting process was cancelled, bail out, with no pdf file.
      if (spawn.cancelled) {
        return "";
      }
    } while (re_run && (run_count < 10));
  }
  // Info for user in logfile.
  gw_message (_("All the data for this document is available in temporal folder ") + working_directory + ".");
  gw_message (_("You can tune the files by hand, then run \"xetex document.tex\" in this folder to convert it into a PDF file."));
  return pdf_file;
}


void XeTeX::add_book (unsigned int id, const vector <ustring>& data)
// Adds a book and its data to the object.
{
  book_ids.push_back (id);
  book_data.push_back (data);
}


void XeTeX::write_stylesheet ()
// Writes the Paratext stylesheet to be used.
{
  ustring stylesheet = stylesheet_get_actual ();
  ustring filename = gw_build_filename (working_directory, "stylesheet.sty");
  stylesheet_export_paratext (stylesheet, filename);
  document_tex.push_back ("\\stylesheet{stylesheet.sty}");
  
}



