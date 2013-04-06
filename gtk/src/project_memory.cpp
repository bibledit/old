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

#include "project_memory.h"
#include "progresswindow.h"
#include "projectutils.h"

ProjectVerse::ProjectVerse(const ustring & number_in)
{
  number = number_in;
}

ProjectChapter::ProjectChapter(unsigned int number_in)
{
  number = number_in;
}

vector < ustring > ProjectChapter::get_data()
{
  vector < ustring > lines;
  for (unsigned int vs = 0; vs < data.size(); vs++) {
    lines.push_back(data[vs].data);
  }
  return lines;
}

void ProjectChapter::set_data(const vector < ustring > &lines)
{
  data.clear();
  CategorizeChapterVerse ccv(lines);
  for (unsigned int vs = 0; vs < ccv.verse.size(); vs++) {
    ProjectVerse projectverse(ccv.verse[vs]);
    projectverse.data = ccv.line[vs];
    data.push_back(projectverse);
  }
}

vector < ustring > ProjectChapter::get_verses()
// Get the numbers of the verses in the object.
{
  vector < ustring > verses;
  for (unsigned int vs = 0; vs < data.size(); vs++) {
    verses.push_back(data[vs].number);
  }
  return verses;
}

ProjectVerse *ProjectChapter::get_verse_pointer(const ustring & number)
// Returns a pointer to the verse, or NULL if it is not there.
{
  for (unsigned int i = 0; i < data.size(); i++)
    if (number == data[i].number)
      return &data[i];
  return NULL;
}

ProjectBook::ProjectBook(unsigned int number_in)
{
  number = number_in;
}

vector < ustring > ProjectBook::get_data()
{
  vector < ustring > lines;
  for (unsigned int ch = 0; ch < data.size(); ch++) {
    for (unsigned int vs = 0; vs < data[ch].data.size(); vs++) {
      lines.push_back(data[ch].data[vs].data);
    }
  }
  return lines;
}

vector < unsigned int >ProjectBook::get_chapters()
// Get the numbers of the chapters in the object.
{
  vector < unsigned int >chapters;
  for (unsigned int ch = 0; ch < data.size(); ch++) {
    chapters.push_back(data[ch].number);
  }
  return chapters;
}

ProjectChapter *ProjectBook::get_chapter_pointer(unsigned int number)
// Returns a pointer to the chapter, or NULL if it is not there.
{
  for (unsigned int i = 0; i < data.size(); i++)
    if (number == data[i].number)
      return &data[i];
  return NULL;
}

ProjectMemory::ProjectMemory(const ustring & name_in, bool gui)
{
  name = name_in;
  cancelled = false;
  ProgressWindow *progresswindow = NULL;
  if (gui)
    progresswindow = new ProgressWindow("Loading " + name, true);
  vector < unsigned int >books = project_get_books(name);
  if (progresswindow)
    progresswindow->set_iterate(0, 1, books.size());
  for (unsigned int bk = 0; bk < books.size(); bk++) {
    if (progresswindow)
      progresswindow->iterate();
    if (progresswindow && progresswindow->cancel) {
      cancelled = true;
      delete progresswindow;
      return;
    }
    ProjectBook projectbook(books[bk]);
    vector < unsigned int >chapters = project_get_chapters(name, books[bk]);
    for (unsigned int ch = 0; ch < chapters.size(); ch++) {
      ProjectChapter projectchapter(chapters[ch]);
      projectchapter.set_data(project_retrieve_chapter(name, books[bk], chapters[ch]));
      projectbook.data.push_back(projectchapter);
    }
    data.push_back(projectbook);
  }
  if (progresswindow)
    delete progresswindow;
}

vector < unsigned int >ProjectMemory::get_books()
// Get the ids of the books in the object.
{
  vector < unsigned int >books;
  for (unsigned int bk = 0; bk < data.size(); bk++) {
    books.push_back(data[bk].number);
  }
  return books;
}

ProjectBook *ProjectMemory::get_book_pointer(unsigned int number)
// Returns a pointer to the book, or NULL if it is not there.
{
  for (unsigned int i = 0; i < data.size(); i++)
    if (number == data[i].number)
      return &data[i];
  return NULL;
}
