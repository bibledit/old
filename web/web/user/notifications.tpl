<h1>{t}Notifications{/t}</h1>
<p>{t}Here you can set up whether you are notified by email about changes in the consultation notes.{/t}</p>

<br>
{if $edited_notes_subscription == true}
  <p>{t}You automatically get subscribed to consultation notes that you create or edit, and therefore you will be notified of any changes made in these notes.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch automatic subscription and notification off.{/t}</a></p>
{else}
  <p>{t}You are not automatically subscribed to consultation notes that you create or edit, and therefore you will not be notified of any changes made in these notes.{/t} <a href="notifications.php?editednotessubscriptiontoggle=">{t}Switch automatic subscription and notification on.{/t}</a> {t}There are other ways as well of getting notified of such changes, such as when you manually subscribe to a certain consultation note, or when you have enabled the setting to get notified of any changes in any consultation note.{/t}</p>
{/if}

<br>
{if $any_notes_subscription == true}
  <p>{t}You get notified of any changes made by anybody in the consultations notes.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch these notifications off.{/t}</a></p>
{else}
  <p>{t}You do not get notified of any changes made in the consultation notes.{/t} <a href="notifications.php?anynotessubscriptiontoggle=">{t}Switch these notifications on.{/t}</a> {t}There may be other reasons why you would still get notified of changes in consultations notes, such as when you are subscribed to one or more of these notes.{/t}</p>
{/if}
