<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Styles{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Styles{/t}</h1>
    <p>{t}This lists the currently available stylesheets.{/t} <a href="indexm.php?new=">{t}You can create a new stylesheet.{/t}</a></p>
    {section name=offset loop=$sheets} 
      <p><a href="sheetm.php?name={$sheets[offset]}">{$sheets[offset]}</a></p>
    {/section} 
