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
  <h1>{t}Write to repository{/t}</h1>

  {if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
  {else}
  <p>{t}Bible{/t}: {$object}</p>
  {/if}
  <p>{t}Repository URL{/t}: {$url}</p>

  <p>{t}A temporal file was added to the cloned repository:{/t} <code>{$command1}</code></p>
  {section name=offset loop=$result1}
    <p><code>{$result1[offset]}</code></p>
  {/section}
  <p class="error">{$error_message1}</p>
  <p class="success">{$success_message1}</p>

  <p>{t}The above changes were committed locally:{/t} <code>{$command2}</code></p>
  {section name=offset loop=$result2}
    <p><code>{$result2[offset]}</code></p>
  {/section}
  <p class="error">{$error_message2}</p>
  <p class="success">{$success_message2}</p>
    
  <p>{t}Changes were pulled from the remote repository:{/t} <code>{$command3}</code></p>
  {section name=offset loop=$result3}
    <p><code>{$result3[offset]}</code></p>
  {/section}
  <p class="error">{$error_message3}</p>
  <p class="success">{$success_message3}</p>

  <p>{t}Changes were pushed to the remote repository:{/t} <code>{$command4}</code></p>
  {section name=offset loop=$result4}
    <p><code>{$result4[offset]}</code></p>
  {/section}
  <p class="error">{$error_message4}</p>
  <p class="success">{$success_message4}</p>

  <p>{t}Changes were committed to the local repository:{/t} <code>{$command5}</code></p>
  {section name=offset loop=$result5}
    <p><code>{$result5[offset]}</code></p>
  {/section}
  <p class="error">{$error_message5}</p>
  <p class="success">{$success_message5}</p>

  <p>{t}Changes were pushed to the remote repository:{/t} <code>{$command6}</code></p>
  {section name=offset loop=$result6}
    <p><code>{$result6[offset]}</code></p>
  {/section}
  <p class="error">{$error_message6}</p>
  <p class="success">{$success_message6}</p>

  {if $error_message4 == ""}
    {if $error_message6 == ""}
      <h2><a href="collaboration_repo_data.php?object={$object}&directory={$directory}">{t}Next page{/t}</a></h2>
    {/if}
  {/if}

  {include file=../assets/footer_full.tpl} 
  </body>
</html>
