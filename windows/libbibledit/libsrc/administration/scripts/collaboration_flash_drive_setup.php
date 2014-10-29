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
<h1><?php echo Locale_Translate::_("USB flash drive repository setup") ?></h1>
<p><?php echo Locale_Translate::_("Bible") ?>: <?php echo $this->object ?></p>
<p><?php echo Locale_Translate::_("In this section you will set up a distributed repository on a USB flash drive.") ?></p>

<h1><?php echo Locale_Translate::_("Formatting the flash drive") ?></h1>
<p><?php echo Locale_Translate::_("Normally a USB flash drive, as it comes out of the box, has been formatted with a FAT filesystem. Such a filesystem is not supported for collaboration. The stick needs to formatted to the ext3 or ext4 filesystem for collaboration to work. How this is done will be described here.") ?></p>
<p><?php echo Locale_Translate::_("Insert the flash drive into a computer running GNU/Linux, and wait a little till it gets mounted. It is supposed to mount automatically. If it does not mount on its own, then automatic mounting needs to be set up. Setting up automatic mounting goes beyond the scope of this tutorial.") ?></p>
<p><?php echo Locale_Translate::_("Open a terminal. In the terminal, give the command:") ?></p>
<pre>df</pre>
<p><?php echo Locale_Translate::_("The output of this command can be something like:") ?></p>
<pre>
Filesystem 1K-blocks      Used Available Use% Mounted on
/dev/sda1   93539428  76882380  11905428  87% /
tmpfs        1038428         0   1038428   0% /lib/init/rw
udev           10240       104     10136   2% /dev
tmpfs        1038428         0   1038428   0% /dev/shm
/dev/hda      629742    629742         0 100% /media/cdrom0
/dev/sdb5  306619956 189335920 101708620  66% /media/293gig
/dev/sdc1  240362656  64350876 163801980  29% /media/250gb
/dev/sdd1    1960684    660896   1200188  36% /media/usbdisk
</pre>
<p><?php echo Locale_Translate::_("Looking through the list of mounting points, we assume that the USB flash drive that was inserted mounts at /media/usbdisk. From there we can see that the device is /dev/sdd1. Once we have that information, we are ready for the next step.") ?></p>
<p><?php echo Locale_Translate::_("Before the stick can be formatted, it must be unmounted first. Issue the following command to unmount the stick:") ?></p>
<pre>sudo umount /media/usbdisk</pre>
<p><?php echo Locale_Translate::_("The 'sudo' in the command means that root privileges are needed to execute the command.") ?></p>
<p><?php echo Locale_Translate::_("To format the USB drive to the ext3 filesystem, issue the following command:") ?></p>
<pre>sudo mkfs.ext3 /dev/sdd1</pre>
<p><?php echo Locale_Translate::_("The formatting process will be visible and at the end some information about it will be given. At this point it is important to see if no errors occurred.") ?></p>
<p><?php echo Locale_Translate::_("The formatting command is very powerful. One could even format the main drive of the computer, wiping out all information. Extreme care should be used to ensure that the USB flash drive is going to be formatted, and nothing else.") ?></p>

<h1><?php echo Locale_Translate::_("Labelling the flash drive") ?></h1>
<p><?php echo Locale_Translate::_("An internal label needs to be written to the USB drive. The reason of this is so as to make sure that this drive will always mount at the same mounting point. In this tutorial we write the label 'usbstick' to it.") ?></p>
<p><?php echo Locale_Translate::_("We take it that the same device is being used as we used for formatting the drive. That is device /dev/sdd1. To write the label to the drive, issue the following two commands:") ?></p>
<pre>sudo e2label /dev/sdd1 usbstick</pre>
<pre>sync</pre>
<p><?php echo Locale_Translate::_("After the label has been written to it, remove the USB flash drive from the computer, and insert it again. It should now mount to the name of the label. In this case it should mount under /media/usbstick.") ?></p>
<p><?php echo Locale_Translate::_("This can be verified with command 'df':") ?></p>
<pre>df</pre>
<p><?php echo Locale_Translate::_("The output of this command:") ?></p>
<pre>
Filesystem 1K-blocks      Used Available Use% Mounted on
/dev/sda1   93539428  76882380  11905428  87% /
tmpfs        1038428         0   1038428   0% /lib/init/rw
udev           10240       104     10136   2% /dev
tmpfs        1038428         0   1038428   0% /dev/shm
/dev/hda      629742    629742         0 100% /media/cdrom0
/dev/sdb5  306619956 189335920 101708620  66% /media/293gig
/dev/sdc1  240362656  64350876 163801980  29% /media/250gb
/dev/sdd1    1960684    660896   1200188  36% /media/usbstick
</pre>
<p><?php echo Locale_Translate::_("This shows that it now mounts under the new name.") ?></p>

<h1><?php echo Locale_Translate::_("Creating the repository on the flash drive") ?></h1>
<p><?php echo Locale_Translate::_("The USB flash drive will hold the data repository.") ?></p>
<p><?php echo Locale_Translate::_("To make this flash drive fit for collaboration, there are a few steps to be taken. These steps are:") ?></p>
<p><?php echo Locale_Translate::_("1. The first step will be to create a directory for the repository, and to make that directory writable. If we assume that the flash drive mounts at /media/usbstick, then the command to create a directory called 'repository' is:") ?></p>
<pre>sudo mkdir /media/usbstick/repository</pre>
<p><?php echo Locale_Translate::_("Notice the ' sudo'  command, which means that this usually needs to be done by user root. If this is done by an ordinary user, permission would be denied.") ?></p>
<p><?php echo Locale_Translate::_("Once the directory for the repository is there, this directory needs to be made writable for ordinary users. This is the command:") ?></p>
<pre>sudo chmod -R 0777 /media/usbstick/repository/</pre>
<p><?php echo Locale_Translate::_("Note the 'sudo' again.") ?></p>
<p><?php echo Locale_Translate::_("Now it is time to check whether the USB drive is writable. We do this by trying to create a file in the repository. If this succeeds, then it is writable. Here's the command:") ?></p>
<pre>touch /media/usbstick/repository/testfile</pre>
<p><?php echo Locale_Translate::_("This command should complete without errors. After that the file needs to be removed again:") ?></p>
<pre>rm /media/usbstick/repository/testfile</pre>
<p><?php echo Locale_Translate::_("2. The second step is to make the USB stick the current working directory. Type:") ?></p>
<pre>cd /media/usbstick/repository</pre>
<p><?php echo Locale_Translate::_("3. The next step is to create a repository on the USB stick.") ?></p>
<p><?php echo Locale_Translate::_("To create a shared repository on the USB stick, type:") ?></p>
<pre>git --bare init --shared</pre>
<p><?php echo Locale_Translate::_("Git will respond saying that an empty shared Git repository has been created.") ?></p>
<p><?php echo Locale_Translate::_("4. The last step is to make the repository accessible to the web server's user. To do this, type the following two commands:") ?></p>
<pre>sudo chown -R <?php echo $this->username ?>:<?php echo $this->username ?> *</pre>
<pre>sudo chmod -R 0777 *</pre>
<p><?php echo Locale_Translate::_("All steps have now been taken. Before removing the flash drive, ensure that the data gets written to it, by issuing the command:") ?></p>
<pre>sync</pre>
<p><?php echo Locale_Translate::_("This finishes setting up the repository on the USB flash drive.") ?></p>

<a name="url"></a>
<h1><?php echo Locale_Translate::_("Repository URL") ?></h1>
<p><?php echo Locale_Translate::_("The repository that was created in this tutorial has the following URL:") ?></p>
<pre>file:///media/usbstick/repository</pre>
<p><?php echo Locale_Translate::_("Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.") ?></p>
<form action="collaboration_flash_drive_setup.php?object=<?php echo $this->object ?>#url" name="url" method="post">
  <p><input type="text" name="urlvalue" value="<?php echo $this->url ?>" /></p>
  <p><input type="submit" name="url" value=<?php echo Locale_Translate::_("Submit") ?> /></p>
</form>

<h2><a href="collaboration_repo_read.php?object=<?php echo $this->object ?>"><?php echo Locale_Translate::_("Next page") ?></a></h2>
