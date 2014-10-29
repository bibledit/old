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
<h1><?php echo Locale_Translate::_("Sentence Structure") ?></h1>
<p>
  <?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->bible ?>.
  <?php echo Locale_Translate::_("The settings here will be used by several checks on the structure of the sentence.") ?>
</p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><?php echo Locale_Translate::_("Enter all the possible capitals in the text of the Bible, separated by a space.") ?></p>
<p><?php echo Locale_Translate::_("For example") ?> A B C D E F G H I J K L M N O P Q R S T U V W X Y Z</p>
<form action="settingssentences.php" name="form1" method="post">
  <p><input type="text" name="capitals" value="<?php echo $this->capitals ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit1" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("Enter all the possible small letters in the text of the Bible, separated by a space.") ?></p>
<p><?php echo Locale_Translate::_("For example") ?> a b c d e f g h i j k l m n o p q r s t u v w x y z</p>
<form action="settingssentences.php" name="form2" method="post">
  <p><input type="text" name="smallletters" value="<?php echo $this->smallletters ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit2" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("Enter all the possible punctuation marks at the ends of sentences, separated by a space.") ?></p>
<p><?php echo Locale_Translate::_("For example") ?> . ! ? :</p>
<form action="settingssentences.php" name="form3" method="post">
  <p><input type="text" name="endpunctuationmarks" value="<?php echo $this->endpunctuationmarks ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit3" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("Enter all the possible punctuation marks within sentences, separated by a space.") ?></p>
<p><?php echo Locale_Translate::_("For example") ?> , ;</p>
<form action="settingssentences.php" name="form4" method="post">
  <p><input type="text" name="middlepunctuationmarks" value="<?php echo $this->middlepunctuationmarks ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit4" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p><?php echo Locale_Translate::_("Enter all the possible characters that should be disregarded within sentences, separated by a space.") ?></p>
<p><?php echo Locale_Translate::_("For example") ?> ( ) [ ] { } ' " * - 0 1 2 3 4 5 6 7 8 9</p>
<form action="settingssentences.php" name="form5" method="post">
  <p><input type="text" name="disregards" value="<?php echo $this->disregards ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit5" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p>
  <?php echo Locale_Translate::_("Enter the names, starting with a capital, that may occur after mid-sentence punctuation.") ?>
  <?php echo Locale_Translate::_("Separate the names by a space.") ?>
  <?php echo Locale_Translate::_("For example") ?> Jesus Paul Peter ...
</p>
<form action="settingssentences.php" name="form6" method="post">
  <p><textarea name="names" class="fullwidth"><?php echo $this->names ?></textarea></p>
  <p><input type="submit" name="submit6" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>
<br>
<p><a href="settings.php"><?php echo Locale_Translate::_("Go back") ?></a></p>
