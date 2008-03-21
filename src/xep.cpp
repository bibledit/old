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
#include "utilities.h"
#include <glib.h>
#include "xep.h"
#include "java.h"
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "fonts.h"
#include "uname.h"
#include "shell.h"
#include "settings.h"
#include "xmlfo-utils.h"
#include "constants.h"
#include "tiny_utilities.h"


/*
As FOP sometimes fails to properly format a Scripture, the commercially 
available proprietary XEP was chosen instead.
*/


ustring standard_xep_command ()
// Return the command to run script xep, if found.
{
  extern Settings * settings;
  ustring xep;
  #ifdef WIN32
  xep = gw_build_filename (settings->genconfig.xep_home_get (), "xep.bat");
  #else
  xep = gw_build_filename (settings->genconfig.xep_home_get (), "xep");
  #endif
  if (gw_find_program_in_path (xep)) return xep;
  return "";
}


ustring modified_xep_command ()
// Return the command to run xep. This command is the script
// xep that comes with the program. The amount of memory assigned to it gets 
// changed in this command, to allow XEP to format a whole project.
{
  // Produce path.
  ustring modified_command (standard_xep_command ());
  
  // Get contents of the standard script.
  ReadText rt (standard_xep_command (), true, false);
  // Insert memory size.
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    // First we had assigned 512 Mbyte, but that was not enough.
    #define XEP_MEMORY1 "-Xmx512M "
    size_t position;
    position = rt.lines[i].find (XEP_MEMORY1);
    if (position != string::npos) {
      rt.lines[i].erase (position, strlen (XEP_MEMORY1));
    }
    // Now we assign 1024 Mbyte.
    #define XEP_MEMORY2 "-Xmx1024M "
    position = rt.lines[i].find (XEP_MEMORY2);
    if (position != string::npos) {
      rt.lines[i].erase (position, strlen (XEP_MEMORY2));
    }
    position = rt.lines[i].find ("-classpath");
    if (position != string::npos) {
      rt.lines[i].insert (position, XEP_MEMORY2);
    }
  }
  // Store modified contents in modified script.
  write_lines (modified_command, rt.lines);
  
  // Return it.
  return modified_command; 
}


bool xep_present ()
// Returns whether the script to run xep can be found.
{
  bool present;
  present = g_file_test (standard_xep_command ().c_str(), G_FILE_TEST_IS_EXECUTABLE);
  return present;
}


void xep_create_xep_xml ()
// Creates the file xep.xml and puts it in the directory where xep has been
// installed. It inserts the font data into the file.
{
  // Configuration / session
  extern Settings * settings;
  // Printing font data.
  PrintingFonts pfonts (settings->genconfig.project_get());
  // Read the template.
  ReadText rt (gw_build_filename (directories_get_package_data (), "xep.xml"), true, false);
  vector<ustring> outputlines;
  // Gather fonts already defined in the template, so we do not redefine them
  // and get an error message.
  set<ustring> fonts_already_defined;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    size_t position;
    position = rt.lines[i].find ("<font-family name=");
    if (position != string::npos) {
      ustring font = rt.lines[i];
      position = font.find ("\"");
      font.erase (0, ++position);
      position = font.find ("\"");
      font = font.substr (0, position);
      fonts_already_defined.insert (font);
    }
  }
  // Get list of fonts for printing, only the good ones.
  // Take from the main project first, ... 
  vector<ustring> printingfonts = pfonts.printing_families;
  // ... and then from any additional projects.
  {
    vector<ustring> additional_projects = settings->session.additional_printing_projects;
    for (unsigned int i = 0; i < additional_projects.size(); i++) {
      PrintingFonts fonts (additional_projects[i]);
      for (unsigned int i2 = 0; i2 < fonts.printing_families.size(); i2++) {
        printingfonts.push_back (fonts.printing_families[i2]);
      }
    }
  }
  set<ustring> printingfonts_set (printingfonts.begin(), printingfonts.end());
  // Go through the template and insert font data.
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    outputlines.push_back (rt.lines[i]);
    if (rt.lines[i].find ("Bibledit") != string::npos) {
      // Insert all the fonts here, with their data.
      ustring previous_family;
      bool family_opened = false;
      // Go through each font.
      for (unsigned int i2 = 0; i2 < pfonts.xep_families.size(); i2++) {
        // Do not insert font already defined.
        if (fonts_already_defined.find (pfonts.xep_families[i2]) != fonts_already_defined.end())
          continue;
        // Do not insert font not set in editor.
        if (printingfonts_set.find (pfonts.xep_families[i2]) == printingfonts_set.end())
          continue;
        // If we find a new family, deal with that.
        if (pfonts.xep_families[i2] != previous_family) {
          // Close previous family, if there was any.
          if (family_opened)
            outputlines.push_back ("      </font-family>");
          outputlines.push_back ("      <font-family name=\"" + pfonts.xep_families[i2] + "\">");
          previous_family = pfonts.xep_families[i2];
          family_opened = true;
        }
        // See what type of font we have.
        bool pfa = g_str_has_suffix (pfonts.xep_paths[i2].c_str(), ".pfa");
        bool pfb = g_str_has_suffix (pfonts.xep_paths[i2].c_str(), ".pfb");
        bool ttf = g_str_has_suffix (pfonts.xep_paths[i2].c_str(), ".ttf");
        // Build the font line to insert.
        ustring fontline;
        fontline = "        <font";
        // Insert data for the variant.
        if ((pfonts.xep_variants[i2] == fvBold) || (pfonts.xep_variants[i2] == fvBoldItalic)) {
          fontline.append (" weight=\"bold\"");
        }
        if ((pfonts.xep_variants[i2] == fvItalic) || (pfonts.xep_variants[i2] == fvBoldItalic)) {
          fontline.append (" style=\"oblique\"");
        }
        fontline.append ("><font-data");
        if (pfa || pfb) {
           // Insert the metrics file.
           ustring afm_file = pfonts.xep_paths[i2];
           font_get_afm_file (afm_file);
           fontline.append (" afm=\"" + afm_file + "\"");
        }
        // Depending on the type, insert the data.
        if (pfa) {
          fontline.append (" pfa");
        }
        if (pfb) {
          fontline.append (" pfb");
        }
        if (ttf) {
          fontline.append (" ttf");
        }
        fontline.append ("=\"" + pfonts.xep_paths[i2] + "\"/></font>");
        outputlines.push_back (fontline); 
      }
      // Close last family if families were inserted.
      if (family_opened) outputlines.push_back ("      </font-family>");
    }
  }
  // Leave or remove option to print footnotes over the whole page.
  if (settings->genconfig.notes_print_full_page_width_get ()) {
    for (unsigned int i = 0; i < outputlines.size(); i++)
      if (outputlines[i].find ("PAGEWIDE_FOOTNOTES") != string::npos)
        outputlines[i].clear();
  }
  // Save file as xep.xml
  ustring filename = gw_build_filename (settings->genconfig.xep_home_get (), "xep.xml");
  write_lines (filename, outputlines);
}


bool xep_convert_from_fo_to_xep (const ustring& xslfo, const ustring& xep)
// Converts file "xslfo" to file "xep" in XEP intermediate format, and returns the result.
{
  xep_create_xep_xml ();
  unlink (xep.c_str ());
  GwSpawn spawn ((char *) modified_xep_command ().c_str());
  spawn.arg ("-valid");
  spawn.arg ("-fo");
  spawn.arg (xslfo);
  spawn.arg ("-out");
  spawn.arg (xep);
  spawn.arg ("-format");
  spawn.arg ("xep");
  spawn.progress ("Typesetting", true);
  spawn.run ();
  return !spawn.cancelled;
}


XepElastics::XepElastics (const ustring& filename)
// Calculates the length of elastic bands.
{
  // If there are no elastics, bail out.
  {
    bool found;
    gchar *contents;
    g_file_get_contents (filename.c_str(), &contents, NULL, NULL);
    found = (g_strrstr (contents, ELASTIC_XEP) != NULL);
    g_free (contents);
    if (!found) return;
  }

  // Load the text in memory.
  ReadText rt (filename, true, false);
  
  // Get the extends: the minimum and maximum y values on a page.
  retrieve_extends ();
  
  // Progress.
  ProgressWindow progresswindow ("Elastics", false);
  progresswindow.set_iterate (0, 1, rt.lines.size());
  
  // Go through the lines, get the page boundaries, and handle elastics if needed.
  unsigned int page_start = 0;
  bool elastic_in_page = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    progresswindow.iterate ();
    if (rt.lines[i].find (ELASTIC_XEP) != string::npos) {
      elastic_in_page = true;
    }
    if (rt.lines[i].find ("<xep:page ") != string::npos)
      page_start = i;
    if (rt.lines[i].find ("</xep:page>") != string::npos) {
      if (elastic_in_page) {
        pull_elastics (rt.lines, page_start, i);
      }
      elastic_in_page = false;
    }
  }    

  // Write the lines back to file.
  write_lines (filename, rt.lines);
}


XepElastics::~XepElastics ()
{
}


void XepElastics::retrieve_extends ()
// Formats a standard file, 
// and retrieves from it the maximum and minimum y value on a page. 
// From our A4 test, this was 774883 and 69283.
// But this depends on other factors as well.
{
  // Produce an xslfo file.
  vector <ustring> xslfo;
  {
    XmlFoRoot xmlforoot (&xslfo);
    {
      XmlFoLayoutMasterSet layoutmasterset (&xslfo, true);
    }
    XmlFoPageSequence pagesequence (&xslfo, true);
    {
      XmlFoStaticContent staticcontent (&xslfo, false, false);
    }
    {
      XmlFoFlow flow (&xslfo);
      for (unsigned int i = 0; i < 150; i++) {
        XmlFoBlock block (&xslfo, 12, 100, OFF, OFF, OFF, OFF, JUSTIFIED, 0, 0, 0, 0, 0, true, false);
        xslfo.push_back (ELASTIC_XEP);        
      }
    }
  }    
  
  // Save this file.
  ustring workingdirectory = directories_get_temp ();
  ustring fofilename = gw_build_filename (workingdirectory, "elastics.fo");
  write_lines (fofilename, xslfo);
  
  // Convert file to intermediate xep format.  
  ustring xepcommand (standard_xep_command ());
  ustring xepfilename = gw_build_filename (workingdirectory, "elastics.xep");
  GwSpawn spawn ((char *) xepcommand.c_str());
  spawn.arg ("-valid");
  spawn.arg ("-fo");
  spawn.arg (fofilename);
  spawn.arg ("-out");
  spawn.arg (xepfilename);
  spawn.arg ("-format");
  spawn.arg ("xep");
  spawn.progress ("Elastics", false);
  spawn.run ();

  // Load the xep file in memory.
  ReadText rt (xepfilename, true, false);
  
  // Find the y extends.
  bool maxfound = false;
  bool minfound = false;
  for (unsigned int i = 0; i < rt.lines.size(); i++) {
    if (!maxfound) {
      if (rt.lines[i].find (ELASTIC_XEP) != string::npos) {
        maximum_y = y_get (rt.lines[i]);
        maxfound = true;
      }
    }
    if (!minfound) {
      if (rt.lines[i].find (ELASTIC_XEP) != string::npos) {
        minimum_y = y_get (rt.lines[i]);
      }
    }
    if (rt.lines[i].find ("</xep:page>") != string::npos) {
      minfound = true;
    }
  }

  // Unlink files.
  unlink (fofilename.c_str());
  unlink (xepfilename.c_str());
}


unsigned int XepElastics::y_get (const ustring& line)
// Get the y value from a xep formatted line.
{
  // Example line:
  // <xep:text value="text" x="56693" y="774883" width="44880"/>
  unsigned int y = 0;
  size_t pos1 = line.find ("y=\"");
  pos1 += 3;
  size_t pos2 = line.find ("\"", pos1);
  y = convert_to_int (line.substr (pos1, pos2 - pos1));
  return y;
}


void XepElastics::y_set (ustring& line, unsigned int y)
// Set the y value in a xep formatted line.
{
  size_t pos1 = line.find ("y=\"");
  pos1 += 3;
  size_t pos2 = line.find ("\"", pos1);
  line.replace (pos1, pos2 - pos1, convert_to_string (y));
}


unsigned int XepElastics::y_from_get (const ustring& line)
// Get the y-from value from a xep formatted line.
{
  // Example line:
  // <xep:image src="file:image.jpeg" type="image/jpeg" x-from="268811" y-from="453359" scale-x="1.0" scale-y="1.0" width="86000" height="107000" role="none"/>
  unsigned int y = 0;
  size_t pos1 = line.find ("y-from=\"");
  pos1 += 8;
  size_t pos2 = line.find ("\"", pos1);
  y = convert_to_int (line.substr (pos1, pos2 - pos1));
  return y;
}


void XepElastics::y_from_set (ustring& line, unsigned int y)
// Set the y-from value in a xep formatted line.
{
  size_t pos1 = line.find ("y-from=\"");
  pos1 += 8;
  size_t pos2 = line.find ("\"", pos1);
  line.replace (pos1, pos2 - pos1, convert_to_string (y));
}


void XepElastics::pull_elastics (vector <ustring>& lines, unsigned int page_start, unsigned int page_end)
{
  // Get the heights of the elastics,
  // remove the visible elastic texts,
  // and get the y value of the last text item.
  vector <unsigned int> elastic_heights;
  unsigned int last_height = G_MAXINT;
  for (unsigned int i = page_start; i < page_end; i++) {
    if (lines[i].find (ELASTIC_XEP) != string::npos) {
      elastic_heights.push_back (y_get (lines[i]));
      lines[i].clear();
    }
    if (lines[i].find ("<xep:text ") != string::npos) {
      unsigned int y = y_get (lines[i]);
      if (y < last_height)
        last_height = y;
    }
  }
  
  // Calculate the height of the remaining white space, 
  // and divide it among the available elastics.
  // If there is no remaining white space, bail out.
  int remaining_white_space = last_height - minimum_y;
  if (remaining_white_space < 0) remaining_white_space = 0;
  if (remaining_white_space == 0) return;
  unsigned int elastic_length = remaining_white_space / elastic_heights.size();

  // Go through all the text lines and rewrite their heights.
  for (unsigned int i = page_start; i < page_end; i++) {
    bool has_xep_text = lines[i].find ("<xep:text ") != string::npos;
    bool has_xep_image = lines[i].find ("<xep:image ") != string::npos;
    if (has_xep_text || has_xep_image) {
      unsigned int old_y = 0;
      if (has_xep_text) old_y = y_get (lines[i]);
      if (has_xep_image) old_y = y_from_get (lines[i]);
      bool smaller_than_previous_height = false;
      for (unsigned int i2 = 0; i2 < elastic_heights.size(); i2++) {
        if (old_y < elastic_heights[i2])
          smaller_than_previous_height = true;
        if (smaller_than_previous_height) {
          bool rewrite_height = false;
          if (i2 == elastic_heights.size() - 1)
            rewrite_height = true;
          if (i2 < elastic_heights.size() - 1) {
            if (old_y > elastic_heights[i2 + 1])
              rewrite_height = true;
          }
          if (rewrite_height) {
            unsigned int new_y = old_y - ((i2 + 1) * elastic_length);
            if (has_xep_text) y_set (lines[i], new_y);
            if (has_xep_image) y_from_set (lines[i], new_y);
            smaller_than_previous_height = false;
          }
        }
      }
    }
  }
  
}


bool xep_convert_from_xep_to_pdf (const ustring& xep, const ustring& pdf)
// Converts file "xep" in XEP intermediate format to file "pdf".
{
  // Remove any previously created pdf file, lest it shows in case of errors,
  // suggesting that the conversion succeeded.
  unlink (pdf.c_str());

  // Take the actions.
  xep_create_xep_xml ();
  ustring xepcommand (modified_xep_command ());
  GwSpawn spawn ((char *) xepcommand.c_str());
  spawn.arg ("-valid");
  spawn.arg ("-xep");
  spawn.arg (xep);
  spawn.arg ("-out");
  spawn.arg (pdf);
  spawn.progress ("Writing document", true);
  spawn.run ();
  return !spawn.cancelled;
}


bool xep_find_installation_package (ustring& path)
{
  // See if the current path contains the package.
  ustring s (path);
  s = gw_path_get_basename (s);
  if (g_str_has_prefix (s.c_str(), "xep"))
    if (g_str_has_suffix (s.c_str(), ".zip"))
      return true;
  // Search for a package in the home directory.
  ReadFiles rf (g_get_home_dir (), "xep", ".zip");
  if (rf.files.size () > 0) {
    path = gw_build_filename (g_get_home_dir (), rf.files[0]);
    return true;
  }
  // Search for the package in the data directory.
  ReadFiles rf2 (directories_get_package_data (), "xep", ".zip");
  if (rf2.files.size () > 0) {
    path = gw_build_filename (directories_get_package_data (), rf2.files[0]);
    return true;
  }
  
  // Nothing found.
  return false;
}


bool xep_find_license (ustring& path)
{
  ustring right_name ("license.xml");
  // See if the current one is a license.
  ustring s = gw_path_get_basename (path);
  if (s == right_name)
    return true;
  // Search for the license in the home directory.
  s = gw_build_filename (g_get_home_dir (), right_name);
  if (g_file_test (s.c_str(), G_FILE_TEST_IS_REGULAR)) {
    path = s;
    return true;
  }
  // Search for the license in the data directory.
  s = gw_build_filename (directories_get_package_data (), right_name);
  if (g_file_test (s.c_str(), G_FILE_TEST_IS_REGULAR)) {
    path = s;
    return true;
  }
  // Sorry, nothing found.
  return false;
}


ustring xep_install (const ustring& java, const ustring& package, const ustring& license)
{
  // Installation folder.
  ustring installationfolder;
  installationfolder = gw_build_filename (directories_get_temp(), "xep");
  
  // Remove any old installation, so xep install does not ask to overwrite existing directory.
  unix_rmdir (installationfolder);

  // (Re)create working folder.
  // Also prevents unzip from asking to overwrite any existing files.
  ustring workingfolder = gw_build_filename (directories_get_temp (), "xepinstall");
  unix_rmdir (workingfolder);
  gw_mkdir_with_parents (workingfolder);
  
  // Unzip the file.
  unix_unzip (package, workingfolder);

  // Create installation policy.
  ustring installationpolicy;
  installationpolicy = "yes\n";
  installationpolicy.append (installationfolder);
  installationpolicy.append ("\nyes\n");
  installationpolicy.append (license);
  installationpolicy.append ("\nyes\n");

  // Get the name of the setup jar.
  ReadFiles rf (workingfolder, "setup", "jar");
  ustring setupjar;
  if (!rf.files.empty()) setupjar = rf.files[0];
    
  // Install.
  GwSpawn spawn ((char *)java.c_str());
  spawn.workingdirectory (workingfolder);
  spawn.arg ("-jar");
  spawn.arg (setupjar);
  spawn.arg ("-c");
  spawn.write (installationpolicy);
  spawn.progress ("Installing", false);
  spawn.run ();
  
  // Remove working folder.
  unix_rmdir (workingfolder);

  // Save and return new installation path.
  extern Settings * settings;
  settings->genconfig.xep_home_set (installationfolder);
  return installationfolder;
}


void xep_check ()
// If XEP has not been installed, and the installation sources are available,
// it will install XEP.
{
  if (!xep_present ()) {
    ustring package;
    if (xep_find_installation_package (package)) {
      ustring license;
      if (xep_find_license (license)) {
        ustring java;
        if (java_search (java)) {
          if (!java.empty()) {
            ustring s;
            if (java_is_sun (java, s)) {
              xep_install (java, package, license);
            }
          }
        }
      }
    }
  }
}


ustring xep_version ()
// Returns version of xep.
{
  GwSpawn spawn ((char *) standard_xep_command ().c_str());
  spawn.arg ("-version");
  spawn.read ();
  spawn.run ();
  ustring version;
  if (!spawn.standarderr.empty()) 
    version = spawn.standarderr[0];
  return version;
}


bool xep_convert_from_xslfo_to_pdf (const ustring& xslfo, const ustring& pdf, NoteCaller * footnotecaller, NoteCaller * crossreferencecaller)
// Converts file "xslfo" to file "pdf", and returns the result.
{
  // Remove any previously created pdf file, lest it would be shown in case of 
  // conversion errors.
  unlink (pdf.c_str());

  // Formatting is done in three stages:
  // 1. Convert from the xslfo file to the xep intermediate format.
  // 2. If need be, modify that intermediate file.
  // 3. Convert from the xep intermediate file to pdf output.  
  ustring xep_filename = xslfo + ".xep";

  // Cancel variable.
  bool cancel = false;
  
  // Start the first formatting process in the background.
  if (!xep_convert_from_fo_to_xep (xslfo, xep_filename))
    return false;
  if (cancel) 
    return true;
    
  // Do another interim stage: calculating the length of elastics.
  XepElastics xep_elastics (xep_filename);
  
  // Do the last stage of the conversion.
  xep_convert_from_xep_to_pdf (xep_filename, pdf);

  // Check the pdf file is there.
  if (!g_file_test (pdf.c_str(), G_FILE_TEST_IS_REGULAR))
    return false;
  
  // Everything's ok.
  return true;
}
