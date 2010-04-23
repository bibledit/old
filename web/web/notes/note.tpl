<p>
<a href="{$caller}?session={$session}">{t}List notes{/t}</a>
|
{if $subscribed}
{t}You are subscribed to this note{/t} <a href="{$caller}?session={$session}&displayconsultationnoteidentifier={$consultation_note_identifier}&consultationnoteunsubscribe=">[{t}unsubscribe{/t}]</a>
{else}
<a href="{$caller}?session={$session}&displayconsultationnoteidentifier={$consultation_note_identifier}&consultationnotesubscribe=">{t}Subscribe to this note{/t}</a>
{/if}
|
<a href="{$caller}?session={$session}&deleteconsultationnote={$consultation_note_identifier}">{t}Delete this note{/t}</a>
</p>
<p>{t}Summary{/t}: {$note_summary}</h3>
<div>{$note_content}</div>
<hr>
{if isset ($note_add_comment)}
  <form action="{$caller}?session={$session}&displayconsultationnoteidentifier={$consultation_note_identifier}&saveconsultationnotecomment=" name="form" method="post">
    <p>{t}Add a comment to this note:{/t}</p>
    <p><textarea name="comment"></textarea></p>
    <p><input type="submit" name="submit" value={t}Save{/t} /><input type="submit" name="cancel" value={t}Cancel{/t} /></p>
  </form>
{else}
  <p><a href="{$caller}?session={$session}&displayconsultationnoteidentifier={$consultation_note_identifier}&addtoconsultationnote=">{t}Add a comment to this note{/t}</a></p>
{/if}
