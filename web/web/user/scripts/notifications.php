<h1><?php echo gettext ("Notifications") ?></h1>

<h2><?php echo gettext ("Consultation Notes") ?></h2>

<p>
<a href="notifications.php?editednotessubscriptiontoggle=">
<?php if ($this->edited_notes_subscription == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Subscribe to a consultation note you created or edited online or by email.") ?>
</p>

<p>
<a href="notifications.php?anynotessubscriptiontoggle=">
<?php if ($this->any_notes_subscription == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Subscribe to a consultation note created or edited by anybody online or by email.") ?>
</p>

<p>
<a href="notifications.php?emailconfirmationyourpoststoggle=">
<?php if ($this->email_confirmation_your_posts == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive email confirmation of your posts to a consultation notes done by email.") ?>
</p>

<p>
<a href="notifications.php?subscribednotenotificationtoggle=">
<?php if ($this->subscription_notification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive email about changes in a consultation note you are subscribed to.") ?>
</p>

<p>
<a href="notifications.php?notesassignmenttoggle=">
<?php if ($this->notes_assignment == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("A consultation note that get changed will be assigned to you.") ?>
</p>

<p>
<a href="notifications.php?assignednotenotificationtoggle=">
<?php if ($this->assignment_notification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive email about changes in a consultation note assigned to you.") ?>
</p>

<p>
<a href="notifications.php?suppressemailsfromnotesyouupdatedtoggle=">
<?php if ($this->suppress_mails_from_your_updates == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Suppress receiving email from an updated consultation note in case the update was made by you.") ?>
</p>

<p>
<a href="notifications.php?assignednotesnotificationtoggle=">
<?php if ($this->assigned_notes_notifications == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive a daily email with the number of consultation notes assigned to you.") ?>
</p>

<p>
<a href="notifications.php?deletednotenotificationtoggle=">
<?php if ($this->deleted_note_notification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive email about a deleted consultation note.") ?>
</p>

<h2><?php echo gettext ("Change Notifications") ?></h2>

<p>
<a href="notifications.php?biblechangesnotificationtoggle=">
<?php if ($this->bible_changes_notification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive a daily email with the changes in the text of the Bibles.") ?>
</p>

<p>
<a href="notifications.php?changenotificationstoggle=">
<?php if ($this->change_notifications == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<a href="../consultations/changes.php"><?php echo gettext ("Notifications about changes in the Bible text will be generated for you online.") ?></a>
</p>

<p>
<a href="notifications.php?pendingchangenotificationstoggle=">
<?php if ($this->pending_changes_notifications == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive a daily email with the number of online change notifications awaiting your approval.") ?>
</p>

<h2><?php echo gettext ("Text Checks") ?></h2>

<p>
<a href="notifications.php?biblechecksnotificationtoggle=">
<?php if ($this->bible_checks_notification == true) { ?> ☑ <?php } else { ?> ☐ <?php } ?>
</a>
<?php echo gettext ("Receive a daily email with results of checks on the text of the Bibles.") ?>
</p>
