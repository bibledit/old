<p><a href="{$caller}?session={$session}&createconsultationnote=">{t}Create new note{/t}</a></p>
<table>
  <thead>
  <tr><td>{t}Identifier{/t}</td><td>{t}Summary{/t}</td></tr>
  </thead>
  <tbody>
    {section name=offset loop=$identifiers}
      <a name="note{$identifiers[offset]}"></a>
      <tr>
        <td><a href="{$caller}?session={$session}&displayconsultationnoteidentifier={$identifiers[offset]}">{$identifiers[offset]}</a></td>
        <td><a href="{$caller}?session={$session}&displayconsultationnoteidentifier={$identifiers[offset]}">{$summaries[offset]}</a></td>
      </tr>
    {/section}
  </tbody>
</table>
