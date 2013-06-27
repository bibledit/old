<h1><?php echo gettext ("Signup") ?></h1>

<form action="signup.php" method="post">

  <table>
  
  <p class="error"><?php echo $this->error_message ?></p>

  <tr>
  <td align="right"><?php echo gettext ("Which username would you like to use?") ?></td>
  <td><input type="text" name="user" maxlength="30" /></td>
  <td class="error"><?php echo $this->username_invalid_message ?></td>
  </tr>
  
  <tr>
  <td align="right"><?php echo gettext ("Which password?") ?></td>
  <td><input type="password" name="pass" maxlength="30" /></td>
  <td class="error"><?php echo $this->password_invalid_message ?></td>
  </tr>
  
  <tr>
  <td align="right"><?php echo gettext ("Your email address?") ?></td>
  <td><input type="text" name="mail" maxlength="256" /></td>
  <td class="error"><?php echo $this->email_invalid_message ?></td>
  </tr>
  
  <tr>
  <td align="right"><?php echo gettext ("Security question:") ?> <?php echo $this->question ?></td>
  <td><input type="text" name="answer" maxlength="40" /></td>
  <td class="error"><?php echo $this->answer_invalid_message ?></td>
  </tr>
  
  <tr>
  <td align="right"></td>
  <td><input type="submit" name="submit" value=<?php echo gettext ("Signup") ?> /></td>
  </tr>
  
  </table>
  
  <input type="hidden" name="standard" maxlength="40" value=<?php echo $this->standard ?> />
  
  <p><?php echo gettext ("Hint to answer the security question:") ?> <?php echo $this->passage ?></p>
  
</form>
