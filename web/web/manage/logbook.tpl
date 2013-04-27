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


{section name=offset loop=$timestamps} 
<p>{$timestamps[offset]} | {$events[offset]}</p>
{/section} 
