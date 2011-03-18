<h1>{t}Notifications{/t}</h1>
<p>{t}Subscribe to consultation notes that you create or edit yourself.{/t}
{if $edited_notes_subscription == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?editednotessubscriptiontoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}Subscribe to consultation notes that are created or edited by anybody.{/t}
{if $any_notes_subscription == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?anynotessubscriptiontoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}Get notified by email about changes in consultation notes that you are subscribed to.{/t}
{if $subscription_notification == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?subscribednotenotificationtoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}Consultation notes that get changed will be assigned to you.{/t}
{if $notes_assignment == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?notesassignmenttoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}Get notified by email about changes in consultation notes that have been assigned to you.{/t}
{if $assignment_notification == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?assignednotenotificationtoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}Get notified by email about changes in the text of the Bibles.{/t}
{if $bible_changes_notification == true}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="notifications.php?biblechangesnotificationtoggle=">[{t}toggle{/t}]</a></p>
