<h1>{t}Settings for Checks{/t}</h1>
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
<br>
<p>{t}Enable any of the following checks to run them regularly:{/t}</p>
<p>
<a href="settings.php?doublespacesusfm=">
{if $double_spaces_usfm == true} ☑ {else} ☐ {/if}
</a>
{t}double spaces in the raw USFM{/t}
</p>
<p>
<a href="settings.php?fullstopheadings=">
{if $full_stop_headings == true} ☑ {else} ☐ {/if}
</a>
{t}full stop at the end of headings{/t}
</p>
<br>
<p>{t}Here is a list of suppressed check results.{/t} {t}Delete a result so that it is visible again in the list with check results.{/t}</p>
{section name=offset loop=$identifiers}
<p><a href="settings.php?release={$identifiers[offset]}"> ✗ </a>{$results[offset]}</p>
{/section} 


