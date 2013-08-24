<?php

 
class filterMergeTest extends PHPUnit_Framework_TestCase
{

  private $mergeBaseFile;
  private $userModificationFile;
  private $serverModificationFile;
  private $outputFile;

  private $mergeBaseData;
  private $userModificationData;
  private $serverModificationData;
  private $outputData;


  protected function setUp ()
  {
    $this->mergeBaseFile = uniqid (sys_get_temp_dir() . "/");
    $this->userModificationFile = uniqid (sys_get_temp_dir() . "/");
    $this->serverModificationFile = uniqid (sys_get_temp_dir() . "/");
    $this->outputFile = uniqid (sys_get_temp_dir() . "/");
    $this->tearDown ();

$this->mergeBaseData = <<<'EOD'
\c 28
\s Ukuvuka lokuzibonakalisa kukaJesu
\p
\v 1 Kwathi ekupheleni kwesabatha\x + Mark. 16.1-8. Luka 24.1-10.\x*, emadabukakusa kusiya o\add sukw\add*ini lokuqala lweviki\x + Joha. 20.1.\x*, kwafika uMariya Magadalena\x + Joha. 20.1.\x*, lomunye uMariya, ukuzabona ingcwaba\x + 27.56,61. Mark. 16.1. Luka 24.10.\x*.
\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\x + 27.51,54.\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\x + 27.60,66.\x*.
\v 3 Lokubonakala kwayo kwakunjengombane\x + Dan. 10.6. Hlu. 13.6.\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\x + Dan. 7.9. Mark. 9.3.\x*.
\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\x + 27.65,66.\x*.
\v 5 Kodwa ingilosi yaphendula yathi kwabesifazana: Lingesabi lina; ngoba ngiyazi ukuthi lidinga uJesu obebethelwe;
\v 6 kakho lapha; ngoba uvukile\x + Hlu. 18.14.\x*, njengokutsho kwakhe; wozani libone indawo lapho iNkosi ebilele khona.
\v 7 Hambani masinyane litshele abafundi bakhe ukuthi uvukile kwabafileyo\x + 27.63.\x*; njalo khangelani, uzalandulela ukuya eGalili\x + 26.32. 28.10,16.\x*; lizambona khona; khangelani, sengilitshelile.
\p
\v 8 Basebephuma engcwabeni ngokuphangisa belokwesaba lentokozo enkulu\x + Mark. 16.8.\x*, bagijima baya ukubikela abafundi bakhe\x + Mark. 16.8,10.\x*.
\v 9 Kwathi besiyatshela abafundi bakhe, futhi khangela, uJesu wabahlangabeza, wathi: Mpilo enhle\x + Mark. 16.9. Joha. 20.14.\x*! Basebesondela babamba inyawo zakhe\x + 2 Kho. 4.27.\x*, bamkhonza\x + 28.17. 8.2.\x*.
\v 10 UJesu wasesithi kubo: Lingesabi\x + 28.5.\x*; hambani, libikele abafowethu\x + Joha. 20.17. Hlab. 22.22. Roma 8.29. Heb. 2.11,12.\x* ukuthi baye eGalili, njalo bazangibona khona\x + 26.32. 28.10,16.\x*.
\p
\v 11 Kwathi sebehamba, khangela, abanye kubalindi baya emzini babikela abapristi abakhulu konke okwenzakeleyo\x + 27.65,66.\x*.
\v 12 Sebebuthene labadala, benza icebo, banika abebutho imali enengi\x + 12.14.\x*,
\v 13 besithi: Kalibokuthi: Abafundi bakhe beze ebusuku bameba silele thina.
\v 14 Futhi uba lokhu kuzwakala kumbusi\x + 27.1.\x*, thina sizamncenga, silenze lingabi senkathazweni\x + Seb. 12.20.\x*.
\v 15 Basebethatha imali benza njengalokhu ababekufundisiwe; lalelilizwi landa phakathi kwamaJuda\x + Mark. 1.45.\x* kuze kube lamuhla\x + 27.8.\x*.
\s Ukulayezela kokucina kukaJesu
\p
\v 16 Kwathi abafundi abalitshumi lanye baya eGalili, entabeni uJesu\x + 28.7.\x* ayebalayele yona\x + 27.10.\x*;
\v 17 sebembonile bamkhonza\x + 28.17. 8.2.\x*; kodwa abanye\x + 1 Kor. 15.6.\x* bathandabuza\x + 14.31.\x*.
\v 18 UJesu wasesondela wakhuluma labo, esithi: Wonke amandla aphiwe mina ezulwini lemhlabeni\x + Dan. 7.13,14. Mat. 11.27; 16.28. Luka 1.32; 10.22. Joha. 3.35; 5.22,27; 13.3; 17.2. Seb. 2.36. Roma 14.9. 1 Kor. 15.27. Efe. 1.10,20-23. Fil. 2.9,10. Kol. 2.10. Heb. 1.2,6; 2.8. 1 Pet. 3.22.\x*;
\v 19 ngakho hambani\x + Mark. 16.15,16.\x* lenze izizwe zonke zibe ngabafundi\x + Isa. 52.10. Luka 24.47. Seb. 2.39. Mat.24.14.\x*, libabhabhathiza\x + Mark. 16.15,16.\x* ebizweni likaYise leleNdodana lelikaMoya oNgcwele\x + Roma 6.3. Gal. 3.27. Seb. 8.16; 19.5. 1 Kor. 1.13; 10. 2.\x*;
\v 20 libafundisa ukugcina konke engililaye khona; futhi khangelani, mina ngilani\x + 18.20.\x* insuku zonke kuze kube sekupheleni kwesikhathi. Ameni\x + 13.39.\x*.
EOD;
    file_put_contents ($this->mergeBaseFile, $this->mergeBaseData);


  }
  
  protected function tearDown ()
  {
    @unlink ($this->mergeBaseFile);
    @unlink ($this->userModificationFile);
    @unlink ($this->serverModificationFile);
    @unlink ($this->outputFile);
  }
  

  public function testLineMergeUserModification () // Todo
  {
    $this->userModificationData = str_replace ("Basebethatha", "bathatha", $this->mergeBaseData);
    $this->serverModificationData = $this->mergeBaseData;
    $this->outputData = $this->userModificationData;
    file_put_contents ($this->userModificationFile, $this->userModificationData);
    file_put_contents ($this->serverModificationFile, $this->serverModificationData);
    Filter_Merge::files ($this->mergeBaseFile, $this->userModificationFile, $this->serverModificationFile, $this->outputFile);


    //$output = file_get_contents ($this->outputFile);
    // $this->assertEquals ($this->outputData, $output);
  }
  
  
  /// Todo test same modifications ok.
  
  // Test server modification.
  
}
?>
