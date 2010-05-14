<h1>{t}Notifications{/t}</h1>
<p>{t}Here you can set up whether you are notified about changes in the consultation notes.{/t}</p>
<br>
{if $edited_notes_subscription == true}
  <p>{t}You automatically get subscribed to consultation notes that you create or edit, and therefore you will be notified by email of any changes made in these notes.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch automatic subscription and notification off.{/t}</a></p>
{else}
  <p>{t}At present you are not automatically subscribed to consultation notes that you create or edit, and therefore you will not be notified by email of any changes made in these notes.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch automatic subscription and notification on.{/t}</a> {t}There are other ways as well of getting notified of such changes, such as when you manually subscribe to a certain consultation note, or when you have enabled the setting to get notified of any changes in any consultation note.{/t}</p>
{/if}
<br>
{if $any_notes_subscription == true}
  <p>{t}You get notified of any changes made by anybody in the consultations notes.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch these notifications off.{/t}</a></p>
{else}
  <p>{t}You do not get notified of any changes made by anybody in the consultation notes.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch these notifications on.{/t}</a> {t}There may be other reasons why you would still get notified of changes in consultations notes, such as when you are subscribed to one or more of these notes.{/t}</p>
{/if}
<br>
{if $notes_assignment == true}
  <p>{t}Any consultation note that gets changed by anybody will be assigned to you. This enables you to quickly see which consultation notes got changed. To see this, you can edit the consultation notes view so that it shows those notes which have been assigned to you.{/t} <a href="notifications.php?notesassignmenttoggle=">{t}Switch this off.{/t}</a></p>
{else}
  <p>{t}At present any consultation note that gets changed by anybody will not be automatically assigned to you.{/t} <a href="notifications.php?notesassignmenttoggle=">{t}Switch this on.{/t}</a> {t}Regardless of this setting, anybody can manually assign a consultation to you.{/t}</p>
{/if}
