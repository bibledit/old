<h1>{t}Exports{/t}</h1>
<p>{t}Bibledit-Web exports the Bibles on a daily basis.{/t}</p>
<br>
<p>{t}The following Bibles will be exported{/t}:
{section name=offset loop=$bibles}
  {$bibles[offset]} 
  <a href="exports.php?removebible={$bibles[offset]}">[{t}remove{/t}]</a>
  |
{/section}
<a href="exports.php?addbible=">[{t}add Bible{/t}]</a>
</p>
<br>
<p><a href="exports.php?generate=">{t}Do a fresh export now, and every day at this same time.{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p><a href="../downloads/exports">{t}View the exported data in the downloads section.{/t}</a></p>
<br>
<p>{t}The exports use the following stylesheet:{/t} {$stylesheet} [<a href="exports.php?sheet=">{t}switch{/t}</a>].</p>
