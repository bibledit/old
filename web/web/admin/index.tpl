<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    </meta>
    <title>{t}Administration{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css">
    </link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Administration{/t}</h1>
    <p><a href="logbook.php">{t}Logbook{/t}</a></p>
    <p><a href="mail.php">{t}Mail{/t}</a></p>
    <p><a href="collaboration.php">{t}Collaboration{/t}</a></p>
    <p><a href="database.php">{t}Database{/t}</a></p>
    <p><a href="phpinfo.php">PHP Info</a></p>
    {foreach key=key item=item from=$plugins} 
      <p><a href={$key}>{$item}</a></p>
    {/foreach} 
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
