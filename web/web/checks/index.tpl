<h1>{t}Checks{/t}</h1>
<p><a href="#help">[{t}help{/t}]</a></p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
{section name=offset loop=$ids}
  <p><a href="index.php?approve={$ids[offset]}">
  ✔
  <a href="index.php?delete={$ids[offset]}">
  ✗
  </a><a href="index.php?goto={$ids[offset]}" target="_blank">
  {$data[offset]}
  </a></p>
{/section} 
<br>
<a id="help"></a>
<p>{t}The page displays a list of results of running the checks.{/t}</p>
<p>{t}Click ✔ to approve and suppress the entry.{/t}</p>
<p>
  {t}Click ✗ to remove the entry for just now.{/t}
  {t}It will be back next day, unless the text of the Bible is corrected.{/t}
</p>
<p>{t}Click an entry to open the chapter in the text editor.{/t}</p>
<p>{t}The Manager should enable the checks.{/t}</p>
<p><a href="suppress.php">{t}Suppressed entries{/t}</a></p>
