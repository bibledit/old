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


#include "check_sentence_structure.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "settings.h"
#include "stylesheetutils.h"
#include "utilities.h"
#include "usfmtools.h"
#include "books.h"
#include "tiny_utilities.h"


CheckSentenceStructure::CheckSentenceStructure (const ustring& project,
                                                const vector<unsigned int>& books, 
                                                bool gui)
/*
It performs checks of the sentence structure.
project: project to check.
books: books to check; if empty it checks them all.
gui: whether to show graphical progressbar.
*/
{
  // Init variables.
  cancelled = false;
  // Classify the styles.
  classify_styles (project);
  // Get a list of the books to check. If no books were given, take them all.
  vector<unsigned int> mybooks (books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books (project);
  // GUI.
  ProgressWindow * progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow ("Checking sentence structure", true);
    progresswindow->set_iterate (0, 1, mybooks.size());
  }
  // Check each book.
  for (unsigned int bk = 0; bk < mybooks.size(); bk++) {
    if (gui) {
      progresswindow->iterate ();
      if (progresswindow->cancel) {
        cancelled = true;
        break;
      }
    }
    book = mybooks[bk];
    // Check each chapter.
    vector <unsigned int> chapters = project_get_chapters (project, book);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      chapter = chapters[ch];
      vector <ustring> lines = project_retrieve_chapter (project, book, chapter);
      // Assemble the line with all data in it.
      ustring line;
      for (unsigned int i = 0; i < lines.size (); i++) {
        line.append (lines[i]);
        line.append (" ");
      }
    }
  }
  // Clean up.
  if (progresswindow) delete progresswindow;
}


CheckSentenceStructure::~CheckSentenceStructure ()
{
}


void CheckSentenceStructure::check (ustring text)
// Do the actual check of one verse.
{
  /*
  // Extract the marker, and deal with it.
  text = trim (text);
  if (text.empty()) return;
  ustring marker = usfm_extract_marker_within_line (text);
  while (!marker.empty()) {
    if (markers.find (marker) != markers.end()) {
      size_t pos = text.find ("\\");
      if (pos != string::npos) {
        if (text.substr (--pos, 1) == " ") {
          if (pos > 1) {
            if (text.substr (--pos, 1) == " ") {
             message ("Text of marker " + marker + " ends with more than one space");              
            }
          }
        } else {
          message ("Text of marker " + marker + " doesn't end with a space");              
        }
      }
    }
    // Extract any next marker in this text.
    marker = usfm_extract_marker_within_line (text);
  }
  */
}



void CheckSentenceStructure::classify_styles (const ustring& project) // Todo
// Classifies the styles of the stylesheet of the project.
{
  extern Settings * settings;
  ProjectConfiguration * projectconfig = settings->projectconfig (project, false);
  ustring stylesheet = projectconfig->stylesheet_get ();
  Usfm usfm (stylesheet);
  for (unsigned int i = 0; i < usfm.styles.size (); i++) {
    switch (usfm.styles[i].type) {
      case stIdentifier:
      {
        break;
      }
      case stNotUsedComment:
      {
        break;
      }
      case stNotUsedRunningHeader:
      {
        break;
      }
      case stStartsParagraph:
      {
        break;
      }
      case stInlineText:
      {
        break;
      }
      case stChapterNumber:
      {
        break;
      }
      case stVerseNumber:
      {
        break;
      }
      case stFootEndNote:
      {
        break;
      }
      case stCrossreference:
      {
        break;
      }
      case stPeripheral:
      {
        break;
      }
      case stPicture:
      {
        break;
      }
      case stPageBreak:
      {
        break;
      }
      case stTableElement:
      {
        break;
      }
      case stWordlistElement:
      {
        break;
      }
    }
  }
}


void CheckSentenceStructure::message (const ustring& verse, const ustring& message)
{
  references.push_back (books_id_to_english (book) + " " + convert_to_string (chapter) + ":" + verse);
  comments.push_back (message);
}


SentenceStructureBlock::SentenceStructureBlock (int dummy)
{
  textbuffer = gtk_text_buffer_new (NULL);
}


SentenceStructureBlock::~SentenceStructureBlock ()
{
  g_object_unref (textbuffer);
}

/*

Todo Checking the sentence structure.


Input parameters:
- A list of punctuation characters that ends a sentence. Pango makes this up.
- A list of punctuation characters that are irrelevant to the sentence structure.
  We can have glib make this list up itself: If the characters is punctuation,
  and it is not in the list of characters that ends the sentence, then this
  character is irrelevant to the sentence structure.
- A list of capitals. Glib's Unicode processing can make these up.
- A list of usfm codes that starts a heading.
- A list of usfm codes that start a normal paragraph.

Steps:

- To load the text in a textbuffer, and have an iterator move back and forth in it. Checks around the iterator.
- While loading the text in the buffer, cut out the markers.
- Put the notes in separate buffers.
- Put the table cells in separate buffers.
- Each buffer has an associated paragraph style and character style and verse number for each offset.
  Or faster, it stores the offsets where verses start.
- It works per chapter.








// No checking of sentence structure when an indifferent character is
found.
if not (Curr in SentenceIndifference) then
begin
if (Curr in SentenceEnders) and (not SentenceHasStarted) then
begin
AddResultMessage (USFM, USFM.PreviousChars + ' - A character ' + Curr
+ ' ends the sentence which hasn''t been started yet.');
end;
if (not (Curr in Capitals)) and (Curr <> ' ') and (not
SentenceHasStarted) then
begin
AddResultMessage (USFM, USFM.PreviousChars + ' - A sentence should
not start with ' + Curr + ' but with a capital.');
SentenceHasStarted := true;
end;

// Solve problems like characters following immediately after a
// sentence ender.
if SentenceEndedAtPreviousCharacter then
begin
if Curr <> ' ' then
AddResultMessage (USFM, USFM.PreviousChars + ' - Invalid character
(' + Curr + ') found straight after a sentence closer.');
SentenceEndedAtPreviousCharacter := false;
end;

// No spaces allowed before a sentence ender.
if Curr in SentenceEnders then
begin
if USFM.PreviousCharacter (1) = ' ' then
AddResultMessage (USFM, USFM.PreviousChars + ' - No space allowed
before a character ending a sentence.');
end;

if USFM.HeadingNow then
if Curr <> ' ' then HeadingText := HeadingText + Curr;
if (USFM.HeadingNow <> PreviousSFMHeading)
or (USFM.HeadingNumber <> PreviousSFMHeadingNumber) then
begin
if USFM.HeadingNow then
// Heading starts here.
begin
if SentenceHasStarted then
AddResultMessage (USFM, USFM.PreviousChars + ' - A heading
started whereas the running sentence has not yet been finished.');
end
else
// Heading ends here.
begin
if HeadingText <> '' then
if not SentenceHasStarted then
AddResultMessage (USFM, USFM.PreviousChars + ' - Heading should
start with a capital, and not end with a full stop.');
SentenceHasStarted := false;
end;
PreviousSFMHeading := USFM.HeadingNow;
PreviousSFMHeadingNumber := USFM.HeadingNumber;
end;
if not USFM.HeadingNow then HeadingText := '';

if USFM.NewParagraphStarted then
begin
if SentenceHasStarted then
AddResultMessage (USFM, USFM.PreviousChars + ' - A new paragraph
started whereas the running sentence has not yet been finished.');
USFM.NewParagraphStarted := false;
end;



*/
