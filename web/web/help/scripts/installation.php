<h1><?php echo gettext ("Installation") ?></h1>
<p><?php echo gettext ("Here are some guidelines as to how to install Bibledit-Web.") ?></p>
<p><a href="installubuntu.php"><?php echo gettext ("Version 0.9 on Ubuntu 13.04.") ?></a></p>
<p><a href="installnewubuntu.php"><?php echo gettext ("Version 1.0 on Ubuntu 13.04.") ?></a></p>

<h2><?php echo gettext ("Version being developed") ?></h2>
<p><?php echo gettext ("In case you'd like to use the newest version, the one being developed, you can clone the source code from the git repository.") ?></p>
<p><a href="http://sites.google.com/site/bibledit/community/development"><?php echo gettext ("See the Development page for more details.") ?></a></p>
<p><?php echo gettext ("Once you have cloned the source code, and have a local repository for the bibledit code, you can then copy directory web/web into your local web server, and get it up and running there. A web server is needed, PHP, and MySQL. If you have copied the web/web directory to, for example, http://localhost/bibledit, then to install the program and create all the databases, you need to run http://localhost/bibledit/setup.php once.") ?></p>

<h2><?php echo gettext ("Open installation") ?></h2>
<p><?php echo gettext ("You can configure the Bibledit-Web installation to be completely open.") ?></p>
<pre>./configure --with-open-installation=true</pre>
<p>
<?php echo gettext ("It means that there is no need to login to the website.") ?> 
<?php echo gettext ("The website appears to be logged in as user 'admin' for any visitor.") ?> 
<?php echo gettext ("This is useful for demo installations, or installations providing a free service to the the community.") ?>
</p>

<h2><?php echo gettext ("Sword module repository") ?></h2>
<p>
  <?php echo gettext ("Bibledit-Web can export Bibles to a Sword module.") ?> 
  <?php echo gettext ("It will store the module in a location accessible through the web server.") ?> 
  <?php echo gettext ("You can also create a Sword module repository.") ?> 
  <?php echo gettext ("This enables users to do an online installation of the Bible in their Sword front-end.") ?> 
  <?php echo gettext ("To create a Sword module repository, install an anonymous FTP server, and set the root for anonymous access to the folder with exported Bibles.") ?> 
</p>
<p><a href="ftpserver.php"><?php echo gettext ("Install FTP server on Ubuntu Server.") ?></a></p>
