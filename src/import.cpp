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
#include "import.h"
#include "utilities.h"
#include "clean.h"
#include "usfmtools.h"
#include "bible.h"
#include "books.h"
#include "gwrappers.h"
#include "directories.h"
#include "unixwrappers.h"
#include "tiny_utilities.h"


ImportBookRead::ImportBookRead (const ustring& filename, const ustring& encoding)
{
  // Read the file.
  gchar *contents;
  gsize  length;
  GError *error = NULL;
  if (!g_file_get_contents (filename.c_str(), &contents, &length, &error)) {
    cerr << error->message << endl;
    g_error_free (error);
    return;
  }

  // Handle files created with Windows Notepad.
  contents = de_windows_notepad (contents);
  
  // Convert the data.
  if (!encoding.empty()) { 
    gchar * output;
    output = unicode_convert (contents, encoding);
    if (output) {
      g_free (contents);
      contents = output;
    }
  }

  // Check whether valid UTF8 data.
  if (!g_utf8_validate (contents, -1, NULL)) {
    cerr << "Data from " << filename << " is not valid UTF8." << endl;
    return;
  }

  // Divide text into separate lines.
  ParseLine parseline (contents);
  g_free (contents);
  rawlines = parseline.lines;
}


void ImportBookRead::usfm ()
{
  // Clean up following USFM standard.
  CleanUsfm cleanusfm (rawlines);
  lines.assign (cleanusfm.lines.begin(), cleanusfm.lines.end());
  
  // Get bookname.
  for (unsigned int i = 0; i < lines.size(); i++) {
    ustring line = lines[i];
    ustring marker = usfm_extract_marker (line);
    if (marker == "id") {
      ustring idp = line.substr (0, 3);
      unsigned int id = books_paratext_to_id (idp);
      bookname = books_id_to_english (id);
      break;
    }
  }
}


void ImportBookRead::bibleworks ()
{
  // If there's nothing to import, bail out.
  if (rawlines.empty())
    return;
  
  try 
  {
    
    // Get the name of the book.
    ustring bookabbreviation = rawlines[0].substr (0, 3);
    unsigned int id = books_bibleworks_to_id (bookabbreviation);
    if (id) 
      bookname = books_id_to_english (id);
    else
      return;
    
    // Store USFM id.
    ustring usfmid = books_id_to_paratext (id);
    lines.push_back ("\\id " + usfmid);
    
    // Convert the BibleWorks lines to USFM code.
    ustring previouschapter = "0";
    for (unsigned int i = 0; i < rawlines.size(); i++) {
      // Convert chapter information.
      ustring line = rawlines[i];
      line.erase (0, 4);
      ustring currentchapter = number_in_string (line);
      line.erase (0, currentchapter.length() + 1);
      if (currentchapter != previouschapter) {
        lines.push_back ("\\c " + currentchapter);
        lines.push_back ("\\p");
        previouschapter = currentchapter;
      }
      // Convert verse data.
      lines.push_back ("\\v " + line);
    }
  } 
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
}


void ImportBookRead::mechonmamre ()
{
  // If there's nothing to import, bail out.
  if (rawlines.empty())
    return;
  
  try 
  {
    
    // Get the name of the book.
    ustring bookname = rawlines[0];
    
    // Store USFM id.
    unsigned int id = books_english_to_id (bookname);
    ustring usfmid = books_id_to_paratext (id);
    lines.push_back ("\\id " + usfmid);
    
    // Go through each chapter of the Mechon Mamre pointers.
    for (unsigned int ch = 1; ch < rawlines.size(); ch++) {
      
      // Convert chapter number to USFM code.
      lines.push_back ("\\c " + convert_to_string (ch));
      lines.push_back ("\\p");
      
      // Read the text of the .htm file, and select only the .htm lines that 
      // contain the actual text.
      vector <ustring> htmlines;
      {
        ReadText rt (rawlines[ch], true, false);
        // Find lower boundary: the relevant parts start with a verse number.
        unsigned int lower_boundary = 0;
        for (unsigned int i = 0; i < rt.lines.size(); i++) {
          if (rt.lines[i].find ("A NAME=") != string::npos) {
            lower_boundary = i;
            break;
          }
        }
        // Find higher boundary: the relevant part ends with a verse number 
        // that is one past the last verse.
        unsigned int higher_boundary = 0;
        for (unsigned int i = rt.lines.size() - 1; i > 0; i--) {
          if (rt.lines[i].find ("A NAME=") != string::npos) {
            higher_boundary = i;
            break;
          }
        }
        for (unsigned int i = lower_boundary; i <= higher_boundary; i++) {
          htmlines.push_back (rt.lines[i]);
        }
      }
      
      // Go through the relevant text and extract the verses.
      ustring verse;
      for (unsigned int ln = 0; ln < htmlines.size(); ln++) {
        ustring s = number_in_string (htmlines[ln]);
        if (!s.empty()) {
          verse = "\\v " + s;
        } else {
          // A text line could be looking so (Psalms 1:1):
          // <P><B>א</B> אַ֥שְֽׁרֵי הָאִ֗ישׁ    אֲשֶׁ֤ר ׀ לֹ֥א הָלַךְ֮ בַּֽעֲצַ֪ת רְשָׁ֫עִ֥ים<BR>וּבְדֶ֣רֶךְ חַ֭טָּאִים לֹ֥א עָמָ֑ד    וּבְמוֹשַׁ֥ב לֵ֝צִ֗ים לֹ֣א יָשָֽׁב׃<BR>
          ustring line = htmlines[ln];
          // Convert <BR>, </P>, etc.
          replace_text (line, "<BR>", "\n\\nb\n");
          replace_text (line, "</P>", "\n\\p\n");
          replace_text (line, "<BIG>", "");
          replace_text (line, "</BIG>", "");
          replace_text (line, "{פ}", "");       
          replace_text (line, "{ס}", "");
          size_t position = line.find_last_of (">");
          line.erase (0, ++position);
          line = trim (line);
          lines.push_back (verse + " " + line);
        }
      }
      
    }
      
  } 
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
}


gchar * unicode_convert (gchar *data, const ustring& encoding)
{
  gchar *output;
  gssize length = strlen (data);
  GError *error = NULL;
  output = g_convert (data, length, "UTF-8", encoding.c_str(), NULL, NULL, &error);
  if (output == NULL) {
    cerr << error->message << endl;
    g_error_free (error);
  }
  return output;
}


ustring bibleworks_file_get_bookname (const ustring& filename)
// Retrieves the bookname from a file exported by BibleWorks. Though there can
// be several books in such a file, this functions retrieves only the name
// of the first book.
{
  #define MYMAX 10
  ReadText rt (filename, true, false);
  unsigned int maximum = MYMAX;
  maximum = CLAMP (maximum, 0, rt.lines.size());
  unsigned int goodline = 0;
  ustring bookname;
  try 
  {
    for (unsigned int i = 0; i < maximum; i++) {
      ustring bookabbreviation = rt.lines[i].substr (0, 3);
      unsigned int id = books_bibleworks_to_id (bookabbreviation);
      if (id) {
        goodline++;
        bookname = books_id_to_english (id);
      }
    }
  } 
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  if (goodline != MYMAX)
    bookname.clear();
  return bookname;
  #undef MYMAX
}


void bibleworks_save_book_internal (const ustring& directory, const ustring& inputfile,
                                    vector<ustring>& lines, vector<ustring>& files)
// This look for which book is in "lines", and then saves it in "directory", 
// giving it a name, and then adds the name to "files".
{
  if (lines.empty()) return;

  ustring bookname;
  ustring filename;
  {
    ustring bookabbreviation = lines[0].substr (0, 3);
    unsigned int id = books_bibleworks_to_id (bookabbreviation);
    if (!id) return;
    bookname = books_id_to_english (id);
    filename = gw_build_filename (directory, gw_path_get_basename (inputfile + "_" + bookname));
  }

  write_lines (filename, lines);
  lines.clear();
  
  files.push_back (filename);
}


vector<ustring> bibleworks_file_divide (const ustring& inputfile)
// This function takes one textfile exported by BibleWorks, and divides it into
// several files, each containing only one book.
// It puts them all in the temporal directory, 
// and returns the list of filenames so produced.
{
  // Clean temporal directory.
  ustring directory = gw_build_filename (directories_get_temp(), "bw-import");
  unix_rmdir (directory);
  gw_mkdir_with_parents (directory);
  
  // Storage for the divided files.
  vector<ustring> divided_files;

  // Read the inputfile and go though it.
  ReadText rt (inputfile, true, false);
  try
  {
    ustring previousbook;
    vector<ustring> booklines;
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      ustring currentbook = rt.lines[i].substr (0, 3);
      if (i == 0) previousbook = currentbook;
      if (currentbook != previousbook) {
        bibleworks_save_book_internal (directory, inputfile, booklines, divided_files);
        booklines.clear();
        previousbook = currentbook;
      }
      booklines.push_back (rt.lines[i]);
    }
    bibleworks_save_book_internal (directory, inputfile, booklines, divided_files);
  }
  catch (exception & ex)
  {
    gw_critical (ex.what ());
  }
  // Return result.
  return divided_files;
}


bool mechon_mamre_copyright (const ustring& inputfile)
// Returns true is a file has "Copyright" and "Mechon Mamre" in it.
{
  // Read the file.
  gchar *contents;
  gsize  length;
  GError *error = NULL;
  if (!g_file_get_contents (inputfile.c_str(), &contents, &length, &error)) {
    gw_critical (error->message);
    g_error_free (error);
    return false;
  }
  
  // Search for the signature.
  char * mechonmamre = g_strstr_len (contents, 400, "Mechon Mamre");
  if (mechonmamre) {
    mechonmamre = g_strstr_len (contents, 400, "Copyright");
  }
  
  // Free memory and return result.
  g_free (contents);
  if (mechonmamre)
    return true;
  else 
    return false;  
}


vector<ustring> mechon_mamre_produce_master_files (const vector<ustring>& inputfiles)
{
  // Clean temporal directory.
  ustring directory = gw_build_filename (directories_get_temp(), "mechon-mamre");
  unix_rmdir (directory);
  gw_mkdir_with_parents (directory);

  // Place to store the newly created masterfiles.
  vector <ustring> masterfiles;

  // Store a set with all available files.
  set<ustring> available_chapters (inputfiles.begin(), inputfiles.end());
  
  // Find the prefix and suffix of the files, and the base directory.
  ustring mechonmamre_prefix;
  ustring mechonmamre_suffix;
  ustring mechonmamre_directory;
  if (!inputfiles.empty()) {
    string s = gw_path_get_basename (inputfiles[0]);
    for (unsigned int i = 0; i < s.length(); i++) {
      if (g_ascii_isdigit (guchar (s[i]))) {
        break;
      } else {
        mechonmamre_prefix.append (s.substr (i, 1));
      }
    }
    for (unsigned int i = s.length() - 1; i > 0 ; i--) {
      if (g_ascii_isdigit (guchar (s[i]))) {
        break;
      } else {
        mechonmamre_suffix.append (s.substr (i, 1));
      }
    }
    mechonmamre_suffix = string_reverse (mechonmamre_suffix);
    mechonmamre_directory = gw_path_get_dirname (inputfiles[0]);    
  }

  // Attempt to produce all books.
  vector <unsigned int> ot_ids = books_type_to_ids (btOldTestament);
  for (unsigned int bk = 0; bk < ot_ids.size(); bk++) {
    
    // Assemble the booknumber as used in the Hebrew Bible.
    ustring mechonmamre_id;
    /*
    Contents in Original Hebrew Bible Order:

    Torah - Genesis, Exodus, Leviticus, Numbers, Deuteronomy

    Prophets - Joshua, Judges, Samuel, Kings, Isaiah, Jeremiah, Ezekiel,
    Hosea, Joel, Amos, Obadiah, Jonah, Micah, Nahum, Habakkuk, Zephaniah,
    Haggai, Zechariah, Malachi

    Writings - Chronicles, Psalms, Job, Proverbs, Ruth, Song of Songs,
    Ecclesiastes, Lamentations, Esther, Daniel, Ezra / Nehemiah
    */
    switch (ot_ids[bk]) {
      case 1 : mechonmamre_id = "01";  break; // Bereshit - Genesis - בראשית
      case 2 : mechonmamre_id = "02";  break; // Shemot - Exodus - שמות
      case 3 : mechonmamre_id = "03";  break; // VaYikra - Leviticus - ויקרא
      case 4 : mechonmamre_id = "04";  break; // BaMidbar - Numbers - במדבר
      case 5 : mechonmamre_id = "05";  break; // Devarim - Deuteronomy - דברים
      case 6 : mechonmamre_id = "06";  break; // Yehoshua - Joshua - יהושוע
      case 7 : mechonmamre_id = "07";  break; // Shoftim - Judges - שופטים
      case 8 : mechonmamre_id = "29";  break; // Ruth - Ruth - רות
      case 9 : mechonmamre_id = "08a"; break; // Shmuel A - 1 Samuel - שמואל א
      case 10: mechonmamre_id = "08b"; break; // Shmuel B - 2 Samuel - שמואל ב
      case 11: mechonmamre_id = "09a"; break; // Melachim A - 1 Kings - מלכים א
      case 12: mechonmamre_id = "09b"; break; // Melachim B - 2 Kings - מלכים ב
      case 13: mechonmamre_id = "25a"; break; // Divrei Yamim A - 1 Chronicles - דברי הימים א
      case 14: mechonmamre_id = "25b"; break; // Divrei Yamim B - 2 Chronicles - דברי הימים ב
      case 15: mechonmamre_id = "35a"; break; // Ezra - Ezra - עזרא
      case 16: mechonmamre_id = "35b"; break; // Nechemiyah - Nehemiah - נחמיה
      case 17: mechonmamre_id = "33";  break; // Esther - Esther - אסתר
      case 18: mechonmamre_id = "27";  break; // Eyov - Job - איוב
      case 19: mechonmamre_id = "26";  break; // Tehilim - Psalms - תהילים
      case 20: mechonmamre_id = "28";  break; // Mishlei - Proverbs - משלי
      case 21: mechonmamre_id = "31";  break; // Keholet - Ecclesiastes - קוהלת
      case 22: mechonmamre_id = "30";  break; // Shir HaShirim - Song of Songs - שיר השירים
      case 23: mechonmamre_id = "10";  break; // Yisheyah - Isaiah - ישעיהו
      case 24: mechonmamre_id = "11";  break; // Yermiyah - Jeremiah - ירמיהו
      case 25: mechonmamre_id = "32";  break; // Eichah - Lamentations - איכה
      case 26: mechonmamre_id = "12";  break; // Yechezchial - Ezekiel - יחזקאל
      case 27: mechonmamre_id = "34";  break; // Daniyel - Daniel - דנייאל
      case 28: mechonmamre_id = "13";  break; // Hoshea - Hosea - הושע
      case 29: mechonmamre_id = "14";  break; // Yoel - Joel - יואל
      case 30: mechonmamre_id = "15";  break; // Amos - Amos - עמוס
      case 31: mechonmamre_id = "16";  break; // Ovadiyah - Obadiah - עובדיה
      case 32: mechonmamre_id = "17";  break; // Yonah - Jonah - יונה
      case 33: mechonmamre_id = "18";  break; // Michah - Micah - מיכה
      case 34: mechonmamre_id = "19";  break; // Nachum - Nahum - נחום
      case 35: mechonmamre_id = "20";  break; // Chabakuk - Habakkuk - חבקוק
      case 36: mechonmamre_id = "21";  break; // Tzefaniyah - Zephaniah - צפניה
      case 37: mechonmamre_id = "22";  break; // Chaggi - Haggai - חגיי
      case 38: mechonmamre_id = "23";  break; // Zechariyah - Zechariah - זכריה
      case 39: mechonmamre_id = "24";  break; // Malachi - Malachi - מלאכי
    }
    
    // Attenmpt to find each chapter, and put it in the list of files for this book.
    vector <ustring> lines;
    lines.push_back (books_id_to_english (ot_ids[bk]));
    for (unsigned int ch = 1; ch <= 150; ch++) {
      ustring mechonmamre_chapter;
      {
        gchar * chapter = NULL;
        if (ch < 100) {
          chapter = g_strdup_printf ("%.2d", ch);
        } else {
          int i1 = ch / 10;
          int i2 = ch % 10;
          chapter = g_strdup_printf ("%x%d", i1, i2);
        }
        mechonmamre_chapter = chapter;
        g_free (chapter);
      }
      ustring inputfilename = gw_build_filename (mechonmamre_directory, mechonmamre_prefix + mechonmamre_id + mechonmamre_chapter + mechonmamre_suffix);
      bool found = available_chapters.find (inputfilename) != available_chapters.end();
      if (found)
        lines.push_back (inputfilename);
      else
        break;      
    }

    // Store the masterfile
    ustring outputfilename = gw_build_filename (directory, books_id_to_english (ot_ids[bk]));
    write_lines (outputfilename, lines);
    masterfiles.push_back (outputfilename);
    
  }
  
  // Return result.
  return masterfiles;
}


ustring import_type_human_readable (ImportType importtype)
// Makes the import type human readable.
{
  switch (importtype) {
    case itUsfm:        return "USFM";
    case itBibleWorks:  return "BibleWorks";
    case itMechonMamre: return "Mechon Mamre";
  }
  return "";
}
