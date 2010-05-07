<p>{t}Number of notes that would display with this selection:{/t} {$totalcount}</p>
<p>
<a href="{$caller}?session={$session}&displaynoteslist=">{t}Ok, I have finished editing the view, display the notes list{/t}</a>
{if $totalcount > 50}
  {t}or{/t}
  <a href="{$caller}?session={$session}&showallconsultationnotes=">{t}display all selected notes for this once{/t}</a>
{/if}
</p>
