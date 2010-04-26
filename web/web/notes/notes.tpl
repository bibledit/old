<p><a href="{$caller}?session={$session}&createconsultationnote=">{t}Create new note{/t}</a></p>
{section name=offset loop=$identifiers}
  <a name="note{$identifiers[offset]}"></a>
  <p><a href="{$caller}?session={$session}&consultationnote={$identifiers[offset]}">{$summaries[offset]}</a></p>
{/section}
