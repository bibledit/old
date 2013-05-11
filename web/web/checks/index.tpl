<h1>{t}Checks{/t}</h1>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
{if $resultcount == 0}
  <p>{t}No results for the checks.{/t}</p>
  <p>{t}This could be because the Bible text is okay.{/t}</p>
  <p>{t}Else check whether the checks been enabled.{/t}</p>
{else}
  {section name=offset loop=$identifiers}
  <p><a href="index.php?approve={$identifiers[offset]}"> 
  ✔
  <a href="index.php?delete={$identifiers[offset]}"> 
  ✗
  </a><a href="index.php?goto={$identifiers[offset]}">
  {$results[offset]}
  </a></p>
  {/section} 
  <br>
  <p>{t}Click on ✔ to approve the entry.{/t} {t}The entry will no longer appear in subsequent checks.{/t}</p>
  <p>{t}Click on ✗ to delete the entry for just now.{/t} {t}The entry will be back in subsequent checks.{/t}</p>
  <p>{t}Click the entry to navigate to the passage.{/t}</p>
{/if}
<br>
<p><a href="settings.php">{t}Settings{/t}</a></p>
