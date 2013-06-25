<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}List{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h2>{$header}</h2>
    <p>{$info_top}</p>
    <p>
    {section name=offset loop=$book_ids} 
    <a href="{$caller}&{$action}={$book_ids[offset]}">{$book_names[offset]}</a>
    {/section} 
    </p>
    <p>{$info_bottom}</p>
    <h3><a href="{$caller}">{t}Cancel{/t}</a></h3>
