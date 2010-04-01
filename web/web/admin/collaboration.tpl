<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd" >
<html xmlns="http://www.w3.org/1999/xhtml">
  <head>
    <meta http-equiv="content-type" content="text/html; charset=UTF-8"></meta>
    <title>{t}Collaboration{/t}</title>
    <link rel="stylesheet" type="text/css" href="../css/stylesheet.css"></link>
  </head>
  <body>
  {include file=../assets/header_full.tpl} 

  <h1>1 {t}Introduction{/t}</h1>
  <p>{t}A brief introduction to the system.{/t}</p>

  <h2>1.1 {t}Web collaboration{/t}</h2>
  <p>{t}With Bibledit-Web several translators can work on one Bible at the same time.{/t}</p>  
  <p>{t}There is nothing special needed to make this happen.{/t}</p>  
  <p>{t}The translators log into the site that runs Bibledit-Web, and translate there.{/t}</p>  
  <p>{t}All the data related to the Bible is saved on that website, and is available to all translators.{/t}</p>  
  <p>{t}When collaborating in this way, the rest of this page below is not relevant.{/t}</p>  

  <h2>1.2 {t}Distributed collaboration{/t}</h2>
  <p>{t}With distributed collaboration there are several copies of the Bible all distributed over the place or the globe.{/t}</p>  
  <p>{t}Translators that collaborate in this way do not need to be connected to the Internet all the time, or even not at all.{/t}</p>  
  <p>{t}The translators collaborate through the network or a USB drive.{/t}</p>  
  <p>{t}Once everything has been set up, it all goes automatic. The translator does not notice the mechanics of the collaboration. It all happens under the hood.{/t}</p>  
  <p>{t}There are a variety of methods for collaboration. The administrator who will set it all up needs to have sufficient knowledge of information technology.{/t}</p>  
  <p>{t}Bibledit-Web stores all data locally. When collaboration is enabled, it also connects to a central repository. Through that repository the data gets shared between the translators.{/t}</p>  
  <p>{t}Under the hood, Bibledit uses Git for collaboration. Git is a content manager. The translator does not notice that Git is used. He does not operate it directly. The only thing that the translator needs to do at regular times is to "Send/Receive" the Bible. After having done that, the changes in the Bible, made by the other translators, appear on his screen too.{/t}</p>  
  <p>{t}There is a variety of repositories that can be used. Each of them has it own use, advantages and disadvantages.{/t}</p>  
  <ul>
    <li><em>{t}USB Flash Drive.{/t}</em> {t}First of all there is the repository that resides on a file system. A USB flash drive can be used in this case. This flash drive can be physically moved from one translator to the other, and so on, updating each translator, and getting updated by each translator.{/t}</li>
    <li><em>{t}Open Network Repository.{/t}</em> {t}Secondly there is the repository that resides on the network. For internal networks a simple open network repository can be used. Whoever knows where the repository resides can write to it.{/t}</li>
    <li><em>{t}Secure Network Repository.{/t}</em> {t}For public networks such as the internet a secure network repository would be recommended. This type of repository is locked down. Only those who have the keys to such a repository can write to it.{/t}</li>
  </ul> 

  <h1>2 {t}Repository setup{/t}</h1>
  <p>{t}The central part to distributed collaboration is the repository. This section helps you, the systems administrator, to set the repository up.{/t}</p>
  <p>{t}There are several types of repositories. See the introduction above. Each of them will be described under separate headers in this tutorial.{/t}</p>

  <h2>2.1 {t}USB flash drive repository setup{/t}</h2>

  <h3>{t}Introduction{/t}</h3>
  <p>{t}In this section you will set up a distributed repository on a USB flash drive.{/t}</p>

  <h3>{t}Formatting the flash drive{/t}</h3>
  <p>{t}Normally a USB flash drive, as it comes out of the box, has been formatted with a FAT filesystem. Such a filesystem is not supported for collaboration. The stick needs to formatted to the ext3 filesystem for collaboration to work. How this is done will be described here.{/t}</p>
  <p>{t}Insert the flash drive into a computer running Linux, and wait a little till it gets mounted. It is supposed to mount automatically. If it does not mount on its own, then automatic mounting needs to be set up. Setting up automatic mounting goes beyond the scope of this tutorial.{/t}</p>
  <p>{t}Open a terminal. In the terminal, give the command:{/t}</p>
  <pre>df</pre>
  <p>{t}The output of this command can be something like:{/t}</p>
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
  <p>{t}Looking through the list of mounting points, we assume that the USB flash drive that was inserted mounts at /media/usbdisk. From there we can see that the device is /dev/sdd1. Once we have that information, we are ready for the next step.{/t}</p>
  <p>{t}Before the stick can be formatted, it must be unmounted first. Issue the following command to unmount the stick:{/t}</p>
  <pre>sudo umount /media/usbdisk</pre>
  <p>{t}The "sudo" in the command means that root privileges are needed to execute the command.{/t}</p>
  <p>{t}To format the USB drive to the ext3 filesystem, issue the following command:{/t}</p>
  <pre>sudo mkfs.ext3 /dev/sdd1</pre>
  <p>{t}The formatting process will be visible and at the end some information about it will be given. At this point it is important to see if no errors occurred.{/t}</p>
  <p>{t}The formatting command is very powerful. One could even format the main drive of the computer, wiping out all information. Extreme care should be used to ensure that the USB flash drive is going to be formatted, and nothing else.{/t}</p>

  <h3>{t}Labelling the flash drive{/t}</h3>
  <p>{t}An internal label needs to be written to the USB drive. The reason of this is so as to make sure that this drive will always mount at the same mounting point. In this tutorial we write the label "usbstick" to it.{/t}</p>
  <p>{t}We take it that the same device is being used as we used for formatting the drive. That is device /dev/sdd1. To write the label to the drive, issue the following two commands:{/t}</p>
  <pre>sudo e2label /dev/sdd1 usbstick</pre>
  <pre>sync</pre>
  <p>{t}After the label has been written to it, remove the drive from the computer, and insert it again. It should now mount to the name of the label. In this case it should mount under /media/usbstick.{/t}</p>
  <p>{t}This can be verified with command "df":{/t}</p>
  <pre>df</pre>
  <p>{t}The output of this command:{/t}</p>
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
  <p>{t}This shows that it now mounts under the new name.{/t}</p>
  
  <h3>{t}Creating the repository on the flash drive{/t}</h3>
  <p>{t}The USB flash drive will hold the data repository.{/t}</p>
  <p>{t}To make this flash drive fit for collaboration, there are a few steps to be taken. These steps are:{/t}</p>
  <p>{t}1. The first step will be to create a directory for the repository, and to make that directory writable. If we assume that the flash drive mounts at /media/usbstick, then the command to create a directory called "repository" is:{/t}</p>
  <pre>sudo mkdir /media/usbstick/repository</pre>
  <p>{t}Notice the "sudo" command, which means that this usually needs to be done by user root. If this is done by an ordinary user, permission would be denied.{/t}</p>
  <p>{t}Once the directory for the repository is there, this directory needs to be made writable for ordinary users. This is the command:{/t}</p>
  <pre>sudo chmod -R 0777 /media/usbstick/repository/</pre>
  <p>{t}Note the "sudo" again.{/t}</p>
  <p>{t}Now it is time to check whether the USB drive is writable. We do this by trying to create a file in the repository. If this succeeds, then it is writable. Here's the command:{/t}</p>
  <pre>touch /media/usbstick/repository/testfile</pre>
  <p>{t}This command should complete without errors. After that the file needs to be removed again:{/t}</p>
  <pre>rm /media/usbstick/repository/testfile</pre>
  <p>{t}2. The second step is to make the USB stick the current working directory. Type:{/t}</p>
  <pre>cd /media/usbstick/repository</pre>
  <p>{t}3. The next step is to create a repository on the USB stick, and to copy the data to it.{/t}</p>
  <p>{t}To create a shared repository on the USB stick, type:{/t}</p>
  <pre>git --bare init --shared</pre>
  <p>{t}Git will respond saying that an empty shared Git repository has been created.{/t}</p>
  <p>{t}Before removing the stick, ensure that the data gets written to it, by issuing the command:{/t}</p>
  <pre>sync</pre>
  <p>{t}This finishes setting up the repository on the USB flash drive.{/t}</p>

  <h3>{t}Repository URL{/t}</h3>
  <p>{t}The repository that was created in this tutorial has the following URL:{/t}</p>
  <pre>file:///media/usbstick/repository</pre>
  <p>{t}Later, when you will use this repository, you will need this URL.{/t}</p>


  <p>{t}{/t}</p>







  {include file=../assets/footer_full.tpl} 
  </body>
</html>
