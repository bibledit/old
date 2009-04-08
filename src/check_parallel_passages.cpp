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

#include "check_parallel_passages.h"
#include "projectutils.h"
#include "settings.h"
#include "utilities.h"
#include "ot-quotations.h"
#include "ot-nt-parallels.h"
#include "tiny_utilities.h"
#include "books.h"
#include "usfmtools.h"


CheckOTQuotationsInNT::CheckOTQuotationsInNT(const ustring& project, const vector <unsigned int>& books, bool includetext)
{
  // Language and versification system.
  extern Settings *settings;
  ustring language = settings->projectconfig(project, false)->language_get();
  ustring versification = settings->projectconfig(project, false)->versification_get();
  Mapping mapping(versification, 0);

  // Get a list of the books to check. If no books were given, take them all.
  vector < unsigned int >mybooks(books.begin(), books.end());
  if (mybooks.empty())
    mybooks = project_get_books(project);
  set < unsigned int >bookset(mybooks.begin(), mybooks.end());

  // Get all quotations in New Testament order, optionally also in OT order.
  OTQuotations otquotations(0);
  otquotations.read();
  bool use_ot_order = settings->session.check_output_in_ot_order;
  if (use_ot_order) {
    otquotations.produce_in_ot_order();
  }
  
  // GUI.
  progresswindow = new ProgressWindow("Producing passages", true);
  progresswindow->set_iterate(0, 1, use_ot_order ? otquotations.quotations_ot_order.size() : otquotations.quotations_nt_order.size());
  
  // Go through the quotations.
  for (unsigned int i = 0; i < (use_ot_order ? otquotations.quotations_ot_order.size() : otquotations.quotations_nt_order.size()); i++) {
    progresswindow->iterate();
    if (progresswindow->cancel)
      return;

    // Skip if the reference book is not to be included.
    unsigned int reference_book = use_ot_order ? otquotations.quotations_ot_order[i].reference.book : otquotations.quotations_nt_order[i].reference.book;
    if (bookset.find(reference_book) == bookset.end())
      continue;

    // Optionally remapping of verses.
    mapping.book_change(reference_book);
    if (use_ot_order)
      mapping.original_to_me(otquotations.quotations_ot_order[i].reference);
    else
      mapping.original_to_me(otquotations.quotations_nt_order[i].reference);

    // Reference chapter and verse.
    unsigned int reference_chapter = use_ot_order ? otquotations.quotations_ot_order[i].reference.chapter : otquotations.quotations_nt_order[i].reference.chapter;
    ustring reference_verse = use_ot_order ? otquotations.quotations_ot_order[i].reference.verse : otquotations.quotations_nt_order[i].reference.verse;
    
    // Reference verse text.
    ustring reference_verse_text = use_ot_order ? otquotations.quotations_ot_order[i].reference.human_readable (language) : otquotations.quotations_nt_order[i].reference.human_readable(language);
    if (includetext) {
      reference_verse_text.append(" ");
      ustring verse = project_retrieve_verse(project, reference_book, reference_chapter, reference_verse);
      verse = usfm_get_verse_text_only (verse);
      reference_verse_text.append(verse);
    }
    
    // Store reference data.
    nt.push_back(reference_verse_text);
    references.push_back(books_id_to_english(reference_book) + " " + convert_to_string(reference_chapter) + ":" + reference_verse);
    comments.push_back("Quoted in New Testament");
    
    // Go through the OT quotations processing them.
    vector <ustring> store;
    unsigned int i2_limit = use_ot_order ? otquotations.quotations_ot_order[i].referents.size() : otquotations.quotations_nt_order[i].referents.size();
    for (unsigned i2 = 0; i2 < i2_limit; i2++) {
      if (use_ot_order) {
        mapping.book_change(otquotations.quotations_ot_order[i].referents[i2].book);
        mapping.original_to_me(otquotations.quotations_ot_order[i].referents[i2]);
      } else {
        mapping.book_change(otquotations.quotations_nt_order[i].referents[i2].book);
        mapping.original_to_me(otquotations.quotations_nt_order[i].referents[i2]);
      }
      ustring verse = use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].human_readable(language) : otquotations.quotations_nt_order[i].referents[i2].human_readable(language);
      if (includetext) {
        verse.append(" ");
        ustring text = project_retrieve_verse(project,
                                              use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].book : otquotations.quotations_nt_order[i].referents[i2].book, 
                                              use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].chapter : otquotations.quotations_nt_order[i].referents[i2].chapter, 
                                              use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].verse : otquotations.quotations_nt_order[i].referents[i2].verse);
        verse.append(usfm_get_verse_text_only(text));
      }
      store.push_back(verse);
      Reference reference ((use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].book : otquotations.quotations_nt_order[i].referents[i2].book),
                           (use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].chapter : otquotations.quotations_nt_order[i].referents[i2].chapter),
                           (use_ot_order ? otquotations.quotations_ot_order[i].referents[i2].verse : otquotations.quotations_nt_order[i].referents[i2].verse));
      references.push_back(reference.human_readable (""));
      comments.push_back("Old Testament verse quoted from");
    }
    // Save data.
    ot.push_back(store);
  }
}

CheckOTQuotationsInNT::~CheckOTQuotationsInNT()
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
        references.push_back(books_id_to_english(mapped_reference.book) + " " + convert_to_string(mapped_reference.chapter) + ":" + mapped_reference.verse);
        comments.push_back("Parallel");
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
