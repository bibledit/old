<form action="{$caller}?session={$session}&createnotebible={$bible}&createnotebook={$book}&createnotechapter={$chapter}&createnoteverse={$verse}&savenewconsultationnote=" name="form" method="post">
  <p>{t}Create a new consultation note.{/t}</p>
  <p>{t}Summary{/t}:</p>
  <p><input type="text" name="summary" maxlength="100" class="fullwidth" /></p>
  <p>{t}Body{/t}: </p>
  <p><textarea name="contents" id="wysiwyg"></textarea></p>
  <p><input type="submit" name="submit" value={t}Save{/t} /><input type="submit" name="cancel" value={t}Cancel{/t} /></p>
</form>
