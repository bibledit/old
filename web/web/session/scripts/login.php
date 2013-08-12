<h1><?php echo gettext ("Login") ?></h1>

<form action="login.php?<?php echo $this->query ?>" name="form" method="post">

<table>

<p class="error"><?php echo $this->error_message ?></p>

<tr>
<td align="right"><?php echo gettext ("Username or email address") ?></td>
<td><input type="text" name="user" maxlength="30" class="focus" /></td>
<td class="error"><?php echo $this->username_or_email_invalid_message ?></td>
</tr>

<tr>
<td align="right"><?php echo gettext ("Password") ?></td>
<td><input type="password" name="pass" maxlength="30" /></td>
<td class="error"><?php echo $this->password_invalid_message ?></td>
</tr>

<tr>
<td align="right"></td>
<td><input type="submit" name="submit" value=<?php echo gettext ("Login") ?> /></td>
</tr>

</table>

<p><a href="password.php"><?php echo gettext ("Forgot password?") ?></a></p>

<p><?php echo gettext ("Not registered?") ?> <a href="signup.php"><?php echo gettext ("Sign up!") ?></a></p>
</form>
