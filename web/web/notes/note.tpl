<p>
<a href="{$caller}?session={$session}&displaynoteslist=">{t}Back to notes list{/t}</a>
|
{if $subscribed}
{t}You are subscribed to this note{/t} <a href="{$caller}?session={$session}&consultationnoteunsubscribe=">[{t}unsubscribe{/t}]</a>
{else}
<a href="{$caller}?session={$session}&consultationnotesubscribe=">{t}Subscribe to this note{/t}</a>
{/if}
|
<a href="{$caller}?session={$session}&deleteconsultationnote={$consultationnote}">{t}Delete this note{/t}</a>
</p>
<p>{t}Summary{/t}: {$note_summary}</h3>
<div>{$note_content}</div>
<hr>
{if isset ($note_add_comment)}
  <form action="{$caller}?session={$session}&saveconsultationnotecomment=" name="form" method="post">
    <p>{t}Add a comment to this note:{/t}</p>
    <p><textarea name="comment"></textarea></p>
    <p><input type="submit" name="submit" value={t}Save{/t} /><input type="submit" name="cancel" value={t}Cancel{/t} /></p>
  </form>
{else}
  <p><a href="{$caller}?session={$session}&addtoconsultationnote=">{t}Add a comment to this note{/t}</a></p>
{/if}
