<h1>{t}Install Bibledit-Web version 0.6 on Ubuntu 11.10{/t}</h1>
<p>{t}This assumes a standard installation of the operating system.{/t}</p>
<p>{t}Connect to or open a terminal.{/t}</p>
<p>{t}Update the software sources. Type the following command, and then press Enter.{/t}</p>
<p><code>sudo apt-get update</code></p>

<h2>{t}Install the web server{/t}</h2>
<p>{t}In the terminal issue these commands:{/t}</p>
<p><code>sudo apt-get install tasksel</code></p>
<p><code>sudo tasksel</code></p>
<p>{t}Tick the LAMP server, and press Enter. Installation takes a while.{/t}</p>
<p>{t}While installing mysql-server, set the administrative password. These guidelines assume that it will be set to "root".{/t}</p>
<p>{t}Test the web server. Open the site in the browser. Where the site exactly is depends on your situation. Examples are:{/t}</p>
<p>{t}http://localhost{/t}</p>
<p>{t}http://yoursite.org{/t}</p>
<p>When your web server works, the browser will say: It works!</p>

<h2>{t}Install the software Bibledit-Web relies on.{/t}</h2>
<p>{t}In the terminal issue these commands:{/t}</p>
<p><code>sudo apt-get install make git curl zip unzip libzend-framework-php wdiff smarty smarty-gettext</code></p>
<p>{t}Anything else to be set up, choose the default settings and enter the correct passwords.{/t}</p>

<h2>{t}Install Bibledit-Web{/t}</h2>
<p>{t}Download Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>wget http://download.savannah.nongnu.org/releases/bibledit/source/web/bibledit-web-0.5.tar.gz</code>
<p>{t}Unpack Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>tar zxf bibledit-web-0.5.tar.gz</code>
<p>{t}Enter Bibledit-Web's code. To do that, type the following command, and press Enter:{/t}</p>
<code>cd bibledit-web-0.5</code>
<p>{t}Configure Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>./configure</code>
<p>{t}Install Bibledit-Web. To do that, type the following command, and press Enter:{/t}</p>
<code>sudo make install</code>

<h2>{t}Setup Bibledit-Web{/t}</h2>
<p>{t}Open the setup url in the browser.{/t}</p>
<p>{t}If the site is "localhost", then this will be:{/t} "http://localhost/bibledit/setup.php"</p>
<p>{t}If the site is for example "yoursite.org", then this will be for example:{/t} "http://yoursite.org/bibledit/setup.php"</p>
<p>{t}Bibledit-Web should now be ready, and accessible through:{/t} "http://localhost/bibledit"</p>
<p>{t}Or through for example:{/t} "http://yoursite.org/bibledit"</p>
<p>{t}You can initially login through default username "adminusername" and password "adminpassword". These only work if no Administrator's account exist on the system. It is recommended, therefore, to create an Administrator's account of your own choice, and then to remove the default adminusername account.{/t}</p>
