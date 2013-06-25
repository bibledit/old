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
    {if !$horizontal}
      <p>{$info_top}</p>
    {/if}
    {if !$horizontal}
      <ul>
    {/if}
    {section name=offset loop=$text_lines} 
      {if !$horizontal}<li>{/if}<a href="{$caller_url}{$get_parameters[offset]}">{$text_lines[offset]}</a>{if !$horizontal}</li>{/if}
    {/section}
    {if !$horizontal}
      </ul> 
    {/if}
    {if !$horizontal}
      <p>{$info_bottom}</p>
    {/if}
    <h3><a href="{$caller_url}">{t}Cancel{/t}</a></h3>
