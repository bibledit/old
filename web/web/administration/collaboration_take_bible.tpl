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
  <h1>{t}Take local Bible{/t}</h1>
  <p>{t}Bible{/t}: {$object}</p>
  <p>{t}Repository URL{/t}: {$url}</p>
  <p>{t}Pushing your data to the remote repository:{/t} <code>{$command}</code></p>
  {section name=offset loop=$result}
    <p><code>{$result[offset]}</code></p>
  {/section}
  <p class="error">{$error_message}</p>
  <p class="success">{$success_message}</p>
  {if $error_message == ""}
    <p>{t}Setting up collaboration is now ready.{/t}</p>
    <p>{t}The Manager can now choose menu Send/Receive any time he wants to synchronize Bibles and Consultation Notes with the remote repository.{/t}</p>
  {/if}
  {include file=../assets/footer_full.tpl} 
  </body>
</html>
