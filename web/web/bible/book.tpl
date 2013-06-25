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
    <h1>{$book_name}</h1>
    <p class="success">{$success_message}</p>
    <p class="error">{$error_message}</p>
    <p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
    <p>{t}Chapters:{/t}
    {section name=offset loop=$chapters}
      <a href="chapter.php?bible={$bible}&book={$book}&chapter={$chapters[offset]}">{$chapters[offset]}</a>
    {/section}
    </p>
    <p><a href="book.php?bible={$bible}&book={$book}&createchapter=">{t}Create chapter{/t}</a></p>
    <p><a href="settings.php?bible={$bible}&deletebook={$book}">{t}Delete this book{/t}</a></p>
