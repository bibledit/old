<h1>{t}Checks{/t}</h1>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
{if $resultcount == 0}
  <p>{t}No results for the checks.{/t}</p>
  <p>{t}This could be because the Bible text is okay.{/t}</p>
  <p>{t}Else check whether the checks been enabled.{/t}</p>
{else}
  {section name=offset loop=$result_ids}
  <p><a href="index.php?approve={$result_ids[offset]}"> 
  ✔
  <a href="index.php?delete={$result_ids[offset]}"> 
  ✗
  </a><a href="index.php?goto={$result_ids[offset]}" target="_blank">
  {$result_data[offset]}
  </a></p>
  {/section} 
  <br>
  <p>{t}Click on ✔ to approve the entry.{/t} {t}The entry will no longer appear in subsequent checks.{/t}</p>
  <p>{t}Click on ✗ to delete the entry for just now.{/t} {t}The entry will be back in subsequent checks.{/t}</p>
  <p>{t}Click the entry to navigate to the passage.{/t}</p>
{/if}
{if $suppressedcount == 0}
{else}
  <br>
  <p>{t}Here is a list of suppressed check results.{/t} {t}Delete a result so that it is visible again in the list with check results.{/t}</p>
{/if}
{section name=offset loop=$suppressed_ids}
<p style="color:grey;"><a href="index.php?release={$suppressed_ids[offset]}"> ✗ </a>{$suppressed_data[offset]}</p>
{/section} 
