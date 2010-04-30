<h2>{$header}</h2>
<p>{$info_top}</p>
<form action="{$caller}?session={$session}&{$parameter}=" name="form" method="post">
  <p><textarea name="contents">{$text}</textarea></p>
  <p><input type="submit" name="submit" value={t}Save{/t} /></p>
</form>
<p>{$info_bottom}</p>
<h3><a href="{$caller}?session={$session}">{t}Cancel{/t}</a></h3>
