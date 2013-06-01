<h1>{t}Change{/t}</h1>
<p><a href="changes.php">[{t}back{/t}]</a> <a href="#help">[{t}help{/t}]</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p>{$passage}</p>
<table>
  <tr>
    <td></td>
    <td>{t}Old{/t}:</td>
    <td>{$old_text}</td>
  </tr>
  <tr>
    <td><a href="changes.php?approve={$id}"> ✔ </a></td>
    <td>{t}Change{/t}:</td>
    <td>{$modification}</td>
  </tr>
  <tr>
    <td></td>
    <td>{t}New{/t}:</td>
    <td>{$new_text}</td>
  </tr>
</table>
<br>
<table>
  {section name=offset loop=$notes} 
    <tr>
      <td><a href="notes.php?consultationnote={$notes[offset]}" target="_blank">{$summaries[offset]}</a></td>
      <td>{if $subscriptions[offset]}<a href="change.php?id={$id}&unsubscribe={$notes[offset]}">[{t}unsubscribe{/t}]</a>{/if}</td>
      <td>{if $assignments[offset]}<a href="change.php?id={$id}&unassign={$notes[offset]}">[{t}I have done my part it{/t}]</a>{/if}</td>
      <td>{if $level >= 5}<a href="change.php?id={$id}&delete={$notes[offset]}">[{t}delete{/t}]</a>{/if}</td>
    </tr>
  {/section} 
</table>
<p><a href="change.php?id={$id}&create={$id}" target="_blank">{t}Create note{/t}</a></p>
<br>
<p>{$timestamp}</p>
<br>
<a id="help"></a>
<p>
  {t}{/t}
</p>
