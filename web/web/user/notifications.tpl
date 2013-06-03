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
<a href="notifications.php?notesassignmenttoggle=">
{if $notes_assignment == true} ☑ {else} ☐ {/if}
</a>
{t}Consultation notes that get changed will be assigned to you.{/t}
</p>

<p>
<a href="notifications.php?changenotificationstoggle=">
{if $change_notifications == true} ☑ {else} ☐ {/if}
</a>
<a href="../consultations/changes.php">{t}Change notifications will be generated for you.{/t}</a>
</p>
 
<br>
<p>
{t}Get notified by email about:{/t}
</p>

<p>
<a href="notifications.php?subscribednotenotificationtoggle=">
{if $subscription_notification == true} ☑ {else} ☐ {/if}
</a>
{t}changes in consultation notes you are subscribed to.{/t}
</p>

<p>
<a href="notifications.php?assignednotenotificationtoggle=">
{if $assignment_notification == true} ☑ {else} ☐ {/if}
</a>
{t}changes in consultation notes assigned to you.{/t}
</p>

<p>
<a href="notifications.php?deletednotenotificationtoggle=">
{if $deleted_note_notification == true} ☑ {else} ☐ {/if}
</a>
{t}deleted consultation notes.{/t}
</p>

<p>
<a href="notifications.php?biblechangesnotificationtoggle=">
{if $bible_changes_notification == true} ☑ {else} ☐ {/if}
</a>
{t}changes in the text of the Bibles.{/t}
</p>

<p>
<a href="notifications.php?biblechecksnotificationtoggle=">
{if $bible_checks_notification == true} ☑ {else} ☐ {/if}
</a>
{t}results of running checks on the text of the Bibles.{/t}
</p>

<p>
<a href="notifications.php?pendingchangenotificationstoggle=">
{if $pending_changes_notifications == true} ☑ {else} ☐ {/if}
</a>
{t}the number of change notifications awaiting your approval.{/t}
</p>

<p>
<a href="notifications.php?assignednotesnotificationtoggle=">
{if $assigned_notes_notifications == true} ☑ {else} ☐ {/if}
</a>
{t}the number of consultation notes assigned to you.{/t}
</p>

