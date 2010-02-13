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

    <p><a href="{$link_inbox}" {if $active_label == 'inbox'}class="active"{/if}>{t}Inbox{/t}</a> | <a href="{$link_emailed}" {if $active_label == 'emailed'}class="active"{/if}>{t}Emailed{/t}</a> | <a href="{$link_sent}"{if $active_label == 'sent'}class="active"{/if}>{t}Sent{/t}</a> | <a href="{$link_trash}"{if $active_label == 'trash'}class="active"{/if}>{t}Trash{/t}</a></p>




    <table>
      <thead>
        <tr>
          <td></td>
          <td>|</td>
          <td>{t}From{/t}</td>
          <td>|</td>
          <td>{t}Subject{/t}</td>
          <td>|</td>
          <td>{t}Date{/t}</td>
        </tr>
      </thead>
      <tbody>
        {section name=offset loop=$ids} 
        <tr>
          <td><a href="{$deletes[offset]}">[{t}delete{/t}]</a></td>
          <td>|</td>
          <td>{$sources[offset]}</td>
          <td>|</td>
          <td>{$subjects[offset]}</td>
          <td>|</td>
          <td>{$timestamps[offset]}</td>
        </tr>
        {/section} 
      </tbody>
    </table>







    {include file=../assets/footer_full.tpl} 
  </body>
</html>
