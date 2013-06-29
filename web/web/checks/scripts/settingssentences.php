<h1><?php echo gettext ("Sentence Structure") ?></h1>
<p><?php echo gettext ("The settings here will be used by several checks on the structure of the sentence.") ?></p>
<p class="error"><?php echo $this->error ?></p>
<p class="success"><?php echo $this->success ?></p>
<br>
<p><?php echo gettext ("Enter all the possible capitals in the text of the Bible, separated by a space.") ?></p>
<p><?php echo gettext ("For example") ?> A B C D E F G H I J K L M N O P Q R S T U V W X Y Z</p>
<form action="settingssentences.php" name="form1" method="post">
  <p><input type="text" name="capitals" value="<?php echo $this->capitals ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit1" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Enter all the possible small letters in the text of the Bible, separated by a space.") ?></p>
<p><?php echo gettext ("For example") ?> a b c d e f g h i j k l m n o p q r s t u v w x y z</p>
<form action="settingssentences.php" name="form2" method="post">
  <p><input type="text" name="smallletters" value="<?php echo $this->smallletters ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit2" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Enter all the possible punctuation marks at the ends of sentences, separated by a space.") ?></p>
<p><?php echo gettext ("For example") ?> . ! ? :</p>
<form action="settingssentences.php" name="form3" method="post">
  <p><input type="text" name="endpunctuationmarks" value="<?php echo $this->endpunctuationmarks ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit3" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Enter all the possible punctuation marks within sentences, separated by a space.") ?></p>
<p><?php echo gettext ("For example") ?> , ;</p>
<form action="settingssentences.php" name="form4" method="post">
  <p><input type="text" name="middlepunctuationmarks" value="<?php echo $this->middlepunctuationmarks ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit4" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Enter all the possible characters that should be disregarded within sentences, separated by a space.") ?></p>
<p><?php echo gettext ("For example") ?> ( ) [ ] { } ' " * - 0 1 2 3 4 5 6 7 8 9</p>
<form action="settingssentences.php" name="form5" method="post">
  <p><input type="text" name="disregards" value="<?php echo $this->disregards ?>" class="fullwidth" /></p>
  <p><input type="submit" name="submit5" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><?php echo gettext ("Enter the names, starting with a capital, that may occur after mid-sentence punctuation, separated by a space.") ?></p>
<p><?php echo gettext ("Separate the names by a space. Put no more than 10 names on one line.") ?></p>
<p><?php echo gettext ("For example") ?> Jesus Paul Peter ...</p>
<form action="settingssentences.php" name="form6" method="post">
  <p><textarea name="names" class="fullwidth"><?php echo $this->names ?></textarea></p>
  <p><input type="submit" name="submit6" value=<?php echo gettext ("Submit") ?> /></p>
</form>
<br>
<p><a href="settings.php"><?php echo gettext ("Go back") ?></a></p>
