<h1>{t}Software update{/t}</h1>
<p>{t}You can update the Bibledit-Web software here. To do it successfully, follow the steps carefully. Be sure to end up with a web site that keeps working after this update.{/t}</p>
<br>
<p>{t}The current version of the Bibledit-Web software is:{/t} {$version}</p>
<p>{t}You only can install newer software. This is because there is an upgrade path from older to newer versions, but not the other way around.{/t}</p>
<p>{t}The Bibledit-Web software is available from:{/t} <a href="http://download.savannah.nongnu.org/releases/bibledit/source/web" target="_blank">http://download.savannah.nongnu.org/releases/bibledit/source/web</a></p>
<p>{t}The software is stored in a file that looks like this:{/t} <code>bibledit-web-x.x.tar.gz</code>. {t}The x.x is a version number.{/t}</p>
<br>
<p>{t}The next thing to do is to get this file. Enter the URL and click Get.{/t}</p>
<form action="update2.php" name="form" method="POST">
  <p>
  <input type="text" name="uri" />
  <input type="submit" name="submit" value="{t}Get{/t}" onClick="this.value = '{t}Please wait{/t}'; return true;" />
  </p>
</form>
<p>{t}Getting the file may take some time.{/t}</p>
<br>
<p>{t}Sample URLs:{/t}</p>
<p>http://download.savannah.nongnu.org/releases/bibledit/source/web/bibledit-web-0.2.tar.gz</p>
<p>file:///home/joe/bibledit-web-0.2.tar.gz</p>
<p>ftp://user:password@site.com/bibledit-web-0.2.tar.gz</p>
