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


#include "readwrite.h"
#include "gwrappers.h"
#include "projectutils.h"
#include "books.h"
#include "tiny_utilities.h"
#include "utilities.h"
#include "import.h"


void read_write_data (int argc, char *argv[], bool read, bool write)
/*
Read or writes data to or from a Bible / project.

Bibledit-rdwrt can read from or write to Bible data.

Syntax: bibledit-rdwrt -r|-w project book chapter|0 fileName

Breaking the syntax down we have:

First parameter: -r|-w 
This can be either -r or -w which determines whether the remaining arguments are going to do a "read" operation 
from the specified Bibledit-Gtk Bible / project, or do a "write" operation to that Bible / project.

Second parameter: project 
This gives the name of the Bibledit-Gtk Bible / project. 
All we have to do is ensure that the project name we want to access is a valid/existing one.

Third parameter: book
This is simply the 3-letter book code for the Bible book that is being read/written to.
I.e., MAT for Matthew, GEN for Genesis, etc.

Fourth parameter: chapter|0
This can be either a chapter number or 0 (zero) for reading/writing either an individual chapter 
or reading/writing a whole book (when the parameter is 0). 

Fifth parameter: fileName
This is a temporary file name that we assign for our use with bibledit-rdwrt. 
For a read (-r) operation this fileName argument is the name of the file that will be created by bibledit-rdwrt
containing a copy of the whole book (corresponding to the 3-letter code), 
or that contains the individual chapter contents (of a designated chapter) of an existing Bibledit-Gtk book file
in the Bible / project. It should be prefixed with a path us. 
Since bibledit-rdwrt is a console operation, after AdaptIt calls it using ::wxExecute, 
it would need to read the resulting temporary file to grab the contents for its use. 
For a write (-w) operation this fileName argument is the name of the temporary file that bibledit-rdwrt reads 
to get the text which it then writes to the appropriate Bible / project file. 
The temporary file can contain the text of a whole book, or just the text of a single chapter 
for the book specified by the book 3-letter code and the chapter (number) argument. 

bibledit-rdwrt may exit with 0 on success, or -1 on failure, as it sees fit.
It may write to stdout or stderr, as it sees fit.
*/

{
  // There should be 5 parameters given.
  if (argc != 6) {
    gw_critical ("Read/write data needs 5 parameters");
    return;
  }
  
  // First parameter: We already have this in 'read' and 'write'. 

  // Second parameter: project. The project must be existing in Bibledit.
  ustring project = argv[2];
  if (!project_exists (project)) {
    gw_critical ("Project " + project + " does not exist");
    return;
  }
  
  // Third parameter: book.
  unsigned int book = books_paratext_to_id (argv[3]);
  if (book == 0) {
    gw_critical ("Unknown book " + string (argv[3]));
    return;
  }
  if (!project_book_exists (project, book)) {
    gw_critical ("Book " + string (argv[3]) + " does not exist in project "  + project);
    return;
  }
  
  // Fourth parameter: chapter | 0.
  // Bibledit-Gtk supports chapter 0.
  // But here it is different. Chapter 0 means to read/write the whole book, not chapter 0.
  // A chapter number other than 0 refers to that particular chapter.
  int chapter = ABS (convert_to_int (argv[4]));
  if (chapter > 0) {
    vector<unsigned int> chapters = project_get_chapters (project, book);
    set <int> chapterset (chapters.begin (), chapters.end ());
    if (chapterset.find (chapter) == chapterset.end ()) {
      gw_critical ("Chapter " + convert_to_string (chapter) + " does not exist in project "  + project + " book " + string (argv[3]));
      return;
    }
  }
  
  // Fifth parameter: The filename to read from or write to.
  string filename = argv[5];
  
  // At this stage, we know the project, the book, the chapter,
  // and the filename, and whether to read or write.
  
  // Read the data from the project.
  if (read) {
    vector<ustring> data;
    if (chapter == 0) {
      data = project_retrieve_book (project, book);
    } else {
      data = project_retrieve_chapter (project, book, chapter);
    }
    // Store it in file.
    write_lines (filename, data);
    // Done.
    return;
  }
  
  // Write the data to the project.
  if (write) {
    vector <ustring> messages;
    import_usfm_file (filename, book, project, messages);
    for (unsigned int i = 0; i < messages.size (); i++) {
      gw_critical (messages[i]);
    }
  }

}

