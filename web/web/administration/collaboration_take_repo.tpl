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
  <h1>{t}Take data in repository{/t}</h1>
  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}Repository URL{/t}: {$url}</p>
  <p>{t}The data in the remote repository was copied over into Bibledit-Web's data, overwriting the data that was there before.{/t}</p>
  <p>{t}Setting up collaboration is now ready.{/t}</p>
  <p>{t}The Translator can now choose menu Send/Receive any time he wants to synchronize the Bible and the Consultation Notes with the remote repository.{/t}</p>
  {include file=../assets/footer_full.tpl} 
  </body>
</html>
