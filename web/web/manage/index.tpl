<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8">
    </meta>
    <title>{t}Management{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css">
    </link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Management{/t}</h1>
    <p><a href="users.php">{t}Users{/t}</a></p>
    <p><a href="../versification/index.php">{t}Versifications{/t}</a></p>
    <p><a href="../styles/indexm.php">{t}Styles{/t}</a></p>
    <p><a href="../bible/manage.php">{t}Bibles{/t}</a></p>
    {foreach key=key item=item from=$plugins} 
      <p><a href={$key}>{$item}</a></p>
    {/foreach} 
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
