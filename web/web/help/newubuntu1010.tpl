<h1>{t}Install Bibledit-Web version 0.4 and higher on Ubuntu 10.10 desktop and server{/t}</h1>
<p>{t}This assumes a standard installation of the operating system.{/t}</p>
<p>{t}Connect to a terminal on the server.{/t}</p>
<p>{t}Install the software Bibledit-Web relies on. To do that, type the following commands, and press Enter after each:{/t}</p>
<p><code>sudo apt-get update</code></p>
<p><code>sudo apt-get install mysql-server phpmyadmin php5-cli git-core curl default-jre zip unzip libzend-framework-php</code></p>
<p>{t}While installing mysql-server, set the administrative password. These guidelines assume that it will be set to root.{/t}</p>
<p>{t}Anything else to be set up, choose the default settings and enter the correct passwords.{/t}</p>
<p>{t}Reboot when through. This is to start the Apache web server.{/t}</p>
<p>{t}Download Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>wget http://download.savannah.nongnu.org/releases/bibledit/source/web/bibledit-web-x.x.tar.gz</code>
<p>{t}The x.x refers to the version number{/t}</p>
<p>{t}Unpack Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>tar zxf bibledit-web-x.x.tar.gz</code>
<p>{t}Enter Bibledit-Web's code. To do that, type the following command, and press Enter:{/t}</p>
<code>cd bibledit-web-x.x</code>
<p>{t}Configure Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>./configure</code>
<p>{t}Install Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>sudo make install</code>
<p>{t}Test the web server. Open the site in the browser. It should say something like{/t}</p>
<p>It works!</p>
<p>{t}Setup Bibledit-Web. Open the setup url in the browser. If the site is "site.org", then this will be something like:{/t} http://site.org/bibledit/setup.php</p>
<p>{t}Bibledit-Web should now be ready, and accessible through:{/t} http://site.org/bibledit</p>
<p>{t}You can initially login through default username "adminusername" and password "adminpassword". These only work if no Administrator's account exist on the system. It is recommended, therefore, to create an Administrator's account of your own choice, and then to remove the default adminusername account.{/t}</p>
