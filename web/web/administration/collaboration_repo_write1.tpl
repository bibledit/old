<h1>{t}Write to repository{/t}</h1>

{if $object == "consultationnotes"}
<p>{t}Consultation Notes{/t}</p>
{else}
<p>{t}Bible{/t}: {$object}</p>
{/if}
<p>{t}Repository URL{/t}: {$url}</p>

{section name=offset loop=$contents}
  <p><code>{$contents[offset]}</code></p>
{/section}
