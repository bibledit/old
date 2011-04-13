<h1>{t}Hyphenation{/t}</h1>
<p>{t}Hyphenation can be used when preparing a Bible for printing.{/t}</p>
<p>{t}It allows for more efficient usage of paper, and for more regular appearance of margins at the end of the lines without requiring spacing adjustments.{/t}</p>
<p>{t}The details of hyphenating properly are complex and dependent on the language, and can interact with other orthographic and typesetting practices.{/t}</p>
<p>{t}Hyphenation may not be so important when preparing a Bible for proofreading. It is usually regarded important for production quality Bibles.{/t}</p>
<br>
<p>{t}When preparing a Bible in LibreOffice, OpenOffice, AbiWord, en so on, the wordprocessor itself provides hyphenation algorithms for many languages.{/t}</p>
<p>{t}If no suitable hyphenation algorithm can be used, perhaps Bibledit-Web can assist to a limited degree.{/t}</p>
<br>
<p>{t}Bibledit-Web can take the data from a Bible, insert soft hyphens in the words according to a certain algorithm, and then save this data as a new Bible.{/t}</p>
<p>{t}This new Bible can then be exported to OpenDocument format, and opened in a wordprocessor.{/t}</p>
<p>{t}The wordprocessor sees these soft hyphens. When a soft hyphen occurs near the end of the line, it breaks the word there.{/t}</p>
<br>
<p>{t}Bibledit-Web has one hyphenating algorithm. When there is a transition in the text from any character of a first set of characters, to any character of a second set of characters, then the soft hyphen will be inserted at that location in the text.{/t}</p>
<br>
<a name="sets"></a>
<form action="hyphenation.php?bible={$bible}#sets" method="post">
<p>{t}First set of characters{/t}:</p>
<p><input type="text" name="firstset" value="{$firstset}" class="fullwidth" /></p>
<p>{t}Second set of characters{/t}:</p>
<p><input type="text" name="secondset" value="{$secondset}" class="fullwidth" /></p>
<p>{t}Enter the sets of characters, and press Save.{/t}</p>
<p><input type="submit" name="sets" value={t}Save{/t} /></p>
</form>
<br>
<p>{t}The Bible where the data will be taken from is:{/t} {$bible} <a href="hyphenation.php?bible={$bible}&changebible=">[{t}change{/t}]</a></p>
{if $bible != ""}
<p>{t}The Bible where the hyphenated data will be written to is:{/t} {$bible}-hyphenated</p>
{/if}
<br>
<p><a href="hyphenation.php?bible={$bible}&run=">{t}Insert the soft hyphens according to the settings.{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
