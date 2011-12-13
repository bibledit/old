/*
** Copyright (©) 2003-2012 Teus Benschop.
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


#include "mechonmamre.h"
#include <gtk/gtk.h>
#include "gwrappers.h"
#include "progresswindow.h"
#include <libxml/xmlreader.h>
#include "utilities.h"
#include "books.h"
#include "tiny_utilities.h"
#include "directories.h"
#include "unixwrappers.h"
#include <sqlite3.h>
#include "sqlite_reader.h"
#include "compress.h"
#include "projectutils.h"
#include "settings.h"


ustring mechon_mamre_html_entry_url ()
{
  return "mechon_mamre_entry";
}


ustring mechon_mamre_download_url ()
{
  return "mechon_mamre_download";
}


ustring mechon_mamre_import_url ()
{
  return "mechon_mamre_import";
}


void mechon_mamre_home_entry (HtmlWriter2& htmlwriter)
{
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add (mechon_mamre_html_entry_url (), "Hebrew Bible with cantillation marks from Mechon Mamre");
  htmlwriter.paragraph_close ();
}


void mechon_mamre_detailed_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("Hebrew Bible with cantillation marks from Mechon Mamre");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("The Hebrew Bible from Mechon Mamre provides a Hebrew text that can be useful for translating the Bible. Bibledit-Gtk can import this text. This consists of two steps: downloading it and importing it.");
  htmlwriter.paragraph_close();
  htmlwriter.heading_open (4);
  htmlwriter.text_add ("1. Downloading the text");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("When you click on the link below, a web browser will open, which will download the Hebrew text. The file will be called ct005.zip. Please save this file in the home directory or on your Desktop.");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (mechon_mamre_download_url (), "Download it");
  htmlwriter.paragraph_close();
  htmlwriter.heading_open (4);
  htmlwriter.text_add ("2. Importing the text");
  htmlwriter.heading_close ();
  htmlwriter.paragraph_open();
  htmlwriter.text_add ("When you click on the link below, a new Bible will be created, and the file that you have downloaded will be imported into it.");
  htmlwriter.paragraph_close();
  htmlwriter.paragraph_open();
  htmlwriter.hyperlink_add (mechon_mamre_import_url (), "Import it");
  htmlwriter.paragraph_close();
}


void mechon_mamre_action_page (HtmlWriter2& htmlwriter)
{
  htmlwriter.heading_open (3);
  htmlwriter.text_add ("Hebrew import from Mechon Mamre");
  htmlwriter.heading_close ();

  vector <ustring> messages;
  bool keep_going = true;

  // Locate the downloaded file.
  ustring ct005zipfilename = gw_build_filename (g_get_home_dir (), "ct005.zip");
  messages.push_back ("Looking for file " + ct005zipfilename);
  if (!g_file_test (ct005zipfilename.c_str(), G_FILE_TEST_IS_REGULAR)) {
    ct005zipfilename.clear();
  }
  if (ct005zipfilename.empty()) {
    ct005zipfilename = gw_build_filename (g_get_home_dir (), "Desktop", "ct005.zip");
    messages.push_back ("Looking for file " + ct005zipfilename);
    if (!g_file_test (ct005zipfilename.c_str(), G_FILE_TEST_IS_REGULAR)) {
      ct005zipfilename.clear();
    }
  }
  if (ct005zipfilename.empty()) {
    messages.push_back ("Can't find Hebrew input file");
    keep_going = false;
  }
  if (keep_going) {
    messages.push_back ("Using file " + ct005zipfilename);
  }

  // Unpack the zipped file.
  ustring directory;
  if (keep_going) {
    directory = gw_build_filename (directories_get_temp (), "uncompress");
    unix_rmdir (directory);
    gw_mkdir_with_parents (directory);
    if (!uncompress (ct005zipfilename, directory)) {
      messages.push_back ("Could not unpack the file");
      keep_going = false;
    }
    messages.push_back ("Unpacking into folder " + directory);
  }

  // Show the readme file.
  if (keep_going) {
    ustring readmefile = gw_build_filename (directory, "readme.txt");
    ReadText rt (readmefile, true, true);
    ustring line;
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      if (rt.lines[i].empty()) {
        if (!line.empty()) {
          messages.push_back (line);
          line.clear();
        }
      } else {
        line.append (rt.lines[i] + " ");
      }
    }
    messages.push_back (line);
  }

  // Look for the directory where all the html files reside.
  if (keep_going) {
    directory = gw_build_filename (directory, "c", "ct");
    if (!g_file_test (directory.c_str(), G_FILE_TEST_IS_DIR)) {
      messages.push_back ("Can't find data in directory " + directory);
    }
    messages.push_back ("Looking for data in directory " + directory);
  }
  
  // Get a list of the html files that have the data.
  vector <ustring> files;
  if (keep_going) {
    ReadFiles rf (directory, "c", ".htm");
    for (unsigned int i = 0; i < rf.files.size(); i++) {
      ustring filename = gw_build_filename (directory, rf.files[i]);
      // Check on a few charasteristics.
      if (mechon_mamre_copyright(filename)) {
        unsigned int digitcount = digit_count_in_string(rf.files[i]);
        if ((digitcount == 3) || (digitcount == 4)) {
          files.push_back(filename);
        }
      }
    }
  }

  // Create a new Bible into which to import the data.
  ustring bible = "Hebrew Mechon Mamre";
  if (keep_going) {
    if (project_exists (bible)) {
      messages.push_back ("A Bible called \"" + bible + "\" already exists");
      keep_going = false;      
    } else {
      project_create_restore (bible, "");
      messages.push_back ("Creating a new Bible called \"" + bible + "\"");
      // Make a couple of settings.
      extern Settings * settings;
      ProjectConfiguration * projectconfig = settings->projectconfig (bible);
      projectconfig->versification_set ("Original");
      projectconfig->editable_set (false);
      projectconfig->right_to_left_set (true);
      projectconfig->spelling_check_set (false);
    }
  }

  // Store all the chapters 0 in each book.
  if (keep_going) {
    vector <unsigned int> books = books_type_to_ids(btOldTestament);
    ProgressWindow progresswindow ("Creating books", false);
    progresswindow.set_iterate (0, 1, books.size());
    for (unsigned int bk = 0; bk < books.size(); bk++) {
      progresswindow.iterate ();
      vector <ustring> usfm;
      usfm.push_back ("\\id " + books_id_to_paratext (books[bk]));
      CategorizeChapterVerse ccv (usfm);      
      project_store_chapter (bible, books[bk], ccv);
    }  
  }

  // Store all the chapters.
  if (keep_going) {
    ProgressWindow progresswindow ("Importing chapters", false);
    progresswindow.set_iterate (0, 1, files.size());
    for (unsigned int i = 0; i < files.size(); i++) {
      progresswindow.iterate ();
      unsigned int book = 0;
      unsigned int chapter = 0;
      mechon_mamre_extract_book_chapter (files[i], book, chapter);
      vector <ustring> contents = mechon_mamre_extract_contents (files[i], chapter);
      CategorizeChapterVerse ccv (contents);
      project_store_chapter (bible, book, ccv);
      messages.push_back ("Importing " + books_id_to_english (book) + " " + convert_to_string (chapter) + " from file " + files[i]);
    }
  }

  // Write accumulated messages.
  htmlwriter.heading_open (3);
  if (keep_going) {
    htmlwriter.text_add ("Success! Bible \"" + bible + "\" was created");
  } else {
    htmlwriter.text_add ("Error!");
  }
  htmlwriter.heading_close ();
  if (keep_going) {
    htmlwriter.paragraph_open ();
    htmlwriter.text_add ("To view the Hebrew text, open the Bible in the editor. Optionally set the font for better display of the Hebrew text. A donation made to Mechon Mamre will support their work.");
    htmlwriter.paragraph_close ();
  }
  for (unsigned int i = 0; i < messages.size(); i++) {
    htmlwriter.paragraph_open ();
    htmlwriter.text_add (messages[i]);
    htmlwriter.paragraph_close ();
  }  
  
  // Write OK.
  htmlwriter.paragraph_open ();
  htmlwriter.hyperlink_add ("", "Ok");
  htmlwriter.paragraph_close ();
}


bool mechon_mamre_copyright(const ustring & inputfile)
// Returns true is a file has "Copyright" and "Mechon Mamre" in it.
{
  // Read the file.
  gchar *contents;
  gsize length;
  GError *error = NULL;
  if (!g_file_get_contents(inputfile.c_str(), &contents, &length, &error)) {
    gw_critical(error->message);
    g_error_free(error);
    return false;
  }
  // Search for the signature.
  char *mechonmamre = g_strstr_len(contents, 400, "Mechon Mamre");
  if (mechonmamre) {
    mechonmamre = g_strstr_len(contents, 400, "Copyright");
  }
  // Free memory and return result.
  g_free(contents);
  if (mechonmamre)
    return true;
  else
    return false;
}


void mechon_mamre_extract_book_chapter (const ustring& file, unsigned int& book, unsigned int& chapter)
// This gets a file passed, and returns its book and its chapter.
{
  // Retrieve the base name, e.g. "c2702.htm".
  ustring book_chapter = gw_path_get_basename(file);
  
  // Remove the first character "c".
  book_chapter.erase (0, 1);
  
  // Remove the dot and what comes after.
  size_t pos = book_chapter.find (".");
  book_chapter.erase (pos, 100);
  
  // Retrieve the chapter number out of the filename.
  ustring chapter_bit = book_chapter.substr (book_chapter.length() - 2, 2);
  book_chapter.erase (book_chapter.length() - 2, 2);
  for (unsigned int ch = 1; ch <= 150; ch++) {
    ustring mechonmamre_chapter;
    {
      gchar *chapter = NULL;
      if (ch < 100) {
        chapter = g_strdup_printf("%.2d", ch);
      } else {
        int i1 = ch / 10;
        int i2 = ch % 10;
        chapter = g_strdup_printf("%x%d", i1, i2);
      }
      mechonmamre_chapter = chapter;
      g_free(chapter);
    }
    if (mechonmamre_chapter == chapter_bit) {
      chapter = ch;
      break;
    }
  }

  // Retrieve the book name from the filename.
  vector < unsigned int >ot_ids = books_type_to_ids(btOldTestament);
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
    case 1:
      mechonmamre_id = "01";
      break;                    // Bereshit - Genesis - בראשית
    case 2:
      mechonmamre_id = "02";
      break;                    // Shemot - Exodus - שמות
    case 3:
      mechonmamre_id = "03";
      break;                    // VaYikra - Leviticus - ויקרא
    case 4:
      mechonmamre_id = "04";
      break;                    // BaMidbar - Numbers - במדבר
    case 5:
      mechonmamre_id = "05";
      break;                    // Devarim - Deuteronomy - דברים
    case 6:
      mechonmamre_id = "06";
      break;                    // Yehoshua - Joshua - יהושוע
    case 7:
      mechonmamre_id = "07";
      break;                    // Shoftim - Judges - שופטים
    case 8:
      mechonmamre_id = "29";
      break;                    // Ruth - Ruth - רות
    case 9:
      mechonmamre_id = "08a";
      break;                    // Shmuel A - 1 Samuel - שמואל א
    case 10:
      mechonmamre_id = "08b";
      break;                    // Shmuel B - 2 Samuel - שמואל ב
    case 11:
      mechonmamre_id = "09a";
      break;                    // Melachim A - 1 Kings - מלכים א
    case 12:
      mechonmamre_id = "09b";
      break;                    // Melachim B - 2 Kings - מלכים ב
    case 13:
      mechonmamre_id = "25a";
      break;                    // Divrei Yamim A - 1 Chronicles - דברי הימים א
    case 14:
      mechonmamre_id = "25b";
      break;                    // Divrei Yamim B - 2 Chronicles - דברי הימים ב
    case 15:
      mechonmamre_id = "35a";
      break;                    // Ezra - Ezra - עזרא
    case 16:
      mechonmamre_id = "35b";
      break;                    // Nechemiyah - Nehemiah - נחמיה
    case 17:
      mechonmamre_id = "33";
      break;                    // Esther - Esther - אסתר
    case 18:
      mechonmamre_id = "27";
      break;                    // Eyov - Job - איוב
    case 19:
      mechonmamre_id = "26";
      break;                    // Tehilim - Psalms - תהילים
    case 20:
      mechonmamre_id = "28";
      break;                    // Mishlei - Proverbs - משלי
    case 21:
      mechonmamre_id = "31";
      break;                    // Keholet - Ecclesiastes - קוהלת
    case 22:
      mechonmamre_id = "30";
      break;                    // Shir HaShirim - Song of Songs - שיר השירים
    case 23:
      mechonmamre_id = "10";
      break;                    // Yisheyah - Isaiah - ישעיהו
    case 24:
      mechonmamre_id = "11";
      break;                    // Yermiyah - Jeremiah - ירמיהו
    case 25:
      mechonmamre_id = "32";
      break;                    // Eichah - Lamentations - איכה
    case 26:
      mechonmamre_id = "12";
      break;                    // Yechezchial - Ezekiel - יחזקאל
    case 27:
      mechonmamre_id = "34";
      break;                    // Daniyel - Daniel - דנייאל
    case 28:
      mechonmamre_id = "13";
      break;                    // Hoshea - Hosea - הושע
    case 29:
      mechonmamre_id = "14";
      break;                    // Yoel - Joel - יואל
    case 30:
      mechonmamre_id = "15";
      break;                    // Amos - Amos - עמוס
    case 31:
      mechonmamre_id = "16";
      break;                    // Ovadiyah - Obadiah - עובדיה
    case 32:
      mechonmamre_id = "17";
      break;                    // Yonah - Jonah - יונה
    case 33:
      mechonmamre_id = "18";
      break;                    // Michah - Micah - מיכה
    case 34:
      mechonmamre_id = "19";
      break;                    // Nachum - Nahum - נחום
    case 35:
      mechonmamre_id = "20";
      break;                    // Chabakuk - Habakkuk - חבקוק
    case 36:
      mechonmamre_id = "21";
      break;                    // Tzefaniyah - Zephaniah - צפניה
    case 37:
      mechonmamre_id = "22";
      break;                    // Chaggi - Haggai - חגיי
    case 38:
      mechonmamre_id = "23";
      break;                    // Zechariyah - Zechariah - זכריה
    case 39:
      mechonmamre_id = "24";
      break;                    // Malachi - Malachi - מלאכי
    }
    if (mechonmamre_id == book_chapter) {
      book = ot_ids[bk];
      break;
    }
  }
}


vector <ustring> mechon_mamre_extract_contents (const ustring& file, unsigned int chapter)
{
  // Usfm data.
  vector <ustring> lines;
  
  // Convert chapter number to USFM code.
  lines.push_back("\\c " + convert_to_string(chapter));
  lines.push_back("\\p");
  
  // Read the text of the .htm file, and select only the .htm lines that 
  // contain the actual text.
  vector < ustring > htmlines;
  {
    ReadText rt (file, true, false);
    // Find lower boundary: the relevant parts start with a verse number.
    unsigned int lower_boundary = 0;
    for (unsigned int i = 0; i < rt.lines.size(); i++) {
      if (rt.lines[i].find("A NAME=") != string::npos) {
        lower_boundary = i;
        break;
      }
    }
    // Find higher boundary: the relevant part ends with a verse number 
    // that is one past the last verse.
    unsigned int higher_boundary = 0;
    for (unsigned int i = rt.lines.size() - 1; i > 0; i--) {
      if (rt.lines[i].find("A NAME=") != string::npos) {
        higher_boundary = i;
        break;
      }
    }
    for (unsigned int i = lower_boundary; i <= higher_boundary; i++) {
      htmlines.push_back(rt.lines[i]);
    }
  }
  
  // Go through the relevant text and extract the verses.
  ustring verse;
  for (unsigned int ln = 0; ln < htmlines.size(); ln++) {
    ustring s = number_in_string(htmlines[ln]);
    if (!s.empty()) {
      verse = "\\v " + s;
    } else {
      // A text line could be looking so (Psalms 1:1):
      // <P><B>א</B> אַ֥שְֽׁרֵי הָאִ֗ישׁ    אֲשֶׁ֤ר ׀ לֹ֥א הָלַךְ֮ בַּֽעֲצַ֪ת רְשָׁ֫עִ֥ים<BR>וּבְדֶ֣רֶךְ חַ֭טָּאִים לֹ֥א עָמָ֑ד    וּבְמוֹשַׁ֥ב לֵ֝צִ֗ים לֹ֣א יָשָֽׁב׃<BR>
      ustring line = htmlines[ln];
      // Convert <BR>, </P>, etc.
      replace_text(line, "<BR>", "\n\\nb\n");
      replace_text(line, "</P>", "\n\\p\n");
      replace_text(line, "<BIG>", "");
      replace_text(line, "</BIG>", "");
      replace_text(line, "{פ}", "");
      replace_text(line, "{ס}", "");
      size_t position = line.find_last_of(">");
      line.erase(0, ++position);
      line = trim(line);
      lines.push_back(verse + " " + line);
    }
  }
  
  // Return the data.
  return lines;  
}

