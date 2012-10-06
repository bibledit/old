<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Bible{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Bible{/t} {$bible}</h1>
    <p>
      {t}Available books{/t}:
      {section name=offset loop=$book_ids}
        <a href="book.php?bible={$bible}&book={$book_ids[offset]}">{$book_names[offset]}</a>
      {/section}
    </p>
    <p>{t}Versification system{/t}: <a href="settings.php?bible={$bible}&versification=">{$versification}</a></p>
    <p><a href="settings.php?bible={$bible}&createbook=">{t}Create book{/t}</a></p>
    <p><a href="import.php?bible={$bible}">{t}Import USFM{/t}</a></p>
    <p><a href="manage.php?copy={$bible}">{t}Copy this Bible{/t}</a></p>
    <p><a href="manage.php?delete={$bible}">{t}Delete this Bible{/t}</a></p>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
