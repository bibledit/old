<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{$book_name}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Import{/t}</h1>
    <p class="success">{$success_message}</p>
    <p class="error">{$error_message}</p>
    <p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
    <p>
      {t}This helps you with importing Unified Standard Format Markers data.{/t}
      <a href="../usfm/usfm.htm">{t}See the Unified Standard Format Markers User Reference for more information about this format.{/t}</a>
      {t}To import the data, paste the data into the box below, and then press the Submit button.{/t}
      {t}The data will overwrite similar data already in the Bible.{/t}
      {t}If you import for example Genesis chapter 1, it will overwrite Genesis 1 if that chapter already was in the Bible.{/t}
      {t}It will not overwrite anything else.{/t}
      {t}If Genesis 1 was not yet there, it will be created.{/t}
      {t}You can import one chapter, or several chapters, or one book, or several books, all in one go.{/t}
      {t}Bibledit will recognize books through the \\id markup, chapters through the \\c markup, and verses through the \\v markup.{/t}
      {t}The text to be imported should be in Unicode UTF-8 format.{/t}
    </p>

    <form action="import.php?bible={$bible}" name="form" method="post">
      <p><textarea name="data">{$xml}</textarea></p>
      <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
    </form>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
