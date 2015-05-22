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
<h1><?php echo Locale_Translate::_("Secure network repository setup") ?></h1>
<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>
<p><?php echo Locale_Translate::_("In this tutorial you are going to set up an secure repository on the network.") ?></p>
<p><?php echo Locale_Translate::_("The tutorial uses a git repository on sourceforge.net.") ?></p>
<p><?php echo Locale_Translate::_("Open an account on http://sourceforge.net. We will use the username 'joe' in this tutorial.") ?></p>
<p><?php echo Locale_Translate::_("Register a new project on your personal account page on sourceforge.net. Let's call this project 'repo'.") ?></p>
<p><?php echo Locale_Translate::_("Enable project feature Git on this project. This provides the repository that we need.") ?></p>
<p><?php echo Locale_Translate::_("The next thing is to try whether you can read the repository properly. To try that, open a terminal and issue the following command:") ?></p>
<code>git ls-remote ssh://joe@git.code.sf.net/p/repo/code</code>
<p><?php echo Locale_Translate::_("This command probably will ask for your password, and the first time it could ask you whether you want to continue connecting. It should show no errors. If it completes without errors, then your secure repository works well.") ?></p>

<a name="url"></a>
<h2><?php echo Locale_Translate::_("Repository URL") ?></h2>
<p><?php echo Locale_Translate::_("The repository that was created in this tutorial has the following URL:") ?></p>
<code>ssh://joe@git.code.sf.net/p/repo/code</code>
<p><?php echo Locale_Translate::_("Where it says 'joe', you would need to fill in the username that can access the account on sourceforge.net. And where it has 'repo' you would need to fill in the name of the project that you created on sourceforge.net.") ?></p>
<p><?php echo Locale_Translate::_("Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.") ?></p>
<form action="collaboration_secure_network_setup.php?object=<?php echo $this->object ?>#url" name="url" method="post">
  <p><input type="text" name="urlvalue" value="<?php echo $this->url ?>" /></p>
  <p><input type="submit" name="url" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>

<a name="keys"></a>
<h2><?php echo Locale_Translate::_("Secure shell keys") ?></h2>
<p><?php echo Locale_Translate::_("Once manual access to the secure repository works, the next step is to set up secure shell keys. These keys allow Bibledit-Web easy access to the secure repository, without the need to type the password each time.") ?></p>

<h3><?php echo Locale_Translate::_("Generating the keys") ?></h3>
<p><?php echo Locale_Translate::_("Keys can be generated with this command:") ?></p>
<pre>ssh-keygen -t dsa -b 1024 -N ''</pre>
<p><?php echo Locale_Translate::_("Enter the private key in your authorized_keys file. See the Internet for more information.") ?></p>

<h3><?php echo Locale_Translate::_("Enter the public key") ?></h3>
<p><?php echo Locale_Translate::_("On your personal account page at sourceforge.net, choose Services, then choose Edit SSH Keys for Shell/CVS. Select all the text of the public key, see above, and copy it to the clipboard. Be sure it is the public key, not the private one. In the screen titled 'SSH Keys', there is a place where it says 'Authorized keys:'. Paste the text of the public key into that space on the web page. After checking that everything looks fine, press button 'Update'. Make sure to enter only the public key, not the private key. Synchronizing the keys into the system has a 10 minute delay.") ?></p>
<p><?php echo Locale_Translate::_("After the delay of 10 minutes has expired you can continue to the next page.") ?></p>

<h2><a href="collaboration_repo_read.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Next page") ?></a></h2>
