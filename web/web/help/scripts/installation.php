<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3 of the License, or
(at your option) any later version.
 
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
  
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
?>
<h1><?php echo gettext ("Installation") ?></h1>
<p><?php echo gettext ("Here are some guidelines as to how to install Bibledit-Web.") ?></p>
<p><a href="installubuntu.php"><?php echo gettext ("Version 1.0 on Ubuntu 13.04.") ?></a></p>
<p><a href="installnewubuntu.php"><?php echo gettext ("Version 1.1 on Ubuntu 13.04.") ?></a></p>

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
  <?php echo gettext ("A disadvantage is that the crawlers of the search engines keep clicking links, and will disturb data and settings.") ?>
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

<h2><?php echo gettext ("Several installations on one server") ?></h2>
<p>
  <?php echo gettext ("It is possible to have several installations of Bibledit-Web on one server. ") ?> 
  <?php echo gettext ("This is how to do it:") ?> 
</p>
<pre>./configure --with-location=location-one && make && sudo make install</pre>
<pre>./configure --with-location=biblename    && make && sudo make install</pre>
<pre>./configure --with-location=numberthree  && make && sudo make install</pre>
<p>
  <?php echo gettext ("This creates installations at the following URL's:") ?> 
</p>
<ul>
<li>http://website.org/location-one</li>
<li>http://website.org/biblename</li>
<li>http://website.org/numberthree</li>
</ul>
<p>
  <?php echo gettext ("Everything is completely separated between the installation.") ?> 
  <?php echo gettext ("This includes all data and the database.") ?> 
  <?php echo gettext ("Every installation has its own independent database.") ?> 
</p>
<p>
  <?php echo gettext ("The --with-location=location option does not take slashes.") ?> 
  <?php echo gettext ("For example, --with-location=/location would fail.") ?> 
  <?php echo gettext ("And --with-location=path/to/location would fail also.") ?> 
</p>
