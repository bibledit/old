<?php
/*
Copyright (©) 2003-2013 Teus Benschop.

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

 
class filterMergeTest extends PHPUnit_Framework_TestCase
{

  private $mergeBaseData;
  private $userModificationData;
  private $serverModificationData;


  protected function setUp ()
  {
  }

  
  protected function tearDown ()
  {
  }
  

  public function testLineMergeSimpleModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\s Ukuvuka lokuzibonakalisa kukaJesu
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\s Ukuvuka kukaJesu
EOD;
    $this->serverModificationData = <<<'EOD'
\c 29
\s Ukuvuka lokuzibonakalisa kukaJesu
\s Ukuvuka lokuzibonakalisa kukaJesu
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\s Ukuvuka lokuzibonakalisa kukaJesu
\s Ukuvuka kukaJesu
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testLineMergeEqualModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\s Ukuvuka lokuzibonakalisa kukaJesu
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\s Ukuvuka kukaJesu
\s Ukuvuka kukaJesu
EOD;
    $this->serverModificationData = <<<'EOD'
\c 28
\s Ukuvuka kukaJesu
\s Ukuvuka kukaJesu
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 28
\s Ukuvuka kukaJesu
\s Ukuvuka kukaJesu
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testLineMergeMultipleModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\pp
\v 1 Kwathi ekupheleni kwesabatha\x + Marko 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya osukwini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\x + 27.65-66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\pp
\v 1 Kwathi ekupheleni kwesabatha\x + Marko 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya osukwini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\x + 27.65-66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testWordMergeSimpleModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 29
\v 4 Abalindi basebethuthumela ngokuyesaba, basebesiba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testWordMergeEqualModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 29
\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testWordMergeMultipleModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 29
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakaliswa kwaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\s Ukuvuka lokuzibonakaliswa kwaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Hlu. 13.6.\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testGraphemeMergeSimpleModifications ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 29
\v 4 Abalindi basebethuthumela besabe baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\v 4 Abalindi bathuthumela besabe baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }
  
  
  public function testConflictTakeServer ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 28
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->userModificationData = <<<'EOD'
\c 28
\v 4 Abalindi basebethuthumela ngokuyesaba baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->serverModificationData = <<<'EOD'
\c 29
\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 29
\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\x + 27.65,66.\x*.
EOD;
    $this->assertEquals ($standard, $output);
  }


  public function testPracticalMergeExampleOne ()
  {
    $this->mergeBaseData = <<<'EOD'
\c 1
\p
\v 1 This is really the text of the first (1st) verse.
\v 2 And this is what the second (2nd) verse contains.
\v 3 The third (3rd) verse.
\v 4 The fourth (4th) verse.
\v 5
EOD;
    $this->userModificationData = <<<'EOD'
\c 1
\p
\v 1 This is really the text of the first (1st) verse.
\v 2 And this is what the second verse contains.
\v 3 The third verse.
\v 4 The fourth (4th) verse.
\v 5
EOD;
    $this->serverModificationData = <<<'EOD'
\c 1
\p
\v 1 This is really the text of the first verse.
\v 2 And this is what the second (2nd) verse contains.
\v 3 The third (3rd) verse.
\v 4 The fourth verse.
\v 5
EOD;
    $output = Filter_Merge::run ($this->mergeBaseData, $this->userModificationData, $this->serverModificationData);
    $standard = <<<'EOD'
\c 1
\p
\v 1 This is really the text of the first verse.
\v 2 And this is what the second verse contains.
\v 3 The third verse.
\v 4 The fourth verse.
\v 5
EOD;
    $this->assertEquals ($standard, $output);
  }

  
}


?>
