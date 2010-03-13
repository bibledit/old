<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Bibles{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Bibles{/t}</h1>
    <p>{t}Available Bibles:{/t}</p>
    <ul>
    {section name=offset loop=$bibles}
      <li><a href="settings.php?bible={$bibles[offset]}">{$bibles[offset]}</a></li>
    {/section}
    </ul>
    <p><a href="manage.php?new=">{t}Create a new empty Bible{/t}</a></p>
    {include file=../assets/footer_full.tpl}
  </body>
</html>
