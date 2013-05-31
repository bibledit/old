<h1>{t}Change{/t}</h1>
<p><a href="changes.php">[{t}back{/t}]</a></p>
<br>
<p>
  {t}This lists the details for the change in the verse.{/t}
</p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<table>
  <tr>
    <td>{t}Old text{/t}:</td>
    <td>{$old_text}</td>
  </tr>
  <tr>
    <td>{t}Change{/t}:</td>
    <td>{$modification}</td>
  </tr>
  <tr>
    <td>{t}New text{/t}:</td>
    <td>{$new_text}</td>
  </tr>
</table>
