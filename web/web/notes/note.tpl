<p>
<a href="{$caller}?session={$session}&displaynoteslist=">{t}Back to notes list{/t}</a>
|
<a href="{$caller}?session={$session}&displaynotesactions=">{t}Actions{/t}</a>
</p>
<p>{t}Summary{/t}: {$summary}</p>
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
