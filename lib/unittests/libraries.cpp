/*
Copyright (©) 2003-2016 Teus Benschop.

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


#include <unittests/libraries.h>
#include <unittests/utilities.h>
#include <filter/string.h>
#include <filter/url.h>
#include <filter/roles.h>
#include <filter/md5.h>
#include <filter/usfm.h>
#include <filter/diff.h>
#include <filter/memory.h>
#include <filter/date.h>
#include <flate/flate.h>
#include <config/globals.h>
#include <database/config/general.h>
#include <database/config/bible.h>
#include <database/logs.h>
#include <database/sqlite.h>
#include <database/users.h>
#include <database/books.h>
#include <database/versifications.h>
#include <database/state.h>
#include <database/login.h>
#include <database/notes.h>
#include <database/modifications.h>
#include <checksum/logic.h>
#include <editor/html2usfm.h>
#include <editor/usfm2html.h>
#include <workspace/logic.h>
#include <client/logic.h>
#include <styles/logic.h>
#include <checks/sentences.h>
#include <checks/versification.h>
#include <checks/usfm.h>
#include <checks/verses.h>
#include <checks/pairs.h>
#include <manage/hyphenate.h>
#include <search/logic.h>
#include <jsonxx/jsonxx.h>
#include <related/logic.h>
#include <editone/logic.h>
#include <webserver/http.h>
#include <demo/logic.h>
#include <search/rebibles.h>
#include <search/renotes.h>
#include <changes/modifications.h>
#include <checks/run.h>
#include <export/usfm.h>
#include <export/textusfm.h>
#include <export/odt.h>
#include <export/esword.h>
#include <export/onlinebible.h>
#include <export/quickbible.h>
#include <sword/logic.h>
#include <tasks/logic.h>
#include <resource/logic.h>
#include <user/logic.h>
#include <rss/logic.h>


using namespace jsonxx;


void test_client_logic ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  // Test Client Enabled.
  {
    bool enabled = client_logic_client_enabled ();
    evaluate (__LINE__, __func__, false, enabled);
    client_logic_enable_client (true);
    // When a client is disabled in config.h, it remains disabled, no matter the setting in the database.
    // It means that this unit test depends on client mode to be off in ./configure.
    enabled = client_logic_client_enabled ();
#ifndef HAVE_CLIENT
    evaluate (__LINE__, __func__, false, enabled);
#endif
    client_logic_enable_client (false);
    enabled = client_logic_client_enabled ();
    evaluate (__LINE__, __func__, false, enabled);
  }
  // Test Create Note En/Decode
  {
    string data = client_logic_create_note_encode ("bible", 1, 2, 3, "summary", "line1\nline2", false);
    string standard =
    "bible\n"
    "1\n"
    "2\n"
    "3\n"
    "summary\n"
    "0\n"
    "line1\n"
    "line2";
    evaluate (__LINE__, __func__, standard, data);

    string bible;
    int book, chapter, verse;
    string summary, contents;
    bool raw;
    client_logic_create_note_decode (standard, bible, book, chapter, verse, summary, contents, raw);
    evaluate (__LINE__, __func__, "bible", bible);
    evaluate (__LINE__, __func__, 1, book);
    evaluate (__LINE__, __func__, 2, chapter);
    evaluate (__LINE__, __func__, 3, verse);
    evaluate (__LINE__, __func__, "summary", summary);
    evaluate (__LINE__, __func__, false, raw);
    standard =
    "line1\n"
    "line2";
    evaluate (__LINE__, __func__, standard, contents);
  }
}


Checks_Sentences test_check_sentences_setup ()
{
  trace_unit_tests (__func__);
  
  Checks_Sentences check;
  check.enterCapitals ("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
  check.enterSmallLetters ("a b c d e f g h i j k l m n o p q r s t u v w x y z");
  check.enterEndMarks (". ! ? :");
  check.enterCenterMarks (", ;");
  check.enterDisregards ("( ) [ ] { } ' \" * - 0 1 2 3 4 5 6 7 8 9");
  check.enterNames ("Nkosi Longnamelongnamelongname");
  check.initialize ();
  return check;
}


void test_check_sentences ()
{
  trace_unit_tests (__func__);
  
  // Test unknown character.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({make_pair (1, "Abc ζ abc.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {make_pair (1, "Unknown character: ζ")};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test capital after mid-sentence punctuation mark.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({make_pair (2, "He said, Go.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {make_pair (2, "Capital follows mid-sentence punctuation mark: He said, Go.")};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test capital straight after mid-sentence punctuation mark.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({make_pair (2, "He said,Go.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { make_pair (2, "Capital follows straight after a mid-sentence punctuation mark: He said,Go.")};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test small letter straight after mid-sentence punctuation mark.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({make_pair (2, "He said,go.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { make_pair (2, "Small letter follows straight after a mid-sentence punctuation mark: He said,go.")};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test two verses okay.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({ make_pair (17, "Jezus kwam naar de wereld,"), make_pair (18, "dat hij zou lijden.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard;
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test name after comma several verses okay.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({
      make_pair (17, "Kwasekusithi esebakhuphele ngaphandle, yathi: Baleka ngenxa yempilo yakho, ungakhangeli ngemva kwakho, ungemi egcekeni lonke; balekela entabeni hlezi ubhujiswe."),
      make_pair (18, "ULothi wasesithi kuwo: Kakungabi njalo, Nkosi yami."),
      make_pair (19, "Khangela-ke, inceku yakho ithole umusa emehlweni akho, ukhulisile isihawu sakho, osenze kimi, ukugcina uphila umphefumulo wami; kodwa mina ngingeke ngiphephele entabeni, hlezi ububi bunamathele kimi, besengisifa."),
      make_pair (20, "Khangela-ke, lumuzi useduze ukubalekela kuwo; futhi umncinyane. Ngicela ngibalekele kuwo (kambe kawumncinyane?) Lomphefumulo wami uphile."),
      make_pair (21, "Yasisithi kuye: Khangela, ngibemukele ubuso bakho lakulolu udaba, ukuze ngingawuchithi umuzi okhulume ngawo."),
      make_pair (22, "Phangisa, balekela kuwo; ngoba ngingeze ngenza ulutho uze ufike kuwo. Ngakho babiza ibizo lomuzi ngokuthi yiZowari.")
    });
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard;
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test long name.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({make_pair (17, "O, Longnamelongnamelongname.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard;
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test no space after full stop.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({ make_pair (2, "He did that.He went.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (2, "A letter follows straight after an end-sentence punctuation mark: He did that.He went."),
      make_pair (2, "No capital after an end-sentence punctuation mark: did that.He went.")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test capital full stop.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({ make_pair (2, "He did that. he went.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { make_pair (2, "No capital after an end-sentence punctuation mark: did that. he went.")};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test paragraph
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"p"}, {""}, {{make_pair (1, "he said")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
                       make_pair (1, "Paragraph does not start with a capital: he said"),
                       make_pair (1, "Paragraph does not end with an end marker: he said")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test that a paragraph started by \q and starting with a small letter, is not flagged.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"q"}, {"q"}, {{ make_pair (1, "he said")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Paragraph does not end with an end marker: he said")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test that a paragraph without proper ending, where the next paragraph starts with e.g. \q, is not flagged.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"p", "q"}, {"q"}, {{ make_pair (1, "He said,")}, { make_pair (1, "he is Jesus.")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test flagging a paragraph that starts with a Greek small letter.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"p"}, {"q"}, {{ make_pair (1, "εὐθέως")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Paragraph does not start with a capital: εὐθέως"),
      make_pair (1, "Paragraph does not end with an end marker: εὐθέως")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test a correct paragraph.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"p"}, {"q"}, {{ make_pair (1, "Immediately εὐθέως.")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard;
    evaluate (__LINE__, __func__, standard, results);
  }
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.paragraphs ({"q1"}, {"q1", "q"}, {{ make_pair (1, "Immediately εὐθέως.")}});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard;
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test two punctuation marks.
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({ make_pair (2, "He did that..")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { make_pair (2, "Two punctuation marks in sequence: He did that..")};
    evaluate (__LINE__, __func__, standard, results);
  }
  {
    Checks_Sentences check = test_check_sentences_setup ();
    check.check ({ make_pair (2, "He did ;. That.")});
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { make_pair (2, "Two punctuation marks in sequence: He did ;. That.")};
    evaluate (__LINE__, __func__, standard, results);
  }
}


void test_check_versification ()
{
  trace_unit_tests (__func__);
  
  // Test Available Books
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    Database_Check database_check;
    database_check.create ();
    vector <int> books = database_versifications.getBooks (english ());
    vector <int> fault = filter_string_array_diff (books, {10});
    Checks_Versification::books ("Bible", fault);
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)results.size());
    if (results.size ()) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 10, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 1, hit.verse);
      evaluate (__LINE__, __func__, "This book is absent from the Bible", hit.data);
    }
  }
  // Test Available Chapters
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    Database_Check database_check;
    database_check.create ();

    // Ruth.
    int book = 8;
    // One missing and one extra chapter.
    vector <int> chapters = {0, 2, 3, 4, 5};
    // Run check.
    Checks_Versification::chapters ("Bible", book, chapters);

    // Verify results.
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, (int)results.size());
    if (results.size () == 2) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 1, hit.verse);
      evaluate (__LINE__, __func__, "This chapter is missing", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, 2, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 5, hit.chapter);
      evaluate (__LINE__, __func__, 1, hit.verse);
      evaluate (__LINE__, __func__, "This chapter is extra", hit.data);
    }
  }
  // Test Available Verses One
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    Database_Check database_check;
    database_check.create ();

    // Ruth 1.
    int book = 8;
    int chapter = 1;
    // One missing and one extra verse.
    vector <int> verses = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24};

    // Run check.
    Checks_Versification::verses ("Bible", book, chapter, verses);

    // Verify results.
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 4, (int)results.size());
    if (results.size () == 4) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 1, hit.verse);
      evaluate (__LINE__, __func__, "This verse is missing according to the versification system", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, 2, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 24, hit.verse);
      evaluate (__LINE__, __func__, "This verse is extra according to the versification system", hit.data);
      hit = results[2];
      evaluate (__LINE__, __func__, 3, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 2, hit.verse);
      evaluate (__LINE__, __func__, "The verse is out of sequence", hit.data);
      hit = results[3];
      evaluate (__LINE__, __func__, 4, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 24, hit.verse);
      evaluate (__LINE__, __func__, "The verse is out of sequence", hit.data);
    }
  }
  // Test Available Verses Two
  {
    refresh_sandbox (true);
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    Database_Check database_check;
    database_check.create ();
    
    // Ruth 1.
    int book = 8;
    int chapter = 1;

    // Verses out of order.
    vector <int> verses = {0, 2, 1, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22};
    
    // Run check.
    Checks_Versification::verses ("Bible", book, chapter, verses);
    
    // Verify results.
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 3, (int)results.size());
    if (results.size () == 3) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 2, hit.verse);
      evaluate (__LINE__, __func__, "The verse is out of sequence", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, 2, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 1, hit.verse);
      evaluate (__LINE__, __func__, "The verse is out of sequence", hit.data);
      hit = results[2];
      evaluate (__LINE__, __func__, 3, hit.rowid);
      evaluate (__LINE__, __func__, "Bible", hit.bible);
      evaluate (__LINE__, __func__, 8, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 3, hit.verse);
      evaluate (__LINE__, __func__, "The verse is out of sequence", hit.data);
    }
  }
}


void test_check_usfm ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  Database_Styles database_styles;
  database_styles.create ();
  // Test Malformed Verse
  {
    string usfm =
    "\\c 1\n"
    "\\p\n"
    "\\v 2,He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (2, "Malformed verse number: \\v 2,He said.")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test New Line In USFM Good
  {
    string usfm =
    "\\c 1\n"
    "\\p He said.\n"
    "\\v 1 He said.\n"
    "\\p He said.\n"
    "\\v 2 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = { };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test New Line In USFM One
  {
    string usfm =
    "\\c 1\n"
    "\\p He said.\n"
    "\\v 1 He said.\n"
    "\\\n"
    "\\p He said.\n"
    "\\v 2 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "New line within USFM:  \\ \\p He s")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test New Line In USFM Two
  {
    string usfm =
    "\\c 1\n"
    "\\p He said.\n"
    "\\v 1 He said.\n"
    "\\\n"
    "\\p He said.\n"
    "\\v 3 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "New line within USFM:  \\ \\p He s")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Unknown Usfm One
  {
    string usfm =
    "\\c 1\n"
    "\\p He said to the \\+nd LORD\\+nd*.\n"
    "\\v 1 He said \\add something\\add*.\n"
    "\\p,p He said.\n"
    "\\v 3 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Marker not in stylesheet: \\p,p ")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Unknown Usfm Two
  {
    string usfm =
    "\\c 1\n"
    "\\pHe said.\n"
    "\\v 1 He said \\add something\\add*.\n"
    "\\p He said.\n"
    "\\v 3 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Marker not in stylesheet: \\pHe ")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Id One
  {
    string usfm =
    "\\id GENN\n"
    "\\p He said.\n"
    "\\v 1 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Unknown ID: \\id GENN")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Id Two
  {
    string usfm =
    "\\id\n"
    "\\p He said.\n"
    "\\v 1 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Unknown ID: \\id\\p ")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Id Three
  {
    string usfm =
    "\\id Gen\n"
    "\\p He said.\n"
    "\\v 1 He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Unknown ID: \\id Gen")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Forward Slash One
  {
    string usfm =
    "\\id GEN\n"
    "\\p He said.\n"
    "\\v 1 He said. He said something/add*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Forward slash instead of backslash: /add")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Forward Slash Two
  {
    string usfm =
    "\\id GEN\n"
    "\\p He said.\n"
    "\\v 1 /v He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (0, "Forward slash instead of backslash: /v")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Clean Usfm
  {
    string usfm =
    "\\id GEN\n"
    "\\c 35\n"
    "\\s UNkulunkulu ubusisa uJakobe eBhetheli\n"
    "\\p\n"
    "\\v 1 UNkulunkulu wasesithi kuJakobe: Sukuma, yenyukela eBhetheli\\x + 28.13-19.\\x*, uhlale khona; umenzele lapho ilathi uNkulunkulu owabonakala kuwe mhla ubaleka ebusweni bukaEsawu umnewenu\\x + 27.43.\\x*.\n"
    "\\v 2 UJakobe wasesithi kundlu yakhe\\x + 18.19. Josh. 24.15.\\x* lakubo bonke ababelaye: Susani onkulunkulu bezizwe abaphakathi kwenu\\x + 31.19. Josh. 24.2,23. 1 Sam. 7.3.\\x*, lizihlambulule, lintshintshe izembatho zenu\\x + Eks. 19.10.\\x*.\n"
    "\\v 3 Asisukume senyukele eBhetheli, ngimenzele khona uNkulunkulu ilathi owangiphendula ngosuku lokuhlupheka kwami\\x + 32.7,24.\\x*, njalo waba lami endleleni engahamba kuyo\\x + 28.20; 31.3.\\x*.\n"
    "\\v 4 Basebenika uJakobe bonke onkulunkulu bezizwe ababesesandleni sabo, lamacici\\x + Hos. 2.13. Gen. 24.22.\\x* ayesezindlebeni zabo; uJakobe wasekufihla ngaphansi \\add kwesihlahla\\add* \\w se-okhi\\w**\\x + Josh. 24.26. 2 Sam. 18.9. 1 Kho. 13.14. 1 Lan. 10.12. Gen. 35.8.\\x* eliseShekema.\n"
    "\\v 5 Basebehamba; lokwesabeka kukaNkulunkulu\\x + 9.2. Eks. 15.16; 23.27. Dute. 11.25. Josh. 2.9. 2 Lan. 14.14.\\x* kwakuphezu kwemizi eyayibazingelezele, njalo kabawaxotshanga amadodana kaJakobe.\n"
    "\\p\n"
    "\\v 6 UJakobe wasefika eLuzi\\x + 28.19.\\x*, eselizweni leKhanani, eyiBhetheli, yena labantu bonke ababelaye.\n"
    "\\v 7 Wasesakha khona ilathi, wayibiza indawo \\add ngokuthi\\add* iEli-Bhetheli\\f + \\fk iEli-Bhetheli: \\fl okuyikuthi, \\fq uNkulunkulu weBhetheli.\\f*, ngoba lapho uNkulunkulu wabonakala kuye ekubalekeleni ubuso bomnewabo.\n"
    "\\v 8 Njalo uDebora umlizane kaRebeka wafa\\x + 24.59.\\x*, wangcwatshwa ezansi kweBhetheli ngaphansi kwesihlahla se-okhi\\x + 13.18; 35.4.\\x*, ngakho wasitha ibizo laso iAloni-Bakuthi\\f + \\fk iAloni-Bakuthi: \\fl okuyikuthi, \\fq ie-oki yokulila.\\f*.\n"
    "\\p\n"
    "\\v 9 UNkulunkulu wasebuya wabonakala kuJakobe evela ePadani-Arama, wambusisa.\n"
    "\\v 10 UNkulunkulu wasesithi kuye: Ibizo lakho nguJakobe; ibizo lakho kalisayikuthiwa nguJakobe\\x + 17.5,15.\\x*, kodwa uIsrayeli kuzakuba libizo lakho. Wabiza ibizo lakhe \\add ngokuthi\\add* uIsrayeli\\x + 32.28.\\x*.\n"
    "\\v 11 UNkulunkulu wasesithi kuye: NginguNkulunkulu uSomandla\\x + 17.1.\\x*; zala wande\\x + 28.3; 48.4.\\x*; isizwe, yebo ibandla lezizwe kuzavela kuwe\\x + 17.5,6,16; 26.4.\\x*; lamakhosi azaphuma ekhalweni lwakho.\n"
    "\\v 12 Lelizwe engalinika uAbrahama loIsaka ngizalinika wena, lenzalweni yakho emva kwakho ngizayinika ilizwe\\x + 12.7; 13.15; 17.8; 26.3; 28.13.\\x*.\n"
    "\\v 13 UNkulunkulu wasesenyuka esuka kuye endaweni lapho ayekhuluma laye khona\\x + 17.22.\\x*.\n"
    "\\v 14 UJakobe wasemisa insika endaweni lapho ayekhuluma laye khona, insika yelitshe\\x + 28.18; 31.45.\\x*. Wathululela phezu kwayo umnikelo wokunathwayo\\x + Nani 28.7.\\x*, wathela phezu kwayo amagcobo.\n"
    "\\v 15 UJakobe waseyitha ibizo lendawo, lapho uNkulunkulu ayekhulume laye khona, iBhetheli\\x + 28.19.\\x*.\n"
    "\\s Ukufa kukaRasheli\n"
    "\\p\n"
    "\\v 16 Basebehamba besuka eBhetheli; kwathi kusesengummango\\f + \\fk ummango: \\fl Heb. \\fq isiqetshana somhlabathi.\\f* ukuya eEfrathi, uRasheli wabeletha, wayelobunzima ekubeletheni.\n"
    "\\v 17 Kwasekusithi ekubeletheni kwakhe nzima, umbelethisi wathi kuye: Ungesabi, ngoba lalo uzakuba yindodana yakho\\x + 30.24. 1 Sam. 4.20.\\x*.\n"
    "\\v 18 Kwasekusithi umphefumulo wakhe usuphuma, ngoba esesifa, wayitha ibizo layo uBenoni\\f + \\fk uBenoni: \\fl okuyikuthi, \\fq indodana yosizi lwami.\\f*; kodwa uyise wayibiza \\add ngokuthi\\add* nguBhenjamini\\f + \\fk uBhenjamini: \\fl okuyikuthi, \\fq indodana yesandla sokunene.\\f*.\n"
    "\\v 19 URasheli wasesifa\\x + 48.7.\\x*, wangcwatshwa endleleni eya eEfrathi, eyiBhethelehema\\x + Ruthe 1.2; 4.11. Mika 5.2. Mat. 2.6,16-18.\\x*.\n"
    "\\v 20 UJakobe wasemisa insika phezu kwengcwaba lakhe; le yinsika yengcwaba likaRasheli kuze kube lamuhla\\x + 1 Sam. 10.2.\\x*.\n"
    "\\p\n"
    "\\v 21 UIsrayeli wasehamba, wamisa ithente lakhe ngaphambili komphotshongo weEderi\\x + Mika 4.8.\\x*.\n"
    "\\v 22 Kwasekusithi uIsrayeli ehlala kulelolizwe, uRubeni waya walala loBiliha\\x + 49.4. 1 Lan. 5.1. Gen. 30.4; 37.2. 2 Sam. 16.22; 20.3. 1 Kor. 5.1.\\x* umfazi omncinyane kayise; uIsrayeli wasekuzwa.\n"
    "\\s Amadodana kaJakobe lokufa kukaIsaka\n"
    "\\p Amadodana kaJakobe-ke ayelitshumi lambili\\x + 46.8-27. Eks. 1.2-4.\\x*;\n"
    "\\v 23 amadodana kaLeya: Izibulo likaJakobe, uRubeni, loSimeyoni loLevi loJuda loIsakari loZebuluni;\n"
    "\\v 24 amadodana kaRasheli: OJosefa loBhenjamini;\n"
    "\\v 25 lamadodana kaBiliha, incekukazi kaRasheli: ODani loNafithali;\n"
    "\\v 26 lamadodana kaZilipa, incekukazi kaLeya: OGadi loAsheri; la ngamadodana kaJakobe azalelwa yena ePadani-Arama.\n"
    "\\p\n"
    "\\v 27 UJakobe wasefika kuIsaka uyise eMamre\\x + 13.18; 23.2,19.\\x*, eKiriyathi-Arba\\x + Josh. 14.15; 15.13.\\x*, eyiHebroni, lapho uAbrahama ahlala khona njengowezizwe, loIsaka\\x + 28.10.\\x*.\n"
    "\\v 28 Lensuku zikaIsaka zaziyiminyaka elikhulu lamatshumi ayisificaminwembili.\n"
    "\\v 29 UIsaka wasehodoza, wafa, wabuthelwa ezizweni zakibo\\x + 15.15; 25.8.\\x*, emdala, enele ngensuku. Amadodana akhe uEsawu loJakobe asemngcwaba\\x + 25.9; 49.31.\\x*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Widow Backslash One
  {
    string usfm =
    "\\id GEN\n"
    "\\p\n"
    "\\v 1 \\ He said.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Widow backslash: \\ ")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Matching Markers One
  {
    string usfm =
    "\\id GEN\n"
    "\\v 1  He said \\add addition\\add*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Matching Markers Two
  {
    string usfm =
    "\\id GEN\n"
    "\\v 1  He said addition\\add*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Closing marker does not match opening marker : \\add*")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Matching Markers Three
  {
    string usfm =
    "\\id GEN\n"
    "\\v 1  He said \\add addition\\add .\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (1, "Repeating opening marker: \\add "),
      make_pair (1, "Unclosed markers: add")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Matching Markers Four
  {
    string usfm =
    "\\id GEN\n"
    "\\v 8 Kodwa uNowa wazuza umusa emehlweni eN\\nd kosi\\x + 19.19.\\nd*\\x*.\n"
    "\\v 9 Kodwa uNowa wazuza umusa emehlweni eN\\nd kosi\\x + 19.19.\\x*\\nd*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {};
    evaluate (__LINE__, __func__, standard, results);
  }
  // Test Matching Markers Five
  {
    string usfm =
    "\\id GEN\n"
    "\\v 8 Kodwa uNowa wazuza umusa \\add emehlweni eN\\nd kosi\\x + 19.19.\\nd*\\x*.\n";
    Checks_Usfm check = Checks_Usfm ("phpunit");
    check.initialize ();
    check.check (usfm);
    check.finalize ();
    vector <pair<int, string>> results = check.getResults ();
    vector <pair<int, string>> standard = {
      make_pair (8, "Unclosed markers: add")
    };
    evaluate (__LINE__, __func__, standard, results);
  }
}


void test_check_verses ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  Database_Check database_check;
  database_check.create ();
  // Test Missing Punctuation At End
  {
    map <int, string> verses = {
      make_pair (2, "He said."),
      make_pair (3, "He didn't say"),
      make_pair (4, "He said."),
      make_pair (5, "He said: “Jesus.”")
    };
    Checks_Verses::missingPunctuationAtEnd ("1", 1, 1, verses, ", ;", ". ! ? :", "”");
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)results.size());
    if (results.size ()) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "1", hit.bible);
      evaluate (__LINE__, __func__, 1, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 3, hit.verse);
      evaluate (__LINE__, __func__, "No punctuation at end of verse: y", hit.data);
    }
  }
  database_check.truncateOutput ("");
  // Test Pattern
  {
    map <int, string> verses = {
      make_pair (2, "He said."),
      make_pair (3, "He didn't say"),
      make_pair (4, "He said.")
    };
    Checks_Verses::patterns ("1", 1, 1, verses, {"did"});
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 1, (int)results.size());
    if (results.size ()) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "1", hit.bible);
      evaluate (__LINE__, __func__, 1, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 3, hit.verse);
      evaluate (__LINE__, __func__, "Pattern found in text: did", hit.data);
    }
  }
  database_check.truncateOutput ("");
  // Test Pattern
  {
    map <int, string> verses = {
      make_pair (2, "He said."),
      make_pair (3, "He didn't say"),
      make_pair (4, "He said.")
    };
    Checks_Verses::patterns ("1", 1, 1, verses, {"Did"});
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, (int)results.size());
  }
  database_check.truncateOutput ("");
  // Test Pattern
  {
    map <int, string> verses = {
      make_pair (2, "He said."),
      make_pair (3, "He didn't say"),
      make_pair (4, "He said.")
    };
    Checks_Verses::patterns ("1", 1, 1, verses, {"said"});
    vector <Database_Check_Hit> results = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, (int)results.size());
    if (results.size () == 2) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, "1", hit.bible);
      evaluate (__LINE__, __func__, 1, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 2, hit.verse);
      evaluate (__LINE__, __func__, "Pattern found in text: said", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, 2, hit.rowid);
      evaluate (__LINE__, __func__, "1", hit.bible);
      evaluate (__LINE__, __func__, 1, hit.book);
      evaluate (__LINE__, __func__, 1, hit.chapter);
      evaluate (__LINE__, __func__, 4, hit.verse);
      evaluate (__LINE__, __func__, "Pattern found in text: said", hit.data);
    }
  }
  database_check.truncateOutput ("");
}


void test_check_pairs ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  Database_Check database_check;
  database_check.create ();
  
  string bible = "bible";
  int book = 2;
  int chapter = 3;
  map <int, string> verses;
  vector <pair <string, string> > pairs = {
    make_pair ("[", "]"),
    make_pair ("(", ")"),
    make_pair ("“", "”"),
  };
  vector <Database_Check_Hit> results;
  
  {
    verses = {
      make_pair (2, "Verse two."),
      make_pair (3, "Verse three."),
      make_pair (4, "Verse four.")
    };
    Checks_Pairs::run (bible, book, chapter, verses, pairs);
    results = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, results.size());
    database_check.truncateOutput ("");
  }

  {
    verses = {
      make_pair (2, "Verse [two."),
      make_pair (3, "Verse three]."),
      make_pair (4, "Verse four.")
    };
    Checks_Pairs::run (bible, book, chapter, verses, pairs);
    results = database_check.getHits ();
    evaluate (__LINE__, __func__, 0, results.size());
    database_check.truncateOutput ("");
  }

  {
    verses = {
      make_pair (2, "Verse [two."),
      make_pair (3, "Verse (three."),
      make_pair (4, "Verse four.")
    };
    Checks_Pairs::run (bible, book, chapter, verses, pairs);
    results = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, results.size());
    if (results.size () == 2) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, bible, hit.bible);
      evaluate (__LINE__, __func__, book, hit.book);
      evaluate (__LINE__, __func__, chapter, hit.chapter);
      evaluate (__LINE__, __func__, 2, hit.verse);
      evaluate (__LINE__, __func__, "Opening character \"[\" without its matching closing character \"]\"", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, "Opening character \"(\" without its matching closing character \")\"", hit.data);
    }
    database_check.truncateOutput ("");
  }

  {
    verses = {
      make_pair (2, "Verse [two."),
      make_pair (3, "Verse three."),
      make_pair (4, "Verse four).")
    };
    Checks_Pairs::run (bible, book, chapter, verses, pairs);
    results = database_check.getHits ();
    evaluate (__LINE__, __func__, 2, results.size());
    if (results.size () == 2) {
      Database_Check_Hit hit = results[0];
      evaluate (__LINE__, __func__, 1, hit.rowid);
      evaluate (__LINE__, __func__, bible, hit.bible);
      evaluate (__LINE__, __func__, book, hit.book);
      evaluate (__LINE__, __func__, chapter, hit.chapter);
      evaluate (__LINE__, __func__, 4, hit.verse);
      evaluate (__LINE__, __func__, "Closing character \")\" without its matching opening character \"(\"", hit.data);
      hit = results[1];
      evaluate (__LINE__, __func__, 2, hit.verse);
      evaluate (__LINE__, __func__, "Opening character \"[\" without its matching closing character \"]\"", hit.data);
    }
    database_check.truncateOutput ("");
  }
}


void test_hyphenate ()
{
  trace_unit_tests (__func__);
  
  vector <string> firstset = {"a", "e", "i", "o", "u"};
  vector <string> secondset = {"b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z"};
  string input =
  "\\s \\nd UNkulunkulu\\nd* u\\add ba\\add*xwayisa ngokulunga okungokwabantu 文字ab化け\n"
  "\\s Ukulunga okuku\\nd Kristu\\nd* אבabגד kuyinzuzo אבגד ab";
  string output = hyphenate_at_transition (firstset, secondset, input);
  string standard =
  "\\s \\nd UNku­lu­nku­lu\\nd* u\\add ba\\add*­xwa­yi­sa ngo­ku­lu­nga oku­ngo­kwa­ba­ntu 文字a­b化け\n"
  "\\s Uku­lu­nga oku­ku\\nd Kri­stu\\nd* אבa­bגד ku­yi­nzu­zo אבגד ab";
  evaluate (__LINE__, __func__, standard, output);
}


void test_search_logic_setup ()
{
  string standardUSFM1 =  "\\c 1\n"
                          "\\p\n"
                          "\\v 1 Text of the first verse.\n"
                          "\\v 2 \\add Text of the second \\add*verse.\n"
                          "\\s Heading\n"
                          "\\p\n"
                          "\\v 3 Text of the 3rd third verse.\n"
                          "\\v 4 Text of the 4th \\add fourth\\add* verse.\n"
                          "\\v 5 Text of the 5th fifth verse is this: Verse five ✆.\n"
                          "\\v 6 Text of the 6th sixth verse ✆.\n"
                          "\\v 7 Text of the seventh verse with some UTF-8 characters: ✆ ❼ ሯ.\n"
                          "\\v 8 Verse eight ✆ ❼ ሯ.\n"
                          "\\v 9 Verse nine nine.\n"
                          "\\v 10 خدا بود و کلمه کلمه خدا بود. در ابتدا کلمه بود و کلمه نزد.\n";
  string standardUSFM2 =  "\\c 1\n"
                          "\\p\n"
                          "\\v 3 And he said.\n";
  string standardUSFM3 =  "\\c 3  \n"
                          "\\s1 Manusia pertama berdosa karena tidak mentaati Allah.\n"
                          "\\p\n"
                          "\\v 1 Ular adalah binatang yang paling licik diantara semua binatang buas yang ALLAH ciptajkan. Ular bertanya kepada perempuan itu, “Apakah benar Allah berkata kepada kalian, ‘Jangan memakan satu buah pun dari semua pohon yang ada di taman ini?’ ’’\n"
                          "\\v 2-3 Perempuan itu menjawab, “ALLAH melarang kami menyentuh apa lagi memakan buah yang berada di tengah-tengah taman. Bila kami melanggar larangannya, maka kami akan mati! Tetapi untuk semua buah yang lain kami boleh memakannya.”\n"
                          "\\v 4,5 Ular berkata kepada perempuan itu,”Tentu saja kamu tidak akan mati. ALLAH mengatakan hal itu karena tahu kalau kamu makan buah dari pohon yang berada di tengah taman itu, kamu akan memahami sesuatu yang baru yaitu mata dan pikiranmu akan terbuka dan kamu akan menjadi sama seperti Allah. Kamu akan mengetahui apa yang baik yang boleh dilakukan dan yang jahat, yang tidak boleh dilakukan.\n"
                          "\\v 6 Perempuan itu melihat bahwa pohon itu menghasilkan buah yang sangat indah dan enak untuk dimakan. Maka dia menginginkannya karena mengira, akan menjadi perempuan yang bijaksana. Lalu, dipetiklah beberapa buah dan dimakannya. Kemudian, dia memberikan beberapa buah juga kepada suaminya dan suaminya juga memakannya.\n";
  Database_State::create ();
  Database_Bibles database_bibles;
  database_bibles.createBible ("phpunit");
  database_bibles.storeChapter ("phpunit", 2, 3, standardUSFM1);
  database_bibles.createBible ("phpunit2");
  database_bibles.storeChapter ("phpunit2", 4, 5, standardUSFM2);
  database_bibles.createBible ("phpunit3");
  database_bibles.storeChapter ("phpunit3", 6, 7, standardUSFM3);
}


void test_search_logic ()
{
  trace_unit_tests (__func__);
  
  // Test updating search fields.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    search_logic_index_chapter ("phpunit", 2, 3);
  }

  // Test searching and getting Bible passage
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_text ("sixth", {"phpunit"});
    evaluate (__LINE__, __func__, 1, (int)passages.size());
    if (!passages.empty ()) {
      evaluate (__LINE__, __func__, "phpunit", passages[0].bible);
      evaluate (__LINE__, __func__, 2, passages[0].book);
      evaluate (__LINE__, __func__, 3, passages[0].chapter);
      evaluate (__LINE__, __func__, "6", passages[0].verse);
    }
  }

  // Search in combined verses.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_text ("ALLAH", {"phpunit3"});
    evaluate (__LINE__, __func__, 4, (int)passages.size());
    if (passages.size () == 4) {
      evaluate (__LINE__, __func__, "phpunit3", passages[0].bible);
      evaluate (__LINE__, __func__, 6, passages[1].book);
      evaluate (__LINE__, __func__, 7, passages[2].chapter);
      evaluate (__LINE__, __func__, "0", passages[0].verse);
      evaluate (__LINE__, __func__, "1", passages[1].verse);
      evaluate (__LINE__, __func__, "2", passages[2].verse);
      evaluate (__LINE__, __func__, "4", passages[3].verse);
    }
  }

  // Test search Bible
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_bible_text ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    passages = search_logic_search_bible_text ("phpunit2", "sixth");
    evaluate (__LINE__, __func__, 0, (int)passages.size ());
    passages = search_logic_search_bible_text ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
  }
  
  // Test search Bible case sensitive.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_bible_text_case_sensitive ("phpunit", "Verse");
    evaluate (__LINE__, __func__, 3, (int)passages.size ());
    passages = search_logic_search_bible_text_case_sensitive ("phpunit", "sixth");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
    passages = search_logic_search_bible_text_case_sensitive ("phpunit2", "said");
    evaluate (__LINE__, __func__, 1, (int)passages.size ());
  }

  // Searching in USFM.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_bible_usfm ("phpunit", "\\Add");
    evaluate (__LINE__, __func__, 2, (int)passages.size ());
  }
  
  // Searching in USFM case-sensitive.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    vector <Passage> passages = search_logic_search_bible_usfm_case_sensitive ("phpunit", "\\Add");
    evaluate (__LINE__, __func__, 0, (int)passages.size ());
    passages = search_logic_search_bible_usfm_case_sensitive ("phpunit", "\\add");
    evaluate (__LINE__, __func__, 2, (int)passages.size ());
  }
  
  // Test getting Bible verse text.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    // Plain.
    string text = search_logic_get_bible_verse_text ("phpunit", 2, 3, 5);
    evaluate (__LINE__, __func__, "Text of the 5th fifth verse is this: Verse five ✆.", text);
    // USFM.
    text = search_logic_get_bible_verse_usfm ("phpunit", 2, 3, 5);
    evaluate (__LINE__, __func__, "\\v 5 Text of the 5th fifth verse is this: Verse five ✆.", text);
  }
  
  // Test deleting a bible or book or chapter.
  {
    refresh_sandbox (true);
    
    test_search_logic_setup ();
    
    vector <Passage> passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 10, (int)passages.size());
    search_logic_delete_bible ("phpunit");
    passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 0, (int)passages.size());
    
    test_search_logic_setup ();

    search_logic_delete_book ("phpunit", 3);
    passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 10, (int)passages.size());
    search_logic_delete_book ("phpunit", 2);
    passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 0, (int)passages.size());
    
    test_search_logic_setup ();

    search_logic_delete_chapter ("phpunit", 3, 3);
    passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 10, (int)passages.size());
    search_logic_delete_chapter ("phpunit", 2, 3);
    passages = search_logic_search_bible_text ("phpunit", "e");
    evaluate (__LINE__, __func__, 0, (int)passages.size());
  }
  
  // Test total verse count in Bible.
  {
    refresh_sandbox (true);
    test_search_logic_setup ();
    int count = search_logic_get_verse_count ("phpunit");
    evaluate (__LINE__, __func__, 11, count);
  }
}


// Temporal testing of bits.
void test_libraries_temporal ()
{
  trace_unit_tests (__func__);
}


// Test included JSON libraries.
void test_json ()
{
  trace_unit_tests (__func__);
  {
    // Convert JSON to xml.
    string json ("{"
                 "  \"foo\" : 1,"
                 "  \"bar\" : false,"
                 "  \"person\" : {\"name\" : \"GWB\", \"age\" : 60,},"
                 "  \"data\": [\"abcd\", 42],"
                 "}");
    Object object;
    object.parse (json);
    string path = filter_url_create_root_path ("unittests", "tests", "jsonxx.txt");
    string xml = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, xml, object.xml (JSONx));
  }
  {
    // Test malformed JSON.
    string json ("{"
                 "  \"foo\" 1,"
                 "  \"bar : false,"
                 "  \"person\" : me\" : \"GWB\", \"age\" : 60,},"
                 "  \"data\": \"abcd\", 42],"
                 );
    Object object;
    object.parse (json);
    evaluate (__LINE__, __func__, "{\n} \n", object.json ());
  }
}


// Test the related verses feature.
void test_related ()
{
  trace_unit_tests (__func__);

  {
    // Test situation of one input passage that has several output passages.
    vector <Passage> output = related_logic_get_verses ({ Passage ("", 1, 10, "6") });
    unsigned int size = 6;
    evaluate (__LINE__, __func__, size, output.size ());
    if (output.size () == size) {
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "6").equal (output[0]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "7").equal (output[1]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "8").equal (output[2]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "8").equal (output[3]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "9").equal (output[4]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "10").equal (output[5]));
    }
  }

  {
    // Test that an input passage that finds no parallel passages in the XML files results in output equal to the input.
    vector <Passage> output = related_logic_get_verses ({ Passage ("", 2, 3, "4") });
    evaluate (__LINE__, __func__, 0, output.size ());
  }

  {
    // Test input passages that match two entries in the XML files.
    vector <Passage> output = related_logic_get_verses ({ Passage ("", 1, 10, "29"), Passage ("", 1, 11, "12") });
    unsigned int size = 12;
    evaluate (__LINE__, __func__, size, output.size ());
    if (output.size () == size) {
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "26").equal (output[0]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "27").equal (output[1]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "28").equal (output[2]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 10, "29").equal (output[3]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 11, "12").equal (output[4]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 11, "13").equal (output[5]));
      evaluate (__LINE__, __func__, true, Passage ("", 1, 11, "14").equal (output[6]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "18").equal (output[7]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "20").equal (output[8]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "21").equal (output[9]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "22").equal (output[10]));
      evaluate (__LINE__, __func__, true, Passage ("", 13, 1, "23").equal (output[11]));
    }
  }

  {
    // Test third passage of synoptic parallels whether it also gets the first two passages, and the fourth.
    // At the same time it takes data from the quotations XML.
    vector <Passage> output = related_logic_get_verses ({ Passage ("", 42, 3, "4") });
    unsigned int size = 10;
    evaluate (__LINE__, __func__, size, output.size ());
    if (output.size () == size) {
      evaluate (__LINE__, __func__, true, Passage ("", 23, 40, "3").equal (output[0]));
      evaluate (__LINE__, __func__, true, Passage ("", 23, 40, "4").equal (output[1]));
      evaluate (__LINE__, __func__, true, Passage ("", 23, 40, "5").equal (output[2]));
      evaluate (__LINE__, __func__, true, Passage ("", 40, 3, "3").equal (output[3]));
      evaluate (__LINE__, __func__, true, Passage ("", 41, 1, "2").equal (output[4]));
      evaluate (__LINE__, __func__, true, Passage ("", 41, 1, "3").equal (output[5]));
      evaluate (__LINE__, __func__, true, Passage ("", 42, 3, "4").equal (output[6]));
      evaluate (__LINE__, __func__, true, Passage ("", 42, 3, "5").equal (output[7]));
      evaluate (__LINE__, __func__, true, Passage ("", 42, 3, "6").equal (output[8]));
      evaluate (__LINE__, __func__, true, Passage ("", 43, 1, "23").equal (output[9]));
    }
  }
}


// Test the logic used in the visual verse editor.
void test_editone_logic ()
{
  trace_unit_tests (__func__);

  string stylesheet = styles_logic_standard_sheet ();
  string directory = filter_url_create_root_path ("unittests", "tests");

  // Prefix.
  {
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_1.usfm"));
    string html;
    string last_paragraph_style;
    editone_logic_prefix_html_stage_one (usfm, stylesheet, html, last_paragraph_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_1.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "p", last_paragraph_style);
  }
  
  // Editable verse text.
  {
    // Convert USFM to html.
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_2.usfm"));
    string html;
    string last_paragraph_style;
    string focused_verse_applied_p_style;
    editone_old_logic_editable_html ("p", usfm, stylesheet, html, last_paragraph_style, focused_verse_applied_p_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_2.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "p", last_paragraph_style);
    evaluate (__LINE__, __func__, "p", focused_verse_applied_p_style);
    
    // Convert the html back to USFM again.
    string round_tripped_usfm = editone_old_logic_html_to_usfm (stylesheet, html, focused_verse_applied_p_style);
    evaluate (__LINE__, __func__, usfm, round_tripped_usfm);
  }
  
  // Editable verse text including a \b.
  {
    // Convert USFM to html.
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_4.usfm"));
    string html;
    string last_paragraph_style;
    string focused_verse_applied_p_style;
    editone_old_logic_editable_html ("p", usfm, stylesheet, html, last_paragraph_style, focused_verse_applied_p_style);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_4.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "q1", last_paragraph_style);
    evaluate (__LINE__, __func__, "p", focused_verse_applied_p_style);
    
    // Convert the html back to USFM again.
    string round_tripped_usfm = editone_old_logic_html_to_usfm (stylesheet, html, focused_verse_applied_p_style);
    evaluate (__LINE__, __func__, usfm, round_tripped_usfm);
  }

  // Suffix.
  {
    string usfm = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_3.usfm"));
    string html;
    string last_paragraph_style;
    editone_logic_suffix_html ("q1", usfm, stylesheet, html);
    string standard = filter_url_file_get_contents (filter_url_create_path (directory, "edit_one_3.html"));
    evaluate (__LINE__, __func__, standard, html);
    evaluate (__LINE__, __func__, "", last_paragraph_style);
  }
  
  // Removing notes from the prefix and appending them to the notes in the suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone1prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone1suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone1prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone1suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }

  // Prefix without notes, so moving nothing to the notes in the suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone2prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone2suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone2prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone2suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Move notes from the prefix to a suffix that does not have notes of its own.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone3prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone3suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone3prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone3suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Test that a empty prefix works fine when trying to move notes from prefix to suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone4prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone4suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone4prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone4suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
  
  // Test that notes from the prefix get moved even to an empty suffix.
  {
    string prefix;
    string suffix;
    string standard;
    prefix = filter_url_file_get_contents (filter_url_create_path (directory, "editone5prefix1.html"));
    suffix = filter_url_file_get_contents (filter_url_create_path (directory, "editone5suffix1.html"));
    editone_logic_move_notes (prefix, suffix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone5prefix2.html"));
    evaluate (__LINE__, __func__, standard, prefix);
    standard = filter_url_file_get_contents (filter_url_create_path (directory, "editone5suffix2.html"));
    evaluate (__LINE__, __func__, standard, suffix);
  }
}


void test_http ()
{
  trace_unit_tests (__func__);

  string host, line;
  
  line = "192.168.1.139:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "192.168.1.139", host);

  line = "localhost:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "localhost", host);

  line = "[::1]:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "[::1]", host);

  line = "[fe80::601:25ff:fe07:6801]:8080";
  host = http_parse_host (line);
  evaluate (__LINE__, __func__, "[fe80::601:25ff:fe07:6801]", host);
}


bool test_memory_run = false;
uint64_t max_memory_usage = 0;
void test_memory_record ()
{
  uint64_t maximum = 0;
  do {
    uint64_t total = filter_memory_total_usage ();
    if (total > maximum) maximum = total;
    this_thread::sleep_for (chrono::microseconds (10));
  } while (test_memory_run);
  max_memory_usage = maximum;
}


void test_memory ()
{
  return;
  
  // Measure maximum memory usage of tasks that normally run in the background.
  trace_unit_tests (__func__);

  // Creating search index for one Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    search_reindex_bibles (true);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " search_reindex_bibles" << endl;
  }

  // Creating search index consultation notes.
  refresh_sandbox (false);
  {
    Database_State::create ();
    Database_Notes database_notes (NULL);
    database_notes.create ();
    for (int i = 0; i < 100; i++) {
      database_notes.storeNewNote ("bible", i, i, i, "summary", "contents", true);
    }
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    search_reindex_notes ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " search_reindex_notes" << endl;
  }
  
  // Generating change notifications.
  refresh_sandbox (false);
  {
    Database_State::create ();
    Database_Users database_users;
    database_users.create ();
    Database_Login::create ();
    demo_create_sample_bible ();
    Webserver_Request request;
    request.session_logic ()->setUsername ("admin");
    request.database_config_user ()->setGenerateChangeNotifications (true);
    request.database_config_user ()->setUserChangesNotification (true);
    request.database_config_user ()->setUserChangesNotificationsOnline (true);
    request.database_config_user ()->setContributorChangesNotificationsOnline (true);
    Database_Bibles database_bibles;
    Database_Modifications database_modifications;
    string bible = demo_sample_bible_name ();
    for (int book = 1; book <= 1; book++) {
      for (int chapter = 1; chapter <= 1; chapter++) {
        string usfm = database_bibles.getChapter (bible, book, chapter);
        usfm = filter_string_str_replace ("the", "THE", usfm);
        database_modifications.storeTeamDiff (bible, book, chapter);
        database_bibles.storeChapter (bible, book, chapter, usfm);
      }
    }
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    changes_modifications ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    uint64_t after_usage = filter_memory_total_usage ();
    cout << max_memory_usage - basic_usage << " changes_modifications, leakage " << after_usage - basic_usage << endl;
  }

  // Running checks.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    Database_Config_Bible::setCheckDoubleSpacesUsfm (bible, true);
    Database_Config_Bible::setCheckFullStopInHeadings (bible, true);
    Database_Config_Bible::setCheckSpaceBeforePunctuation (bible, true);
    Database_Config_Bible::setCheckSentenceStructure (bible, true);
    Database_Config_Bible::setCheckParagraphStructure (bible, true);
    Database_Config_Bible::setCheckBooksVersification (bible, true);
    Database_Config_Bible::setCheckChaptesVersesVersification (bible, true);
    Database_Config_Bible::setCheckWellFormedUsfm (bible, true);
    Database_Config_Bible::setCheckMissingPunctuationEndVerse (bible, true);
    Database_Config_Bible::setCheckPatterns (bible, true);
    Database_Config_Bible::setCheckMatchingPairs (bible, true);
    Database_Check database_check;
    database_check.create ();
    Database_Users database_users;
    database_users.create ();
    Database_Versifications database_versifications;
    database_versifications.create ();
    database_versifications.defaults ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    checks_run (bible);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " checks_run" << endl;
  }

  // Exporting Bible to USFM.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_usfm (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_usfm" << endl;
  }

  // Exporting Bible book to USFM and plain text.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_text_usfm_book (bible, 1, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_text_usfm_book" << endl;
  }

  // Exporting Bible book to Open Document.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_odt_book (bible, 1, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_odt_book" << endl;
  }

  // Exporting Bible to eSword.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_esword (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_esword" << endl;
  }

  // Exporting Bible to Online Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_onlinebible (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_onlinebible" << endl;
  }

  // Exporting Bible to Quick Bible.
  refresh_sandbox (false);
  {
    Database_State::create ();
    demo_create_sample_bible ();
    string bible = demo_sample_bible_name ();
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    export_quickbible (bible, false);
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " export_quickbible" << endl;
  }

  // Refreshing SWORD module list.
  refresh_sandbox (false);
  {
    thread * recorder = nullptr;
    uint64_t basic_usage = filter_memory_total_usage ();
    test_memory_run = true;
    recorder = new thread (test_memory_record);
    sword_logic_refresh_module_list ();
    test_memory_run = false;
    recorder->join ();
    delete recorder;
    cout << max_memory_usage - basic_usage << " sword_logic_refresh_module_list" << endl;
  }
  
  // Done.
  refresh_sandbox (false);
  
  /*
   
   First iteration displaying memory usage in bytes per function:
   40960 search_reindex_bibles
   8192 search_reindex_notes
   113590272 changes_modifications
   8364032 checks_run
   40960 export_usfm
   339968 export_text_usfm_book
   929792 export_odt_book
   30154752 export_esword
   15814656 export_onlinebible
   118784 export_quickbible
   20480 sword_logic_refresh_module_list
   This is on macOS, but valgrind on Linux gives different values, lower values.
  
  */
}


void test_tasks_logic ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  tasks_logic_queue ("task1");
  tasks_logic_queue ("task3");
  tasks_logic_queue ("task4", { "parameter1", "parameter2" });
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task1"));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task2"));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task1", { "parameter" }));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4"));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4", { "parameter1" }));
  evaluate (__LINE__, __func__, true, tasks_logic_queued ("task4", { "parameter1", "parameter2" }));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task4", { "parameter1", "parameter3" }));
  evaluate (__LINE__, __func__, false, tasks_logic_queued ("task4", { "parameter2" }));
  
}


void test_biblegateway ()
{
  trace_unit_tests (__func__);
  
  string resource;
  int book;
  string text;

  resource = "Библия, ревизирано издание (BPB)";

  book = 1; // Genesis.

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "В началото Бог сътвори небето и земята.", text);

  resource = "21st Century King James Version (KJ21)";

  book = 34; // Nahum.
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "The burden of Nineveh. The book of the vision of Nahum the Elkoshite.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 15);
  evaluate (__LINE__, __func__, "Behold upon the mountains the feet of Him that bringeth good tidings, that publisheth peace! O Judah, keep thy solemn feasts, perform thy vows; for the wicked shall no more pass through thee; he is utterly cut off.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 16);
  evaluate (__LINE__, __func__, "", text);

  book = 64; // 3 John.
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);
  
  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "The Elder, Unto the well-beloved Gaius, whom I love in the truth:", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 14);
  evaluate (__LINE__, __func__, "but I trust I shall shortly see thee, and we shall speak face to face. Peace be to thee. Our friends salute thee. Greet the friends by name.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 15);
  evaluate (__LINE__, __func__, "", text);
  
  resource = "Expanded Bible (EXB)";

  book = 34; // Nahum.

  text = resource_logic_bible_gateway_get (resource, book, 1, 0);
  evaluate (__LINE__, __func__, "", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 1);
  evaluate (__LINE__, __func__, "This is the ·message [oracle; burden] for the city of Nineveh [C the capital of the Assyrian empire]. This is the book of the vision of Nahum, ·who was from the town of Elkosh [L the Elkoshite; C of uncertain location].The Lord Is Angry with Nineveh", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 2);
  evaluate (__LINE__, __func__, "The Lord is a ·jealous [zealous] God [Ex. 20:5; 34:14; Deut. 4:24; 5:9; Josh. 24:19] who ·punishes [avenges]; the Lord ·punishes [avenges] and is filled with ·anger [wrath]. The Lord ·punishes [takes vengeance on] ·those who are against him [his adversaries/enemies], and he ·stays angry with [or vents his wrath against] his enemies.", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 11);
  evaluate (__LINE__, __func__, "Someone has come from ·Nineveh [L you] who ·makes evil plans [plots evil] against the Lord and gives wicked ·advice [counsel; strategy].", text);

  text = resource_logic_bible_gateway_get (resource, book, 1, 12);
  evaluate (__LINE__, __func__, "This is what the Lord says: “Although Assyria ·is strong [or has allies] and has many people, it will be ·defeated [destroyed] and ·brought to an end [pass away]. Although I have ·made you suffer, Judah [L afflicted you], I will ·make you suffer [afflict you] no more.", text);
}


void test_rss_feed ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  refresh_sandbox (true);
  
  string path = rss_logic_xml_path ();
  Database_Config_General::setSiteURL ("http://localhost:8080/");
  
  // Write two items.
  Database_Config_General::setMaxRssFeedItems (10);
  rss_logic_update_xml ({ "titleone", "titletwo" }, { "authorone", "authortwo" }, { "description one", "description two"} );
  evaluate (__LINE__, __func__, 828, filter_url_filesize (path));
  
  // Set maximum number of items to 0: Should remove the file.
  Database_Config_General::setMaxRssFeedItems (0);
  rss_logic_update_xml ({ "titleone", "titletwo" }, { "authorone", "authortwo" }, { "description one", "description two"} );
  evaluate (__LINE__, __func__, 0, filter_url_filesize (path));

  // Add many entries and clipping their number.
  Database_Config_General::setMaxRssFeedItems (10);
  vector <string> titles;
  vector <string> authors;
  vector <string> descriptions;
  for (size_t i = 0; i < 100; i++) {
    titles.push_back ("title " + convert_to_string (i));
    authors.push_back ("author " + convert_to_string (i));
    descriptions.push_back ("description " + convert_to_string (i));
  }
  rss_logic_update_xml (titles, authors, descriptions);
  evaluate (__LINE__, __func__, 2508, filter_url_filesize (path));
#endif
}


void test_libraries_dev ()
{
  trace_unit_tests (__func__);
}
