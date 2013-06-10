<h1>{t}Exports{/t}</h1>
<p>{t}Bibledit-Web exports the Bibles every day at midnight.{/t}</p>
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
<p><a href="exports.php?generate=">{t}Do a fresh export now.{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p><a href="../downloads/exports">{t}View the exported data in the downloads section.{/t}</a></p>
<br>
<p>{t}The exports use the following stylesheet:{/t} {$stylesheet} [<a href="exports.php?sheet=">{t}switch{/t}</a>].</p>
<br>
<p>
<a href="exports.php?dropcapstoggle=">{if $dropcaps} ☑ {else} ☐ {/if}</a>
{t}Use frames for OpenDocument format in case the chapter number gets put just before the first verse in drop caps.{/t}  
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
<a href="exports.php?dateinheadertoggle=">{if $dateinheader} ☑ {else} ☐ {/if}</a>
{t}Display the current date in the running headers.{/t}
</p>
<br>
<p>
 {t}It also exports to a file suitable for the Online Bible module creation process.{/t}
 {t}For more information about how to create a module, see Online Bible, menu Help - Table Of Contents - How To - Create Your Own Bible Version.{/t}
</p>
<br>
<p>
 {t}You can edit the configuration file used for exporting to a Sword module.{/t}
 {t}Update the text below, and click button Save.{/t}
</p>
<p>{t}There are two special variables:{/t}</p>
<p>&lt;bible&gt;: {t}This will be replaced with the name of the Bible being exported.{/t}</p>
<p>&lt;date&gt;: {t}This will be replaced with the date of export.{/t}</p>
<form action="exports.php" name="formswordconfig" method="post">
  <p><textarea name="swordconfig" class="fullwidth">{$swordconfig}</textarea></p>
  <p><input type="submit" name="submitswordconfig" value={t}Save{/t} /> <a href="exports.php?resetswordconfig=">{t}Reset{/t}</a></p>
</form>
<p>
 {t}{/t}
 {t}{/t}
 {t}{/t}
</p>
