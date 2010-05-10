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
<p>
{t}Select notes that have been edited:{/t}
  <a {if $editselector == 0}class="active"{/if} href="{$caller}?session={$session}&consultationnoteseditselector=0">{t}at any time{/t}</a>
  |
  <a {if $editselector == 1}class="active"{/if} href="{$caller}?session={$session}&consultationnoteseditselector=1">{t}during the last 30 days{/t}</a>
  |
  <a {if $editselector == 2}class="active"{/if} href="{$caller}?session={$session}&consultationnoteseditselector=2">{t}during the last 7 days{/t}</a>
  |
  <a {if $editselector == 3}class="active"{/if} href="{$caller}?session={$session}&consultationnoteseditselector=3">{t}since yesterday{/t}</a>
  |
  <a {if $editselector == 4}class="active"{/if} href="{$caller}?session={$session}&consultationnoteseditselector=4">{t}today{/t}</a>
</p>
<p>
{t}Select notes that have a certain status:{/t}
  <a {if $statusselector == ""}class="active"{/if} href="{$caller}?session={$session}&consultationnotesstatusselector=">{t}Any status{/t}</a>
  {section name=offset loop=$statusids}
    |
    <a {if $statusselector == $statusids[offset]}class="active"{/if} href="{$caller}?session={$session}&consultationnotesstatusselector={$statusids[offset]}">{$statuslocs[offset]}</a>
  {/section}
</p>
<p>
{t}Select notes that belong to:{/t}
  <a {if $bibleselector == 0}class="active"{/if} href="{$caller}?session={$session}&consultationnotesbibleselector=0">{t}any Bible{/t}</a>
  |
  <a {if $bibleselector == 1}class="active"{/if} href="{$caller}?session={$session}&consultationnotesbibleselector=1">{t}the current Bible{/t}</a>
</p>
<p>
{t}Select notes that have been assigned to somebody:{/t}
  <a {if $assignmentselector == 0}class="active"{/if} href="{$caller}?session={$session}&consultationnotesassignmentselector=0">{t}do not care about this{/t}</a>
  |
  <a {if $assignmentselector == 1}class="active"{/if} href="{$caller}?session={$session}&consultationnotesassignmentselector=1">{t}assigned to me{/t}</a>
  |
  <a {if $assignmentselector == 2}class="active"{/if} href="{$caller}?session={$session}&consultationnotesassignmentselector=2">{t}assigned to somebody but not me{/t}</a>
</p>
<p>
{t}Select notes by subscription:{/t}
  <a {if $subscriptionselector == 0}class="active"{/if} href="{$caller}?session={$session}&consultationnotessubscriptionselector=0">{t}do not care about this{/t}</a>
  |
  <a {if $subscriptionselector == 1}class="active"{/if} href="{$caller}?session={$session}&consultationnotessubscriptionselector=1">{t}notes I am subscribed to{/t}</a>
</p>
{t}Select notes that have a severity of:{/t}
  <a {if $severityselector == -1}class="active"{/if} href="{$caller}?session={$session}&consultationnotesseverityselector=-1">{t}Any{/t}</a>
  {section name=offset loop=$severities}
    |
    <a {if $severityselector == $severities[offset][0]}class="active"{/if} href="{$caller}?session={$session}&consultationnotesseverityselector={$severities[offset][0]}">{$severities[offset][1]}</a>
  {/section}
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
