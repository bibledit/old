<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/


class Filter_Notes
{

  /**
  * This imports one note from Bibledit-Gtk.
  * The note is available in $filename.
  * It returns the identifier of the note if imported successfully.
  * Else it returns NULL.
  */
  public static function importFromBibleditGtkFile ($filename)
  {
    $note_identifier = NULL;
    if (file_exists ($filename)) {
      // The note is in the format as used by Bibledit-Gtk.
      // The filename represents the note ID in Bibledit-Gtk.
      // This ID is not relevant for import.
      // Read the note.
      $note = file ($filename);
      // line 0: date created.
      // This information is not used here. The same information will be in the logbook entries, see later.
      // line 1: user who created it.
      // This information is not used here. The same information will be in the logbook entries, see later.
      // line 2: note references.
      // Sample: Exod.29.23
      // Sample: Lev.26.16 Deut.28.22
      // It uses OSIS for book encoding.
      $passages = array ();
      foreach (explode (" ", trim ($note[2])) as $bibledit_gtk_reference) {
        $passages [] = Filter_Books::explodePassage ($bibledit_gtk_reference);
      }
      // line 3: note category.
      $category = trim ($note[3]);
      // line 4: Bible.
      $bible = trim ($note[4]);
      // line 5: date modified.
      // This information is list since the note will be modified upon import.
      // line 6 and up: note text, "Logbook:", and logbook entries.
      // Summary will be taken from the first line.
      $summary = trim ($note[6]);
      $contents = "";
      for ($i = 6; $i < count ($note); $i++) {
        $contents .= $note[$i] . "\n";
      }
      // Store note.
      // (In client mode, upon sync to the server, these notes will be erased: Import them on the server).
      $database_notes = Database_Notes::getInstance();
      $note_identifier = $database_notes->storeNewNote ($bible, 0, 0, 0, $summary, $contents, true);
      $database_notes->setPassages ($note_identifier, $passages, true);
      $database_notes->setStatus ($note_identifier, $category, true);
    }
    return $note_identifier;
  }



}

?>
