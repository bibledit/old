<h1><?php echo gettext ("Settings for Checks") ?></h1>
<p><?php echo gettext ("Bibledit-Web runs the checks on the Bibles every day just after midnight.") ?></p>
<br>
<p><a href="settings.php?run="><?php echo gettext ("Run the checks now") ?></a></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><?php echo gettext ("The following Bibles will be checked") ?>:
<?php foreach ($this->bibles as $bible) { ?>
  <?php echo $bible ?>
  <a href="settings.php?removebible=<?php echo $bible ?>">[<?php echo gettext ("remove") ?>]</a>
  |
<?php } ?>
<a href="settings.php?addbible=">[<?php echo gettext ("add Bible") ?>]</a>
</p>
<br>
<p><?php echo gettext ("Enable any of the following checks to run them regularly:") ?></p>

<p>
<a href="settings.php?doublespacesusfm=">
<?php if ($this->double_spaces_usfm == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("double spaces in the raw USFM") ?>
</p>

<p>
<a href="settings.php?fullstopheadings=">
<?php if ($this->full_stop_headings == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("punctuation at the end of a heading") ?>
</p>

<p>
<a href="settings.php?spacebeforepunctuation=">
<?php if ($this->space_before_punctuation == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("a space before a comma, semicolon, colon, full stop, question mark, or exclamation mark") ?>
</p>

<p>
<a href="settings.php?sentencestructure=">
<?php if ($this->sentence_structure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("sentence structure") ?>
</p>

<p>
<a href="settings.php?paragraphstructure=">
<?php if ($this->paragraph_structure == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("paragraph starts with a capital and ends with a correct end marker") ?>
</p>

<p>
<a href="settings.php?chaptersversesversification=">
<?php if ($this->chapters_verses_versification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("correct number of books, chapters, and verses, according to the versification system") ?>
</p>

<p>
<a href="settings.php?wellformedusfm=">
<?php if ($this->well_formed_usfm == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("well-formed USFM") ?>
</p>

<p>
<a href="settings.php?punctuationatendverse=">
<?php if ($this->punctuation_end_verse == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("missing punctuation at the end of a verse") ?>
</p>

<p>
<a href="settings.php?patterns=">
<?php if ($this->check_patterns == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
  <?php echo gettext ("patterns in the text") ?>
  [<a href="settingspatterns.php"><?php echo gettext ("settings") ?></a>]
</p>

<br>
<p><a href="settingssentences.php"><?php echo gettext ("Settings for the sentence structure") ?></a></p>
