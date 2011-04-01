<h1>{t}Import{/t}</h1>
<p class="success">{$success_message}</p>
<p class="error">{$error_message}</p>
<p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
<p>
  {t}This helps you with importing Unified Standard Format Markers data.{/t}
  <a href="../usfm/usfm.htm">{t}See the Unified Standard Format Markers User Reference for more information about this format.{/t}</a>
</p>
<p>
  {t}The data to be imported will overwrite similar data already in the Bible.{/t}
  {t}If you import for example Genesis chapter 1, it will overwrite Genesis 1 if that chapter already was in the Bible.{/t}
  {t}It will not overwrite anything else.{/t}
  {t}If Genesis 1 was not yet there, it will be created.{/t}
  {t}You can import one chapter, or several chapters, or one book, or several books, all in one go.{/t}
  {t}Bibledit will recognize books through the \\id markup and chapters through the \\c markup.{/t}
  {t}The text to be imported should be in Unicode UTF-8 format.{/t}
</p>
<br>
<p>
  {t}To import the data, paste the data into the box below, and then press the Submit button.{/t}
  <a href="import1.php?bible={$bible}">{t}Or upload a file.{/t}</a>
</p>
<form action="import.php?bible={$bible}" name="form" method="post">
  <p><textarea name="data"></textarea></p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
