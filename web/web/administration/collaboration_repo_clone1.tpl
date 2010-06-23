<h1>{t}Clone repository{/t}</h1>

{if $object == "consultationnotes"}
<p>{t}Consultation Notes{/t}</p>
{else}
<p>{t}Bible{/t}: {$object}</p>
{/if}
<p>{t}Repository URL{/t}: {$url}</p>
  
<p>{t}Bibledit-Web clones the remote repository, and optimizes the clone for your use.{/t}</p>

{section name=offset loop=$contents}
  <p><code>{$contents[offset]}</code></p>
{/section}
