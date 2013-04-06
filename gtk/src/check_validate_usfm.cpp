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


#include "check_validate_usfm.h"
#include "projectutils.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "tiny_utilities.h"


CheckValidateUsfm::CheckValidateUsfm(const ustring & project, const vector < unsigned int >&books, bool gui, bool checksheet)
/*
It performs checks related to the USFM standard.
project: project to check.
books: books to check; if empty it checks them all.
gui: whether to show graphical progressbar.
checksheet: check whether markers are in the stylesheet of the project.
*/
{
  // Init variables.
  cancelled = false;
  mychecksheet = checksheet;
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  // Get all styles in the attached stylesheet.
  vector <ustring> styless = stylesheet_get_markers(stylesheet_get_actual (), NULL);
  for (unsigned int i = 0; i < styless.size(); i++)
    styles.insert(styless[i]);
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Validating markers", true);
    progresswindow->set_iterate(0, 1, mybooks.size());
  }
  // Check each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate();
      progresswindow->set_text(books_id_to_english(book));
      if (progresswindow->cancel) {
        cancelled = true;
        return;
      }
    }
    book = mybooks[bk];
    // Check each chapter.
    vector <unsigned int> chapters = project_get_chapters(project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector <ustring> verses = project_get_verses(project, book, chapter);
      // Check each verse.
      for (unsigned int vs = 0; vs < verses.size(); vs++) {
        verse = verses[vs];
        ustring line = project_retrieve_verse(project, book, chapter, verse);
        // Check each line.
        ParseLine parseline(line);
        for (unsigned int ln = 0; ln < parseline.lines.size(); ln++) {
          check(parseline.lines[ln]);
        }
      }
    }
  }
}


CheckValidateUsfm::~CheckValidateUsfm()
{
  if (progresswindow)
    delete progresswindow;
}


void CheckValidateUsfm::check(const ustring & text)
// Do the actual check of one verse.
{
  // Extract the marker, and deal with it.
  ustring utext = trim(text);
  if (utext.empty())
    return;
  ustring line = utext;
  ustring marker = usfm_extract_marker_within_line(line);
  if (marker.empty()) {
    // No marker found.
    message("Line without USFM");
  }
  while (!marker.empty()) {

    if (marker == "id") {
      // Is the line long enough?
      if (line.length() < 3) {
        message("ID line is too short: " + line);
      } else {
        ustring id = line.substr(0, 3);
        unsigned int myid = books_paratext_to_id(id);
        if (myid == 0)
          message("Unknown id " + id);
        if (id != upperCase(id))
          message("Non-uppercase id code");
      }
    } else if (marker == "c") {
    } else if (marker == "v") {
      // Is there a space after the verse number?
      // Check it by extracting the text will the first space, and see if any unusual characters are in it.
      // It should cover things like \v 10a-11b or \v 10,11
      size_t position = line.find(" ");
      position = CLAMP(position, 0, line.length());
      ustring verse_number = line.substr (0, position);
      for (unsigned int i = 0; i < verse_number.length(); i++) {
        ustring character = verse_number.substr (i, 1);
        if (number_in_string (character) != character) {
          if ((character != "a") || (character != "b") || (character != "-") || (character != ",")) {
            message ("Unusual character in verse number " + verse_number);
            break;
          }
        }
      }
    } else if (marker == "add") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "bdit") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "bd") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "bk") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "b") {
    } else if (marker == "ca") {
    } else if (marker == "cd") {
    } else if (marker == "cls") {
    } else if (marker == "cl") {
    } else if (marker == "conc") {
    } else if (marker == "cov") {
    } else if (marker == "cp") {
    } else if (marker == "dc") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "d") {
    } else if (marker == "em") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "fdc") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fe") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "fig") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "fk") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fl") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fm") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "fp") {
    } else if (marker == "fqa") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fq") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fr") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "ft") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "fv") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "f") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "fb") { 
      // todo rogue fix - this check needs to be based on stylesheet, not hard coded.
      // A task was opened for this to be done.
      check_on_endmarker(line, marker, false);
    } else if (marker == "fx") { 
      // todo rogue fix - this check needs to be based on stylesheet, not hard coded.
      // A task was opened for this to be done.
      check_on_endmarker(line, marker, false);
    } else if (marker == "glo") {
    } else if (marker == "h1") {
    } else if (marker == "h2") {
    } else if (marker == "h3") {
    } else if (marker == "h") {
    } else if (marker == "ib") {
    } else if (marker == "ide") {
    } else if (marker == "idx") {
    } else if (marker == "ie") {
    } else if (marker == "iex") {
    } else if (marker == "imi") {
    } else if (marker == "imq") {
    } else if (marker == "imt1") {
    } else if (marker == "imt2") {
    } else if (marker == "imt3") {
    } else if (marker == "imt4") {
    } else if (marker == "imte") {
    } else if (marker == "imt") {
    } else if (marker == "im") {
    } else if (marker == "intro") {
    } else if (marker == "io1") {
    } else if (marker == "io2") {
    } else if (marker == "io3") {
    } else if (marker == "io4") {
    } else if (marker == "ior") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "iot") {
    } else if (marker == "io") {
    } else if (marker == "ipi") {
    } else if (marker == "ipq") {
    } else if (marker == "ipr") {
    } else if (marker == "ip") {
    } else if (marker == "iq1") {
    } else if (marker == "iq2") {
    } else if (marker == "iq3") {
    } else if (marker == "iq") {
    } else if (marker == "is1") {
    } else if (marker == "is2") {
    } else if (marker == "is") {
    } else if (marker == "it") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "k1") {
    } else if (marker == "k2") {
    } else if (marker == "k") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "li1") {
    } else if (marker == "li2") {
    } else if (marker == "li3") {
    } else if (marker == "li4") {
    } else if (marker == "lit") {
    } else if (marker == "li") {
    } else if (marker == "maps") {
    } else if (marker == "mi") {
    } else if (marker == "mr") {
    } else if (marker == "ms1") {
    } else if (marker == "ms2") {
    } else if (marker == "ms") {
    } else if (marker == "mt1") {
    } else if (marker == "mt2") {
    } else if (marker == "mt3") {
    } else if (marker == "mt4") {
    } else if (marker == "mte1") {
    } else if (marker == "mte2") {
    } else if (marker == "mte") {
    } else if (marker == "mt") {
    } else if (marker == "m") {
    } else if (marker == "nb") {
    } else if (marker == "nd") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "ndx") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "no") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "ord") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "p1") {
    } else if (marker == "p2") {
    } else if (marker == "pb") {
    } else if (marker == "pc") {
    } else if (marker == "pi1") {
    } else if (marker == "pi2") {
    } else if (marker == "pi3") {
    } else if (marker == "pi") {
    } else if (marker == "pmc") {
    } else if (marker == "pmo") {
    } else if (marker == "pmr") {
    } else if (marker == "pm") {
    } else if (marker == "pn") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "pref") {
    } else if (marker == "pro") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "pubinfo") {
    } else if (marker == "pub") {
    } else if (marker == "p") {
    } else if (marker == "q1") {
    } else if (marker == "q2") {
    } else if (marker == "q3") {
    } else if (marker == "qac") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "qa") {
    } else if (marker == "qc") {
    } else if (marker == "qm1") {
    } else if (marker == "qm2") {
    } else if (marker == "qm3") {
    } else if (marker == "qm") {
    } else if (marker == "qr") {
    } else if (marker == "qs") {
    } else if (marker == "qt") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "q") {
    } else if (marker == "rem") {
    } else if (marker == "restore") {
    } else if (marker == "r") {
    } else if (marker == "rq") {
    } else if (marker == "s1") {
    } else if (marker == "s2") {
    } else if (marker == "s3") {
    } else if (marker == "s4") {
    } else if (marker == "sc") {
      check_on_endmarker(line, marker, false);
      deprecated_marker(marker);
    } else if (marker == "sig") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "sls") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "spine") {
    } else if (marker == "sp") {
    } else if (marker == "sr") {
    } else if (marker == "s") {
    } else if (marker == "tc1") {
    } else if (marker == "tc2") {
    } else if (marker == "tc3") {
    } else if (marker == "tc4") {
    } else if (marker == "tcr1") {
    } else if (marker == "tcr2") {
    } else if (marker == "tcr3") {
    } else if (marker == "tcr4") {
    } else if (marker == "th1") {
    } else if (marker == "th2") {
    } else if (marker == "th3") {
    } else if (marker == "th4") {
    } else if (marker == "thr1") {
    } else if (marker == "thr2") {
    } else if (marker == "thr3") {
    } else if (marker == "thr4") {
    } else if (marker == "tl") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "toc1") {
    } else if (marker == "toc2") {
    } else if (marker == "toc3") {
    } else if (marker == "toc") {
    } else if (marker == "tr") {
    } else if (marker == "va") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "vp") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "wg") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "wh") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "wj") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "w") {
      check_on_endmarker(line, marker, false);
    } else if (marker == "xdc") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "xk") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "xo") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "xq") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "xt") {
      check_on_endmarker(line, marker, true);
    } else if (marker == "x") {
      check_on_endmarker(line, marker, false);
    } else if (marker.find("*") != string::npos) {
      message("Unmatched end marker " + marker);
    } else {
      message("Unknown USFM " + marker);
    }

    // Optionally check whether this marker is in the stylesheet.
    if (mychecksheet) {
      if (styles.find(marker) == styles.end()) {
        message("Marker " + marker + " not in stylesheet");
      }
    }
    // Extract any next marker in this line.    
    marker = usfm_extract_marker_within_line(line);
  }

  // Deal with accidentally entered forward slashes instead of backslashes.
  line = utext;
  marker = usfm_extract_marker_with_forwardslash(line);
  while (!marker.empty()) {
    ustring originalmarker(marker);
    size_t pos = marker.find("*");
    if (pos != string::npos) {
      marker.erase(pos, 1);
    }
    if ((marker == "id")
        || (marker == "c")
        || (marker == "v")
        || (marker == "add")
        || (marker == "bdit")
        || (marker == "bd")
        || (marker == "bk")
        || (marker == "b")
        || (marker == "ca")
        || (marker == "cd")
        || (marker == "cls")
        || (marker == "cl")
        || (marker == "conc")
        || (marker == "cov")
        || (marker == "cp")
        || (marker == "dc")
        || (marker == "d")
        || (marker == "em")
        || (marker == "fdc")
        || (marker == "fe")
        || (marker == "fig")
        || (marker == "fk")
        || (marker == "fl")
        || (marker == "fm")
        || (marker == "fp")
        || (marker == "fqa")
        || (marker == "fq")
        || (marker == "fr")
        || (marker == "ft")
        || (marker == "fv")
        || (marker == "f")
        || (marker == "glo")
        || (marker == "h1")
        || (marker == "h2")
        || (marker == "h3")
        || (marker == "h")
        || (marker == "ib")
        || (marker == "ide")
        || (marker == "idx")
        || (marker == "ie")
        || (marker == "iex")
        || (marker == "imi")
        || (marker == "imq")
        || (marker == "imt1")
        || (marker == "imt2")
        || (marker == "imt3")
        || (marker == "imt4")
        || (marker == "imte")
        || (marker == "imt")
        || (marker == "im")
        || (marker == "intro")
        || (marker == "io1")
        || (marker == "io2")
        || (marker == "io3")
        || (marker == "io4")
        || (marker == "ior")
        || (marker == "iot")
        || (marker == "io")
        || (marker == "ipi")
        || (marker == "ipq")
        || (marker == "ipr")
        || (marker == "ip")
        || (marker == "iq1")
        || (marker == "iq2")
        || (marker == "iq3")
        || (marker == "iq")
        || (marker == "is1")
        || (marker == "is2")
        || (marker == "is")
        || (marker == "it")
        || (marker == "k1")
        || (marker == "k2")
        || (marker == "k")
        || (marker == "li1")
        || (marker == "li2")
        || (marker == "li3")
        || (marker == "li4")
        || (marker == "lit")
        || (marker == "li")
        || (marker == "maps")
        || (marker == "mi")
        || (marker == "mr")
        || (marker == "ms1")
        || (marker == "ms2")
        || (marker == "ms")
        || (marker == "mt1")
        || (marker == "mt2")
        || (marker == "mt3")
        || (marker == "mt4")
        || (marker == "mte1")
        || (marker == "mte2")
        || (marker == "mte")
        || (marker == "mt")
        || (marker == "m")
        || (marker == "nb")
        || (marker == "nd")
        || (marker == "ndx")
        || (marker == "no")
        || (marker == "ord")
        || (marker == "p1")
        || (marker == "p2")
        || (marker == "pb")
        || (marker == "pc")
        || (marker == "pi1")
        || (marker == "pi2")
        || (marker == "pi3")
        || (marker == "pi")
        || (marker == "pmc")
        || (marker == "pmo")
        || (marker == "pmr")
        || (marker == "pm")
        || (marker == "pn")
        || (marker == "pref")
        || (marker == "pro")
        || (marker == "pubinfo")
        || (marker == "pub")
        || (marker == "p")
        || (marker == "q1")
        || (marker == "q2")
        || (marker == "q3")
        || (marker == "qac")
        || (marker == "qa")
        || (marker == "qc")
        || (marker == "qm1")
        || (marker == "qm2")
        || (marker == "qm3")
        || (marker == "qm")
        || (marker == "qr")
        || (marker == "qs")
        || (marker == "qt")
        || (marker == "q")
        || (marker == "rem")
        || (marker == "restore")
        || (marker == "r")
        || (marker == "rq")
        || (marker == "s1")
        || (marker == "s2")
        || (marker == "s3")
        || (marker == "s4")
        || (marker == "sc")
        || (marker == "sig")
        || (marker == "sls")
        || (marker == "spine")
        || (marker == "sp")
        || (marker == "sr")
        || (marker == "s")
        || (marker == "tc1")
        || (marker == "tc2")
        || (marker == "tc3")
        || (marker == "tc4")
        || (marker == "tcr1")
        || (marker == "tcr2")
        || (marker == "tcr3")
        || (marker == "tcr4")
        || (marker == "th1")
        || (marker == "th2")
        || (marker == "th3")
        || (marker == "th4")
        || (marker == "thr1")
        || (marker == "thr2")
        || (marker == "thr3")
        || (marker == "thr4")
        || (marker == "tl")
        || (marker == "toc1")
        || (marker == "toc2")
        || (marker == "toc3")
        || (marker == "toc")
        || (marker == "tr")
        || (marker == "va")
        || (marker == "vp")
        || (marker == "wg")
        || (marker == "wh")
        || (marker == "wj")
        || (marker == "w")
        || (marker == "xdc")
        || (marker == "xk")
        || (marker == "xo")
        || (marker == "xq")
        || (marker == "xt")
        || (marker == "x")
        ) {
      message("Normal slash for /" + originalmarker);
    }
    // Extract any next marker in this line.    
    marker = usfm_extract_marker_with_forwardslash(line);
  }

  // Check for widow backslashes.
  line = utext + " ";
  if ((line.find("\\ ") != string::npos) || (line == "\\")) {
    message("Widow backslash");
  }
}


void CheckValidateUsfm::check_on_endmarker(ustring & line, const ustring & marker, bool optional)
// This test is ran by any marker that needs an endmarker.
// It checks on that, and if found, removes it from the line,
// and if not found, gives a message.
// If the endmarker is "optional" no message will be given.
{
  // Look for the endmarker.
  ustring endmarker;
  endmarker = "\\" + marker + "*";
  size_t endmarkerposition;
  endmarkerposition = line.find(endmarker);
  if (endmarkerposition != string::npos) {
    // Found: remove it from the line.
    line.erase(endmarkerposition, endmarker.length());
  } else {
    // Not found: Error message.
    // No error message if the endmarker is optional.
    if (!optional) {
      message("Endmarker " + endmarker + " not found");
    }
  }
}


void CheckValidateUsfm::deprecated_marker(const ustring & marker)
{
  message("Deprecated marker " + marker);
}


ustring CheckValidateUsfm::usfm_extract_marker_with_forwardslash(ustring & line)
// Returns the usfm marker from the line, but only if it starts with a forward slash
{
  ustring returnvalue;
  line = trim(line);
  size_t offposition;
  offposition = line.find("/");
  if (offposition != string::npos) {
    line.erase(0, offposition);
    size_t endposition;
    endposition = line.find_first_of(" *", 1);
    if (endposition != string::npos) {
      returnvalue = line.substr(0, ++endposition);
      line.erase(0, endposition);
    } else {
      returnvalue = line;
      line.clear();
    }
  }
  if (returnvalue.length() > 0)
    returnvalue.erase(0, 1);    // Remove slash.
  return trim(returnvalue);
}


void CheckValidateUsfm::message(const ustring & message)
{
  references.push_back(books_id_to_english(book) + " " + convert_to_string(chapter) + ":" + verse);
  comments.push_back(message);
}

