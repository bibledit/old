<h1>{t}Settings for Checks{/t}</h1>
<p>{t}Bibledit-Web runs the checks on the Bibles every day just after midnight.{/t}</p>
<br>
<p><a href="settings.php?run=">{t}Run the checks now{/t}</a></p>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<br>
<p>{t}The following Bibles will be checked{/t}:
{section name=offset loop=$bibles}
  {$bibles[offset]} 
  <a href="settings.php?removebible={$bibles[offset]}">[{t}remove{/t}]</a>
  |
{/section}
<a href="settings.php?addbible=">[{t}add Bible{/t}]</a>
</p>
<br>
<p>{t}Enable any of the following checks to run them regularly:{/t}</p>

<p>
<a href="settings.php?doublespacesusfm=">
{if $double_spaces_usfm == true} ☑ {else} ☐ {/if}
</a>
{t}double spaces in the raw USFM{/t}
</p>

<p>
<a href="settings.php?fullstopheadings=">
{if $full_stop_headings == true} ☑ {else} ☐ {/if}
</a>
{t}punctuation at end of heading{/t}
</p>

<p>
<a href="settings.php?spacebeforepunctuation=">
{if $space_before_punctuation == true} ☑ {else} ☐ {/if}
</a>
{t}a space before a comma, semicolon, colon, full stop, question mark, or exclamation mark{/t}
</p>

<p>
<a href="settings.php?sentencestructure=">
{if $sentence_structure == true} ☑ {else} ☐ {/if}
</a>
{t}sentence structure{/t}
</p>

<p>
<a href="settings.php?chaptersversesversification=">
{if $chapters_verses_versification == true} ☑ {else} ☐ {/if}
</a>
{t}correct number of books, chapters, and verses, according to the versification system{/t}
</p>

<br>
<p><a href="settingssentences.php">{t}Settings for the sentence structure{/t}</a></p>
