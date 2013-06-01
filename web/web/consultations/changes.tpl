<h1>{t}Changes{/t}</h1>
<p><a href="#help">[{t}help{/t}]</a></p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<table>
  {section name=offset loop=$ids} 
  <tr>
    <td><a href="changes.php?approve={$ids[offset]}"> ✔ </a></td>
    <td><a href="changes.php?goto={$ids[offset]}" target="_blank">{$passages[offset]}</a></td>
    <td><a href="change.php?id={$ids[offset]}">{$modifications[offset]}</a></td>
    <td>{$totalNotesCount[offset]}</td>
    <td>{$yourNotesCount[offset]}</td>
  </tr>
  {/section} 
</table>
<br>
<a id="help"></a>
<p>
  {t}This page lists the changes in the text of the Bibles.{/t}
</p>
<p>
  {t}Click ✔ to approve a change.{/t}
  {t}The change will be removed from the list.{/t}
</p>
<p>
  {t}Click the passage to open the Text Editor for the chapter.{/t}
</p>
<p>
  {t}Click the changed text to display the Change Details.{/t}
</p>
<p>
  {t}Any changes in the text of the Bibles will be added to the list the next day.{/t}
</p>
<p>
  {t}This works if enabled in your personal notifications.{/t}
</p>
<p>
  {t}There are two numbers at the right of each change.{/t}
</p>
<p>
  {t}The first number shows the total number of Consultation Notes about the verse where the change was made.{/t}
</p>
<p>
  {t}The second one indicates the number of notes you are subscribed to or that have been assigned to you.{/t}
</p>
