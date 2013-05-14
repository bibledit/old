<h1>{t}Sentence Structure{/t}</h1>
<p>{t}This checks the sentence structure of the Bible text.{/t}</p>
<p>{t}It checks whether sentences start with a capital and end with correct punctuation marks.{/t}</p>
<p>{t}It checks whether correct punctuation marks are used within sentences.{/t}</p>
<p>{t}It checks whether sentences contain only characters listed below.{/t}</p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p>{t}Enter all the possible capitals in the text of the Bible, separated by a space.{/t}</p>
<p>{t}For example{/t} A B C D E F G H I J K L M N O P Q R S T U V W X Y Z</p>
<form action="settingssentencestructure.php" name="form1" method="post">
  <p><input type="text" name="capitals" value="{$capitals}" class="fullwidth" /></p>
  <p><input type="submit" name="submit1" value={t}Submit{/t} /></p>
</form>
<br>
<p>{t}Enter all the possible small letters in the text of the Bible, separated by a space.{/t}</p>
<p>{t}For example{/t} a b c d e f g h i j k l m n o p q r s t u v w x y z</p>
<form action="settingssentencestructure.php" name="form2" method="post">
  <p><input type="text" name="smallletters" value="{$smallletters}" class="fullwidth" /></p>
  <p><input type="submit" name="submit2" value={t}Submit{/t} /></p>
</form>
<br>
<p>{t}Enter all the possible punctuation marks at the ends of sentences, separated by a space.{/t}</p>
<p>{t}For example{/t} . ! ? :</p>
<form action="settingssentencestructure.php" name="form3" method="post">
  <p><input type="text" name="endpunctuationmarks" value="{$endpunctuationmarks}" class="fullwidth" /></p>
  <p><input type="submit" name="submit3" value={t}Submit{/t} /></p>
</form>
<br>
<p>{t}Enter all the possible punctuation marks within sentences, separated by a space.{/t}</p>
<p>{t}For example{/t} , ;</p>
<form action="settingssentencestructure.php" name="form4" method="post">
  <p><input type="text" name="middlepunctuationmarks" value="{$middlepunctuationmarks}" class="fullwidth" /></p>
  <p><input type="submit" name="submit4" value={t}Submit{/t} /></p>
</form>
<br>
<p>{t}Enter all the possible characters that should be disregarded within sentences, separated by a space.{/t}</p>
<p>{t}For example{/t} {literal}( ) [ ] { } ' " * 0 1 2 3 4 5 6 7 8 9{/literal}</p>
<form action="settingssentencestructure.php" name="form5" method="post">
  <p><input type="text" name="disregards" value="{$disregards}" class="fullwidth" /></p>
  <p><input type="submit" name="submit5" value={t}Submit{/t} /></p>
</form>
<br>
<p><a href="settings.php">{t}Go back{/t}</a></p>
