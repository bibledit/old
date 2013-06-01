<h1>{t}Change{/t}</h1>
<p><a href="changes.php">[{t}back{/t}]</a> <a href="#help">[{t}help{/t}]</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p>{$passage}</p>
<table>
  <tr>
    <td>{t}Old{/t}:</td>
    <td>{$old_text}</td>
  </tr>
  <tr>
    <td>{t}Change{/t}<a href="changes.php?approve={$id}"> ✔ </a>:</td>
    <td>{$modification}</td>
  </tr>
  <tr>
    <td>{t}New{/t}:</td>
    <td>{$new_text}</td>
  </tr>
</table>
<br>
{section name=offset loop=$notes} 
  <p><a href="notes.php?consultationnote={$notes[offset]}" target="_blank">{$summaries[offset]}</a></p>
{/section} 
<br>
<p>{$timestamp}</p>
<br>
<a id="help"></a>
<p>
  {t}This lists the details for the change in the verse.{/t}
</p>
