<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Text{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <p>
      {t}Bible{/t}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$verse}&newbible=">{$bible}</a>
      |
      {t}Go to{/t}
      {if is_numeric ($previous_book)}
      <a href="index.php?bible={$bible}&book={$previous_book}&chapter=1&verse=1">««</a>
      {else}
      ««
      {/if}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$verse}&newbook=">{$book_name}</a>
      {if is_numeric ($next_book)}
      <a href="index.php?bible={$bible}&book={$next_book}&chapter=1&verse=1">»»</a>
      {else}
      »»
      {/if}
      {if is_numeric ($previous_chapter)}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$previous_chapter}&verse=1">««</a>
      {else}
      ««
      {/if}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$verse}&newchapter=">{t}chapter{/t} {$chapter}</a>
      {if is_numeric ($next_chapter)}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$next_chapter}&verse=1">»»</a>
      {else}
      »»
      {/if}
      {if is_numeric ($previous_verse)}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$previous_verse}">««</a>
      {else}
      ««
      {/if}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$verse}&newverse=">{t}verse{/t} {$verse}</a>
      {if is_numeric ($next_verse)}
      <a href="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$next_verse}">»»</a>
      {else}
      »»
      {/if}
    </p>
    <p class="success">{$success_message}</p>
    <p class="error">{$error_message}</p>

    <form action="index.php?bible={$bible}&book={$book}&chapter={$chapter}&verse={$verse}" name="form" method="post">
      <p><textarea name="data">{$chapter_data}</textarea></p>
      <p><input type="submit" name="submit" value={t}Save{/t} /></p>
    </form>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
