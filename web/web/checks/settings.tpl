<h1>{t}Checks - Settings{/t}</h1>
<p>{t}Bibledit-Web runs the checks on the Bibles every day just after midnight.{/t}</p>
<br>
<p><a href="settings.php?run=">{t}Run the checks now.{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p>{t}The following Bibles will be checked{/t}:
{section name=offset loop=$bibles}
  {$bibles[offset]} 
  <a href="settings.php?removebible={$bibles[offset]}">[{t}remove{/t}]</a>
  |
{/section}
<a href="settings.php?addbible=">[{t}add Bible{/t}]</a>
</p>
