<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Entry{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body onload="document.form.entry.focus();">
    {include file=../assets/header_full.tpl} 
    <h2>{$question}</h2>
    <form action="{$php_self}" name="form" method="post">
      <p><input type="text" name="entry" maxlength="300" /></p>
      <p><input type="submit" name="{$submit}" value={t}Submit{/t} /></p>
    </form>
    <h3><a href="{$php_self}">{t}Cancel{/t}</a></h3>
    {include file=../assets/footer_full.tpl} 
  </body>
</html>
