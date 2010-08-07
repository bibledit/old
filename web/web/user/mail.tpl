<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Mail{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
    {include file=../assets/header_full.tpl} 
    <h1>{t}Mail{/t}</h1>

    <p><a href="{$link_inbox}" {if $active_label == 'inbox'}class="active"{/if}>{t}Inbox{/t}</a> | <a href="{$link_trash}"{if $active_label == 'trash'}class="active"{/if}>{t}Trash{/t}</a></p>

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
            <td>{t}Subject{/t}</td>
            <td></td>
            <td>{t}Date{/t}</td>
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
    <p>{t}Mail older than 90 days will automatically be deleted.{/t}</p>

    {include file=../assets/footer_full.tpl} 
  </body>
</html>
