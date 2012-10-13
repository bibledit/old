<h1>{t}Import BibleWorks{/t}</h1>
<p class="success">{$success_message}</p>
<p class="error">{$error_message}</p>
<p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
<p>{t}This helps you with importing BibleWorks data.{/t}</p>
<p>
  {t}Each line in this data starts with a book, and a chapter and verse, like so:{/t} Hab 1:1.
  {t}Then a space follows, and then the text of the verse.{/t}
  {t}That's basically it.{/t}
</p>
<p>
  {t}There may be numbers in the text that key to Strong's data and other grammatical information, like so:{/t}
  The burden <04853b> which <0834> the prophet <05030> Habakkuk <02265> saw <02372> (08804).
  {t}Normally these tags are removed.{/t}
  {t}There is a checkbox for keeping them.{/t}
</p>
<p>
  {t}Words in italics are placed between square brackets, like so:{/t} [italics].
</p>
<p>
  {t}The text of footnotes is placed between curly brackets.{/t}
</p>
<p>
  {t}The data to be imported will overwrite similar data already in the Bible.{/t}
  {t}If you import for example Genesis chapter 1, it will overwrite Genesis 1 if that chapter already was in the Bible.{/t}
  {t}It will not overwrite anything else.{/t}
  {t}If Genesis 1 was not yet there, it will be created.{/t}
  {t}You can import one chapter, or several chapters, or one book, or several books, all in one go.{/t}
  {t}Bibledit will recognize books and chapters from the BibleWorks markup.{/t}
  {t}The text to be imported should be in Unicode UTF-8 format.{/t}
</p>
<br>
<p>
  {t}To import the data, paste the data into the box below, and then press the Submit button.{/t}
</p>
<form action="?bible={$bible}" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="checkbox" name="tags" />{t}Keep the grammatical tags{/t}</p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
