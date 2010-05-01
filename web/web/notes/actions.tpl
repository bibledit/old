<p><a href="{$caller}?session={$session}&displaynotecontent=">{t}Go back to the note contents{/t}</a></p>
<p>{t}Summary{/t}: {$summary}</p>
<p>
{if $subscribed}
{t}You are subscribed to this note{/t} <a href="{$caller}?session={$session}&consultationnoteunsubscribe=">[{t}unsubscribe{/t}]</a>
{else}
<a href="{$caller}?session={$session}&consultationnotesubscribe=">{t}Subscribe to this note{/t}</a>
{/if}
<p>{t}The note has been assigned to{/t}:
{section name=offset loop=$assignees}
  {$assignees[offset]} 
  {if $level >= 5}
    {* To be able to assign a note to somebody, one should at least have the level of manager *}
    <a href="{$caller}?session={$session}&consultationnoteremoveassignee={$assignees[offset]}">[{t}unassign{/t}]</a>
    |
  {/if}
{/section}
{if $level >= 5}
  {* To be able to assign a note to somebody, one should at least have the level of manager *}
  <a href="{$caller}?session={$session}&consultationnoteaddassignee=">[{t}add assignee{/t}]</a>
{/if}
</p>
{if $assignee == true}
  <p>
  {t}This note has been specifically assigned to you for taking action on.{/t} 
  <a href="{$caller}?session={$session}&consultationnoteunassignme=">{t}I have done my part on it.{/t}</a>
  </p>
{/if}
<p>
{t}Status{/t}:
{if $level >= 4}
  {* The level of Translator and up can change the status *}
  <a href="{$caller}?session={$session}&consultationnotestatus=">{$status}</a>
{else}
  {$status}
{/if}
</p>
<p>
{t}Verses{/t}: {$verses} |
{if $level >= 3}
  {* The level of Consultant and up can edit the verses list. Lower levels only can add a verse to the list *}
  <a href="{$caller}?session={$session}&consultationnoteeditverses=">[{t}edit{/t}]</a>
{else}
  <a href="{$caller}?session={$session}&consultationnoteaddverse=">[{t}add{/t}]</a>
{/if}
</p>
<p>
{t}Bible{/t}:
{if $consultationnotebible != ""}
  {$consultationnotebible}
{else}
  {t}This is a general note, it does not apply to any specific Bible{/t}
{/if}
{if $level >= 5}
  {* To be able to change the note's Bible, one should at least have the level of manager *}
  <a href="{$caller}?session={$session}&consultationnotechangebible=">[{t}change{/t}]</a>
{/if}
</p>
<p>{t}Identifier{/t}: {$consultationnote}</p>
</p>
{if $level >= 5}
  {* To delete a note, one should at least have the level of manager *}
  <p><a href="{$caller}?session={$session}&deleteconsultationnote={$consultationnote}">{t}Delete this note{/t}</a></p>
{/if}
