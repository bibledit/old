<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Collaboration{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
  {include file=../assets/header_full.tpl} 
  <h1>{t}Test read access{/t}</h1>

  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}Repository URL{/t}: {$url}</p>
    
  <p>{t}Bibledit-Web attempted to read data from this repository. This is the result:{/t}</p>  

  <p><code>{$read_access_result}</code></p>

  <p class="error">{$error_message}</p>
  <p class="success">{$success_message}</p>

  <p>{t}The shell command that was used is:{/t} <code>{$command}</code></p>  

  <p>{t}If you'd like to retry, after updating the URL, press button Submit.{/t}</p>
  <form action="collaboration_repo_read.php?object={$object}" name="url" method="post">
    <p><input type="text" name="urlvalue" value="{$url}" /></p>
    <p><input type="submit" name="url" value={t}Submit{/t} /></p>
  </form>

  <h2><a href="collaboration_repo_clone.php?object={$object}&directory={$directory}">{t}Next page{/t}</a></h2>

  {include file=../assets/footer_full.tpl} 
  </body>
</html>
