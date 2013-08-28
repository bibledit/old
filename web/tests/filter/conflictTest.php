<?php

 
class filterConflictTest extends PHPUnit_Framework_TestCase // Todo
{

  private $repository;
  private $userclone;
  private $serverclone;
  private $userdata1;
  private $userdata2;
  private $serverdata1;
  private $serverdata2;
  private $mergeddata1;
  private $mergeddata2;


  protected function setUp ()
  {
    // Create repository.
    $this->repository = uniqid (sys_get_temp_dir () . "/") . "repository";
    mkdir ($this->repository);
    $command = "cd " . $this->repository . "; git --bare init --shared 2>&1";
    exec ($command, $output, $exit_code);
    // Clone repository for user.
    $this->userclone = uniqid (sys_get_temp_dir () . "/") . "userclone";
    $command = "git clone file://" . $this->repository . " " . $this->userclone . "  2>&1";
    exec ($command, $output, $exit_code);
    // Store data in the user clone.
    $data1 = "line one\nline two\n";
    $data2 = "line three\nline four\n";
    mkdir ($this->userclone . "/Genesis/3", 0777, true);
    file_put_contents ($this->userclone . "/Genesis/3/data", $data1);
    mkdir ($this->userclone . "/Exodus/2", 0777, true);
    file_put_contents ($this->userclone . "/Exodus/2/data", $data1);
    mkdir ($this->userclone . "/path/to/data", 0777, true);
    file_put_contents ($this->userclone . "/path/to/data/file1", $data2);
    file_put_contents ($this->userclone . "/file2", $data2);
    // Commit and push the data in the user clone.
    $command = "cd " . $this->userclone . "; git add . 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd " . $this->userclone . "; git commit -a -m commit 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd " . $this->userclone . "; git push 2>&1";
    exec ($command, $output, $exit_code);
    // Clone repository for server.
    $this->serverclone = uniqid (sys_get_temp_dir () . "/") . "serverclone";
    $command = "git clone file://" . $this->repository . " " . $this->serverclone . "  2>&1";
    exec ($command, $output, $exit_code);
    // Store conflicting data in the server clone.
    $this->serverdata1 = "line one1\nline two\n";
    $this->serverdata2 = "line three3\nline four\n";
    file_put_contents ($this->serverclone . "/Genesis/3/data", $this->serverdata1);
    file_put_contents ($this->serverclone . "/Exodus/2/data", $this->serverdata1);
    file_put_contents ($this->serverclone . "/path/to/data/file1", $this->serverdata2);
    file_put_contents ($this->serverclone . "/file2", $this->serverdata2);
    // Commit and push the data in the server clone.
    $command = "cd " . $this->serverclone . "; git add . 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd " . $this->serverclone . "; git commit -a -m commit 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd " . $this->serverclone . "; git push 2>&1";
    exec ($command, $output, $exit_code);
    // Store conflicting data in the user clone.
    $this->userdata1 = "line one\nline two2\n";
    $this->userdata2 = "line three\nline four4\n";
    file_put_contents ($this->userclone . "/Genesis/3/data", $this->userdata1);
    file_put_contents ($this->userclone . "/Exodus/2/data", $this->userdata1);
    file_put_contents ($this->userclone . "/path/to/data/file1", $this->userdata2);
    file_put_contents ($this->userclone . "/file2", $this->userdata2);
    // Commit the data in the user clone.
    $command = "cd " . $this->userclone . "; git add . 2>&1";
    exec ($command, $output, $exit_code);
    $command = "cd " . $this->userclone . "; git commit -a -m commit 2>&1";
    exec ($command, $output, $exit_code);
    // Pull changes from the server, leading to conflicts.
    $command = "cd " . $this->userclone . "; git pull 2>&1";
    exec ($command, $output, $exit_code);
    // Eventually merged data.
    $this->mergeddata1 = "line one1\nline two2\n";
    $this->mergeddata2 = "line three3\nline four4\n";
  }

  
  protected function tearDown ()
  {
    Filter_Rmdir::rmdir ($this->repository);
    Filter_Rmdir::rmdir ($this->userclone);
    Filter_Rmdir::rmdir ($this->serverclone);
  }
  

  public function testResolveConflictsStandard ()
  {
    Filter_Conflict::run ($this->userclone);
    $data = file_get_contents ($this->userclone . "/Genesis/3/data");
    $this->assertEquals ($this->mergeddata1, $data);
    $data = file_get_contents ($this->userclone . "/Exodus/2/data");
    $this->assertEquals ($this->mergeddata1, $data);
    $data = file_get_contents ($this->userclone . "/path/to/data/file1");
    $this->assertEquals ($this->mergeddata2, $data);
    $data = file_get_contents ($this->userclone . "/file2");
    $this->assertEquals ($this->mergeddata2, $data);
  }
  

  public function testResolveConflictsCli ()
  {
    $folder = dirname (dirname (dirname (__FILE__)));
    $script = $folder . "/web/filter/conflictcli.php";
    $sript = escapeshellarg ($script);
    $command = "php $script " . $this->userclone;
    system ($command);
    $data = file_get_contents ($this->userclone . "/Genesis/3/data");
    $this->assertEquals ($this->mergeddata1, $data);
    $data = file_get_contents ($this->userclone . "/Exodus/2/data");
    $this->assertEquals ($this->mergeddata1, $data);
    $data = file_get_contents ($this->userclone . "/path/to/data/file1");
    $this->assertEquals ($this->mergeddata2, $data);
    $data = file_get_contents ($this->userclone . "/file2");
    $this->assertEquals ($this->mergeddata2, $data);
  }
  
  


  
}


?>
