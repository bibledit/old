<?php
require_once 'PHPUnit/Framework.php';
 
class filterGitTest extends PHPUnit_Framework_TestCase
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
    // Filter_Git, when called with this data, is supposed to erase this data if it is not in the filesystem.
    $database_bibles = Database_Bibles::getInstance();
    $bible = "PHPUnit";
    $database_bibles->createBible ($bible);
    $database_bibles->storeChapter ($bible, 2, 1, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 3, 4, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($bible, 5, 6, $this->song_of_solomon_2_data);

    // Call the filter for each chapter.
    Filter_Git::bibleFiledata2database ($directory, $bible, "Exodus/1/data |    2 +-");
    Filter_Git::bibleFiledata2database ($directory, $bible, "Leviticus/4/data |    2 +-");
    Filter_Git::bibleFiledata2database ($directory, $bible, "Deuteronomy/6/data |    2 +-");
    Filter_Git::bibleFiledata2database ($directory, $bible, "Psalms/0/data |    2 +-");
    Filter_Git::bibleFiledata2database ($directory, $bible, "Psalms/11/data |    2 +-");
    Filter_Git::bibleFiledata2database ($directory, $bible, "Song of Solomon/2/data |    2 +-");

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

    // Create a few known notes to be used as testing data.
    $identifier1 = $database_notes->storeNewNote ("PHPUnit", 1, 2, 3, "PHPUnit1", "PHPUnit2", false);
    $identifier2 = $database_notes->storeNewNote ("PHPUnit", 4, 5, 6, "PHPUnit3", "PHPUnit4", false);
    $identifier3 = $database_notes->storeNewNote ("PHPUnit", 7, 8, 9, "PHPUnit5", "PHPUnit6", false);
    $identifier4 = $database_notes->storeNewNote ("bible4", 7, 8, 9, "summary4", "contents4", false);
    $identifier5 = $database_notes->storeNewNote ("bible5", 7, 8, 9, "summary5", "contents5", false);
    $database_notes->assignUser ($identifier2, "user1");
    $database_notes->assignUser ($identifier2, "user2");
    $database_notes->assignUser ($identifier2, "user3");
    $database_notes->subscribeUser ($identifier1, "subscriber1");
    $database_notes->subscribeUser ($identifier1, "subscriber2");
    $database_notes->setPrivacy ($identifier3, 5);
    
    // Transfer these notes (and other ones that may be in the database) to the file system.
    Filter_Git::notesDatabase2filedata ($directory);

    // Load the three notes files into an array.
    $contents1 = explode ("\n", file_get_contents ("$directory/$identifier1"));
    $contents2 = explode ("\n", file_get_contents ("$directory/$identifier2"));
    $contents3 = explode ("\n", file_get_contents ("$directory/$identifier3"));
    
    // Do some checks on the notes files.
    $this->assertTrue (in_array ("subscriber1", $contents1));
    $this->assertTrue (in_array ("subscriber2", $contents1));
    $this->assertTrue (in_array ("user1", $contents2));
    $this->assertFalse (in_array ("user0", $contents2));
    $this->assertTrue (in_array ("4.5.6", $contents2));
    $this->assertTrue (in_array ("7.8.9", $contents3));
    $this->assertTrue (in_array ("PHPUnit5", $contents3));

    // Simulate the git repository changing the file contents.
    $contents1[1] = 1234567890; // Change modification time.
    $contents1[7] = "TestBible"; // Change Bible.
    $contents1[11] = "Old"; // Change Status.
    unset ($contents1[19]); // Change Contents.
    unset ($contents1[20]);
    unset ($contents1[21]);
    $contents1 [] = "New Contents";
    unset ($contents1[5]); // Remove subscriber2.
    unset ($contents2[3]); // Remove assignee 'user1';
    $contents2[10] = "40.50.60"; // Update passages.
    $contents3[11] = 10; // Update severity.
    $contents3[13] = 55; // Update privacy.
    $contents3[15] = "-summary-"; // Update summary.
    
    // Store the new content to file; delete or rename notes.
    file_put_contents ("$directory/$identifier1", implode ("\n", $contents1));
    file_put_contents ("$directory/$identifier2", implode ("\n", $contents2));
    file_put_contents ("$directory/$identifier3", implode ("\n", $contents3));
    unlink ("$directory/$identifier4");
    $identifier6 = $identifier5 + 1;
    rename ("$directory/$identifier5", "$directory/$identifier6");

    // Put the file contents back into the consultation notes database.
    Filter_Git::notesFiledata2database ($directory, "$identifier1 |    4 ++--");
    Filter_Git::notesFiledata2database ($directory, "$identifier2 |    4 ++--");
    Filter_Git::notesFiledata2database ($directory, "$identifier3 |    4 ++--");
    Filter_Git::notesFiledata2database ($directory, "$identifier4 |    4 ++--");
    Filter_Git::notesFiledata2database ($directory, "$identifier5 |    4 ++--");
    Filter_Git::notesFiledata2database ($directory, "$identifier6 |    4 ++--");

    // Some notes should be there, and others should have been removed.    
    $this->assertTrue ($database_notes->identifierExists ($identifier1));
    $this->assertTrue ($database_notes->identifierExists ($identifier2));
    $this->assertTrue ($database_notes->identifierExists ($identifier3));
    $this->assertFalse ($database_notes->identifierExists ($identifier4));
    $this->assertFalse ($database_notes->identifierExists ($identifier5));
    $this->assertTrue ($database_notes->identifierExists ($identifier6));

    // See if all changed data made it.
    $this->assertEquals (1234567890, $database_notes->getModified ($identifier1));
    $this->assertEquals ("TestBible", $database_notes->getBible ($identifier1));
    $this->assertEquals ("Old", $database_notes->getStatus ($identifier1));
    $this->assertEquals ("New Contents", $database_notes->getContents ($identifier1));
    $this->assertEquals (array ("subscriber1"), $database_notes->getSubscribers ($identifier1));
    $this->assertTrue (in_array ("user2", $database_notes->getAssignees ($identifier2)));
    $this->assertTrue (in_array ("user3", $database_notes->getAssignees ($identifier2)));
    $this->assertEquals (array (array (40, 50, 60)), $database_notes->getPassages ($identifier2));
    $this->assertEquals (10, $database_notes->getRawSeverity ($identifier3));
    $this->assertEquals (55, $database_notes->getPrivacy ($identifier3));
    $this->assertEquals ("-summary-", $database_notes->getSummary ($identifier3));
    $this->assertEquals ("bible5", $database_notes->getBible ($identifier6));
    $this->assertEquals ("summary5", $database_notes->getSummary ($identifier6));

    // Tear down.
    $database_notes->delete ($identifier1);
    $database_notes->delete ($identifier2);
    $database_notes->delete ($identifier3);
    $database_notes->delete ($identifier4);
    $database_notes->delete ($identifier5);
    $database_notes->delete ($identifier6);
  }


  /*
  * Tests the git filter that resolves a conflict.
  */
  public function testResolveConflict()
  {
$contents = <<<'EOD'
<<<<<<< HEAD:3 John/1/data
\v 1 xFrom the church leader.
=======
\v 1 xxFrom the church leader.
>>>>>>> a62f843ce41ed2d0325c8a2767993df6acdbc933:3 John/1/data
EOD;
    $filename = tempnam (sys_get_temp_dir(), '');
    file_put_contents ($filename, $contents);
    Filter_Git::resolveConflict ($contents, $filename);
$reference = <<<'EOD'
\v 1 xxFrom the church leader.
EOD;
    $this->assertEquals ($reference, $contents);
    $contents = file_get_contents ($filename);
    $this->assertEquals ($reference, $contents);

$contents = <<<'EOD'
\v 1 xFrom the church leader.
EOD;
    file_put_contents ($filename, $contents);
    Filter_Git::resolveConflict ($contents, NULL);
$reference = <<<'EOD'
\v 1 xFrom the church leader.
EOD;
    $this->assertEquals ($reference, $contents);
    $contents = file_get_contents ($filename);
    $this->assertEquals ($reference, $contents);
  }


}
?>


