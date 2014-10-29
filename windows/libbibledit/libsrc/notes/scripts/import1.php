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
?>
<h1><?php echo Locale_Translate::_("Importing notes") ?></h1>
<p><?php echo Locale_Translate::_("This page assists you with importing Consultation Notes.") ?></p>
<p><?php echo Locale_Translate::_("Bibledit-Gtk, part of the Bibledit Suite, has what it calls Project Notes. These can be imported into Bibledit-Web, and will appear there as Consultation Notes.") ?></p>
<p><?php echo Locale_Translate::_("When doing this import, it is important to know that this is a one-way operation. It means that, at the time of writing this, notes can be moved from Bibledit-Gtk to Bibledit-Web, but not the other way round.") ?></p>
<p><?php echo Locale_Translate::_("Importing the notes into Bibledit-Web, and using these there, has some advantages. For example, users can be notified of changes in the notes.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Have you decided that you'd like to move your notes from Bibledit-Gtk to Bibledit-Web? Let's give it a go, then.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The first thing to do is to get your notes out of Bibledit-Gtk and to save these to file.") ?></p>
<p><?php echo Locale_Translate::_("Start Bibledit-Gtk, and display the project notes window.") ?></p>
<p><?php echo Locale_Translate::_("In the menu, choose File, and then Backup. Note that at the time of writing this tutorial, the Export function does not work for Project Notes. Only Backup works.") ?></p>
<p><?php echo Locale_Translate::_("Go through the Backup assistant step by step, backup the 'Notes', and save them as a file called 'notes'. Bibledit-Gtk will change this into a file called 'notes.tar.gz'. Complete the Backup procedure. It may take a while depending on the number of notes you have.") ?></p>
<p><?php echo Locale_Translate::_("Locate the file on disk, and see if it looks fine.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The next thing is to prepare this notes.tar.gz file for uploading.") ?></p>
<p><?php echo Locale_Translate::_("The configuration of the server on which Bibledit-Web runs imposes some limits on the size of the file to be uploaded. The size of the file should not exceed either of the two limits given below:") ?></p>
<p><?php echo Locale_Translate::_("Maximum size of a file to be posted:") ?> <?php echo $this->post_max_size ?></p>
<p><?php echo Locale_Translate::_("Maximum size of a file to be uploaded:") ?> <?php echo $this->upload_max_filesize ?></p>
<p><?php echo Locale_Translate::_("The file to be uploaded is a compressed archive. If it is too large, you can remove parts from it. Files that can be removed are 'index.sql', and the whole '.git' folder that resides in the 'data' folder.") ?></p>
<p><?php echo Locale_Translate::_("If it is still too large, you can remove some of the notes data files from it, keep these, and import these through a second upload. The note data files are the files that have numeric names, e.g. '44166759'.") ?></p>
<br>
<p><?php echo Locale_Translate::_("The next thing is to upload this file to Bibledit-Web.") ?></p>
<form enctype="multipart/form-data" action="import2.php" method="POST">
  <p><?php echo Locale_Translate::_("Choose the notes file to be uploaded. This tutorial assumes that the file is 'notes.tar.gz'.") ?></p>
  <input name="notes" type="file" />
  <p><?php echo Locale_Translate::_("Then press the Upload button.") ?></p>
  <input type="submit" value="<?php echo Locale_Translate::_("Upload") ?>" />
</form>
<p>
  <?php echo Locale_Translate::_("Uploading may take some time.") ?>
  <?php echo Locale_Translate::_("It depends on the speed of your network and on the size of the file.") ?>
</p>
<br>
<p><?php echo Locale_Translate::_("If uploading fails, it may be because the file is too large, or the network too slow.") ?></p>
