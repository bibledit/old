<h1>{t}FTP server{/t}</h1>
<p>
  {t}This page describes installation of two dfferent FTP servers.{/t}
  {t}Pick the one that suits you.{/t}
</p>

<h2>vsftpd</h2>
<p>{t}Install the vsftpd FTP server:{/t}</p>
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

<h2>pure-ftpd</h2>
<p>{t}Install the pure-ftpd FTP server:{/t}</p>
<pre>$ sudo apt-get install pure-ftpd</pre>
<p>{t}It will log to:{/t} /var/log/pure-ftpd/transfer.log</p>
<p>{t}Enable anonymous access and secure the server:{/t}</p>
<pre>
$ sudo su
$ cd /etc/pure-ftpd/conf
$ echo no > NoAnonymous
$ echo no > PAMAuthentication
$ echo no > PureDB
$ echo 0 > TrustedGID
$ exit
</pre>
<p>
  {t}Users with a local account normally can login through FTP.{/t}
  {t}Disable that for security reasons.{/t}
  {t}Edit file /etc/ftpusers, and add the local accounts to that file.{/t}
</p>
<pre>$ sudo nano /etc/ftpusers</pre>
<p>{t}The anonymous users will read from:{/t} /srv/ftp</p>
<p>{t}Update the root folder for anonymous access:{/t}</p>
<pre>$ sudo usermod -d /var/lib/bibledit-web/bibledit-web/exports/ ftp</pre>
<p>{t}Restart the FTP server:{/t}</p>
<pre>$ sudo service pure-ftpd restart</pre>

<h2>{t}Sword module repository{/t}</h2>
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
<p><a href="http://www.crosswire.org/wiki/Creating_and_Maintaining_a_Module_Repository">{t}More information{/t}</a></p>
