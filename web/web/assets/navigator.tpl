<p>
  {t}Bible{/t}
  <a href="index.php?session={$session}&switchbible=">{$bible}</a>
  |
  {t}Go to{/t}
  {if is_numeric ($previous_book)}
  <a href="index.php?session={$session}&switchbook={$previous_book}&switchchapter=1&switchverse=1">««</a>
  {else}
  ««
  {/if}
  <a href="index.php?session={$session}&switchbook=">{$book_name}</a>
  {if is_numeric ($next_book)}
  <a href="index.php?session={$session}&switchbook={$next_book}&switchchapter=1&switchverse=1">»»</a>
  {else}
  »»
  {/if}
  {if is_numeric ($previous_chapter)}
  <a href="index.php?session={$session}&book={$book}&chapter={$previous_chapter}&verse=1">««</a>
  {else}
  ««
  {/if}
  <a href="index.php?session={$session}&book={$book}&chapter={$chapter}&verse={$verse}&newchapter=">{t}chapter{/t} {$chapter}</a>
  {if is_numeric ($next_chapter)}
  <a href="index.php?session={$session}&book={$book}&chapter={$next_chapter}&verse=1">»»</a>
  {else}
  »»
  {/if}
  {if is_numeric ($previous_verse)}
  <a href="index.php?session={$session}&book={$book}&chapter={$chapter}&verse={$previous_verse}">««</a>
  {else}
  ««
  {/if}
  <a href="index.php?session={$session}&book={$book}&chapter={$chapter}&verse={$verse}&newverse=">{t}verse{/t} {$verse}</a>
  {if is_numeric ($next_verse)}
  <a href="index.php?session={$session}&book={$book}&chapter={$chapter}&verse={$next_verse}">»»</a>
  {else}
  »»
  {/if}
</p>
