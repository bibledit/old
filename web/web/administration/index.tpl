<h1>{t}Administration{/t}</h1>
{*<p><a href="update.php">{t}Update{/t}</a></p>*}
<p><a href="mail.php">{t}Mail{/t}</a></p>
<p><a href="backup.php">{t}Backup{/t}</a></p>
<p><a href="collaboration.php">{t}Collaboration{/t}</a></p>
<p><a href="database.php">{t}Database{/t}</a></p>
<p><a href="timezone.php">Timezone</a></p>
<p><a href="phpinfo.php">PHP Info</a></p>
{foreach key=key item=item from=$plugins} 
  <p><a href={$key}>{$item}</a></p>
{/foreach} 
