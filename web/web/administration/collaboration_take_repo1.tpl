<h1>{t}Take data in repository{/t}</h1>
{if $object == "consultationnotes"}
  <p>{t}Consultation Notes{/t}</p>
{else}
  <p>{t}Bible{/t}: {$object}</p>
{/if}
<p>{t}Repository URL{/t}: {$url}</p>
<p>{t}The data in the remote repository is being copied over into Bibledit-Web's data, overwriting the data that was there before.{/t}</p>
{section name=offset loop=$contents}
  <p><code>{$contents[offset]}</code></p>
{/section}
