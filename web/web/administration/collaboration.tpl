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

  <h1>{t}Web collaboration{/t}</h1>
  <p>{t}With Bibledit-Web several translators can work on one Bible at the same time.{/t}</p>  
  <p>{t}There is nothing special needed to make this happen.{/t}</p>  
  <p>{t}The translators log into the site that runs Bibledit-Web, and translate there.{/t}</p>  
  <p>{t}All the data related to the Bible is saved on that website, and is available to all translators.{/t}</p>  
  <p>{t}When collaborating in this way, the information given below on the rest of the page is not relevant.{/t}</p>  

  <h1>{t}Distributed collaboration{/t}</h1>
  <p>{t}With distributed collaboration there are several copies of the Bible distributed all over the place or the globe.{/t}</p>  
  <p>{t}Translators that collaborate in this way do not need to be connected to the Internet all the time, or even not at all.{/t}</p>  
  <p>{t}The translators collaborate through the network or a USB drive.{/t}</p>  
  <p>{t}Once everything has been set up, it all goes automatic. The translator does not notice the mechanics of the collaboration. It all happens under the hood.{/t}</p>  
  <p>{t}There are a variety of methods for collaboration. The administrator who will set it all up needs to have sufficient knowledge of information technology.{/t}</p>  
  <p>{t}Bibledit-Web stores all data locally. When collaboration is enabled, it also connects to a central repository. Through that repository the data gets shared between the translators.{/t}</p>  
  <p>{t}Under the hood, Bibledit uses Git for collaboration. Git is a content manager. The translator does not notice that Git is used. He does not operate it directly. The only thing that the translator needs to do at regular times is to "Send/Receive" the Bible. After having done that, the changes in the Bible, made by the other translators, appear on his screen too. And the changes made by the translator get stored in the repository, and if the other translators do Send/Receive also, then these changes will reflect on their screens also.{/t}</p>  
  <p>{t}If distributed collaboration is used, it needs to be set up per Bible.{/t} <a href="collaboration.php?bible={$bible}&select">{t}Select which Bible to use.{/t}</a></p>  
  {if $bible != ""}
    <p>{t}Bible{/t}: {$bible}</p>
    {if $url != ""}
    <p>{t}Distributed collaboration for this Bible is enabled.{/t} <a href="collaboration.php?bible={$bible}&disable=">{t}Disable it.{/t}</a></p> 
    {/if}
    <p>{t}There is a variety of repositories that can be used. Each of them has it own use, advantages and disadvantages.{/t}</p>  
    <ul>
      <li><em>{t}USB Flash Drive.{/t}</em> {t}First of all there is the repository that resides on a file system. A USB flash drive can be used in this case. This flash drive can be physically moved from one translator to the other, and so on, updating each translator, and getting updated by each translator.{/t} <a href="collaboration_flash_drive_setup.php?bible={$bible}">{t}Set it up.{/t}</a></li>
      <li><em>{t}Open Network Repository.{/t}</em> {t}Secondly there is the repository that resides on the network. For internal networks a simple open network repository can be used. Whoever knows where the repository resides can write to it.{/t} <a href="collaboration_open_network_setup.php?bible={$bible}">{t}Set it up.{/t}</a></li>
      <li><em>{t}Secure Network Repository.{/t}</em> {t}For public networks such as the internet a secure network repository would be recommended. This type of repository is locked down. Only those who have the keys to such a repository can write to it.{/t} <a href="collaboration_secure_network_setup.php?bible={$bible}">{t}Set it up.{/t}</a></li>
    </ul> 
  {/if}

  {include file=../assets/footer_full.tpl} 
  </body>
</html>
