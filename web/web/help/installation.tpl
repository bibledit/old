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
  {t}This is how it can be done on Ubuntu Server:{/t} 
</p>
<br>
<p>{t}Install the FTP server:{/t}</p>
<pre>$ sudo apt-get install vsftpd</pre>
<p>{t}Create the ftp user:{/t}</p>
<pre>$ sudo useradd ftp</pre>
<p>{t}Edit file /etc/vsftpd.conf and make the settings below:{/t}</p>
<pre>
anonymous_enable=YES
local_enable=NO
no_anon_password=YES
anon_root=/var/lib/bibledit-web/bibledit-web/exports
</pre>
<p>{t}Restart the FTP server:{/t}</p>
<pre>$ sudo service vsftpd restart</pre>
<p>
  {t}The Sword module repository it now ready for use.{/t} 
  {t}You can add a remote source to your Sword front-end.{/t} 
  {t}Bibledit-Web creates four flavours of the Sword module, so there will be four remote sources.{/t} 
  {t}If bibledit-web exports a Bible called "ndebele", then the following four remote sources will work:{/t} 
</p>
<table>
  <tr>
    <td>{t}Host:{/t}</td>
    <td>yourwebsite.org</td>
  </tr>
  <tr>
    <td>{t}Directory 1:{/t}</td>
    <td>/ndebele/sword/basic-1</td>
  </tr>
  <tr>
    <td>{t}Directory 2:{/t}</td>
    <td>/ndebele/sword/basic-2</td>
  </tr>
  <tr>
    <td>{t}Directory 3:{/t}</td>
    <td>/ndebele/sword/full-1</td>
  </tr>
  <tr>
    <td>{t}Directory 4:{/t}</td>
    <td>/ndebele/sword/full-2</td>
  </tr>
</table>
<br>
<p>{t}If vsftpd does not work for you, any other FTP server will do the job also:{/t} tinyftp, ftpd, pure-ftpd...</p>
