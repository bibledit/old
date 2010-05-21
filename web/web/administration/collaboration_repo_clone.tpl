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
  <h1>{t}Clone repository{/t}</h1>

  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}Repository URL{/t}: {$url}</p>
    
  <p>{t}Bibledit-Web attempts to clone the remote repository. The progress of this, and its results, are displayed below.{/t}</p>  
  <p>{t}You can only go to the next page after the cloning completed successfully.{/t}</p>  

  <p>{t}Bibledit-Web attempted to clone the remote repository. This is the result:{/t}</p>  

  {section name=offset loop=$clone_result}
    <p><code>{$clone_result[offset]}</code></p>
  {/section}

  <p class="error">{$error_message}</p>
  <p class="success">{$success_message}</p>

  <p>{t}The shell command used:{/t} <code>{$command}</code></p>  

  <p>{t}The cloned git repository was optimized for your use.{/t}</p>  

  {if $success_message != ""}
    <h2><a href="collaboration_repo_write.php?object={$object}&directory={$directory}">{t}Next page{/t}</a></h2>
  {/if}

  {include file=../assets/footer_full.tpl} 
  </body>
</html>
