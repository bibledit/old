<h1>{t}Timezone{/t}</h1>
<p>{t}Here you can set up your own timezone.{/t}</p>
<p>{t}All dates and times will then be shown as relevant to your timezone.{/t}</p>
{if $timezone != ""}
  <p>{t}Current timezone:{/t} {$timezone}.</p>
{else}
  <p>{t}Your timezone has not yet been set.{/t}</p>
{/if}
<p><a href="timezone.php?timezone=">{t}Change the timezone.{/t}</a></p>
