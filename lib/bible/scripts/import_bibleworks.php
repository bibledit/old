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
<h1><?php echo gettext("Import BibleWorks") ?></h1>
<p class="success"><?php echo $this->success_message ?></p>
<p class="error"><?php echo $this->error_message ?></p>
<p><?php echo gettext("Bible") ?>: <a href="settings.php?bible=<?php echo $this->bible ?>"><?php echo $this->bible ?></a></p>
<p><?php echo gettext("This helps you with importing BibleWorks data.") ?></p>
<p>
  <?php echo gettext("Each line in this data starts with a book, and a chapter and verse, like so:") ?> Hab 1:1.
  <?php echo gettext("Then a space follows, and then the text of the verse.") ?>
  <?php echo gettext("That's basically it.") ?>
</p>
<p>
  <?php echo gettext("There may be numbers in the text that key to Strong's data and other grammatical information, like so:") ?>
  The burden <04853b> which <0834> the prophet <05030> Habakkuk <02265> saw <02372> (08804).
  <?php echo gettext("Normally these tags are removed.") ?>
  <?php echo gettext("There is a checkbox for keeping them.") ?>
</p>
<p>
  <?php echo gettext("Words in italics are placed between square brackets, like so:") ?> [italics].
</p>
<p>
  <?php echo gettext("The text of footnotes is placed between curly brackets.") ?>
</p>
<p>
  <?php echo gettext("The data to be imported will overwrite similar data already in the Bible.") ?>
  <?php echo gettext("If you import for example Genesis chapter 1, it will overwrite Genesis 1 if that chapter already was in the Bible.") ?>
  <?php echo gettext("It will not overwrite anything else.") ?>
  <?php echo gettext("If Genesis 1 was not yet there, it will be created.") ?>
  <?php echo gettext("You can import one chapter, or several chapters, or one book, or several books, all in one go.") ?>
  <?php echo gettext("Bibledit will recognize books and chapters from the BibleWorks markup.") ?>
  <?php echo gettext("The text to be imported should be in Unicode UTF-8 format.") ?>
</p>
<br>
<p>
  <?php echo gettext("To import the data, paste the data into the box below, and then press the Submit button.") ?>
</p>
<form action="?bible=<?php echo $this->bible ?>" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="checkbox" name="tags" /><?php echo gettext("Keep the grammatical tags") ?></p>
  <p><input type="submit" name="submit" value=<?php echo gettext("Submit") ?> /></p>
</form>
