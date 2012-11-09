<h1>{t}Management{/t}</h1>
<p><a href="../desktop/index.php?desktop=editusfm">{t}Edit Bible in USFM code{/t}</a></p>
<p><a href="sendreceive.php">{t}Send and Receive{/t}</a></p>
<p><a href="searching.php">{t}Searching{/t}</a></p>
<p><a href="logbook.php">{t}Logbook{/t}</a></p>
<p><a href="changes.php">{t}Changes{/t}</a></p>
<p><a href="users.php">{t}Users{/t}</a></p>
<p><a href="../bible/manage.php">{t}Bibles{/t}</a></p>
<p><a href="exports.php">{t}Exports{/t}</a></p>
<p><a href="../versification/index.php">{t}Versifications{/t}</a></p>
<p><a href="../styles/indexm.php">{t}Styles{/t}</a></p>
<p><a href="../notes/manage.php">{t}Consultation Notes{/t}</a></p>
<p><a href="hyphenation.php">{t}Hyphenation{/t}</a></p>
{foreach key=key item=item from=$plugins} 
  <p><a href={$key}>{$item}</a></p>
{/foreach} 
