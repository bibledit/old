<h1>{t}Styles{/t}</h1>
<p>{t}This lists the currently available stylesheets.{/t} <a href="indexm.php?new=">{t}You can create a new stylesheet.{/t}</a></p>
{section name=offset loop=$sheets} 
  <p><a href="sheetm.php?name={$sheets[offset]}">{$sheets[offset]}</a></p>
{/section} 
