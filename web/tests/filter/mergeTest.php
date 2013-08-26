<?php

 
class filterMergeTest extends PHPUnit_Framework_TestCase
{

  private $mergeBaseData;
  private $userModificationData;
  private $serverModificationData;


  protected function setUp ()
  {
    $this->tearDown ();
$usfm = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $usfm = trim ($usfm) . "\n";
    $this->mergeBaseData = $usfm;
  }

  
  protected function tearDown ()
  {
  }
  

  public function testLineMergeOneModification () // Todo
  {
    $this->userModificationData = str_replace ("basebethuthumela", "bathuthumela", $this->mergeBaseData);
    $this->serverModificationData = str_replace ("28", "29", $this->mergeBaseData);
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = str_replace ("basebethuthumela", "bathuthumela", $this->mergeBaseData);
    $standard = str_replace ("28", "29", $standard);
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testLineMergeUserModificationInEachLine ()
  {
    return;
    $this->userModificationData = $this->mergeBaseData;
    $this->userModificationData = str_replace ("28", "29", $this->userModificationData);
    $this->userModificationData = str_replace ("lokuzibonakalisa", "", $this->userModificationData);
    $this->userModificationData = str_replace ("\\p", "\\pp", $this->userModificationData);
    $this->userModificationData = str_replace ("Kwathi", "", $this->userModificationData);
    $this->userModificationData = str_replace ("khangela", "khangela-ke", $this->userModificationData);
    $this->userModificationData = str_replace ("kwayo", "kwengilosi", $this->userModificationData);
    $this->userModificationData = str_replace ("Dan. 7.9", "Danieli 7.9", $this->userModificationData);
    $this->userModificationData = str_replace ("basebethuthumela", "bathuthumela", $this->userModificationData);
    $this->serverModificationData = $this->mergeBaseData;
    file_put_contents ($this->userModificationFile, $this->userModificationData);
    file_put_contents ($this->serverModificationFile, $this->serverModificationData);
    Filter_Merge::files ($this->mergeBaseFile, $this->userModificationFile, $this->serverModificationFile, $this->outputFile);
    $output = file_get_contents ($this->outputFile);
    $this->assertEquals ($this->userModificationData, $output);
  }


  public function testLineMergeOneServerModification () // Todo
  {
    return;
    $this->userModificationData = $this->mergeBaseData;
    $this->serverModificationData = str_replace ("basebethuthumela", "bathuthumela", $this->mergeBaseData);
    file_put_contents ($this->userModificationFile, $this->userModificationData);
    file_put_contents ($this->serverModificationFile, $this->serverModificationData);
    Filter_Merge::files ($this->mergeBaseFile, $this->userModificationFile, $this->serverModificationFile, $this->outputFile);
    $output = file_get_contents ($this->outputFile);
    $this->assertEquals ($this->serverModificationData, $output);
  }


  public function testLineMergeServerModificationInEachLine () // Todo
  {
    return;
    $this->usersModificationData = $this->mergeBaseData;
    $this->serverModificationData = $this->mergeBaseData;
    //$this->serverModificationData = str_replace ("28", "29", $this->serverModificationData);
    $this->serverModificationData = str_replace ("lokuzibonakalisa", "", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("\\p", "\\pp", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("Kwathi", "", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("khangela", "khangela-ke", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("kwayo", "kwengilosi", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("Dan. 7.9", "Danieli 7.9", $this->serverModificationData);
    //$this->serverModificationData = str_replace ("basebethuthumela", "bathuthumela", $this->serverModificationData);
    file_put_contents ($this->userModificationFile, $this->userModificationData);
    file_put_contents ($this->serverModificationFile, $this->serverModificationData);
    Filter_Merge::files ($this->mergeBaseFile, $this->userModificationFile, $this->serverModificationFile, $this->outputFile);
    $output = file_get_contents ($this->outputFile);
    $this->assertEquals ($this->serverModificationData, $output);
  }

  public function testDie () // Todo
  {
    return;
    die;
  }  
  
  /// Todo test same modifications ok.
  
  // Test server modification.
  
}
?>
