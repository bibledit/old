<h1>{t}Changes{/t}</h1>
<p>
  {t}This lists the changes in the text of the Bibles.{/t}
  {t}Click ✔ to approve a change.{/t}
</p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<table>
  <thead>
    <tr>
      <td></td>
      <td>{t}{/t}</td>
      <td>{t}{/t}</td>
      <td></td>
      <td>{t}{/t}</td>
    </tr>
  </thead>
  <tbody>
    {section name=offset loop=$ids} 
    <tr>
      <td><a href="changes.php?approve={$ids[offset]}"> ✔ </a></td>
      <td>{$passages[offset]}</td>
      <td>{$modifications[offset]}</td>
      <td></td>
      <td></td>
    </tr>
    {/section} 
  </tbody>
</table>
