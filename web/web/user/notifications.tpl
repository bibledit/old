<h1>{t}Notifications{/t}</h1>
<p>{t}Here you can set up whether and how you are notified about changes in the consultation notes and the text of the Bibles.{/t}</p>
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that you create or edit yourself.{/t}</p>
{if $edited_notes_subscription == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that are created or edited by anybody.{/t}</p>
{if $any_notes_subscription == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, you can be notified by email about any change in any consultation note that you are subscribed to.{/t}</p>
{if $subscription_notification == true}
  <p>{t}This setting is now on.{/t} <a href="notifications.php?subscribednotenotificationtoggle=">{t}Switch it off.{/t}</a></p>
{else}
  <p>{t}This setting is now off.{/t} <a href="notifications.php?subscribednotenotificationtoggle=">{t}Switch it on.{/t}</a></p>
{/if}
<br>
<p>{t}If you wish, any consultation note that gets changed by anybody will be automatically assigned to you.{/t}</p>
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
