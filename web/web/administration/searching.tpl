<h1>{t}Searching{/t}</h1>
<p>{t}Bibledit-Web uses Sphinx Search for most of its searching functions.{/t}</p>
<p>{t}Sphinx Search needs a local network port number to operate.{/t}</p>
<p>{t}Normally the default port number is okay.{/t}</p>
<p>{t}In case more than one instance of Bibledit-Web run on the same server, each instance needs its own port number.{/t}</p>
<p>{t}The port number below is used for the main search function.{/t}</p>
<p>{t}Each additional exported Bible will use the next port number.{/t}</p>
<p>{t}Thus, if five Bibles are exported, Bibledit-Web uses the port number below, plus the five subsequent ones.{/t}</p>
<br>
<p class="error">{$error}</p>
<p class="success">{$success}</p>
<form action="searching.php" name="search" method="post">
  <p><input type="text" name="port" maxlength="5" value="{$port}" /></p>
  <p><input type="submit" name="submit" value={t}Submit{/t} /></p>
</form>
<br>
<p>{t}Update the port number and press Submit.{/t}</p>
<p>{t}To reset the number to default, enter nothing and press Submit.{/t}</p>
<p>{t}The updated port number will take effect next time the site is indexed. Normally this is at midnight.{/t}</p>
