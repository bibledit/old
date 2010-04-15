{section name=offset loop=$line_data_before}
  <a name="line{$line_numbers_before[offset]}"></a>
  <p><a href="{$caller}?session={$session}&editusfmline={$line_numbers_before[offset]}">{$line_data_before[offset]}</a></p>
{/section}
{if isset ($line_data_edit) }
  <form action="{$caller}?session={$session}&bible={$bible}&book={$book}&chapter={$chapter}&textsaveline={$line_number_edit}" name="form" method="post">
  <p><textarea name="data">{$line_data_edit}</textarea></p>
  <p><input type="submit" name="submit" value={t}Save{/t} /><input type="submit" name="cancel" value={t}Cancel{/t} /></p>
</form>
{/if}
{section name=offset loop=$line_data_after}
  <a name="line{$line_numbers_after[offset]}"></a>
  <p><a href="{$caller}?session={$session}&editusfmline={$line_numbers_after[offset]}">{$line_data_after[offset]}</a></p>
{/section}
