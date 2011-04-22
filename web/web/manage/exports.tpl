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
{t}When exporting Bibles to OpenDocument format, when the chapter number gets put just before the first verse in bigger font, frames are used for this. It is to achieve better appearance. When this is problematic, Bibledit-Web can also output the chapter number in drop caps.{/t}
{if $dropcaps}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="exports.php?dropcapstoggle=">[{t}toggle{/t}]</a></p>
</p>
<br>
<p>{t}Page size and margins are relevant for export to a format that supports those.{/t}</p>
<table>
  <tr><td>{t}Page width{/t}</td><td><a href="exports.php?pagewidth=">{$pagewidth} {t}mm{/t}</a></td></tr>
  <tr><td>{t}Page height{/t}</td><td><a href="exports.php?pageheight=">{$pageheight} {t}mm{/t}</a></td></tr>
  <tr><td>{t}Inner margin{/t}</td><td><a href="exports.php?innermargin=">{$innermargin} {t}mm{/t}</a></td></tr>
  <tr><td>{t}Outer margin{/t}</td><td><a href="exports.php?outermargin=">{$outermargin} {t}mm{/t}</a></td></tr>
  <tr><td>{t}Top margin{/t}</td><td><a href="exports.php?topmargin=">{$topmargin} {t}mm{/t}</a></td></tr>
  <tr><td>{t}Bottom margin{/t}</td><td><a href="exports.php?bottommargin=">{$bottommargin} {t}mm{/t}</a></td></tr>
</table>
<br>
<p>
{t}Display the current date in the running headers.{/t}
{if $dateinheader}
  {t}This setting is on.{/t}
{else}
  {t}This setting is off.{/t}
{/if}
  <a href="exports.php?dateinheadertoggle=">[{t}toggle{/t}]</a></p>
</p>
<br>
{t}It also exports to a file suitable for the Online Bible module creation process. For more information about how to create a module, see Online Bible, menu Help - Table Of Contents - How To - Create Your Own Bible Version.{/t}
