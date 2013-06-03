<h1>{t}Suppressed check results{/t}</h1>
<p><a href="index.php">[{t}back{/t}] <a href="#help">[{t}help{/t}]</a></p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
{section name=offset loop=$ids}
  <p style="color:grey;">
  <a href="index.php?release={$ids[offset]}">
  ✗ 
  </a>
  {$data[offset]}
  </p>
{/section} 
<br>
<a id="help"></a>
<p>
  {t}The page lists the suppressed results of the checks.{/t}
</p>
<p>
  {t}Click ✗ to remove the items from the suppressed list.{/t}
  {t}It will be visible again in the list of check results.{/t}
</p>
