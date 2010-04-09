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

  {include file=../assets/install_git.tpl} 

  <h1>{t}Open network repository setup{/t}</h1>
  <h2><a href="collaboration_repo_init.php?bible={$bible}">{t}Next page{/t}</a></h2>
  <p>{t}Bible{/t}: {$bible}</p>
  <p>{t}In this tutorial you are going to set up an open repository on the network.{/t}</p>
  <p>{t}You need a server to run this repository. Any of your Linux computers attached to your network will do. In this tutorial we assume that the computer is accessible from the network through IP address 192.168.0.1. The user of this computer will be Jim.{/t}</p>
  <p>{t}To create the repository, login as user jim. Then open a terminal. The repository is going to be in Jim's home directory. In the terminal type the following:{/t}</p>
  <pre>
cd
mkdir -p git/repository
cd git/repository
git --bare init --shared
  </pre>
  <p>{t}The repository is now there, but it is not yet accessible from the network.{/t}</p>
  <p>{t}To make the repository available to the network, type:{/t}</p>
  <pre>git daemon --export-all --base-path=/home/jim/git --detach --enable=receive-pack</pre>
  <p>{t}Each time that the server boots, it is necessary to execute the above command again. It would be easier to put the command in the boot script. Then it will automatically be started after boot.{/t}</p>
  <p>{t}To test access to the new repository from another computer on the network, type:{/t}</p>
  <pre>git ls-remote git://192.168.0.1/repository</pre>
  <p>{t}If everything is fine, no errors will occur.{/t}</p>

  <a name="url"></a>
  <h1>{t}Repository URL{/t}</h1>
  <p>{t}The repository that was created in this tutorial has the following URL:{/t}</p>
  <pre>git://192.168.0.1/repository</pre>
  <p>{t}Your own repository that you have created has a URL too, the one that will be used from now on. Please enter it below, and press the Submit button to save it.{/t}</p>
  <form action="collaboration_flash_drive_setup.php?bible={$bible}#url" name="url" method="post">
    <p><input type="text" name="urlvalue" value="{$url}" /></p>
    <p><input type="submit" name="url" value={t}Submit{/t} /></p>
  </form>

  <h2><a href="collaboration_repo_init.php?bible={$bible}">{t}Next page{/t}</a></h2>

  {include file=../assets/footer_full.tpl} 
  </body>
</html>
