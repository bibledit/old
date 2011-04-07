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
<br>
<p>
{t}When exporting a Bible to OpenDocument format, when the chapter number gets put just before the first verse in bigger font, frames are used for this. It is to achieve better appearance. When this is problematic, Bibledit-Web can also output the chapter number in drop caps.{/t}
{if $dropcaps}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="exports.php?dropcapstoggle={$dropcaps}">[{t}toggle{/t}]</a></p>
</p>
