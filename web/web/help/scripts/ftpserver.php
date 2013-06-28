<h1><?php echo gettext ("FTP server") ?></h1>
<p>
  <?php echo gettext ("This page describes installation of two dfferent FTP servers.") ?>
  <?php echo gettext ("Pick the one that suits you.") ?>
</p>

<h2>vsftpd</h2>
<p><?php echo gettext ("Install the vsftpd FTP server:") ?></p>
<pre>$ sudo apt-get install vsftpd</pre>
<p><?php echo gettext ("Create the ftp user:") ?></p>
<pre>$ sudo useradd ftp</pre>
<p><?php echo gettext ("Edit file /etc/vsftpd.conf and make the settings below:") ?></p>
<pre>
anonymous_enable=YES
local_enable=NO
no_anon_password=YES
anon_root=/var/lib/bibledit-web/bibledit-web/exports
</pre>
<p><?php echo gettext ("Restart the FTP server:") ?></p>
<pre>$ sudo service vsftpd restart</pre>

<h2>pure-ftpd</h2>
<p><?php echo gettext ("Install the pure-ftpd FTP server:") ?></p>
<pre>$ sudo apt-get install pure-ftpd</pre>
<p><?php echo gettext ("It will log to:") ?> /var/log/pure-ftpd/transfer.log</p>
<p><?php echo gettext ("Enable anonymous access and secure the server:") ?></p>
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
  <?php echo gettext ("Users with a local account normally can login through FTP.") ?>
  <?php echo gettext ("Disable that for security reasons.") ?>
  <?php echo gettext ("Edit file /etc/ftpusers, and add the local accounts to that file.") ?>
</p>
<pre>$ sudo nano /etc/ftpusers</pre>
<p><?php echo gettext ("The anonymous users will read from:") ?> /srv/ftp</p>
<p><?php echo gettext ("Update the root folder for anonymous access:") ?></p>
<pre>$ sudo usermod -d /var/lib/bibledit-web/bibledit-web/exports/ ftp</pre>
<p><?php echo gettext ("Restart the FTP server:") ?></p>
<pre>$ sudo service pure-ftpd restart</pre>

<h2><?php echo gettext ("Sword module repository") ?></h2>
<p>
  <?php echo gettext ("The Sword module repository it now ready for use.") ?> 
  <?php echo gettext ("You can add a remote source to your Sword front-end.") ?> 
  <?php echo gettext ("Bibledit-Web creates four flavours of the Sword module, so there will be four remote sources.") ?> 
  <?php echo gettext ("If bibledit-web exports a Bible called 'ndebele', then the following four remote sources will work:") ?> 
</p>
<table>
  <tr>
    <td><?php echo gettext ("Host:") ?></td>
    <td>yourwebsite.org</td>
  </tr>
  <tr>
    <td><?php echo gettext ("Directory 1:") ?></td>
    <td>/ndebele/sword/basic-1</td>
  </tr>
  <tr>
    <td><?php echo gettext ("Directory 2:") ?></td>
    <td>/ndebele/sword/basic-2</td>
  </tr>
  <tr>
    <td><?php echo gettext ("Directory 3:") ?></td>
    <td>/ndebele/sword/full-1</td>
  </tr>
  <tr>
    <td><?php echo gettext ("Directory 4:") ?></td>
    <td>/ndebele/sword/full-2</td>
  </tr>
</table>
<br>
<p><a href="http://www.crosswire.org/wiki/Creating_and_Maintaining_a_Module_Repository"><?php echo gettext ("More information") ?></a></p>
