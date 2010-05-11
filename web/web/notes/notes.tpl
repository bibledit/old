<p><a href="{$caller}?session={$session}&createconsultationnote=">{t}Create new note{/t}</a></p>
<p>
{if $firstnote > 1}
  <a href="{$caller}?session={$session}&showpreviousconsultationnotes=">««</a>
{/if}
{t}Displaying notes{/t} {$firstnote} - {$lastnote} / {$totalcount}
[<a href="{$caller}?session={$session}&consultationnoteseditview=">{t}edit{/t}</a>]
{if $lastnote < $totalcount}
  <a href="{$caller}?session={$session}&shownextconsultationnotes=">»»</a>
{/if}
</p>
{section name=offset loop=$identifiers}
  <a name="note{$identifiers[offset]}"></a>
  <p><a href="{$caller}?session={$session}&consultationnote={$identifiers[offset]}">{$summaries[offset]}</a></p>
  <p>{$contents[offset]}</p>
{/section}
