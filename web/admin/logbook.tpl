<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Logbook{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Logbook{/t}</h1>

    {if $body != ""}
    
      {* Email contents display *}
      <p>{$subject}</p>
      <p>-</p>
      <p>{$body}</p>
    
    {else}
    
      {* Email list display *}
      <table>
        <thead>
          <tr>
            <td>{t}Timestamp{/t}</td>
            <td></td>
            <td>{t}Entry{/t}</td>
            <td></td>
            <td></td>
          </tr>
        </thead>
        <tbody>
          {section name=offset loop=$ids} 
          <tr>
            <td><a href="{$views[offset]}">{$subjects[offset]}</a></td>
            <td>|</td>
            <td>{$timestamps[offset]}</td>
            <td>|</td>
            <td><a href="{$deletes[offset]}">[{t}delete{/t}]</a></td>
          </tr>
          {/section} 
        </tbody>
      </table>
    
    {/if}

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
