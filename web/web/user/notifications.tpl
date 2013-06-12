<h1>{t}Notifications{/t}</h1>

<h2>{t}Consultation Notes{/t}</h2>

<p>
<a href="notifications.php?editednotessubscriptiontoggle=">
{if $edited_notes_subscription == true} ☑ {else} ☐ {/if}
</a>
{t}Subscribe to a consultation note you created or edited online or by email.{/t}
</p>

<p>
<a href="notifications.php?anynotessubscriptiontoggle=">
{if $any_notes_subscription == true} ☑ {else} ☐ {/if}
</a>
{t}Subscribe to a consultation note created or edited by anybody online or by email.{/t}
</p>

<p>
<a href="notifications.php?emailconfirmationyourpoststoggle=">
{if $email_confirmation_your_posts == true} ☑ {else} ☐ {/if}
</a>
{t}Receive email confirmation of your posts to a consultation notes done by email.{/t}
</p>

<p>
<a href="notifications.php?subscribednotenotificationtoggle=">
{if $subscription_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Receive email about changes in a consultation note you are subscribed to.{/t}
</p>

<p>
<a href="notifications.php?notesassignmenttoggle=">
{if $notes_assignment == true} ☑ {else} ☐ {/if}
</a>
{t}A consultation note that get changed will be assigned to you.{/t}
</p>

<p>
<a href="notifications.php?assignednotenotificationtoggle=">
{if $assignment_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Receive email about changes in a consultation note assigned to you.{/t}
</p>

<p>
<a href="notifications.php?suppressemailsfromnotesyouupdatedtoggle=">
{if $suppress_mails_from_your_updates == true} ☑ {else} ☐ {/if}
</a>
{t}Suppress receiving email from an updated consultation note in case the update was made by you.{/t}
</p>

<p>
<a href="notifications.php?assignednotesnotificationtoggle=">
{if $assigned_notes_notifications == true} ☑ {else} ☐ {/if}
</a>
{t}Receive a daily email with the number of consultation notes assigned to you.{/t}
</p>

<p>
<a href="notifications.php?deletednotenotificationtoggle=">
{if $deleted_note_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Receive email about a deleted consultation note.{/t}
</p>

<h2>{t}Change Notifications{/t}</h2>

<p>
<a href="notifications.php?biblechangesnotificationtoggle=">
{if $bible_changes_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Receive a daily email with the changes in the text of the Bibles.{/t}
</p>

<p>
<a href="notifications.php?changenotificationstoggle=">
{if $change_notifications == true} ☑ {else} ☐ {/if}
</a>
<a href="../consultations/changes.php">{t}Notifications about changes in the Bible text will be generated for you online.{/t}</a>
</p>

<p>
<a href="notifications.php?pendingchangenotificationstoggle=">
{if $pending_changes_notifications == true} ☑ {else} ☐ {/if}
</a>
{t}Receive a daily email with the number of online change notifications awaiting your approval.{/t}
</p>

<h2>{t}Text Checks{/t}</h2>

<p>
<a href="notifications.php?biblechecksnotificationtoggle=">
{if $bible_checks_notification == true} ☑ {else} ☐ {/if}
</a>
{t}Receive a daily email with results of checks on the text of the Bibles.{/t}
</p>

