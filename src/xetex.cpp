/*
** Copyright (©) 2003-2009 Teus Benschop.
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


// Tex editors: lyx, kile, texmaker


XeTeX::XeTeX(int dummy)
{
	create_work_area ();
	place_ptx2pdf_macros ();
	place_glw_sample ();
	initialize_variables ();
}


XeTeX::~XeTeX()
{
}


void XeTeX::initialize_variables ()
{
  // The page size defaults to A4 = 210 x 297 millimeters (8.27 x 11.69 inches).
  page_width_centimeters = 21;
  page_height_centimeters = 29.7;
  
  // Page margins default in centimeters.
  inside_page_margin_centimeters = 2.5;
  outside_page_margin_centimeters = 1.5;
  top_page_margin_centimeters = 2;
  bottom_page_margin_centimeters = 2;
  

  

}


void XeTeX::create_work_area ()
{
  working_directory = gw_build_filename (directories_get_temp (), "xetex");
	unix_rmdir (working_directory);
	gw_mkdir_with_parents (working_directory);
}


void XeTeX::place_ptx2pdf_macros ()
{
	GwSpawn spawn ("tar");
	spawn.workingdirectory (working_directory);
	spawn.arg ("zxf");
	spawn.arg (gw_build_filename (directories_get_package_data (), "ptx2pdf.tar.gz"));
	spawn.run ();
	ustring ptx2pdf_directory = "ptx2pdf";
  ReadFiles rf (gw_build_filename (working_directory, ptx2pdf_directory), "", "");
  for (unsigned int i = 0; i < rf.files.size(); i++) {
		unix_mv (gw_build_filename (working_directory, ptx2pdf_directory, rf.files[i]), gw_build_filename (working_directory, rf.files[i]));
	}
	unix_rmdir (gw_build_filename (working_directory, ptx2pdf_directory));
}


void XeTeX::place_glw_sample ()
{
	GwSpawn spawn ("unzip");
	spawn.workingdirectory (working_directory);
	spawn.arg (gw_build_filename (directories_get_package_data (), "ptx2pdf-sample.zip"));
	spawn.run ();
	unlink (gw_build_filename (working_directory, "GLW.tex").c_str());
	unlink (gw_build_filename (working_directory, "GLW-setup.txt").c_str());
	unlink (gw_build_filename (working_directory, "README.txt").c_str());
}


void XeTeX::write_document_tex_file () // Todo writing this one
{
  // Include a bunch of ptx2pdf macros.
  document_tex.push_back ("\\input paratext2.tex");

  // Paper size.
  document_tex.push_back ("\\PaperWidth=" + convert_to_string (page_width_centimeters) + "cm");
  document_tex.push_back ("\\PaperHeight=" + convert_to_string (page_height_centimeters) + "cm");

  document_tex.push_back ("\\CropMarkstrue");

  // Basic unit for margins is 1 cm; changing this will alter them all.
  document_tex.push_back ("\\MarginUnit=1cm");

  // Relative sizes of margins, based on the unit above.
  document_tex.push_back ("\\def\\TopMarginFactor{" + convert_to_string (top_page_margin_centimeters) + "}");
  document_tex.push_back ("\\def\\BottomMarginFactor{" + convert_to_string (bottom_page_margin_centimeters) + "}");
  document_tex.push_back ("\\def\\SideMarginFactor{" + convert_to_string (outside_page_margin_centimeters) + "}");

	// Optionally extra margin for the gutter on the binding side.
	if (inside_page_margin_centimeters != outside_page_margin_centimeters) {
	  document_tex.push_back ("\\BindingGuttertrue");
    document_tex.push_back ("\\BindingGutter=" + convert_to_string (inside_page_margin_centimeters = outside_page_margin_centimeters) + "cm");
    document_tex.push_back ("\\DoubleSidedtrue");
	}
 // Todo
  // Fonts to use for "plain", "bold", "italic", and "bold italic" from the Paratext stylesheet
  // (they need not really be italic, etc, of course)
  document_tex.push_back ("\\def\\regular{\"Charis SIL\"}");
  document_tex.push_back ("\\def\\bold{\"Charis SIL/B\"}");
  document_tex.push_back ("\\def\\italic{\"Charis SIL/I\"}");
  document_tex.push_back ("\\def\\bolditalic{\"Charis SIL/BI\"}");

  // Use right-to-left layout mode
  //document_tex.push_back ("\\RTLtrue");

  // Unit for font sizes in the stylesheet; changing this will scale all text proportionately
  document_tex.push_back ("\\FontSizeUnit=0.8pt");

  // Scaling factor used to adjust line spacing, relative to font size
  document_tex.push_back ("\\def\\LineSpacingFactor{1.05}");
  document_tex.push_back ("\\def\\VerticalSpaceFactor{1.0}");

  // Information to include in the running header (at top of pages, except first)
  // We set the items to print at left/center/right of odd and even pages separately
  // Possible contents:
  //   \rangeref = Scripture reference of the range of text on the page;
  //   \firstref = reference of the first verse on the page)
  //   \lastref = reference of the last verse on the page)
  //   \pagenumber = the page number
  //   \empty = print nothing in this position
  document_tex.push_back ("\\def\\RHoddleft{\\empty}");
  document_tex.push_back ("\\def\\RHoddcenter{\\empty}");
  document_tex.push_back ("\\def\\RHoddright{\\rangeref}");

  document_tex.push_back ("\\def\\RHevenleft{\\rangeref}");
  document_tex.push_back ("\\def\\RHevencenter{\\empty}");
  document_tex.push_back ("\\def\\RHevenright{\\empty}");

  document_tex.push_back ("\\def\\RHtitleleft{\\empty}");
  document_tex.push_back ("\\def\\RHtitlecenter{\\empty}");
  document_tex.push_back ("\\def\\RHtitleright{\\empty}");

  document_tex.push_back ("\\def\\RFoddcenter{\\pagenumber}");
  document_tex.push_back ("\\def\\RFevencenter{\\pagenumber}");
  document_tex.push_back ("\\def\\RFtitlecenter{\\pagenumber}");

	// whether to include verse number in running head, or only chapter
  document_tex.push_back ("\\VerseRefstrue");

	// whether to skip printing verse number 1 at start of chapter
  document_tex.push_back ("\\OmitVerseNumberOnetrue");
	// whether to use paragraph indent at drop-cap chapter numbers
  //\IndentAtChaptertrue");

  document_tex.push_back ("\\AutoCallers{f}{*,†,‡,¶,§}");
  document_tex.push_back ("\\PageResetCallers{f}");
  //\NumericCallers{f}
  //\OmitCallerInNote{f}

	// reformat \x notes as a single paragraph
  document_tex.push_back ("\\ParagraphedNotes{x}");

  document_tex.push_back ("\\TitleColumns=1");
  document_tex.push_back ("\\IntroColumns=1");
  document_tex.push_back ("\\BodyColumns=2");

	// gutter between double cols, relative to font size
	document_tex.push_back ("\\def\\ColumnGutterFactor{15}");

  // Define the Paratext stylesheet to be used as a basis for formatting
  document_tex.push_back ("\\stylesheet{usfm.sty}");
  document_tex.push_back ("\\stylesheet{GLW-custom.sty}");

  document_tex.push_back ("\\ptxfile{Intro-GLW.sfm}");
  document_tex.push_back ("\\ptxfile{43-JHN-GLW.sfm}");
  document_tex.push_back ("\\ptxfile{62-1JN-GLW.sfm}");
  document_tex.push_back ("\\ptxfile{63-2JN-GLW.sfm}");
  document_tex.push_back ("\\ptxfile{64-3JN-GLW.sfm}");

  document_tex.push_back ("\\end");

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
  gw_message ("All the data for this document is available in temporal folder " + working_directory + ".");
  gw_message ("You can tune the files by hand, then run \"xetex document.tex\" in this folder to convert it into a PDF file.");
	return pdf_file;
}


void XeTeX::page_size_set(double width_centimeters, double height_centimeters) // Todo
{
	page_width_centimeters = width_centimeters;
	page_height_centimeters = height_centimeters;
}


void XeTeX::page_margins_set(double inside_margin_centimeters, double outside_margin_centimeters, double top_margin_centimeters, double bottom_margin_centimeters) // Todo
{
  inside_page_margin_centimeters = inside_margin_centimeters;
  outside_page_margin_centimeters = outside_margin_centimeters;
  top_page_margin_centimeters = top_margin_centimeters;
  bottom_page_margin_centimeters = bottom_margin_centimeters;
}


/*

Todo xetex


One dialog that does the settings. Several tabs could be used.
Information may be in the dialog that describe how to set certain things, such as font in the View menu, and
other settings in the stylesheet.

task #8017: use/allow xetex as formatter

Remember that to do complex non-Roman scripts like I think Farsi is, you will
need the latest version of XeTeX. The one that gets install with the current
version of Ubuntu (9.10) may not be good enough. You can pull the most up to
date version from the SIL repo. (deb http://packages.sil.org/ubuntu karmic
main).












bug #27949: Farsi Right-to-Left Cross Referencing and Printing
1. When a printed pdf version of the text is made, whenever the footnotes and cross-references appear on the same page, they overlay on each other.

2. Also page numbering and chapter numbering in the header part of the printed pages are still in Latin format and not Farsi.

I have included 1John which has the references and footnotes on chapter 1. Also I have included a sample copy of the PDF output.





*/
