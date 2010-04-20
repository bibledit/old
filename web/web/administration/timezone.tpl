<h1>{t}Site timezone{/t}</h1>
<p>{t}This page deals with the default timezone for the site.{/t}</p>
<p>{t}Visitors will use this default timezone.{/t}</p>
{if $timezone != ""}
  <p>{t}Current timezone:{/t} {$timezone}.</p>
{else}
  <p>{t}The default timezone has not yet been set.{/t}</p>
{/if}
<p><a href="timezone.php?timezone=">{t}Change the timezone.{/t}</a></p>
<p>{t}Those who have an account on this site can override this by setting their own preferred timezone.{/t}</p>
