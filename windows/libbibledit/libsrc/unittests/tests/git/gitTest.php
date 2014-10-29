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


class filterGitTest extends PHPUnit_Framework_TestCase
{


  private $bible;
  private $repository;
  private $newrepository;
  private $psalms_0_data;
  private $psalms_11_data;
  private $song_of_solomon_2_data;


  protected function setUp () 
  {
    $this->bible = "PHPUnit";
    $this->repository = uniqid (sys_get_temp_dir() . "/");
    $this->newrepository = uniqid (sys_get_temp_dir() . "/");

    $this->tearDown ();

    $database_search = Database_Search::getInstance ();
    $database_search->create ();
    $database_bibles = Database_Bibles::getInstance ();
    $database_bibles->createBible ($this->bible);

    mkdir ($this->repository);
    mkdir ($this->newrepository);

    $command = "cd " . $this->repository . "; git init 2>&1";
    exec ($command, $result, $exit_code);
    $command = "cd " . $this->newrepository . "; git init 2>&1";
    exec ($command, $result, $exit_code);

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

    mkdir ($this->repository . "/Psalms");
    mkdir ($this->repository . "/Psalms/0");
    mkdir ($this->repository . "/Psalms/11");
    mkdir ($this->repository . "/Song of Solomon");
    mkdir ($this->repository . "/Song of Solomon/2");

    file_put_contents ($this->repository . "/Psalms/0/data", $this->psalms_0_data);
    file_put_contents ($this->repository . "/Psalms/11/data", $this->psalms_11_data);
    file_put_contents ($this->repository . "/Song of Solomon/2/data", $this->song_of_solomon_2_data);

    $_SERVER['HTTP_USER_AGENT'] = "PHPUnit";
    $_SERVER['REMOTE_ADDR'] = "127.0.0.1";
    $session_logic = Session_Logic::getInstance ();
    $session_logic->setUsername ("PHPUnit");
  }


  protected function tearDown ()
  {
    $database_modifications = Database_Modifications::getInstance ();
    $database_bibles = Database_Bibles::getInstance();
    $database_modifications->deleteTeamDiffBible ($this->bible);
    $database_bibles->deleteBible ($this->bible);
    Filter_Rmdir::rmdir ($this->repository);
    Filter_Rmdir::rmdir ($this->newrepository);
  }


  public function testSyncBibleToGit1 ()
  {
    $database_bibles = Database_Bibles::getInstance();

    $repository = $this->repository;
    
    $this->assertFileExists ("$repository/.git");
    $this->assertFileExists ("$repository/Psalms/0/data");
    $this->assertFileExists ("$repository/Psalms/11/data");
    $this->assertFileExists ("$repository/Song of Solomon/2/data");
    $this->assertFileNotExists ("$repository/Exodus/1/data");

    $database_bibles->storeChapter ($this->bible, 2, 1, $this->song_of_solomon_2_data);
    Filter_Git::syncBible2Git ($this->bible, $this->repository);

    $this->assertFileExists ("$repository/.git");
    $this->assertFileNotExists ("$repository/Psalms/0/data");
    $this->assertFileNotExists ("$repository/Psalms/11/data");
    $this->assertFileNotExists ("$repository/Song of Solomon/2/data");
    $this->assertFileExists ("$repository/Exodus/1/data");
  }


  public function testSyncBibleToGit2 ()
  {
    $database_bibles = Database_Bibles::getInstance();

    $repository = $this->repository;
    
    $this->assertFileExists ("$repository/.git");
    $this->assertFileExists ("$repository/Psalms/0/data");
    $this->assertFileExists ("$repository/Psalms/11/data");
    $this->assertFileExists ("$repository/Song of Solomon/2/data");
    $this->assertFileNotExists ("$repository/Exodus/1/data");

    $database_bibles->storeChapter ($this->bible, 19, 1, $this->song_of_solomon_2_data);
    Filter_Git::syncBible2Git ($this->bible, $this->repository);

    $this->assertFileExists ("$repository/.git");
    $this->assertFileNotExists ("$repository/Psalms/0/data");
    $this->assertFileExists ("$repository/Psalms/1/data");

    $data = file_get_contents ("$repository/Psalms/1/data");
    $this->assertEquals ($this->song_of_solomon_2_data, $data);
  }

  
  public function testSyncBibleToGit3 ()
  {
    $database_bibles = Database_Bibles::getInstance();

    $repository = $this->repository;
    
    $this->assertFileExists ("$repository/.git");
    $this->assertFileExists ("$repository/Psalms/0/data");
    $this->assertFileExists ("$repository/Psalms/11/data");
    $this->assertFileExists ("$repository/Song of Solomon/2/data");
    $this->assertFileNotExists ("$repository/Exodus/1/data");

    $database_bibles->storeChapter ($this->bible, 19, 1, $this->song_of_solomon_2_data);
    $database_bibles->storeChapter ($this->bible, 22, 2, $this->psalms_11_data);
    $database_bibles->storeChapter ($this->bible, 19, 11, $this->song_of_solomon_2_data);
    Filter_Git::syncBible2Git ($this->bible, $this->repository);

    $this->assertFileExists ("$repository/.git");
    $this->assertFileExists ("$repository/Song of Solomon/2/data");
    $this->assertFileExists ("$repository/Psalms/1/data");
    $this->assertFileExists ("$repository/Song of Solomon/2/data");
    $this->assertFileExists ("$repository/Psalms/11/data");

    $data = file_get_contents ("$repository/Song of Solomon/2/data");
    $this->assertEquals ($this->psalms_11_data, $data);

    $data = file_get_contents ("$repository/Psalms/11/data");
    $this->assertEquals ($this->song_of_solomon_2_data, $data);

    $data = file_get_contents ("$repository/Psalms/1/data");
    $this->assertEquals ($this->song_of_solomon_2_data, $data);
  }


  public function testGetPullPassage ()
  {
    $output = Filter_Git::getPullPassage ("From https://github.com/joe/test");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage ("   443579b..90dcb57  master     -> origin/master");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage ("Updating 443579b..90dcb57");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage ("Fast-forward");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage (" Genesis/3/data | 2 +-");
    $this->assertEquals (array ('book' => "1", 'chapter' => "3"), $output);
    $output = Filter_Git::getPullPassage (" 1 file changed, 1 insertion(+), 1 deletion(-)");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage (" delete mode 100644 Leviticus/1/data");
    $this->assertNull ($output);
    $output = Filter_Git::getPullPassage (" Revelation/3/data | 2 +-");
    $this->assertEquals (array ('book' => "66", 'chapter' => "3"), $output);
  }
  

  public function testSyncGitToBibleAddChapters ()
  {
    $database_bibles = Database_Bibles::getInstance();
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    // Run the filter, and check that all three chapters are now the database.
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);
    $books = $database_bibles->getBooks ($this->bible);
    $this->assertEquals ($books, array (19, 22));
    // Check that the data matches.
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEquals ($this->psalms_0_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ($this->psalms_11_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEquals ($this->song_of_solomon_2_data, $usfm);
  }


  public function testSyncGitToBibleDeleteChapters ()
  {
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    $database_bibles = Database_Bibles::getInstance();
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);
    // Remove one book and one chapter from the git repository,
    // and check that after running the filter, the database is updated accordingly.
    Filter_Rmdir::rmdir ($this->repository . "/Song of Solomon");
    Filter_Rmdir::rmdir ($this->repository . "/Psalms/0");
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);
    $books = $database_bibles->getBooks ($this->bible);
    $this->assertEquals ($books, array (19));
    // Check that the data matches.
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEquals ("", $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ($this->psalms_11_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEquals ("", $usfm);
  }


  public function testSyncGitToBibleUpdateChapters ()
  {
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    $database_bibles = Database_Bibles::getInstance();
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);
    // Update some chapters in the git repository,
    // and check that after running the filter, the database is updated accordingly.
    file_put_contents ($this->repository . "/Psalms/11/data", "\\c 11");
    file_put_contents ($this->repository . "/Song of Solomon/2/data", "\\c 2");
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEquals ($this->psalms_0_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ("\\c 11", $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEquals ("\\c 2", $usfm);
  }


  public function testSyncGitChapterToBibleAddChapters ()
  {
    $database_bibles = Database_Bibles::getInstance();

    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEmpty ($usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEmpty ($usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEmpty ($usfm);

    // Run the filter for each chapter, and check that all three chapters make it into the database.
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 19, 0);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEquals ($this->psalms_0_data, $usfm);

    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 19, 11);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ($this->psalms_11_data, $usfm);
    
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 22, 2);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEquals ($this->song_of_solomon_2_data, $usfm);

    // Check the two books are there.
    $books = $database_bibles->getBooks ($this->bible);
    $this->assertEquals ($books, array (19, 22));
  }


  public function testSyncGitChapterToBibleDeleteChapters ()
  {
    $database_bibles = Database_Bibles::getInstance ();

    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);

    // Remove one book and one chapter from the git repository,
    Filter_Rmdir::rmdir ($this->repository . "/Song of Solomon");
    Filter_Rmdir::rmdir ($this->repository . "/Psalms/0");

    // Run updates on the three chapters.
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 19, 0);
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 19, 11);
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 22, 2);

    // There should still be two books, although one book would have no chapters.
    $books = $database_bibles->getBooks ($this->bible);
    $this->assertEquals ($books, array (19, 22));

    // Check that the chapter data matches.
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEmpty ($usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ($this->psalms_11_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEmpty ($usfm);
  }
  

  public function testSyncGitChapterToBibleUpdateChapters ()
  {
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the Bible database.
    $database_bibles = Database_Bibles::getInstance ();
    Filter_Git::syncGit2Bible ($this->repository, $this->bible);

    // Update some chapters in the git repository.
    file_put_contents ($this->repository . "/Psalms/11/data", "\\c 11");
    file_put_contents ($this->repository . "/Song of Solomon/2/data", "\\c 2");

    // Run updates on the two chapters.
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 19, 11);
    Filter_Git::syncGitChapter2Bible ($this->repository, $this->bible, 22, 2);

    // Check that the database is updated accordingly.
    $usfm = $database_bibles->getChapter ($this->bible, 19, 0);
    $this->assertEquals ($this->psalms_0_data, $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 19, 11);
    $this->assertEquals ("\\c 11", $usfm);
    $usfm = $database_bibles->getChapter ($this->bible, 22, 2);
    $this->assertEquals ("\\c 2", $usfm);
  }
  

  public function testExplodePath ()
  {
    $bookChapter = Filter_Git::explodePath ("Genesis/2/data");
    $this->assertEquals (array ('book' => 1, 'chapter' => 2), $bookChapter);
    $bookChapter = Filter_Git::explodePath ("Genesi/2/data");
    $this->assertEquals (NULL, $bookChapter);
    $bookChapter = Filter_Git::explodePath ("Exodus/3/data");
    $this->assertEquals (array ('book' => 2, 'chapter' => 3), $bookChapter);
    $bookChapter = Filter_Git::explodePath ("dictionary");
    $this->assertEquals (NULL, $bookChapter);
  }


}
?>
