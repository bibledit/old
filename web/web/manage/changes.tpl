<h1>{t}Changes{/t}</h1>
<p>{t}Bibledit-Web tracks the changes in the text of the Bibles.{/t}</p>
<p>{t}If there have been changes, it will generate lists of them at midnight.{/t} <a href="changes.php?generate=">{t}Generate fresh lists now.{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<p><a href="../downloads/changes">{t}View the lists in the downloads section.{/t}</a></p>
<br>
<p>
  {t}Bibledit-Web can email lists with the changes to the users.{/t} 
  {t}It can also generate Change Notifications for users who have enabled that option.{/t}
  <a href="../user/notifications.php">{t}Settings are in the notifications.{/t}</a>
</p>
<br>
<p>{t}List of users who still have change notifications awaiting their approval:{/t}</p>
<table>
  {section name=offset loop=$users} 
  <tr>
    <td>{$users[offset]}</td>
    <td>|</td>
    <td>{$count[offset]}</td>
    <td>|</td>
    <td><a href="changes.php?clear={$users[offset]}">[{t}clear{/t}]</a></td>
  </tr>
  {/section} 
</table>
