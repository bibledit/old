<h1>{t}Checks{/t}</h1>
{section name=offset loop=$results} 
<p><a href="index.php?suppress={$results[offset]}"> ☒ </a>{$results[offset]}</p>
{/section} 
<br>
<p><a href="settings.php">{t}Settings{/t}</a></p>
