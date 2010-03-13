<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{$book_name} {$chapter}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{$book_name}  {$chapter}</h1>
    <p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
    <p>{t}Book{/t}: <a href="book.php?bible={$bible}&book={$book}">{$book_name}</a></p>
    <p><a href="book.php?bible={$bible}&book={$book}&deletechapter={$chapter}">{t}Delete this chapter{/t}</a></p>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
