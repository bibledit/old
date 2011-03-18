<h1>{t}Notifications{/t}</h1>
<p>{t}Here you can set up whether and how you are notified about changes in the consultation notes and the text of the Bibles.{/t}</p>
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that you create or edit yourself.{/t}</p>
<p>
{if $edited_notes_subscription == true}
  {t}This setting is now on.{/t}
{else}
  {t}This setting is now off.{/t}
{/if}
  <a href="notifications.php?editednotessubscriptiontoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}If you wish, you can automatically subscribe to consultation notes that are created or edited by anybody.{/t}</p>
<p>
{if $any_notes_subscription == true}
  {t}This setting is now on.{/t}
{else}
  {t}This setting is now off.{/t}
{/if}
  <a href="notifications.php?anynotessubscriptiontoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}If you wish, you can be notified by email about any change in any consultation note that you are subscribed to.{/t}</p>
<p>
{if $subscription_notification == true}
  {t}This setting is now on.{/t}
{else}
  {t}This setting is now off.{/t}
{/if}
  <a href="notifications.php?subscribednotenotificationtoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}If you wish, any consultation note that gets changed by anybody will be automatically assigned to you.{/t}</p>
<p>
{if $notes_assignment == true}
  {t}This setting is now on.{/t}
{else}
  {t}This setting is now off.{/t}
{/if}
  <a href="notifications.php?notesassignmenttoggle=">[{t}toggle{/t}]</a></p>
<br>
<p>{t}If you wish, you can be notified by email about any change in any consultation note that has been assigned to you.{/t}</p>
<p>
{if $assignment_notification == true}
  {t}This setting is now on.{/t}
{else}
  {t}This setting is now off.{/t}
{/if}
  <a href="notifications.php?assignednotenotificationtoggle=">[{t}toggle{/t}]</a></p>
