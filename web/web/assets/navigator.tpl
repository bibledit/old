<p>
  {t}Bible{/t}
  <a href="{$caller}?session={$session}&switchbible=">{$bible}</a>
  |
  {t}Go to{/t}
  {if is_numeric ($previous_book)}
  <a href="{$caller}?session={$session}&switchbook={$previous_book}&switchchapter=1&switchverse=1">««</a>
  {else}
  ««
  {/if}
  <a href="{$caller}?session={$session}&switchbook=">{$book_name}</a>
  {if is_numeric ($next_book)}
  <a href="{$caller}?session={$session}&switchbook={$next_book}&switchchapter=1&switchverse=1">»»</a>
  {else}
  »»
  {/if}
  {if is_numeric ($previous_chapter)}
  <a href="{$caller}?session={$session}&book={$book}&switchchapter={$previous_chapter}&switchverse=1">««</a>
  {else}
  ««
  {/if}
  <a href="{$caller}?session={$session}&book={$book}&switchchapter=">{t}chapter{/t} {$chapter}</a>
  {if is_numeric ($next_chapter)}
  <a href="{$caller}?session={$session}&book={$book}&switchchapter={$next_chapter}&switchverse=1">»»</a>
  {else}
  »»
  {/if}
  {if is_numeric ($previous_verse)}
  <a href="{$caller}?session={$session}&book={$book}&switchverse={$previous_verse}">««</a>
  {else}
  ««
  {/if}
  <a href="{$caller}?session={$session}&book={$book}&switchverse=">{t}verse{/t} {$verse}</a>
  {if is_numeric ($next_verse)}
  <a href="{$caller}?session={$session}&book={$book}&switchverse={$next_verse}">»»</a>
  {else}
  »»
  {/if}
</p>
