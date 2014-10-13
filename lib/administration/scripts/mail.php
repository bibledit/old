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
<h1><?php echo Locale_Translate::_("Mail") ?></h1>
<p><?php echo Locale_Translate::_("This page deals with setting up the email system.") ?></p>
<p><?php echo Locale_Translate::_("Bibledit-Web sends out email. It also collects email every five minutes and acts upon these emails.") ?></p>
<p><?php echo Locale_Translate::_("It needs an external email account for sending and receiving email.") ?></p>
<p><?php echo Locale_Translate::_("The help information on this page assumes that we use a Google mail account. You will have your own email account dedicated to Bibledit-Web.") ?></p>
<p><?php echo Locale_Translate::_("The email address is bibletranslation@gmail.com, and the password is pazzwrd.") ?></p>

<a name="name"></a>
<h2><?php echo Locale_Translate::_("Site name and email address") ?></h2>
<form action="mail.php#name" name="email" method="post">
  <p><?php echo Locale_Translate::_("Our own sample values are given below. Yours will be different.") ?></p>
  <p><?php echo Locale_Translate::_("Name: Bible Translation.") ?></p>
  <p><?php echo Locale_Translate::_("Email address: bibletranslation@gmail.com.") ?></p>
  <p><?php echo Locale_Translate::_("To switch the mailer off, leave the fields empty.") ?></p>
  <h4><?php echo Locale_Translate::_("Enter the name and the email address, and press Submit.") ?></h4>
  <table>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Name") ?></td>
      <td><input type="text" name="sitename" maxlength="50" value="<?php echo $this->sitename ?>" /></td>
    </tr>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Email address") ?></td>
      <td><input type="text" name="sitemail" maxlength="50" value="<?php echo $this->sitemail ?>" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="email" value=<?php echo Locale_Translate::_("Submit") ?> /></td>
      <td class="error"><?php echo $this->site_name_error ?></td>
      <td class="success"><?php echo $this->site_name_success ?></td>
    </tr>
  </table>
</form>

<a name="retrieve"></a>
<h2><?php echo Locale_Translate::_("Retrieving mail") ?></h2>
<form action="mail.php#retrieve" name="retrieve" method="post">
  <p><?php echo Locale_Translate::_("Our own sample values are given below. Yours will be different.") ?></p>
  <p><?php echo Locale_Translate::_("POP3 Host: pop.gmail.com.") ?></p>
  <p><?php echo Locale_Translate::_("Username: bibletranslation@gmail.com.") ?></p>
  <p><?php echo Locale_Translate::_("Password: pazzwrd.") ?></p>
  <p><?php echo Locale_Translate::_("Security: SSL.") ?></p>
  <p><?php echo Locale_Translate::_("Port: Leave empty. The right port will be chosen automatically.") ?></p>
  <h4><?php echo Locale_Translate::_("Enter the account details, and press Submit.") ?></h4>
  <p><?php echo Locale_Translate::_("When you submit the values, Bibledit will attempt to contact this email account and tell you how many messages there are in the Inbox. If something goes wrong, the message given may be cryptic at times.") ?></p>
  <table>
    <tr>
      <td align="right">POP3 Host</td>
      <td><input type="text" name="storagehost" maxlength="50" value="<?php echo $this->storagehost ?>" /></td>
    </tr>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Username") ?></td>
      <td><input type="text" name="storageusername" maxlength="50" value="<?php echo $this->storageusername ?>" /></td>
    </tr>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Password") ?></td>
      <td><input type="text" name="storagepassword" maxlength="50" value="<?php echo $this->storagepassword ?>" /></td>
    </tr>
    <tr>
      <td align="right">Security</td>
      <td>
        <select name="storagesecurity">
        <option value="NONE" >None</option>
        <option value="SSL" <?php if ($this->storagesecurity == "SSL") { ?> selected="selected" <?php } ?>>SSL</option>
        <option value="TLS" <?php if ($this->storagesecurity == "TLS") { ?> selected="selected" <?php } ?>>TLS</option>
      </td>
    </tr>
    <tr>
      <td align="right">Port</td>
      <td><input type="text" name="storageport" maxlength="10" value="<?php echo $this->storageport ?>" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="retrieve" value=<?php echo Locale_Translate::_("Submit") ?> onClick="this.value = '<?php echo Locale_Translate::_ ("Please wait") ?>'; return true;" /></td>
      <td class="error"><?php echo $this->storage_error ?></td>
      <td class="success"><?php echo $this->storage_success ?></td>
    </tr>
  </table>
</form>

<a name="send"></a>
<h2><?php echo Locale_Translate::_("Sending mail") ?></h2>
<form action="mail.php#send" name="send" method="post">
  <p><?php echo Locale_Translate::_("Our own sample values are given below. Yours will be different.") ?></p>
  <p><?php echo Locale_Translate::_("SMTP Host: smtp.gmail.com. If it is left empty, it will use the default sendmail transport.") ?></p>
  <p><?php echo Locale_Translate::_("Authentication: LOGIN.") ?></p>
  <p><?php echo Locale_Translate::_("Username: bibletranslation@gmail.com.") ?></p>
  <p><?php echo Locale_Translate::_("Password: pazzwrd.") ?></p>
  <p><?php echo Locale_Translate::_("Security: SSL.") ?></p>
  <p><?php echo Locale_Translate::_("Port: 465.") ?></p>
  <p><?php echo Locale_Translate::_("If the SMTP server does not need authenticaton, just leave the fields for username and password empty.") ?></p>
  <h4><?php echo Locale_Translate::_("Enter the account details, and press Submit.") ?></h4>
  <p><?php echo Locale_Translate::_("When you submit new values, Bibledit will attempt to send an email the site's email address. If something goes wrong, the message given may be cryptic at times. Please enter the correct details for this to work.") ?></p>
  <table>
    <tr>
      <td align="right">SMTP Host</td>
      <td><input type="text" name="sendhost" maxlength="50" value="<?php echo $this->sendhost ?>" /></td>
    </tr>
    <tr>
      <td align="right">Authentication</td>
      <td>
        <select name="sendauthentication">
        <option value="None" >None</option>
        <option value="Plain" <?php if ($this->sendauthentication == "Plain") { ?> selected="selected" <?php } ?>>PLAIN</option>
        <option value="Login" <?php if ($this->sendauthentication == "Login") { ?> selected="selected" <?php } ?>>LOGIN</option>
        <option value="Crammd5" <?php if ($this->sendauthentication == "Crammd5") { ?> selected="selected" <?php } ?>>CRAM-MD5</option>
      </td>
    </tr>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Username") ?></td>
      <td><input type="text" name="sendusername" maxlength="50" value="<?php echo $this->sendusername ?>" /></td>
    </tr>
    <tr>
      <td align="right"><?php echo Locale_Translate::_("Password") ?></td>
      <td><input type="text" name="sendpassword" maxlength="50" value="<?php echo $this->sendpassword ?>" /></td>
    </tr>
    <tr>
      <td align="right">Security</td>
      <td>
        <select name="sendsecurity">
        <option value="NONE" >None</option>
        <option value="SSL" <?php if ($this->sendsecurity == "SSL") { ?> selected="selected" <?php } ?>>SSL</option>
        <option value="TLS" <?php if ($this->sendsecurity == "TLS") { ?> selected="selected" <?php } ?>>TLS</option>
      </td>
    </tr>
    <tr>
      <td align="right">Port</td>
      <td><input type="text" name="sendport" maxlength="10" value="<?php echo $this->sendport ?>" /></td>
    </tr>
    <tr>
      <td></td>
      <td><input type="submit" name="send" value=<?php echo Locale_Translate::_("Submit") ?> onClick="this.value = '<?php echo Locale_Translate::_ ("Please wait") ?>'; return true;" /></td>
      <td class="error"><?php echo $this->send_error ?></td>
      <td class="success"><?php echo $this->send_success ?></td>
    </tr>
  </table>
</form>
