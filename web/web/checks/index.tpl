<h1>{t}Checks{/t}</h1>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
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
<br>
<p><a href="settings.php">{t}Settings{/t}</a></p>
