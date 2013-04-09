<h1>{t}Notifications{/t}</h1>

<p>
<a href="notifications.php?editednotessubscriptiontoggle=">
{if $edited_notes_subscription == true} ☑ {else} ☐ {/if}
</a>
{t}Subscribe to consultation notes that you create or edit yourself.{/t}
</p>

<p>
<a href="notifications.php?anynotessubscriptiontoggle=">
{if $any_notes_subscription == true} ☑ {else} ☐ {/if}
</a>
{t}Subscribe to consultation notes that are created or edited by anybody.{/t}
</p>

<p>
<a href="notifications.php?subscribednotenotificationtoggle=">
{if $subscription_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Get notified by email about changes in consultation notes that you are subscribed to.{/t}
</p>

<p>
<a href="notifications.php?notesassignmenttoggle=">
{if $notes_assignment == true} ☑ {else} ☐ {/if}
</a>
{t}Consultation notes that get changed will be assigned to you.{/t}
</p>
 
<p>
<a href="notifications.php?assignednotenotificationtoggle=">
{if $assignment_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Get notified by email about changes in consultation notes that have been assigned to you.{/t}
</p>

<p>
<a href="notifications.php?biblechangesnotificationtoggle=">
{if $bible_changes_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Get notified by email about changes in the text of the Bibles.{/t}
</p>
