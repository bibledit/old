<h1>{t}Change{/t}</h1>
<p><a href="changes.php">[{t}back{/t}]</a></p>
<br>
<p>
  {t}This lists the details for the change in the verse.{/t}
  {t}{/t}
</p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<table>
  {section name=offset loop=$ids} 
  <tr>
    <td><a href="changes.php?approve={$ids[offset]}"> ✔ </a></td>
    <td>{$passages[offset]}</td>
    <td>{$modifications[offset]}</td>
    <td>{$totalNotesCount[offset]}</td>
    <td>{$yourNotesCount[offset]}</td>
  </tr>
  {/section} 
</table>
