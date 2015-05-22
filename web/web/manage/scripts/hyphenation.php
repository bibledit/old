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
<h1><?php echo Locale_Translate::_("Hyphenation") ?></h1>
<p><?php echo Locale_Translate::_("Hyphenation can be used when preparing a Bible for printing.") ?></p>
<p><?php echo Locale_Translate::_("It allows for more efficient usage of paper, and for more regular appearance of margins at the end of the lines without requiring spacing adjustments.") ?></p>
<p><?php echo Locale_Translate::_("The details of hyphenating properly are complex and dependent on the language, and can interact with other orthographic and typesetting practices.") ?></p>
<p><?php echo Locale_Translate::_("Hyphenation may not be so important when preparing a Bible for proofreading. It is usually regarded important for production quality Bibles.") ?></p>
<br>
<p><?php echo Locale_Translate::_("When preparing a Bible in LibreOffice, OpenOffice, AbiWord, en so on, the wordprocessor itself provides hyphenation algorithms for many languages.") ?></p>
<p><?php echo Locale_Translate::_("If no suitable hyphenation algorithm can be used, perhaps Bibledit-Web can assist to a limited degree.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Bibledit-Web can take the data from a Bible, insert soft hyphens in the words according to a certain algorithm, and then save this data as a new Bible.") ?></p>
<p><?php echo Locale_Translate::_("This new Bible can then be exported to OpenDocument format, and opened in a wordprocessor.") ?></p>
<p><?php echo Locale_Translate::_("The wordprocessor sees these soft hyphens. When a soft hyphen occurs near the end of the line, it breaks the word there.") ?></p>
<br>
<p><?php echo Locale_Translate::_("Bibledit-Web has one hyphenating algorithm. When there is a transition in the text from any character of a first set of characters, to any character of a second set of characters, then the soft hyphen will be inserted at that location in the text.") ?></p>
<br>
<a name="sets"></a>
<form action="hyphenation.php#sets" method="post">
<p><?php echo Locale_Translate::_("First set of characters") ?>:</p>
<p><input type="text" name="firstset" value="<?php echo $this->firstset ?>" class="fullwidth" /></p>
<p><?php echo Locale_Translate::_("Second set of characters") ?>:</p>
<p><input type="text" name="secondset" value="<?php echo $this->secondset ?>" class="fullwidth" /></p>
<p><?php echo Locale_Translate::_("Enter the sets of characters, and press Save.") ?></p>
<p><input type="submit" name="sets" value=<?php echo Locale_Translate::_("Save") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("The Bible where the data will be taken from is:") ?> <a href="?bible="><?php echo $this->bible ?></a></p>
<p><?php echo Locale_Translate::_("The Bible where the hyphenated data will be written to is:") ?> <?php echo $this->bible ?>-hyphenated</p>
<br>
<p><a href="?run="><?php echo Locale_Translate::_("Insert the soft hyphens according to the settings.") ?></a></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
