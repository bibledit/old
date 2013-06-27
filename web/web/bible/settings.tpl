<h1>{t}Bible{/t} {$bible}</h1>
<p>
  {t}Available books{/t}:
  {section name=offset loop=$book_ids}
    <a href="book.php?bible={$bible}&book={$book_ids[offset]}">{$book_names[offset]}</a>
  {/section}
</p>
<p>{t}Versification system{/t}: <a href="settings.php?bible={$bible}&versification=">{$versification}</a></p>
<p><a href="settings.php?bible={$bible}&createbook=">{t}Create book{/t}</a></p>
<p><a href="import_usfm.php?bible={$bible}">{t}Import USFM{/t}</a></p>
<p><a href="import_bibleworks.php?bible={$bible}">{t}Import BibleWorks{/t}</a></p>
<p><a href="manage.php?copy={$bible}">{t}Copy this Bible{/t}</a></p>
<p><a href="manage.php?delete={$bible}">{t}Delete this Bible{/t}</a></p>
