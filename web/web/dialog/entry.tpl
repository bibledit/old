<body onload="document.form.entry.focus();">
<h2>{$question}</h2>
<form action="{$base_url}" name="form" method="post">
  <p><input type="text" name="entry" maxlength="300" value="{$value}" /></p>
  <p><input type="submit" name="{$submit}" value={t}Submit{/t} /></p>
</form>
<h3><a href="{$base_url}">{t}Cancel{/t}</a></h3>
<p>{$help}</p>
