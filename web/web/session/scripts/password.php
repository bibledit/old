<h1><?php echo gettext ("New password") ?></h1>

<form action="password.php" name="form" method="post">

<table>

<p class="success"><?php echo $this->success_message ?></p>

<?php if ($this->success_message == "") { ?> 

<p><?php echo gettext ("If you lost the password, submit your username or email address, and a new password will be emailed to you.") ?></p>

<tr>
<td align="right"><?php echo gettext ("Username or email address") ?></td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error"><?php echo $this->error_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></td>
</tr>

<?php } ?>

</table>

</form>
