<h1>{t}Install Bibledit-Web version 0.5 on Ubuntu 11.04{/t}</h1>
<p>{t}This assumes a standard installation of the operating system.{/t}</p>
<p>{t}Connect to or open a terminal.{/t}</p>
<p>{t}Update the software sources. Type the following command, and then press Enter.{/t}</p>
<p><code>sudo apt-get update</code></p>
<p>{t}Install the LAMP server.{/t}</p>
<p><code>sudo apt-get install tasksel</code></p>
<p><code>sudo tasksel</code></p>
<p>{t}Tick the LAMP server, and press Enter. Installation takes a while.{/t}</p>
<p>{t}While installing mysql-server, set the administrative password. These guidelines assume that it will be set to "root".{/t}</p>
<p>{t}Install the software Bibledit-Web relies on.{/t}</p>
<p><code>sudo apt-get install make git-core curl default-jre zip unzip libzend-framework-php</code></p>
<p>{t}Anything else to be set up, choose the default settings and enter the correct passwords.{/t}</p>
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
