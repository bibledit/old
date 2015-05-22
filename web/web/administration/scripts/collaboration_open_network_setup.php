<?php
/*
Copyright (©) 2003-2014 Teus Benschop.

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
<h1><?php echo Locale_Translate::_("Open network repository setup") ?></h1>
<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>
<p><?php echo Locale_Translate::_("In this tutorial you are going to set up an open repository on the network.") ?></p>
<p><?php echo Locale_Translate::_("You need a server to run this repository. Any of your GNU/Linux computers attached to your network will do. In this tutorial we assume that the computer is accessible from the network through IP address 192.168.0.1. The user of this computer will be Jim.") ?></p>
<p><?php echo Locale_Translate::_("To create the repository, login as user jim. Then open a terminal. The repository is going to be in Jim's home directory. In the terminal type the following:") ?></p>
<pre>
cd
mkdir -p git/repository
cd git/repository
git --bare init --shared
</pre>
<p><?php echo Locale_Translate::_("The repository is now there, but it is not yet accessible from the network.") ?></p>
<p><?php echo Locale_Translate::_("To make the repository available to the network, type:") ?></p>
<pre>git daemon --export-all --base-path=/home/jim/git --detach --enable=receive-pack</pre>
<p><?php echo Locale_Translate::_("Each time the server boots, it is necessary to execute the above command again. It would be easier to put the command in the boot script. Then it will automatically be started after boot.") ?></p>
<p><?php echo Locale_Translate::_("To test access to the new repository from another computer on the network, type:") ?></p>
<pre>git ls-remote git://192.168.0.1/repository</pre>
<p><?php echo Locale_Translate::_("If everything is fine, no errors will occur.") ?></p>

<a name="url"></a>
<h1><?php echo Locale_Translate::_("Repository URL") ?></h1>
<p><?php echo Locale_Translate::_("The repository that was created in this tutorial has the following URL:") ?></p>
<pre>git://192.168.0.1/repository</pre>
<p><?php echo Locale_Translate::_("Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.") ?></p>
<form action="collaboration_open_network_setup.php?object=<?php echo $this->object ?>#url" name="url" method="post">
  <p><input type="text" name="urlvalue" value="<?php echo $this->url ?>" /></p>
  <p><input type="submit" name="url" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>

<h2><a href="collaboration_repo_read.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Next page") ?></a></h2>
