<h1>{t}Take local data{/t}</h1>
{if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
{else}
  <p>{t}Bible{/t}: {$object}</p>
{/if}
<p>{t}Repository URL{/t}: {$url}</p>
<p>{t}Your data is being pushed to the remote repository.{/t}</p>
{section name=offset loop=$contents}
  <p><code>{$contents[offset]}</code></p>
{/section}
