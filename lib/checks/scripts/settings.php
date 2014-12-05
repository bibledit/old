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
<h2><?php echo gettext("Manage Checks") ?></h2>
<p>
  <?php echo gettext("Bible") ?>: <a href="?bible="><?php echo $this->bible ?></a>.
  <?php echo gettext("It will be checked every night.") ?>
  <a href="?run="><?php echo gettext("Check it now") ?></a>.
</p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><?php echo gettext("Enable the desired checks:") ?></p>

<p>
<a href="settings.php?doublespacesusfm=">
<?php if ($this->double_spaces_usfm == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("double spaces in the raw USFM") ?>
</p>

<p>
<a href="settings.php?fullstopheadings=">
<?php if ($this->full_stop_headings == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("punctuation at the end of a heading") ?>
</p>

<p>
<a href="settings.php?spacebeforepunctuation=">
<?php if ($this->space_before_punctuation == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("a space before a comma, semicolon, colon, full stop, question mark, or exclamation mark") ?>
</p>

<p>
<a href="settings.php?sentencestructure=">
<?php if ($this->sentence_structure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("sentence structure") ?>
</p>

<p>
<a href="settings.php?paragraphstructure=">
<?php if ($this->paragraph_structure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("paragraph starts with a capital and ends with a correct end marker") ?>
</p>

<p>
<a href="settings.php?chaptersversesversification=">
<?php if ($this->chapters_verses_versification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("correct number of books, chapters, and verses, according to the versification system") ?>
</p>

<p>
<a href="settings.php?wellformedusfm=">
<?php if ($this->well_formed_usfm == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("well-formed USFM") ?>
</p>

<p>
<a href="settings.php?punctuationatendverse=">
<?php if ($this->punctuation_end_verse == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext("missing punctuation at the end of a verse") ?>
</p>

<p>
<a href="settings.php?patterns=">
<?php if ($this->check_patterns == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
  <?php echo gettext("patterns in the text") ?>
  [<a href="settingspatterns.php"><?php echo gettext("settings") ?></a>]
</p>

<br>
<p><a href="settingssentences.php"><?php echo gettext("Settings for the sentence structure") ?></a></p>
