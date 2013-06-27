<h1>{t}Versifications{/t}</h1>
<p>{t}This lists the currently available versification systems.{/t} <a href="index.php?new=">{t}You can create a new versification system.{/t}</a></p>
{section name=offset loop=$systems} 
  <p><a href="system.php?name={$systems[offset]}">{$systems[offset]}</a></p>
{/section} 
