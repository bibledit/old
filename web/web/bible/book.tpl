<h1>{$book_name}</h1>
<p class="success">{$success_message}</p>
<p class="error">{$error_message}</p>
<p>{t}Bible{/t}: <a href="settings.php?bible={$bible}">{$bible}</a></p>
<p>{t}Chapters:{/t}
{section name=offset loop=$chapters}
  <a href="chapter.php?bible={$bible}&book={$book}&chapter={$chapters[offset]}">{$chapters[offset]}</a>
{/section}
</p>
<p><a href="book.php?bible={$bible}&book={$book}&createchapter=">{t}Create chapter{/t}</a></p>
<p><a href="settings.php?bible={$bible}&deletebook={$book}">{t}Delete this book{/t}</a></p>
