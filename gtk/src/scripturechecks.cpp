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

#include "libraries.h"
#include "utilities.h"
#include <glib.h>
#include "gwrappers.h"
#include "gtkwrappers.h"
#include "scripturechecks.h"
#include "directories.h"
#include "dialogcheck.h"
#include "style.h"
#include "stylesheetutils.h"
#include "shell.h"
#include "projectutils.h"
#include "progresswindow.h"
#include "checks.h"
#include "check_validate_usfm.h"
#include "check_count_words.h"
#include "check_count_usfms.h"
#include "check_count_characters.h"
#include "check_chapters_verses.h"
#include "check_compare_usfm.h"
#include "check_unwanted_patterns.h"
#include "check_unwanted_words.h"
#include "check_repetition.h"
#include "check_capitalization.h"
#include "check_matching_pairs.h"
#include "books.h"
#include "check_markers_spacing.h"
#include "settings.h"
#include "check_ref_inventory.h"
#include "check_validate_refs.h"
#include "check_parallel_passages.h"
#include "check_sentence_structure.h"


vector < unsigned int >checks_generate_booknames()
{
  vector < unsigned int >book_ids;
  extern Settings *settings;
  vector < unsigned int >books = project_get_books(settings->genconfig.project_get());
  set < unsigned int >selected_books = settings->session.selected_books;
  for (unsigned int i = 0; i < books.size(); i++) {
    if (selected_books.find(books[i]) != selected_books.end())
      book_ids.push_back(books[i]);
  }
  return book_ids;
}


bool scripture_checks_chapters_verses(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (results == NULL) {
    CheckDialog dialog(cdtChaptersVerses);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  vector < unsigned int >books = checks_generate_booknames();
  CheckChaptersVerses check(settings->genconfig.project_get(), books, true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_count_usfms(bool gui)
{
  if (gui) {
    CheckDialog dialog(cdtMarkersCount);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  vector < unsigned int >books = checks_generate_booknames();
  CheckCountUsfms check(settings->genconfig.project_get(), books, settings->session.checksorttype, true);
  if (check.cancelled)
    return false;
  DisplayCheckingResults display("Marker Count");
  display.usfm_count(check.markers, check.counts, settings->genconfig.stylesheet_get());
  return true;
}


bool scripture_checks_validate_usfms(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtMarkersValidate);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckValidateUsfm check(settings->genconfig.project_get(), checks_generate_booknames(), true, true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_compare_usfms(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtMarkersCompare);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckCompareUsfms check(settings->genconfig.project_get(), settings->genconfig.check_markers_compare_project_get(), checks_generate_booknames(), true, settings->genconfig.check_markers_compare_all_markers_get(), settings->genconfig.check_markers_compare_include_only_get(), settings->genconfig.check_markers_compare_ignore_get(), settings->genconfig.check_markers_compare_ignore_verse_zero_get());
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}

bool scripture_checks_count_characters(bool gui)
{
  if (gui) {
    CheckDialog dialog(cdtCharactersCount);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  bool sortcharacter = (settings->session.checksorttype == cstSort1);
  bool sortcount = (settings->session.checksorttype == cstSort2);
  vector < unsigned int >books = checks_generate_booknames();
  CheckCountCharacters check(settings->genconfig.project_get(), books, sortcharacter, sortcount, true);
  if (check.cancelled)
    return false;
  DisplayCheckingResults display("Character Count");
  display.character_count(check.characters, check.counts);
  return true;
}


bool scripture_checks_unwanted_patterns(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtUnwantedPatterns);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckUnwantedPatterns check(settings->genconfig.project_get(), checks_generate_booknames(), checks_unwanted_patterns_get_filename(settings->genconfig.project_get()), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_capitalization(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtWordsCapitalization);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckCapitalization check(settings->genconfig.project_get(),
                            checks_generate_booknames(), settings->genconfig.check_capitalization_punctuation_get(), settings->genconfig.check_capitalization_ignore_get(), checks_abbreviations_get_filename(settings->genconfig.project_get()), settings->genconfig.check_capitalization_allow_any_prefixes_get(), checks_uncapitalized_prefixes_get_filename(settings->genconfig.project_get()), checks_capitalized_suffixes_get_filename(settings->genconfig.project_get()), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_repetition(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtWordsRepetition);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  ustring only_these_file = checks_repetition_show_only_get_filename(settings->genconfig.project_get());
  if (!settings->genconfig.check_repetition_show_only_these_get())
    only_these_file.clear();
  ustring ignore_these_file = checks_repetition_ignore_get_filename(settings->genconfig.project_get());
  if (!settings->genconfig.check_repetition_ignore_these_get())
    ignore_these_file.clear();
  CheckRepetition check(settings->genconfig.project_get(), checks_generate_booknames(), settings->genconfig.check_repetition_ignore_case_get(), only_these_file, ignore_these_file, true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_matching_pairs(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtMatchingPairs);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckMatchingPairs check(settings->genconfig.project_get(), checks_generate_booknames(), settings->genconfig.check_matching_pairs_ignore_get(), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_unwanted_words(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtWordsUnwanted);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckUnwantedWords check(settings->genconfig.project_get(), checks_generate_booknames(), checks_unwanted_words_get_filename(settings->genconfig.project_get()), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


bool scripture_checks_word_inventory(bool gui)
{
  if (gui) {
    CheckDialog dialog(cdtWordsCount);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckCountWords check(settings->genconfig.project_get(), checks_generate_booknames(), settings->genconfig.check_words_inventory_word_forming_characters_get(), settings->session.checksorttype == cstSort1, settings->session.checksorttype == cstSort2, settings->genconfig.check_words_inventory_not_include_words_count_get(), true);
  if (check.cancelled)
    return false;
  DisplayCheckingResults display("Word Inventory");
  display.word_inventory(check.words, check.counts, check.total_count, check.total_unique_count, check.filtered_count, check.filtered_unique_count, settings->genconfig.check_words_inventory_not_include_words_count_get());
  return true;
}


bool scripture_checks_usfm_spacing(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtMarkersSpacing);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckMarkersSpacing check(settings->genconfig.project_get(), checks_generate_booknames(), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


ustring checks_abbreviations_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "abbreviations");
}

ustring checks_uncapitalized_prefixes_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "uncapitalized-prefixes");
}

ustring checks_capitalized_suffixes_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "capitalized_suffixes");
}

ustring checks_repetition_show_only_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "repetitions_show_only");
}

ustring checks_repetition_ignore_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "repetitions_ignore");
}

ustring checks_unwanted_patterns_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "unwanted_patterns");
}

ustring checks_unwanted_words_get_filename(const ustring & project)
{
  return gw_build_filename(directories_get_projects(), project, "unwanted_words");
}


bool scripture_checks_references_inventory(bool gui)
{
  if (gui) {
    CheckDialog dialog(cdtReferencesInventory);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckReferenceInventory check(settings->genconfig.project_get(), checks_generate_booknames(), settings->session.check_include_verse_text, true);
  if (check.cancelled)
    return false;
  DisplayCheckingResults display("References Inventory");
  display.references_inventory(check.verses, check.references);
  return true;
}


bool scripture_checks_validate_references(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtReferencesValidate);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckValidateReferences check(settings->genconfig.project_get(), checks_generate_booknames(), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


void scripture_checks_nt_quotations_from_ot(WindowReferences * references_window)
{
  CheckDialog dialog(cdtNTQuotationsFromOT);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  extern Settings *settings;
  CheckOTQuotationsInNT check(settings->genconfig.project_get(), checks_generate_booknames(), settings->session.check_include_verse_text);
  checks_display_references_comments(check.references, check.comments, references_window);
  ustring main_heading = "Old Testament quotations in the New Testament, project " + settings->genconfig.project_get();
  DisplayCheckingResults display(main_heading.c_str());
  display.ot_quotations_in_nt(check.nt_refs, check.nt_texts, check.ot_refs, check.ot_texts, main_heading.c_str());
}


void scripture_checks_synoptic_parallels_from_nt(WindowReferences * references_window)
{
  CheckDialog dialog(cdtSynopticParallelsNT);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  extern Settings *settings;
  CheckParallelPassages check(true, settings->genconfig.project_get(), checks_generate_booknames(), settings->session.check_include_verse_text, true);
  checks_display_references_comments(check.references, check.comments, references_window);
  ustring main_heading = "Synoptic parallel passages of the New Testament, project " + settings->genconfig.project_get();
  DisplayCheckingResults display(main_heading.c_str());
  display.parallel_passages(check.data, main_heading.c_str());
}


void scripture_checks_parallels_from_ot(WindowReferences * references_window)
{
  CheckDialog dialog(cdtParallelsOT);
  if (dialog.run() != GTK_RESPONSE_OK)
    return;
  extern Settings *settings;
  CheckParallelPassages check(false, settings->genconfig.project_get(), checks_generate_booknames(), settings->session.check_include_verse_text, true);
  checks_display_references_comments(check.references, check.comments, references_window);
  ustring main_heading = "Parallel passages of the Old Testament, project " + settings->genconfig.project_get();
  DisplayCheckingResults display(main_heading.c_str());
  display.parallel_passages(check.data, main_heading.c_str());
}


bool scripture_checks_sentence_structure(WindowReferences * references_window, CollectCheckingResults * results)
{
  if (!results) {
    CheckDialog dialog(cdtSentenceStructure);
    if (dialog.run() != GTK_RESPONSE_OK)
      return false;
  }
  extern Settings *settings;
  CheckSentenceStructure check(settings->genconfig.project_get(), checks_generate_booknames(), true);
  if (check.cancelled)
    return false;
  if (results)
    results->add(check.references, check.comments);
  else
    checks_display_references_comments(check.references, check.comments, references_window);
  return true;
}


