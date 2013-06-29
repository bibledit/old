<h1><?php echo gettext ("Account") ?> <?php echo $this->username ?></h1>

<form action="account.php" name="form" method="post">

<table>

<p class="error"><?php echo $this->error_message ?></p>
<?php foreach ($this->success_messages as $success_message) { ?>
  <p class="success"><?php echo $success_message ?></p>
<?php } ?>

<?php if ($this->actions_taken == false) { ?>

<p><?php echo gettext ("You can change your password or your email address. Your current email address is") ?> <?php echo $this->email ?>.</p>

<tr>
<td align="right"><?php echo gettext ("Current password") ?></td>
<td><input type="text" name="currentpassword" maxlength="30" /></td>
<td class="error"><?php echo $this->current_password_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo gettext ("New password") ?></td>
<td><input type="text" name="newpassword" maxlength="30" /></td>
<td class="error"><?php echo $this->new_password_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo gettext ("Again new password") ?></td>
<td><input type="text" name="newpassword2" maxlength="30" /></td>
<td class="error"><?php echo $this->new_password2_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo gettext ("New email address") ?></td>
<td><input type="text" name="newemail" maxlength="256" /></td>
<td class="error"><?php echo $this->new_email_invalid_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo gettext ("Submit") ?> /></td>
</tr>

<?php } ?>

</table>

</form>
