<h1>{t}Notifications{/t}</h1>
<p>{t}Here you can set up whether and how you are notified about changes in the consultation notes.{/t}</p>
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that you create or edit yourself. In that case you will be notified by email of any changes that you make in these notes.{/t}</p>
{if $edited_notes_subscription == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that are created or edited by anybody. In that case you will be notified by email of any changes that are made in these notes.{/t}</p>
{if $any_notes_subscription == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, any consultation note that gets changed by anybody will be automatically assigned to you. This enables you to quickly see which consultation notes got changed. To see the notes assigned to you, you can edit the consultation notes view so that it shows those notes which have been assigned to you.{/t}</p>
{if $notes_assignment == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?notesassignmenttoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?notesassignmenttoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, you can be notified by email about any change in any consultation note that has been assigned to you.{/t}</p>
{if $assignment_notification == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?assignednotenotificationtoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?assignednotenotificationtoggle=">{t}Switch it on.{/t}</a></p>
{/if}
