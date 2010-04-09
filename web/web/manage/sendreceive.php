<?php
require_once ("../bootstrap/bootstrap.php");
page_access_level (MANAGER_LEVEL);
$smarty = new Smarty_Bibledit (__FILE__);
$smarty->display("sendreceive.tpl");

include_once ("messages/messages.php");    
message_information (gettext ("Starting to send and receive the relevant Bibles and Project Notes."));
flush ();

$database_bibles = Database_Bibles::getInstance();
$database_config_user = Database_Config_User::getInstance();
$database_repositories = Database_Repositories::getInstance();
// The Bible that the user viewed last should be synced first, since other Bibles may take their time,
// thus delaying the sync of the Bible the user viewed, and probably is most interested in.
$bibles = array_merge (array ($database_config_user->getBible ()), $database_bibles->getBibles());
$bibles = array_unique ($bibles);
foreach ($bibles as $bible) {
  $remote_repository_url = $database_config_user->getRemoteRepositoryUrl ($bible);
  if ($remote_repository_url != "") {
    message_information ("");
    message_information (gettext ("Bible") . ": " . $bible);
    message_information (gettext ("Remote repository URL") . ": " . $remote_repository_url);
    $dot_git = $database_repositories->getRepository ($bible);
    if ($dot_git == "") {
      message_warning (gettext ("Cannot send and receive this Bible because the git repository was not found in the database."));
      continue;
    }

    $secure_key_directory = Filter_Git::git_config ($remote_repository_url);

    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);
    message_information (gettext ("Working directory") . ": " . $directory);
    flush ();
    Filter_Git::database2filedata ($bible, $directory);
    Filter_Git::database2repository ($bible, $directory);
    flush ();

    $command = "cd $directory; git add . 2>&1";
    message_code ($command);
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) message_code ($line);
    flush ();
    
    $command = "cd $directory; git commit -a -m sync 2>&1";
    message_code ($command);
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) message_code ($line);
    flush ();
    
    $command = "cd $directory; git pull 2>&1";
    message_code ($command);
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) {
      message_code ($line);
      if (strstr ($line, "CONFLICT") !== false) {
        message_error (gettext ("A conflict was found in the above book and chapter. Please resolve this conflict manually. Open the chapter in the editor in USFM view, and select which of the two conflicting lines of text should be retained, and remove the other line, and the conflict markup. After that it is recommended to send and receive the Bibles again. This will remove the conflict from the repository."));
      }
    }
    flush ();

    $command = "cd $directory; git push 2>&1";
    message_code ($command);
    unset ($result);
    exec ($command, &$result, &$exit_code);
    foreach ($result as $line) message_code ($line);
    flush ();

    Filter_Git::filedata2database ($directory, $bible);
    Filter_Git::repository2database ($directory, $bible);
    flush ();

    Filter_Git::git_un_config ($secure_key_directory);

    message_information (gettext ("This Bible has been done."));
    flush ();
  }
}

message_information ("");
message_information (gettext ("Ready. All relevant Bibles have been done."));
flush ();


?>
