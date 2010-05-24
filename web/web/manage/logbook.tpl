<h1>{t}Logbook{/t}</h1>

<p>
  {if is_numeric ($previous)}
  <a href="logbook.php?page={$previous}">««</a>
  {else}
  ««
  {/if}
  |
  {t}Page{/t} {$page}
  |
  {if is_numeric ($next)}
  <a href="logbook.php?page={$next}">»»</a>
  {else}
  »»
  {/if}
  |
  <a href="logbook.php?delete=">{t}clear{/t}</a>  
</p>


<table>
  <thead>
    <tr>
      <td>{t}Timestamp{/t}</td>
      <td></td>
      <td>{t}Event{/t}</td>
    </tr>
  </thead>
  <tbody>
    {section name=offset loop=$timestamps} 
    <tr>
      <td>{$timestamps[offset]}</td>
      <td>|</td>
      <td>{$events[offset]}</td>
    </tr>
    {/section} 
  </tbody>
</table>
