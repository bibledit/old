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

#include "check_parallel_passages.h"
#include "projectutils.h"
#include "settings.h"
#include "utilities.h"
#include "ot-quotations.h"
#include "ot-nt-parallels.h"
#include "tiny_utilities.h"

CheckNTQuotationsFromOT::CheckNTQuotationsFromOT(const ustring & project, const vector < unsigned int >&books, bool includetext, bool gui)
{
  // Language and versification.
  extern Settings *settings;
  ustring language = settings->projectconfig(project, false)->language_get();
  ustring versification = settings->projectconfig(project, false)->versification_get();
  Mapping mapping(versification, 0);
  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  set < unsigned int >bookset(mybooks.begin(), mybooks.end());
  // Get all quotations.
  OTQuotations otquotations(0);
  otquotations.read();
  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Producing passages", true);
    progresswindow->set_iterate(0, 1, otquotations.quotations.size());
  }
  // Go through each quotations.
  for (unsigned int i = 0; i < otquotations.quotations.size(); i++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel)
        return;
    }
    // Skip if NT book is not to be included.
    if (bookset.find(otquotations.quotations[i].nt.book) == bookset.end())
      continue;
    mapping.book_change(otquotations.quotations[i].nt.book);
    mapping.original_to_me(otquotations.quotations[i].nt);
    ustring verse = otquotations.quotations[i].nt.human_readable(language);
    if (includetext) {
      verse.append(" ");
      verse.append(project_retrieve_verse(project, otquotations.quotations[i].nt.book, otquotations.quotations[i].nt.chapter, otquotations.quotations[i].nt.verse));
    }
    nt.push_back(verse);
    // Go through the OT quotations processing them.
    vector < ustring > store;
    for (unsigned i2 = 0; i2 < otquotations.quotations[i].ot.size(); i2++) {
      mapping.book_change(otquotations.quotations[i].ot[i2].book);
      mapping.original_to_me(otquotations.quotations[i].ot[i2]);
      ustring verse = otquotations.quotations[i].ot[i2].human_readable(language);
      if (includetext) {
        verse.append(" ");
        verse.append(project_retrieve_verse(project, otquotations.quotations[i].ot[i2].book, otquotations.quotations[i].ot[i2].chapter, otquotations.quotations[i].ot[i2].verse));
      }
      store.push_back(verse);
    }
    // Save data.
    ot.push_back(store);
  }
}

CheckNTQuotationsFromOT::~CheckNTQuotationsFromOT()
{
  if (progresswindow)
    delete progresswindow;
}

CheckParallelPassages::CheckParallelPassages(bool nt, const ustring & project, const vector < unsigned int >&books, bool includetext, bool gui)
{
  // Language.
  extern Settings *settings;
  ustring language = settings->projectconfig(project, false)->language_get();

  // Mapping.
  ustring versification = settings->projectconfig(project, false)->versification_get();
  Mapping mapping(versification, 0);

  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  set < unsigned int >bookset(mybooks.begin(), mybooks.end());

  // Get the parallel passages.
  OtNtParallels otntparallels(0);
  if (nt)
    otntparallels.readnt();
  else
    otntparallels.readot();

  // GUI.
  progresswindow = NULL;
  if (gui) {
    progresswindow = new ProgressWindow("Producing passages", true);
    progresswindow->set_iterate(0, 1, otntparallels.sections.size());
  }
  // Go through each section.
  for (unsigned int i = 0; i < otntparallels.sections.size(); i++) {
    if (gui) {
      progresswindow->iterate();
      if (progresswindow->cancel)
        return;
    }
    OtNtParallelDataSection datasection(0);
    // Section's heading.
    datasection.title = otntparallels.sections[i].title;
    // Go through each set of references.
    for (unsigned int i2 = 0; i2 < otntparallels.sections[i].sets.size(); i2++) {
      // Go through the references in the set.
      OtNtParallelDataSet dataset(0);
      for (unsigned int i3 = 0; i3 < otntparallels.sections[i].sets[i2].references.size(); i3++) {
        // Skip if NT book is not to be included.
        if (bookset.find(otntparallels.sections[i].sets[i2].references[i3].book) == bookset.end())
          continue;
        vector < int >remapped_chapter;
        vector < int >remapped_verse;
        mapping.book_change(otntparallels.sections[i].sets[i2].references[i3].book);
        mapping.original_to_me(otntparallels.sections[i].sets[i2].references[i3].chapter, otntparallels.sections[i].sets[i2].references[i3].verse, remapped_chapter, remapped_verse);
        Reference mapped_reference(otntparallels.sections[i].sets[i2].references[i3].book, remapped_chapter[0], convert_to_string(remapped_verse[0]));
        ustring verse = mapped_reference.human_readable(language);
        if (includetext) {
          verse.append(" ");
          verse.append(project_retrieve_verse(project, mapped_reference.book, mapped_reference.chapter, mapped_reference.verse));
        }
        dataset.data.push_back(verse);
      }
      datasection.sets.push_back(dataset);
    }
    data.push_back(datasection);
  }
}

CheckParallelPassages::~CheckParallelPassages()
{
  if (progresswindow)
    delete progresswindow;
}
