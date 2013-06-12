<h1>{t}Installation{/t}</h1>
<p>{t}Here are some guidelines as to how to install Bibledit-Web.{/t}</p>
<p><a href="installubuntu.php">{t}Version 0.9 on Ubuntu 13.04.{/t}</a></p>
<p><a href="installnewubuntu.php">{t}Version 1.0 (not yet released) on Ubuntu 13.04.{/t}</a></p>

<h2>{t}Version being developed{/t}</h2>
<p>{t}In case you'd like to use the newest version, the one being developed, you can clone the source code from the git repository.{/t}</p>
<p><a href="http://sites.google.com/site/bibledit/community/development">{t}See the Development page for more details.{/t}</a></p>
<p>{t}Once you have cloned the source code, and have a local repository for the bibledit code, you can then copy directory web/web into your local web server, and get it up and running there. A web server is needed, PHP, and MySQL. If you have copied the web/web directory to, for example, http://localhost/bibledit, then to install the program and create all the databases, you need to run http://localhost/bibledit/setup.php once.{/t}</p>

<h2>{t}Open installation{/t}</h2>
<p>{t}You can configure the Bibledit-Web installation to be completely open.{/t}</p>
<pre>./configure --with-open-installation=true</pre>
<p>
{t}It means that there is no need to login to the website.{/t} 
{t}The website appears to be logged in as user "admin" for any visitor.{/t} 
{t}This is useful for demo installations, or installations providing a free service to the the community.{/t}
</p>

<h2>{t}Sword module repository{/t}</h2>
<p>
  {t}Bibledit-Web can export Bibles to a Sword module.{/t} 
  {t}It will store the module in a location accessible through the web server.{/t} 
  {t}You can also create a Sword module repository.{/t} 
  {t}This enables users to do an online installation of the Bible in their Sword front-end.{/t} 
  {t}To create a Sword module repository, install an anonymous FTP server, and set the root for anonymous access to the folder with exported Bibles.{/t} 
</p>
<p><a href="ftpserver.php">{t}Install FTP server on Ubuntu Server.{/t}</a></p>
