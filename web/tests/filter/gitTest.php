<?php
require_once 'PHPUnit/Framework.php';
 
class gitTest extends PHPUnit_Framework_TestCase
{
  private $psalms_0_data;
  private $psalms_11_data;
  private $song_of_solomon_2_data;

  private function initialize_data () 
  {
$this->psalms_0_data = <<<EOD
\\id PSA
\\h Izihlabelelo
\\toc2 Izihlabelelo
\\mt2 UGWALO
\\mt LWEZIHLABELELO
EOD;
$this->psalms_11_data = <<<EOD
\\c 11
\\s IN\\sc KOSI\\sc* iyisiphephelo sabaqotho
\\d Kumqondisi wokuhlabelela. EsikaDavida
\\p
\\v 1 Ngithembela eN\\sc KOSI\\sc*ni\\x + Hlab. 25.2.\\x*. Lingatsho njani emphefumulweni wami: Balekela entabeni yenu \\add njeng\\add*enyoni\\x + 1 Sam. 23.14,19. 26.19,20.\\x*.
\\v 2 Ngoba, khangela, ababi bayagobisa idandili\\x + Hlab. 7.12. Hlab. 64.4.\\x*, balungisa umtshoko wabo entanjeni\\x + Hlab. 7.12. 21.12.\\x*, ukuze batshoke emnyameni abaqotho ngenhliziyo\\x + Hlab. 7.10.\\x*.
\\v 3 Nxa izisekelo zidilizwa\\x + Isa. 19.10. Hlab. 82.5. Hlab. 75.3.\\x*, angenzani olungileyo\\x + Jobe 22.13.\\x*?
\\p
\\v 4 IN\\sc KOSI\\x + Hab. 2.20.\\x*\\sc* isethempelini layo elingcwele\\x + Hlab. 5.7. Hlab. 150.1.\\x*; iN\\sc KOSI\\sc*, isihlalo sayo sobukhosi sisemazulwini\\x + Hlab. 2.4. 103.19. 115.3. 123.1. Isa. 66.1. Mat. 5.34. 23.22. Seb. 7.49. Isam. 4.2.\\x*; amehlo ayo ayakhangela\\x + Jobe 24.23. Hlab. 33.13. 34.15. 66.7. Hlab. 14.2. 102.19. 113.5,6.\\x*, inkophe zayo ziyahlola, abantwana babantu.
\\v 5 IN\\sc KOSI\\sc* iyamhlola olungileyo, kodwa omubi lothanda ubudlwangudlwangu, umphefumulo wayo uyamzonda\\x + Gen. 22.1.\\x*.
\\v 6 Uzanisa phezu kwababi imijibila, umlilo, lesolufa*\\x + Jobe 18.15.\\x*, lomoya otshisayo\\x + Hlab. 119.53. Lilo 5.10.\\x*, kuzakuba yisabelo senkezo yabo\\x + Hlab. 75.8. Jobe 21.20. Hlab. 16.5.\\x*.
\\v 7 Ngoba ilungile iN\\sc KOSI\\sc*, iyathanda ukulunga\\x + Hlab. 33.5. 45.7. Hlab. 37.28. 146.8.\\x*; ubuso bayo buyabona oqotho\\x + Hlab. 33.18. Hlab. 17.2.\\x*.
EOD;
$this->song_of_solomon_2_data = <<<EOD
\\c 2
\\p
\\v 1 Ngilirozi\\x + Isa. 35.1.\\x* leSharoni\\x + Josh. 12.18.\\x*, umduze wezigodi\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*.
\\p
\\v 2 Njengomduze phakathi kwameva\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*, unjalo umngane wami phakathi kwamadodakazi\\x + 1.15.\\x*.
\\p
\\v 3 Njengesihlahla sama-aphula phakathi kwezihlahla zeganga, sinjalo isithandwa sami phakathi kwamadodana\\x + Zaga 25.11.\\x*. Ngahlala emthunzini waso ngathokoza kakhulu\\x + Isa. 25.4. 32.2.\\x*, lesithelo saso simnandi ekunambitheni kwami\\x + Isam. 22.2.\\x*.
\\v 4 Sangisa endlini yewayini, lesiboniso saso phezu kwami siluthando\\x + 1.4.\\x*.
\\v 5 Ngisekelani\\x + Gen. 27.37.\\x* ngeziphiso zewayini\\x + 2 Sam. 6.19. 1 Lan. 16.3. Hos. 3.1.\\x*, lingiqinise ngama-aphula\\x + Zaga 25.11.\\x*, ngoba ngigul\\add isw\\add*a \\add lu\\add*thando\\x + 5.8.\\x*.
\\v 6 Isandla saso sokhohlo singaphansi kwekhanda lami\\x + 8.3. Dute. 33.27.\\x*, lesokunene saso siyangigona\\x + 8.3. Dute. 33.27.\\x*.
\\v 7 Ngiyalifungisa\\x + 3.5. 8.4. 5.8,9.\\x*, madodakazi eJerusalema\\x + 1.5.\\x*, ngemiziki\\x + 2.9,17.\\x*\\x + Zaga 6.5.\\x* kumbe ngezimpala zeganga\\x + 2.9,17.\\x*\\x + Zaga 5.19.\\x*, ukuze lingaphazamisi lingavusi uthando luze luthande.
\\p
\\v 8 Ilizwi lesithandwa sami! Khangela sona siyeza, siseqa phezu kwezintaba, siqolotsha phezu kwamaqaqa\\x + Isa. 52.7.\\x*.
\\v 9 Isithandwa sami sinjengomziki\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x* kumbe njengethole lendluzele\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x*\\x + 4.5. 7.3.\\x*. Khangela simi ngemva komduli wethu, silunguza emawindini, sizibonakalisa\\f + \\fk zibonakalisa: \\fl Heb. \\fq hluma.\\f* eminxibeni yewindi\\x + Isa. 14.16.\\x*.
\\v 10 Isithandwa sami saphendula sathi kimi\\x + Hlu. 18.14.\\x*: Vuka wena\\x + 2.13.\\x*, mngane wami, omuhle wami, a\\add si\\add*hambe wena\\x + 1.15.\\x*.
\\v 11 Ngoba khangela, ubusika sebudlulile, izulu seliphelile, lihambile.
\\v 12 Amaluba ayabonakala emhlabeni, isikhathi sokuhlabelela \\add se\\add*sifikile, lelizwi lejuba liyezwakala elizweni lakithi\\x + Hlab. 74.19. Jer. 8.7.\\x*.
\\v 13 Isihlahla somkhiwa sivuthisa imikhiwa yaso eluhlaza, lamavini \\add ale\\add*mpoko\\x + 2.15. 7.12.\\x* aletha iphunga elimnandi\\x + Tshu. 7.1.\\x*. Vuka wena, mngane wami, omuhle wami wena, \\add si\\add*hambe\\x + 2.10.\\x*.
\\p
\\v 14 Juba lami\\x + 5.2. 6.9. 1.15. Mat. 10.16.\\x*, \\add elis\\add*engoxweni yedwala\\x + Jer. 48.28.\\x*\\x + Jer. 49.16. Obad. 3.\\x*, ekusithekeni kweliwa\\x + Hez. 38.20.\\x*, ngitshengisa ubuso bakho, ngizwise ilizwi lakho\\x + 8.13.\\x*, ngoba ilizwi lakho limnandi, lobuso bakho buyabukeka\\x + 1.5.\\x*.
\\v 15 Sibambeleni amakhanka, amakhanka amancinyane, ona izivini\\x + Hez. 13.4. Luka 13.32.\\x*, ngoba izivini zethu \\add zile\\add*zimpoko\\x + 2.15. 7.12.\\x*.
\\v 16 Isithandwa sami ngesami, lami ngingowaso\\x + 6.3. 7.10.\\x*, eselusa phakathi kwemiduze\\x + 2.1. 4.5. 6.3.\\x*.
\\v 17 Kuze kube semadabukakusa, lamathunzi abaleke\\x + 4.6.\\x*, phenduka, sithandwa sami, ube njengomziki kumbe njengethole lendluzele\\x + 8.14. 2.9.\\x* phezu kwezintaba zeBhetheri\\x + 2 Sam. 2.29.\\x*.
EOD;
    $_SERVER['HTTP_USER_AGENT'] = "PHPUnit";
    $_SERVER['REMOTE_ADDR'] = "127.0.0.1";
    $_SESSION['user'] = "PHPUnit";
  }
  

  private function initialize_directories ($directory) 
  {
    mkdir ("$directory/Psalms");
    mkdir ("$directory/Psalms/0");
    mkdir ("$directory/Psalms/11");
    mkdir ("$directory/Song of Solomon");
    mkdir ("$directory/Song of Solomon/2");
  }

  /**
  * This tests round-tripping git Bible data in the file system,
  * being transferred to the database, then back to the filesystem.
  */
  public function testFiledata2database2filedata()
  {
    // Working directory.
    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);

    // Set up testing data.    
    $this->initialize_data();
    $this->initialize_directories ($directory);
    
    // Simulate the presence of some chapters.
    file_put_contents ("$directory/Psalms/0/data", $this->psalms_0_data);
    file_put_contents ("$directory/Psalms/11/data", $this->psalms_11_data);
    file_put_contents ("$directory/Song of Solomon/2/data", $this->song_of_solomon_2_data);

    // Create a temporal Bible in the database and store some data in it.
    // Filter_Git is supposed to erase this data if it is not in the filesystem.
    $database_bibles = Database_Bibles::getInstance();
    $bible = "PHPUnit";
    $database_bibles->createBible ($bible);
    $database_bibles->storeChapter ($bible, 2, 1, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 3, 4, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 5, 6, $this->song_of_solomon_2_data);

    // Call the filter.
    Filter_Git::bibleFiledata2database ($directory, $bible);

    $database_bibles = Database_Bibles::getInstance();
    
    // Assert database has certain books.
    $books = $database_bibles->getBooks ($bible);
    $this->assertEquals(array(19, 22), $books);
    
    // Assert Psalms has certain chapters.
    $chapters = $database_bibles->getChapters ($bible, 19);
    $this->assertEquals(array(0, 11), $chapters);

    // Assert Song of Solomon has a certain chapter.
    $chapters = $database_bibles->getChapters ($bible, 22);
    $this->assertEquals(array(2), $chapters);

    // Assert data in Psalm 11.
    $data = $database_bibles->getChapter ($bible, 19, 11);
    $this->assertEquals($this->psalms_11_data, $data);

    // New working directory.
    $newdirectory = tempnam (sys_get_temp_dir(), '');
    unlink ($newdirectory);
    mkdir ($newdirectory);

    // Call the filter.
    Filter_Git::bibleDatabase2filedata ($bible, $newdirectory);

    // Compare new directory with the standard one.
    system ("diff -r $newdirectory $directory", &$exit_code);
    $this->assertEquals(0, $exit_code);
    
    // Tear down.
    $database_bibles->deleteBible ($bible);
  }


  /**
  * This tests round-tripping a git repository in the file system,
  * being transferred to the database, then back to the filesystem.
  */
  public function testRepository2database2repository()
  {
    // Working directory.
    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);

    // Set up testing data.    
    $this->initialize_data();
    $this->initialize_directories ($directory);

    // Simulate the presence of some chapters.
    file_put_contents ("$directory/Psalms/0/data", $this->psalms_0_data);
    file_put_contents ("$directory/Psalms/11/data", $this->psalms_11_data);
    file_put_contents ("$directory/Song of Solomon/2/data", $this->song_of_solomon_2_data);

    // Create a git repository in the working directory.
    system ("cd $directory; git init > /dev/null 2>&1", &$exit_code);
    $this->assertEquals(0, $exit_code);
    system ("cd $directory; git add .", &$exit_code);
    $this->assertEquals(0, $exit_code);
    system ("cd $directory; git commit -a -m test > /dev/null 2>&1", &$exit_code);
    $this->assertEquals(0, $exit_code);

    // Remove the data so the .git directory only is left.
    unlink ("$directory/Psalms/0/data");
    unlink ("$directory/Psalms/11/data");
    unlink ("$directory/Song of Solomon/2/data");
    rmdir ("$directory/Psalms/0");
    rmdir ("$directory/Psalms/11");
    rmdir ("$directory/Psalms");
    rmdir ("$directory/Song of Solomon/2");
    rmdir ("$directory/Song of Solomon");

    // Run the filter.
    $bible = "PHPUnit";
    Filter_Git::repository2database ($directory, $bible);

    // Copy the git repository from the database into a new directory.
    $newdirectory = tempnam (sys_get_temp_dir(), '');
    unlink ($newdirectory);
    mkdir ($newdirectory);
    Filter_Git::database2repository ($bible, $newdirectory);

    // Compare new directory with the standard one.
    system ("diff -r $newdirectory/.git $directory/.git", &$exit_code);
    $this->assertEquals(0, $exit_code);
    
    // Tear down.
    $database_repositories = Database_Repositories::getInstance();
    $database_repositories->deleteRepository ($bible);
  }


  /**
  * This tests round-tripping consultation notes to the file system.
  * The notes are in the database, get copied to the file system, 
  * then back into the database.
  */
  public function testNotesDatabaseToFileToDatabase()
  {
    $database_notes = Database_Notes::getInstance ();
    $this->initialize_data ();
    
    // Working directory.
    $directory = tempnam (sys_get_temp_dir(), '');
    unlink ($directory);
    mkdir ($directory);
    echo "\n$directory\n";

    // Create a few known notes to be used as testing data.
    $identifier1 = $database_notes->storeNewNote ("PHPUnit", 1, 2, 3, "PHPUnit1", "PHPUnit2", false);
    $identifier2 = $database_notes->storeNewNote ("PHPUnit", 4, 5, 6, "PHPUnit3", "PHPUnit4", false);
    $identifier3 = $database_notes->storeNewNote ("PHPUnit", 7, 8, 9, "PHPUnit5", "PHPUnit6", false);
    $database_notes->assignUser ($identifier2, "user1");
    $database_notes->assignUser ($identifier2, "user2");
    $database_notes->assignUser ($identifier2, "user3");
    $database_notes->subscribeUser ($identifier1, "subscriber1");
    $database_notes->subscribeUser ($identifier1, "subscriber2");
    $database_notes->setPrivacy ($identifier3, 5);
    
    // Transfer these notes (and other ones that may be in the database) to the file system.
    Filter_Git::notesDatabase2filedata ($directory);


    /*

    


    // Create a temporal Bible in the database and store some data in it.
    // Filter_Git is supposed to erase this data if it is not in the filesystem.
    $database_bibles = Database_Bibles::getInstance();
    $bible = "PHPUnit";
    $database_bibles->createBible ($bible);
    $database_bibles->storeChapter ($bible, 2, 1, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 3, 4, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 5, 6, $this->song_of_solomon_2_data);

    // Call the filter.
    Filter_Git::bibleFiledata2database ($directory, $bible);

    $database_bibles = Database_Bibles::getInstance();
    
    // Assert database has certain books.
    $books = $database_bibles->getBooks ($bible);
    $this->assertEquals(array(19, 22), $books);
    
    // Assert Psalms has certain chapters.
    $chapters = $database_bibles->getChapters ($bible, 19);
    $this->assertEquals(array(0, 11), $chapters);

    // Assert Song of Solomon has a certain chapter.
    $chapters = $database_bibles->getChapters ($bible, 22);
    $this->assertEquals(array(2), $chapters);

    // Assert data in Psalm 11.
    $data = $database_bibles->getChapter ($bible, 19, 11);
    $this->assertEquals($this->psalms_11_data, $data);

    // New working directory.
    $newdirectory = tempnam (sys_get_temp_dir(), '');
    unlink ($newdirectory);
    mkdir ($newdirectory);

    // Call the filter.
    Filter_Git::bibleDatabase2filedata ($bible, $newdirectory);

    // Compare new directory with the standard one.
    system ("diff -r $newdirectory $directory", &$exit_code);
    $this->assertEquals(0, $exit_code);
    
    */
    // Tear down.
    $database_notes->delete ($identifier1);
    $database_notes->delete ($identifier2);
  }




}
?>


