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
    <form action="{$base_url}" name="form" method="post">
      <p><input type="text" name="entry" maxlength="300" value="{$value}" /></p>
      <p><input type="submit" name="{$submit}" value={t}Submit{/t} /></p>
    </form>
    <h3><a href="{$base_url}">{t}Cancel{/t}</a></h3>
    <p>{$help}</p>
