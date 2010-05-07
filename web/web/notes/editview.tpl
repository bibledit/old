<h1>{t}Edit Notes Display{/t}</h1>
<p>
{t}Select notes that refer to:{/t}
  <a {if $passageselector == 0}class="active"{/if} href="{$caller}?session={$session}&consultationnotespassageselector=0">{t}the current verse{/t}</a>
  |
  <a {if $passageselector == 1}class="active"{/if} href="{$caller}?session={$session}&consultationnotespassageselector=1">{t}the current chapter{/t}</a>
  |
  <a {if $passageselector == 2}class="active"{/if} href="{$caller}?session={$session}&consultationnotespassageselector=2">{t}the current book{/t}</a>
  |
  <a {if $passageselector == 3}class="active"{/if} href="{$caller}?session={$session}&consultationnotespassageselector=3">{t}any passage{/t}</a>
</p>
<p>{t}Number of notes selected:{/t} {$totalcount}</p>
<h3>
{t}Ok, I have finished{/t},
<a href="{$caller}?session={$session}&displaynoteslist=">{t}display the notes list{/t}</a>
{if $totalcount > 50}
  | {t}or{/t} |
  <a href="{$caller}?session={$session}&showallconsultationnotes=">{t}display all selected notes for this once{/t}</a>
{/if}
</h3>
