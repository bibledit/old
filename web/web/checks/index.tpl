<h1>{t}Checks{/t}</h1>
<p><a href="#help">[{t}help{/t}]</a></p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
{section name=offset loop=$result_ids}
  <p><a href="index.php?approve={$result_ids[offset]}"> 
  ✔
  <a href="index.php?delete={$result_ids[offset]}"> 
  ✗
  </a><a href="index.php?goto={$result_ids[offset]}" target="_blank">
  {$result_data[offset]}
  </a></p>
{/section} 
{section name=offset loop=$suppressed_ids}
  <p style="color:grey;"><a href="index.php?release={$suppressed_ids[offset]}"> ✗ </a>{$suppressed_data[offset]}</p>
{/section} 
<br>
<a id="help"></a>
<p>
  {t}The page displays a list of results of running the checks.{/t}
</p>
<p>
  {t}Click ✔ to approve the entry.{/t}
  {t}The entry will move down to the list of grayed-out entries.{/t}
  {t}It will remain suppressed there.{/t}
</p>
<p>
  {t}Click ✗ to remove the entry for just now.{/t}
  {t}It will be back next day, unless the text of the Bible is corrected.{/t}
</p>
<p>
  {t}Click an entry to open the chapter in the text editor.{/t}
</p>
<p>
  {t}The Manager should enable the checks.{/t}
</p>
<p>
  {t}The greyed-out entries are the suppressed check results.{/t}
</p>
<p>
  {t}Delete a suppressed result to make it visible again in the list above.{/t}
</p>
