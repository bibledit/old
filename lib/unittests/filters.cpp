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


#include <unittests/unittest.h>
#include <unittests/utilities.h>
#include <config/libraries.h>
#include <library/bibledit.h>
#include <database/config/user.h>
#include <database/styles.h>
#include <database/books.h>
#include <database/config/bible.h>
#include <database/modifications.h>
#include <database/state.h>
#include <database/git.h>
#include <database/login.h>
#include <database/logs.h>
#include <config/globals.h>
#include <filter/url.h>
#include <filter/string.h>
#include <filter/roles.h>
#include <filter/usfm.h>
#include <filter/archive.h>
#include <filter/text.h>
#include <filter/css.h>
#include <filter/diff.h>
#include <filter/abbreviations.h>
#include <filter/git.h>
#include <filter/merge.h>
#include <filter/date.h>
#include <filter/shell.h>
#include <filter/memory.h>
#include <session/logic.h>
#include <text/text.h>
#include <esword/text.h>
#include <onlinebible/text.h>
#include <html/text.h>
#include <odf/text.h>
#include <styles/logic.h>
#include <styles/css.h>
#include <ipc/notes.h>
#include <client/logic.h>
#include <bible/logic.h>
#include <developer/logic.h>
#include <ldap/logic.h>


// Todo move into smaller units.


void test_replace ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, "ABXEFG", substr_replace ("ABCDEFG", "X", 2, 2));
  evaluate (__LINE__, __func__, "ABX", substr_replace ("ABCDEFG", "X", 2, 5));
  evaluate (__LINE__, __func__, "ABXG", substr_replace ("ABCDEFG", "X", 2, 4));
}


void test_styles_css ()
{
  trace_unit_tests (__func__);
  
  Webserver_Request request;
  Database_State database_state;
  database_state.create ();
  // Basic.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "basic.css"));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Export.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.exports ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "exports.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
  // Editor.
  {
    refresh_sandbox (true);
    Database_Styles database_styles;
    database_styles.createSheet ("phpunit");
    Styles_Css styles_css = Styles_Css (&request, "phpunit");
    styles_css.editor ();
    styles_css.generate ();
    string css = styles_css.css ();
    string standard = filter_string_trim (filter_url_file_get_contents (filter_url_create_path ("unittests", "tests", "editor.css")));
    evaluate (__LINE__, __func__, standard, css);
  }
}


void test_filter_custom_css ()
{
  trace_unit_tests (__func__);
  
  // Direction
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionUnspecified (100));
    evaluate (__LINE__, __func__, "", Filter_Css::directionUnspecified (101));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionLeftToRight (101));
    evaluate (__LINE__, __func__, "", Filter_Css::directionLeftToRight (102));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::directionRightToLeft (102));
    evaluate (__LINE__, __func__, "", Filter_Css::directionRightToLeft (103));
    
    evaluate (__LINE__, __func__, 0, Filter_Css::directionValue (""));
    evaluate (__LINE__, __func__, 1, Filter_Css::directionValue ("ltr"));
    evaluate (__LINE__, __func__, 2, Filter_Css::directionValue ("rtl"));
  }
  // Writing Mode
  {
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeUnspecified (102));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeUnspecified (112));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomLeftRight (112));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomLeftRight (122));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeTopBottomRightLeft (122));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeTopBottomRightLeft (132));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopLeftRight (132));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopLeftRight (142));
    
    evaluate (__LINE__, __func__, "checked", Filter_Css::writingModeBottomTopRightLeft (142));
    evaluate (__LINE__, __func__, "", Filter_Css::writingModeBottomTopRightLeft (152));
  }
  // Css
  {
    string css = Filter_Css::getCss ("class", "", 0);
    
    string standard =
    ".class\n"
    "{\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 101);
    standard =
    ".class\n"
    "{\n"
    "direction: ltr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 102);
    standard =
    ".class\n"
    "{\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("class", "", 110);
    standard =
    ".class\n"
    "{\n"
    "writing-mode: tb-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 130);
    standard =
    ".CLass\n"
    "{\n"
    "writing-mode: bt-lr;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("CLass", "", 1322);
    standard =
    ".CLass\n"
    "{\n"
    "direction: rtl;\n"
    "writing-mode: tb-rl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 0);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("Class", "sherif", 102);
    standard =
    ".Class\n"
    "{\n"
    "font-family: sherif;\n"
    "direction: rtl;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
    
    css = Filter_Css::getCss ("classs", "../font.ttf", 0);
    standard =
    "@font-face\n"
    "{\n"
    "font-family: classs;\n"
    "src: url(../font.ttf);\n"
    "}\n"
    ".classs\n"
    "{\n"
    "font-family: classs;\n"
    "}\n";
    evaluate (__LINE__, __func__, filter_string_trim (standard), filter_string_trim (css));
  }
  // Class.
  {
    string clss = Filter_Css::getClass ("ആഈഘലറ");
    evaluate (__LINE__, __func__, "customf86528", clss);
  }
}


void test_filter_diff ()
{
  trace_unit_tests (__func__);
  
  // Diff 1
  {
    string output = filter_diff_diff ("Old text", "New text");
    string standard = "<span style=\"text-decoration: line-through;\">Old</span> <span style=\"font-weight: bold;\">New</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Diff 2
  {
    string output = filter_diff_diff ("this is really old text", "and this is new text");
    string standard = "<span style=\"font-weight: bold;\">and</span> this is <span style=\"text-decoration: line-through;\">really</span> <span style=\"text-decoration: line-through;\">old</span> <span style=\"font-weight: bold;\">new</span> text";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Diff with new lines in the text.
  {
    string oldtext =
    "Genesis 1.1 1 In the beginning God created the heavens and the earth.\n"
    "Genesis 1.2 2 And the earth was without form, and void; and darkness was upon the face of the deep. And the Spirit of God moved upon the face of the waters.\n"
    "Genesis 1.3 3 And God said, Let there be light: and there was light.\n";
    
    string newtext =
    "Genesis 1.1 1 In the beginning God created the heaven and the earth.\n"
    "Genesis 1.2 2 And the earth was without form and void and darkness was upon the face of the deep. And the Spirit of God moved upon the face of the waters.\n"
    "Genesis 1.3 3 And God said: \"Let there be light\". And there was light.\n";
    
    string output = filter_diff_diff (oldtext, newtext);
    
    string standard = filter_url_file_get_contents (filter_url_create_root_path ("unittests", "tests", "diff.txt"));
    evaluate (__LINE__, __func__, standard, output);
  }
  // Character similarity 1.
  {
    int similarity = filter_diff_character_similarity ("Old text", "New text");
    evaluate (__LINE__, __func__, 45, similarity);

    similarity = filter_diff_character_similarity ("New text", "New text");
    evaluate (__LINE__, __func__, 100, similarity);

    similarity = filter_diff_character_similarity ("ABCDEFGH", "IJKLMNOPQRST");
    evaluate (__LINE__, __func__, 0, similarity);
  }
  // Similarity with text that used to crash the routine but was fixed.
  {
    // Invalid UTF8 results in 0% similarity.
    string path = filter_url_create_root_path ("unittests", "tests");
    string oldtext = filter_url_file_get_contents (filter_url_create_path (path, "invalid-utf8-old.txt"));
    string newtext = filter_url_file_get_contents (filter_url_create_path (path, "invalid-utf8-new.txt"));
    int similarity = filter_diff_character_similarity (oldtext, newtext);
    evaluate (__LINE__, __func__, 0, similarity);
  }
  // Similarity.
  {
    string first =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 38 Zvino ndizvo zvaunofanira kubaira paaritari: Zuva rimwe nerimwe nguva dzese makwayana maviri.\n"
    "\\v 39 Rimwe gwayana unofanira kuribaira mangwanani, rimwe gwayana unofanira kuribaira madekwani.\n"
    "\\v 40 Pamwe chete negwayana rimwe unofanira kuisa chipiriso choupfu hwakatsetseka hwakasvika pachegumi cheefa hwakakanyiwa nechechina chehini yamafuta akasviniwa nechechina chehini yewaini, chive chipiriso chinodururwa.\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    
    string second =
    "\\c 29\n"
    "\\s Kugadzwa kwevapristi\n"
    "\\p\n"
    "\\v 1 Zvino ichi ndicho chinhu chauchavaitira kuvaita vatsvene, kuti vandishumire pabasa reupristi: Tora nzombe imwe diki; nemakondohwe maviri asina chaanopomerwa,\n"
    "\\v 2 nechingwa chisina kuviriswa, nemakeke asina kuviriswa akakanyiwa nemafuta, nezvingwa zvitete zvisina kuviriswa zvine mafuta; uchazviita neupfu hwakatsetseka hwegorosi.\n"
    "\\v 3 Zvino uchazviisa mudengu rimwe, ugopinda nazvo zviri mudengu, pamwe nenzombe diki nemakondohwe maviri.\n"
    "\\v 4 NaAroni nevanakomana vake uchauya navo kumukova wetende rekusangana, ugovashambidza nemvura;\n"
    "\\v 5 uchatorawo hanzu ugopfekedza Aroni nguvo refu, nejazi reefodhi, neefodhi, nechidzitiro chechipfuva; uye umusunge chiuno nebhanhire reefodhi rakarukwa neumhizha;\n"
    "\\v 6 uisewo ngundu pamusoro wake, nekuisa korona tsvene pangundu.\n"
    "\\v 7 Zvino uchatora mafuta ekuzodza, ugodira pamusoro wake, ugomuzodza.\n"
    "\\v 8 Nevanakomana vake uchavaswededza pedo ugovapfekedza majazi.\n"
    "\\v 9 Uchavasungawo zviuno nebhanhire, Aroni nevanakomana vake, uvasungirire nguwani; uye basa reupristi richava ravo chive chimiso nekusingaperi; uye uchaeresa Aroni nevanakomana vake.\n"
    "\\v 10 Zvino uchauisa nzombe pamberi petende rekusangana, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wenzombe.\n"
    "\\v 11 Zvino uchabaya nzombe pamberi paJehovha pamukova wetende rekusangana.\n"
    "\\v 12 Zvino uchatora ropa renzombe, ugoisa panyanga dzearitari nemunwe wako, ndokudururira ropa rese mujinga memuzasi mearitari:\n"
    "\\v 13 Zvino uchatora mafuta ese anofukidza zvemukati, nebutiro riri pamusoro pechiropa, neitsvo mbiri, nemafuta ari padziri uzvipise pamusoro pearitari.\n"
    "\\v 14 Asi nyama yenzombe, nedehwe rayo, nechizvizvi chayo, uchapisa nemoto kunze kwemisasa; chipiriso chezvivi.\n"
    "\\p\n"
    "\\v 15 Uchatorawo gondohwe rimwe, uye Aroni nevanakomana vake vachaisa maoko avo pamusoro pemusoro wegondohwe,\n"
    "\\v 16 zvino uchauraya gondohwe ndokutora ropa raro ndokusasa kupoteredza aritari.\n"
    "\\v 17 Zvino uchaguranya gondohwe ndokusuka zvemukati zvaro nemakumbo aro ugoisa panhindi dzaro nepamusoro waro.\n"
    "\\v 18 Zvino uchapisa gondohwe rese paaritari; chipiriso chinopiswa kuna Jehovha, hwema hunonhuhwira, chipiriso chinoitirwa Jehovha, nemoto.\n"
    "\\v 19 Zvino uchatora gondohwe rechipiri, uye Aroni nevanakomana vake vagoisa maoko avo pamusoro pemusoro wegondohwe.\n"
    "\\v 20 Ipapo uchauraya gondohwe ndokutora paropa raro, ndokuisa pamucheto wezasi wenzeve yerudyi yaAroni, nepamicheto yezasi yenzeve dzerudyi dzevanakomana vake, nepagunwe reruoko rwerudyi, nepagumwe guru retsoka yavo yerudyi, nekusasawo ropa paaritari kupoteredza.\n"
    "\\v 21 Zvino uchatora paropa riri paaritari nepamafuta ekuzodza ugosasa pamusoro paAroni napanguvo dzake nepamusoro pevanakomana vake, nepanguvo dzevanakomana vake pamwe naye, agoitwa mutsvene iye nenguvo dzake nevanakomana vake nenguvo dzevanakomana vake pamwe naye.\n"
    "\\v 22 Zvino utorewo mamwe mafuta egondohwe, nebemhe raro, namafuta anofukidza ura, namafuta ari pamusoro pechiropa, netsvo mbiri, namafuta ari padziri, nebandauko rorudyi (nokuti igondohwe rokugadza naro),\n"
    "\\v 23 nebundu rimwe rechingwa, nechingwa chimwe chidiki chinamafuta, nechingwa chimwe chitete chinobva mudengu rezvingwa zvisina kuviriswa, zviri pamberi paJehovha.\n"
    "\\v 24 Zvino uzviise zvese pamaoko aAroni napamaoko avanakomana vake, vagozvizunguzira, chive chipo chinozunguzirwa pamberi paJehovha.\n"
    "\\v 25 Zvino uzvibvise pamaoko avo, uzvipise paaritari pamusoro pechipiriso chinopiswa, kuti chive chipo chinonhuhwira pamberi paJehovha; chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 26 Utorewo chityu chegondohwe rokugadza naro raAroni, urizunguzire pamberi paJehovha, kuti chive chipo chinozunguzirwa; ndiwo mugove wako.\n"
    "\\v 27 Zvino chityu chinozunguzirwa uchiite chitsvene, nebandauko rechipiriso chinosimudzwa, chinozunguzirwa nekusimudzwa, zvegondohwe rokugadza naro, kuti zvive zvaAroni nezvavanakomana vake.\n"
    "\\v 28 Izvozvo zvinofanira kupiwa Aroni navanakomana vake navana vaIsraeri nokusingaperi nokuti chipiriso chinosimudzwa, chinobva kuvana vaIsraeri pazvibairo zvezvipiriso zvavo zvokuyananisa, chive chipiriso chavo chinosimudzirwa Jehovha.\n"
    "\\p\n"
    "\\v 29 Nguvo dzaAroni dzichava dzavanakomana vake vanomutevera, kuti vazodzwe vakadzipfeka, vagadzwe vakadzipfeka.\n"
    "\\v 30 Mwanakomana wake anopinda paupristi panzvimbo yake, anofanira kudzipfeka mazuva manomwe, kana achipinda mutende rekusangana kushumira panzvimbo tsvene.\n"
    "\\p\n"
    "\\v 31 Zvino unofanira kutora gondohwe rokugadza naro, ubike nyama yaro panzvimbo tsvene.\n"
    "\\v 32 Aroni navanakomana vake ngavadye nyama yegondohwe, nechingwa chiri mudengu pamukova wetende rokusangana.\n"
    "\\v 33 Vanofanira kudya izvozvo zvaiyananiswa nazvo, pakugadzwa nokuitwa vatsvene kwavo; asi mweni ngaarege kuzvidya, nokuti zvitsvene.\n"
    "\\v 34 Kana imwe nyama yokugadza nayo ikasara, kana chimwe chingwa, kusvikira mangwana, unofanira kupisa zvasara nomoto; hazvifaniri kudyiwa, nokuti zvitsvene.\n"
    "\\p\n"
    "\\v 35 Unofanira kuitira Aroni navanakomana vake saizvozvo, zvese sezvandakuraira; uite mazuva manomwe uchivagadza.\n"
    "\\v 36 Zuva rimwe nerimwe unofanira kubaira nzombe yechipiriso chezvivi, chokuyananisa nacho; unofanira kunatsa aritari, kana uchiiyananisira; uizodze kuti uiite tsvene.\n"
    "\\v 37 Unofanira kuita mazuva manomwe pakuyananisira aritari, nokuiita tsvene; kuti aritari ive tsvene kwazvo, uye zvese zvinobata paaritari zvinofanira kuva zvitsvene.\n"
    "\\s Zvibairo zvezuva rimwe nerimwe\n"
    "\\p\n"
    "\\v 41 Rimwe gwayana uribaire madekwani, uriitire sezvawakaitira chipiriso choupfu chamangwanani, nezvawakaitira chipiriso charo chinomwiwa, kuti chive chipiriso chinonhuhwira, chipiriso chakaitirwa Jehovha pamoto.\n"
    "\\v 42 Chinofanira kuva chipiriso chinopiswa nguva dzese pamarudzi enyu ese, pamukova wetende rekusangana pamberi paJehovha, pandichasangana nemi, nditaurepo newe.\n"
    "\\v 43 Ndipo pandichasangana nevana vaIsraeri, uye tende richaitwa dzvene nekubwinya kwangu.\n"
    "\\v 44 Tende rokusangana nearitari ndichazviita zvitsvene; uye Aroni navanakomana vake ndichavaita vatsvene, kuti vandishumire pabasa roupristi.\n"
    "\\v 45 Ndichagara pakati pavana vaIsraeri, kuti ndive Mwari wavo.\n"
    "\\v 46 Vachaziva kuti ndini Jehovha wavo, wakavabudisa panyika kuti ndigare pakati pavo; ndini Jehovha Mwari wavo.\n";
    int similarity = filter_diff_character_similarity (first, second);
    evaluate (__LINE__, __func__, 94, similarity);

    similarity = filter_diff_word_similarity (first, second);
    evaluate (__LINE__, __func__, 94, similarity);

    similarity = filter_diff_word_similarity ("one two three", "three two one");
    evaluate (__LINE__, __func__, 20, similarity);

    similarity = filter_diff_word_similarity ("one two three", "one two three four");
    evaluate (__LINE__, __func__, 75, similarity);
  }
  {
    refresh_sandbox (true);
    Webserver_Request request;
    Database_Modifications database_modifications;
    Database_State::create ();

    client_logic_enable_client (false);
    database_modifications.truncateTeams ();
    
    string temporary_folder = filter_url_tempfile ();
    filter_url_mkdir (temporary_folder);
    
    request.database_bibles()->createBible ("phpunit");
    bible_logic_store_chapter ("phpunit", 1, 2, "old chapter text");
    database_modifications.truncateTeams ();
    bible_logic_store_chapter ("phpunit", 1, 2, "new chapter text");

    filter_diff_produce_verse_level ("phpunit", temporary_folder);

    string path, standard, output;

    path = filter_url_create_path ("unittests", "tests", "verses_old.usfm");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_old.usfm");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    path = filter_url_create_path ("unittests", "tests", "verses_new.usfm");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_new.usfm");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);

    path = filter_url_create_path ("unittests", "tests", "verses_old.txt");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_old.txt");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    path = filter_url_create_path ("unittests", "tests", "verses_new.txt");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "verses_new.txt");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
    
    string oldfile = filter_url_create_path (temporary_folder, "verses_old.usfm");
    string newfile = filter_url_create_path (temporary_folder, "verses_new.usfm");
    string outputfile = filter_url_create_path (temporary_folder, "changed_verses.html");
    filter_diff_run_file (oldfile, newfile, outputfile);

    path = filter_url_create_path ("unittests", "tests", "changed_verses.html");
    standard = filter_url_file_get_contents (path);
    path = filter_url_create_path (temporary_folder, "changed_verses.html");
    output = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, standard, output);
  }
}


void test_filter_abbreviations ()
{
  trace_unit_tests (__func__);
  
  // Read ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n"
    "Exodu = Exod.\n"
    "\n"
    "Psalms = Psa.\n"
    "\n"
    "Joshua =\n"
    "\n";
    vector <pair <int, string> > output = filter_abbreviations_read (input);
    vector <pair <int, string> > standard = { make_pair (19, "Ps."), make_pair (2, "Exod."), make_pair (19, "Psa.") };
    evaluate (__LINE__, __func__, standard, output);
  }
  // Display ()
  {
    string input =
    "Psalms = Ps.\n"
    "\n"
    "Exodus = Exod.\n"
    "\n";
    string output = filter_abbreviations_display (input);
    string standard =
    "Genesis = \n"
    "Exodus = Exod.\n"
    "Leviticus = \n"
    "Numbers = \n"
    "Deuteronomy = \n"
    "Joshua = \n"
    "Judges = \n"
    "Ruth = \n"
    "1 Samuel = \n"
    "2 Samuel = \n"
    "1 Kings = \n"
    "2 Kings = \n"
    "1 Chronicles = \n"
    "2 Chronicles = \n"
    "Ezra = \n"
    "Nehemiah = \n"
    "Esther = \n"
    "Job = \n"
    "Psalms = Ps.\n"
    "Proverbs = \n"
    "Ecclesiastes = \n"
    "Song of Solomon = \n"
    "Isaiah = \n"
    "Jeremiah = \n"
    "Lamentations = \n"
    "Ezekiel = \n"
    "Daniel = \n"
    "Hosea = \n"
    "Joel = \n"
    "Amos = \n"
    "Obadiah = \n"
    "Jonah = \n"
    "Micah = \n"
    "Nahum = \n"
    "Habakkuk = \n"
    "Zephaniah = \n"
    "Haggai = \n"
    "Zechariah = \n"
    "Malachi = \n"
    "Matthew = \n"
    "Mark = \n"
    "Luke = \n"
    "John = \n"
    "Acts = \n"
    "Romans = \n"
    "1 Corinthians = \n"
    "2 Corinthians = \n"
    "Galatians = \n"
    "Ephesians = \n"
    "Philippians = \n"
    "Colossians = \n"
    "1 Thessalonians = \n"
    "2 Thessalonians = \n"
    "1 Timothy = \n"
    "2 Timothy = \n"
    "Titus = \n"
    "Philemon = \n"
    "Hebrews = \n"
    "James = \n"
    "1 Peter = \n"
    "2 Peter = \n"
    "1 John = \n"
    "2 John = \n"
    "3 John = \n"
    "Jude = \n"
    "Revelation = \n"
    "Front Matter = \n"
    "Back Matter = \n"
    "Other Material = ";
    evaluate (__LINE__, __func__, standard, output);
  }
  // Read 
  {
    vector <pair <int, string> > input =
      { make_pair (1, "One"), make_pair (2, "Two"), make_pair (2, "Two."), make_pair (3, "3") };
    vector <pair <int, string> > output = filter_abbreviations_sort (input);
    vector <pair <int, string> > standard =
      { make_pair (2, "Two."), make_pair (1, "One"), make_pair (2, "Two"), make_pair (3, "3") };
    evaluate (__LINE__, __func__, standard, output);
  }
}


void test_filter_markup ()
{
  trace_unit_tests (__func__);
  
  {
    string text =
    "Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.\n"
    "Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    vector <string> words = { "makumbo", "akasia", "matanda" };
    string result = filter_string_markup_words (words, text);
    string standard =
    "Zvino uchagadzira <mark>makumbo</mark> uye <mark>Makumbo</mark> uye <mark>maKumbo</mark> uye <mark>MAKUMBO</mark> emu<mark>akasia</mark>*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa <mark>makumbo</mark> muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dze<mark>makumbo</mark> kutakura tafura.\n"
    "Zvino uchaita <mark>makumbo</mark> ne<mark>matanda</mark> ne<mark>Matanda</mark> ne<mark>maTANDA</mark> emu<mark>Akasia</mark>, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    evaluate (__LINE__, __func__, standard, result);
  }
  {
    string text =
    "Zvino uchagadzira makumbo uye Makumbo uye maKumbo uye MAKUMBO emuakasia*, ndokuaputira negoridhe.\n"
    "Zvino uchaisa makumbo muzvindori panhivi dzeareka, kuti areka itakurwe nawo.\n"
    "Zvindori zvichava pamupendero kuti zvive nzvimbo dzemakumbo kutakura tafura.\n"
    "Zvino uchaita makumbo nematanda neMatanda nemaTANDA emuAkasia, ugoiputira negoridhe, kuti tafura itakurwe nawo.\n";
    vector <string> words;
    string result = filter_string_markup_words (words, text);
    evaluate (__LINE__, __func__, text, result);
  }
}


void test_filter_git_setup (Webserver_Request * request, string bible, string newbible,
                            string psalms_0_data, string psalms_11_data, string song_of_solomon_2_data)
{
#ifdef HAVE_CLIENT
  (void) request;
  (void) bible;
  (void) newbible;
  (void) psalms_0_data;
  (void) psalms_11_data;
  (void) song_of_solomon_2_data;
#endif
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);
  
  Database_State::create ();
  Database_Git::create ();
  Database_Login::create ();

  string repository = filter_git_directory (bible);
  string newrepository = filter_git_directory (newbible);

  filter_url_mkdir (repository);
  filter_url_mkdir (newrepository);
  
  request->remote_address = "127.0.0.1";
  request->user_agent = "unittest";
  request->database_users ()->create ();
  request->session_logic ()->setUsername ("unittest");
  request->database_bibles()->createBible (bible);

  bool result;
  result = filter_git_init (repository);
  evaluate (__LINE__, __func__, true, result);
  result = filter_git_init (newrepository);
  evaluate (__LINE__, __func__, true, result);
  
  filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
  filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
  filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
  
  filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
  filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
  filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);

  filter_url_mkdir (filter_url_create_path (newrepository, "Psalms", "0"));
  filter_url_mkdir (filter_url_create_path (newrepository, "Psalms", "11"));
  filter_url_mkdir (filter_url_create_path (newrepository, "Song of Solomon", "2"));
  
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), psalms_0_data);
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "11", "data"), psalms_11_data);
  filter_url_file_put_contents (filter_url_create_path (newrepository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
#endif
}


void test_filter_git ()
{
#ifdef HAVE_CLOUD
  trace_unit_tests (__func__);
  
  string bible = "localrepo";
  string newbible = "newlocalrepo";
  string repository = filter_git_directory (bible);
  string newrepository = filter_git_directory (newbible);
  string remoterepository = filter_git_directory ("remoterepo");
  string clonedrepository = filter_git_directory ("clonedrepo");
  Webserver_Request request;
  
  string psalms_0_data =
    "\\id PSA\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 UGWALO\n"
    "\\mt LWEZIHLABELELO";

  string psalms_11_data =
  "\\c 11\n"
  "\\s IN\\sc KOSI\\sc* iyisiphephelo sabaqotho\n"
  "\\d Kumqondisi wokuhlabelela. EsikaDavida\n"
  "\\p\n"
  "\\v 1 Ngithembela eN\\sc KOSI\\sc*ni\\x + Hlab. 25.2.\\x*. Lingatsho njani emphefumulweni wami: Balekela entabeni yenu \\add njeng\\add*enyoni\\x + 1 Sam. 23.14,19. 26.19,20.\\x*.\n"
  "\\v 2 Ngoba, khangela, ababi bayagobisa idandili\\x + Hlab. 7.12. Hlab. 64.4.\\x*, balungisa umtshoko wabo entanjeni\\x + Hlab. 7.12. 21.12.\\x*, ukuze batshoke emnyameni abaqotho ngenhliziyo\\x + Hlab. 7.10.\\x*.\n"
  "\\v 3 Nxa izisekelo zidilizwa\\x + Isa. 19.10. Hlab. 82.5. Hlab. 75.3.\\x*, angenzani olungileyo\\x + Jobe 22.13.\\x*?\n"
  "\\p\n"
  "\\v 4 IN\\sc KOSI\\x + Hab. 2.20.\\x*\\sc* isethempelini layo elingcwele\\x + Hlab. 5.7. Hlab. 150.1.\\x*; iN\\sc KOSI\\sc*, isihlalo sayo sobukhosi sisemazulwini\\x + Hlab. 2.4. 103.19. 115.3. 123.1. Isa. 66.1. Mat. 5.34. 23.22. Seb. 7.49. Isam. 4.2.\\x*; amehlo ayo ayakhangela\\x + Jobe 24.23. Hlab. 33.13. 34.15. 66.7. Hlab. 14.2. 102.19. 113.5,6.\\x*, inkophe zayo ziyahlola, abantwana babantu.\n"
  "\\v 5 IN\\sc KOSI\\sc* iyamhlola olungileyo, kodwa omubi lothanda ubudlwangudlwangu, umphefumulo wayo uyamzonda\\x + Gen. 22.1.\\x*.\n"
  "\\v 6 Uzanisa phezu kwababi imijibila, umlilo, lesolufa*\\x + Jobe 18.15.\\x*, lomoya otshisayo\\x + Hlab. 119.53. Lilo 5.10.\\x*, kuzakuba yisabelo senkezo yabo\\x + Hlab. 75.8. Jobe 21.20. Hlab. 16.5.\\x*.\n"
  "\\v 7 Ngoba ilungile iN\\sc KOSI\\sc*, iyathanda ukulunga\\x + Hlab. 33.5. 45.7. Hlab. 37.28. 146.8.\\x*; ubuso bayo buyabona oqotho\\x + Hlab. 33.18. Hlab. 17.2.\\x*.";

  string song_of_solomon_2_data =
  "\\c 2\n"
  "\\p\n"
  "\\v 1 Ngilirozi\\x + Isa. 35.1.\\x* leSharoni\\x + Josh. 12.18.\\x*, umduze wezigodi\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*.\n"
  "\\p\n"
  "\\v 2 Njengomduze phakathi kwameva\\x + 2.16. 4.5. 5.13. 6.2,3. 7.2. 2 Lan. 4.5. Hos. 14.5. Hlab. 45.\\x*, unjalo umngane wami phakathi kwamadodakazi\\x + 1.15.\\x*.\n"
  "\\p\n"
  "\\v 3 Njengesihlahla sama-aphula phakathi kwezihlahla zeganga, sinjalo isithandwa sami phakathi kwamadodana\\x + Zaga 25.11.\\x*. Ngahlala emthunzini waso ngathokoza kakhulu\\x + Isa. 25.4. 32.2.\\x*, lesithelo saso simnandi ekunambitheni kwami\\x + Isam. 22.2.\\x*.\n"
  "\\v 4 Sangisa endlini yewayini, lesiboniso saso phezu kwami siluthando\\x + 1.4.\\x*.\n"
  "\\v 5 Ngisekelani\\x + Gen. 27.37.\\x* ngeziphiso zewayini\\x + 2 Sam. 6.19. 1 Lan. 16.3. Hos. 3.1.\\x*, lingiqinise ngama-aphula\\x + Zaga 25.11.\\x*, ngoba ngigul\\add isw\\add*a \\add lu\\add*thando\\x + 5.8.\\x*.\n"
  "\\v 6 Isandla saso sokhohlo singaphansi kwekhanda lami\\x + 8.3. Dute. 33.27.\\x*, lesokunene saso siyangigona\\x + 8.3. Dute. 33.27.\\x*.\n"
  "\\v 7 Ngiyalifungisa\\x + 3.5. 8.4. 5.8,9.\\x*, madodakazi eJerusalema\\x + 1.5.\\x*, ngemiziki\\x + 2.9,17.\\x*\\x + Zaga 6.5.\\x* kumbe ngezimpala zeganga\\x + 2.9,17.\\x*\\x + Zaga 5.19.\\x*, ukuze lingaphazamisi lingavusi uthando luze luthande.\n"
  "\\p\n"
  "\\v 8 Ilizwi lesithandwa sami! Khangela sona siyeza, siseqa phezu kwezintaba, siqolotsha phezu kwamaqaqa\\x + Isa. 52.7.\\x*.\n"
  "\\v 9 Isithandwa sami sinjengomziki\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x* kumbe njengethole lendluzele\\x + 2.7,17. 8.14. 2 Sam. 22.34.\\x*\\x + 4.5. 7.3.\\x*. Khangela simi ngemva komduli wethu, silunguza emawindini, sizibonakalisa\\f + \\fk zibonakalisa: \\fl Heb. \\fq hluma.\\f* eminxibeni yewindi\\x + Isa. 14.16.\\x*.\n"
  "\\v 10 Isithandwa sami saphendula sathi kimi\\x + Hlu. 18.14.\\x*: Vuka wena\\x + 2.13.\\x*, mngane wami, omuhle wami, a\\add si\\add*hambe wena\\x + 1.15.\\x*.\n"
  "\\v 11 Ngoba khangela, ubusika sebudlulile, izulu seliphelile, lihambile.\n"
  "\\v 12 Amaluba ayabonakala emhlabeni, isikhathi sokuhlabelela \\add se\\add*sifikile, lelizwi lejuba liyezwakala elizweni lakithi\\x + Hlab. 74.19. Jer. 8.7.\\x*.\n"
  "\\v 13 Isihlahla somkhiwa sivuthisa imikhiwa yaso eluhlaza, lamavini \\add ale\\add*mpoko\\x + 2.15. 7.12.\\x* aletha iphunga elimnandi\\x + Tshu. 7.1.\\x*. Vuka wena, mngane wami, omuhle wami wena, \\add si\\add*hambe\\x + 2.10.\\x*.\n"
  "\\p\n"
  "\\v 14 Juba lami\\x + 5.2. 6.9. 1.15. Mat. 10.16.\\x*, \\add elis\\add*engoxweni yedwala\\x + Jer. 48.28.\\x*\\x + Jer. 49.16. Obad. 3.\\x*, ekusithekeni kweliwa\\x + Hez. 38.20.\\x*, ngitshengisa ubuso bakho, ngizwise ilizwi lakho\\x + 8.13.\\x*, ngoba ilizwi lakho limnandi, lobuso bakho buyabukeka\\x + 1.5.\\x*.\n"
  "\\v 15 Sibambeleni amakhanka, amakhanka amancinyane, ona izivini\\x + Hez. 13.4. Luka 13.32.\\x*, ngoba izivini zethu \\add zile\\add*zimpoko\\x + 2.15. 7.12.\\x*.\n"
  "\\v 16 Isithandwa sami ngesami, lami ngingowaso\\x + 6.3. 7.10.\\x*, eselusa phakathi kwemiduze\\x + 2.1. 4.5. 6.3.\\x*.\n"
  "\\v 17 Kuze kube semadabukakusa, lamathunzi abaleke\\x + 4.6.\\x*, phenduka, sithandwa sami, ube njengomziki kumbe njengethole lendluzele\\x + 8.14. 2.9.\\x* phezu kwezintaba zeBhetheri\\x + 2 Sam. 2.29.\\x*.";

  // Sync Bible To Git 1
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));
    
    request.database_bibles()->storeChapter (bible, 2, 1, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));

    // Remove generated journal entries.
    refresh_sandbox (false);
  }

  // Sync Bible To Git 2
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));
    
    request.database_bibles()->storeChapter (bible, 19, 1, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);

    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "1", "data")));
    
    string data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "1", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    // Remove generated journal entries.
    refresh_sandbox (false);
  }
  
  // Sync Bible To Git 3
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "0", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, false, file_or_dir_exists (filter_url_create_path (repository, "Exodus", "1", "data")));

    request.database_bibles()->storeChapter (bible, 19, 1, song_of_solomon_2_data);
    request.database_bibles()->storeChapter (bible, 22, 2, psalms_11_data);
    request.database_bibles()->storeChapter (bible, 19, 11, song_of_solomon_2_data);
    filter_git_sync_bible_to_git (&request, bible, repository);
    
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, ".git")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "1", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Song of Solomon", "2", "data")));
    evaluate (__LINE__, __func__, true, file_or_dir_exists (filter_url_create_path (repository, "Psalms", "11", "data")));

    string data = filter_url_file_get_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"));
    evaluate (__LINE__, __func__, psalms_11_data, data);
    
    data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "11", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    data = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "1", "data"));
    evaluate (__LINE__, __func__, song_of_solomon_2_data, data);
    
    // Remove generated journal entries.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Add Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    // Run the filter, and check that all three chapters are now in the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);
    // Check that the data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, song_of_solomon_2_data, usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Delete Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    // Remove one book and one chapter from the git repository,
    // and check that after running the filter, the database is updated accordingly.
    filter_url_rmdir (repository + "/Song of Solomon");
    filter_url_rmdir (repository + "/Psalms/0");
    filter_git_sync_git_to_bible (&request, repository, bible);
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19}, books);
    // Check that the data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git To Bible Update Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    // Update some chapters in the git repository,
    // and check that after running the filter, the database is updated accordingly.
    filter_url_file_put_contents (repository + "/Psalms/11/data", "\\c 11");
    filter_url_file_put_contents (repository + "/Song of Solomon/2/data", "\\c 2");
    filter_git_sync_git_to_bible (&request, repository, bible);
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "\\c 11", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "\\c 2", usfm);
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Add Chapters ()
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // The Bible has been created, but has no data yet.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);
    
    // Run the filter for each chapter, and check that all three chapters make it into the database.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 0);
    usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, song_of_solomon_2_data, usfm);
    
    // Check the two books are there.
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);

    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Delete Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    
    // Remove one book and one chapter from the git repository,
    filter_url_rmdir (repository + "/Song of Solomon");
    filter_url_rmdir (repository + "/Psalms/0");
    
    // Run updates on the three chapters.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 0);
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    
    // There should still be two books, although one book would have no chapters.
    vector <int> books = request.database_bibles()->getBooks (bible);
    evaluate (__LINE__, __func__, {19, 22}, books);
    
    // Check that the chapter data matches.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, "", usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, psalms_11_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "", usfm);

    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }
  
  // Sync Git Chapter To Bible Update Chapters
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    
    // The git repository has Psalm 0, Psalm 11, and Song of Solomon 2.
    // Put that into the Bible database.
    filter_git_sync_git_to_bible (&request, repository, bible);
    
    // Update some chapters in the git repository.
    filter_url_file_put_contents (repository + "/Psalms/11/data", "\\c 11");
    filter_url_file_put_contents (repository + "/Song of Solomon/2/data", "\\c 2");
    
    // Run updates on the two chapters.
    filter_git_sync_git_chapter_to_bible (repository, bible, 19, 11);
    filter_git_sync_git_chapter_to_bible (repository, bible, 22, 2);
    
    // Check that the database is updated accordingly.
    string usfm = request.database_bibles()->getChapter (bible, 19, 0);
    evaluate (__LINE__, __func__, psalms_0_data, usfm);
    usfm = request.database_bibles()->getChapter (bible, 19, 11);
    evaluate (__LINE__, __func__, "\\c 11", usfm);
    usfm = request.database_bibles()->getChapter (bible, 22, 2);
    evaluate (__LINE__, __func__, "\\c 2", usfm);
    
    // Remove the journal entries the test created.
    refresh_sandbox (false);
  }

  // Setting values in the configuration.
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    filter_git_config_set_bool (repository, "foo.bar", false);
    filter_git_config_set_int (repository, "bar.baz", 11);
    string path = filter_url_create_path (repository, ".git", "config");
    string contents = filter_url_file_get_contents (path);
    evaluate (__LINE__, __func__, true, contents.find ("[foo]") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("[bar]") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("bar = false") != string::npos);
    evaluate (__LINE__, __func__, true, contents.find ("baz = 11") != string::npos);
    refresh_sandbox (false);
  }

  // Test of basic git operations in combination with a remote repository.
  {
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);
    string error;
    bool success;
    string remoteurl = "file://" + remoterepository;
    vector <string> messages;
    
    // Create bare remote reository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    
    // Test read access to the remote repository.
    success = filter_git_remote_read (remoterepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Test cloning the repository.
    success = filter_git_remote_clone (remoteurl, clonedrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Store some Bible data in the cloned repository.
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Psalms", "0"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Psalms", "11"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_mkdir (filter_url_create_path (clonedrepository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (clonedrepository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);

    // Add the Bible data to the git index.
    success = filter_git_add_remove_all (clonedrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Commit the index to the repository.
    success = filter_git_commit (clonedrepository, "username", "unittest", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Remove some Bible data from the cloned repository.
    filter_url_rmdir (filter_url_create_path (clonedrepository, "Psalms"));
    success = filter_git_add_remove_all (clonedrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Commit the index to the repository.
    success = filter_git_commit (clonedrepository, "username", "unittest", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Push to the remote repository.
    success = filter_git_push (clonedrepository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 2, (int)messages.size());

    // Pull from remote repository.
    success = filter_git_pull (clonedrepository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, {"Already up-to-date."}, messages);

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Get Git Passage
  {
    Passage passage = filter_git_get_passage ("From https://github.com/joe/test");
    evaluate (__LINE__, __func__, 0, passage.book);

    passage = filter_git_get_passage ("   443579b..90dcb57  master     -> origin/master");
    evaluate (__LINE__, __func__, 0, passage.book);

    passage = filter_git_get_passage ("Updating 443579b..90dcb57");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage ("Fast-forward");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" Genesis/3/data | 2 +-");
    Passage standard = Passage ("", 1, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
    
    passage = filter_git_get_passage (" 1 file changed, 1 insertion(+), 1 deletion(-)");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" delete mode 100644 Leviticus/1/data");
    evaluate (__LINE__, __func__, 0, passage.book);
    
    passage = filter_git_get_passage (" Revelation/3/data | 2 +-");
    standard = Passage ("", 66, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
    
    passage = filter_git_get_passage ("	modified:   Exodus/3/data");
    standard = Passage ("", 2, 3, "");
    evaluate (__LINE__, __func__, true, standard.equal (passage));
  }
  
  // Exercise the "git status" filter.
  {
    // Refresh the repository, and store three chapters in it.
    test_filter_git_setup (&request, bible, newbible, psalms_0_data, psalms_11_data, song_of_solomon_2_data);

    vector <string> paths;

    // There should be three modified paths.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "Psalms/") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "Song of Solomon/") != paths.end());

    // Add the files to the index.
    string error;
    vector <string> messages;
    filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, "", error);

    // There should still be three paths.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Psalms/11/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "new file:   Song of Solomon/2/data") != paths.end());
    
    // Commit the index.
    filter_git_commit (repository, "user", "unittest", messages, error);
    evaluate (__LINE__, __func__, "", error);

    // There should be no modified paths now.
    paths = filter_git_status (repository);
    evaluate (__LINE__, __func__, {"On branch master", "nothing to commit, working tree clean"}, paths);

    // Remove both Psalms chapters.
    filter_url_rmdir (filter_url_create_path (repository, "Psalms"));

    // There should be two modified paths now.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/11/data") != paths.end());

    // Add / remove the files to the index.
    filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, "", error);
    
    // There should still be two paths now.
    paths = filter_git_status (repository);
    for (auto & path : paths) path = filter_string_trim (path);
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/0/data") != paths.end());
    evaluate (__LINE__, __func__, true, find (paths.begin(), paths.end (), "deleted:    Psalms/11/data") != paths.end());
    
    // Commit the index.
    filter_git_commit (repository, "user", "unittest", messages, error);
    evaluate (__LINE__, __func__, "", error);
    
    // There should be no modified paths now.
    paths = filter_git_status (repository);
    evaluate (__LINE__, __func__, {"On branch master", "nothing to commit, working tree clean"}, paths);

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test git's internal conflict resolution.
  {
    refresh_sandbox (true);
    string error;
    bool success;
    vector <string> messages;

    // Create remote repository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    string remoteurl = "file://" + remoterepository;

    // Clone the remote repository.
    success = filter_git_remote_clone (remoteurl, repository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Store three chapters in the local repository and push it to the remote repository.
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
    filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (repository, messages, true);
    evaluate (__LINE__, __func__, true, success);

    // Clone the remote repository to a new local repository.
    success = filter_git_remote_clone (remoteurl, newrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);

    // Set the stage for a conflict that git itself cannot merge:
    // Change something in the new repository, push it to the remote.
    string newcontents =
    "\\id PSA\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 THE BOOK\n"
    "\\mt OF PSALMS\n";
    filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), newcontents);
    success = filter_git_add_remove_all (newrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (newrepository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (newrepository, messages, true);
    // Change something in the repository, and pull from remote:
    // Git fails to merge by itself.
    string contents =
    "\\id PSALM\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 UGWALO\n"
    "\\mt LWEZIHLABELELO\n";
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), contents);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_pull (repository, messages);
    evaluate (__LINE__, __func__, false, success);
    success = find (messages.begin(), messages.end(), "Auto-merging Psalms/0/data") != messages.end();
    evaluate (__LINE__, __func__, true, success);
    success = find (messages.begin(), messages.end(), "CONFLICT (content): Merge conflict in Psalms/0/data") != messages.end();
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_push (repository, messages);
    evaluate (__LINE__, __func__, false, success);
    vector <string> paths = { "Psalms/0/data" };
    success = filter_git_resolve_conflicts (repository, paths, error);
    evaluate (__LINE__, __func__, "", error);
    // Check the merge result.
    string standard =
    "\\id PSALM\n"
    "\\h Izihlabelelo\n"
    "\\toc2 Izihlabelelo\n"
    "\\mt2 THE BOOK\n"
    "\\mt OF PSALMS";
    contents = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "0", "data"));
    evaluate (__LINE__, __func__, standard, contents);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }

  {
    refresh_sandbox (true);
    string error;
    bool success;
    vector <string> messages;
    
    // Create remote repository.
    filter_url_mkdir (remoterepository);
    filter_git_init (remoterepository, true);
    string remoteurl = "file://" + remoterepository;
    
    // Clone the remote repository.
    success = filter_git_remote_clone (remoteurl, repository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Store three chapters in the local repository and push it to the remote repository.
    psalms_0_data =
    "Line one one one\n"
    "Line two two two\n"
    "Line three three three\n";
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "0"));
    filter_url_mkdir (filter_url_create_path (repository, "Psalms", "11"));
    filter_url_mkdir (filter_url_create_path (repository, "Song of Solomon", "2"));
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), psalms_0_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "11", "data"), psalms_11_data);
    filter_url_file_put_contents (filter_url_create_path (repository, "Song of Solomon", "2", "data"), song_of_solomon_2_data);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (repository, messages, true);
    evaluate (__LINE__, __func__, true, success);
    
    // Clone the remote repository to a new local repository.
    success = filter_git_remote_clone (remoteurl, newrepository, 0, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    
    // Set the stage for a conflict that git itself can merge:
    // Change something in the new repository, push it to the remote.
    string newcontents =
    "Line 1 one one\n"
    "Line two two two\n"
    "Line three three three\n";
    filter_url_file_put_contents (filter_url_create_path (newrepository, "Psalms", "0", "data"), newcontents);
    success = filter_git_add_remove_all (newrepository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (newrepository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_push (newrepository, messages, true);
    // Change something in the repository, and pull from remote:
    // Git fails to merge by itself.
    string contents =
    "Line one one 1 one\n"
    "Line two 2 two 2 two\n"
    "Line three 3 three 3 three\n";
    filter_url_file_put_contents (filter_url_create_path (repository, "Psalms", "0", "data"), contents);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    success = filter_git_commit (repository, "test", "test", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    // Pulling changes should result in a merge conflict.
    success = filter_git_pull (repository, messages);
    evaluate (__LINE__, __func__, false, success);

    // Resolve the conflict.
    success = filter_git_resolve_conflicts (repository, messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, "", error);
    evaluate (__LINE__, __func__, {"Psalms/0/data"}, messages);

    // Verify the resolved contents on correctness.
    contents = filter_url_file_get_contents (filter_url_create_path (repository, "Psalms", "0", "data"));
    string standard =
    "Line 1 one 1 one\n"
    "Line two 2 two 2 two\n"
    "Line three 3 three 3 three";
    evaluate (__LINE__, __func__, standard, contents);

    // The status still displays the file as in conflict.
    // messages = filter_git_status (repository);
    // evaluate (__LINE__, __func__, {"Psalms/0/data"}, messages);

    // Commit and push the result.
    success = filter_git_commit (repository, "", "message", messages, error);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 4, (int)messages.size());
    success = filter_git_push (repository, messages);
    evaluate (__LINE__, __func__, true, success);
    evaluate (__LINE__, __func__, 2, (int)messages.size());
    
    // Status up-to-date.
    messages = filter_git_status (repository);
    evaluate (__LINE__, __func__, 3, (int)messages.size ());

    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test one user saving Bible data in an uninterrupted sequence, that it leads to correct records in git.
  {
    refresh_sandbox (true);

    string error;
    bool success;
    vector <string> messages;

    test_filter_git_setup (&request, bible, newbible, "Psalm 0\n", "Psalm 11\n", "Song of Solomon 2\n");
    
    string repository = filter_git_directory (bible);

    // Commit the data to the repository.
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_commit (repository, "", "initial commit", messages, error);
    evaluate (__LINE__, __func__, true, success);

    int psalms = 19;
    string user1 = "user1";
    string user2 = "user2";
    string oldusfm1, newusfm1;
    string out_err;

    // Create records of user saving data.
    oldusfm1 = "Psalm 11\n";
    newusfm1 = oldusfm1 + "Praise";
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" Jesus");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" forever.\n");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    filter_git_sync_modifications_to_git (bible, repository);

    // Check the diff.
    filter_shell_run ("cd " + repository + " && git log -p", out_err);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus forever.") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user1 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("User modification") != string::npos);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }
  
  // Test one user saving Bible data, but this time the sequence is interrupted by undefined other users.
  {
    refresh_sandbox (true);
    
    string error;
    bool success;
    vector <string> messages;
    
    test_filter_git_setup (&request, bible, newbible, "Psalm 0\n", "Psalm 11\n", "Song of Solomon 2\n");
    
    string repository = filter_git_directory (bible);
    
    // Commit the data to the repository.
    success = filter_git_add_remove_all (repository, error);
    evaluate (__LINE__, __func__, true, success);
    success = filter_git_commit (repository, "", "initial commit", messages, error);
    evaluate (__LINE__, __func__, true, success);
    
    int psalms = 19;
    string user1 = "user1";
    string user2 = "user2";
    string oldusfm1, newusfm1;
    string out_err;

    // Create records of two users saving data.
    oldusfm1 = "Psalm 11\n";
    newusfm1 = oldusfm1 + "Praise";
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    oldusfm1 = newusfm1;
    newusfm1.append (" Jesus");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    Database_Git::store_chapter (user2, bible, psalms, 11, oldusfm1 + " xx\n", newusfm1 + " xxx\n");
    oldusfm1 = newusfm1;
    newusfm1.append (" forever.\n");
    Database_Git::store_chapter (user1, bible, psalms, 11, oldusfm1, newusfm1);
    filter_git_sync_modifications_to_git (bible, repository);
    filter_shell_run ("cd " + repository + " && git log -p", out_err);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus forever.") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user1 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("Author: user2 <bibledit@bibledit.org>") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("User modification") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("System-generated to clearly display user modification in next commit") != string::npos);
    evaluate (__LINE__, __func__, true, out_err.find ("+Praise Jesus xxx") != string::npos);
    
    // Remove journal entries.
    refresh_sandbox (false);
  }
#endif
}


void test_filter_merge ()
{
  trace_unit_tests (__func__);
  
  // Test Line Merge Simple Modifications.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string serverModificationData =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka kukaJesu";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Line Merge Equal Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 28\n"
    "\\s Ukuvuka kukaJesu\n"
    "\\s Ukuvuka kukaJesu";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Line Merge Multiple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\pp\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Marko 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya osukwini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65-66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\pp\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Marko 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya osukwini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesematho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65-66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Word Merge Simple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, true, conflicts.empty ());
  }
  
  // Test Word Merge Conflicting Modifications.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, basebesiba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 1, conflicts.size ());
  }
  
  // Test Word Merge Multiple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakalisa kukaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Hlu. 13.6.\\x*, lesembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 28\n"
    "\\s Ukuvuka lokuzibonakaliswa kwaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Dan. 10.6. Hlu. 13.6.\\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\s Ukuvuka lokuzibonakaliswa kwaJesu\n"
    "\\p\n"
    "\\v 1 Kwathi ekupheleni kwesabatha\\x + Mark. 16.1-8. Luka 24.1-10.\\x*, emadabukakusa kusiya o\\add sukw\\add*ini lokuqala lweviki\\x + Joha. 20.1.\\x*, kwafika uMariya Magadalena\\x + Joha. 20.1.\\x*, lomunye uMariya, ukuzabona ingcwaba\\x + 27.56,61. Mark. 16.1. Luka 24.10.\\x*.\n"
    "\\v 2 Futhi khangela, kwaba khona ukuzamazama komhlaba okukhulu\\x + 27.51,54.\\x*; ngoba ingilosi yeNkosi yehla ivela ezulwini\\x + Mark. 16.5. Luka 24.4. Joha. 20.12.\\x*, yasondela yagiqa ilitshe yalisusa emnyango, yahlala phezu kwalo\\x + 27.60,66.\\x*.\n"
    "\\v 3 Lokubonakala kwayo kwakunjengombane\\x + Hlu. 13.6.\\x*, njalo isembatho sayo sasimhlophe njengeliqhwa elikhithikileyo\\x + Dan. 7.9. Mark. 9.3.\\x*.\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Test Grapheme Merge Simple Modifications
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi bathuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela besabe baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi bathuthumela besabe baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Test Conflict Take Server.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba, baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string userModificationData =
    "\\c 28\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string serverModificationData =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\\x + 27.65,66.\\x*.\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 29\n"
    "\\v 4 Abalindi basebethuthumela ngokuyesaba; baba njengabafileyo\\x + 27.65,66.\\x*.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 1, conflicts.size ());
  }

  // Realistic merge example
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first verse.\n"
    "\\v 2 And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Merge situation taken from real life.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string path;
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_base.usfm");
    string mergeBaseData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_modification.usfm");
    string userModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_server.usfm");
    string serverModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_1_result.usfm");
    string standard = filter_url_file_get_contents (path);

    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());

    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Testing the clever merge routine on chapter 0.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\id GEN\n"
    "\\p Some text one.\n";
    string userModificationData =
    "\\id GEN\n"
    "\\p Some text two.\n";
    string serverModificationData =
    "\\id GEN\n"
    "\\p Some text one.\n";
    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    string standard =
    "\\id GEN\n"
    "\\p Some text two.";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Testing switching from separate verses into a combined verse.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
   
    conflicts.clear ();
    output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }

  // Testing switching from a combined verse to separate verses.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string mergeBaseData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string userModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string serverModificationData =
    "\\c 1\n"
    "\\p\n"
    "\\v 1-2 This is really the text of the first (1st) verse. And this is what the second verse contains.\n"
    "\\v 3 The third verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5\n";
    string output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, false, conflicts);
    string standard =
    "\\c 1\n"
    "\\p\n"
    "\\v 1 This is really the text of the first (1st) verse.\n"
    "\\v 2 And this is what the second (2nd) verse contains.\n"
    "\\v 3 The third (3rd) verse.\n"
    "\\v 4 The fourth (4th) verse.\n"
    "\\v 5";
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 0, conflicts.size ());
  }
  
  // Merge situation taken from real life.
  {
    vector <tuple <string, string, string, string, string>> conflicts;
    string path;
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_base.usfm");
    string mergeBaseData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_modification.usfm");
    string userModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_server.usfm");
    string serverModificationData = filter_url_file_get_contents (path);
    path = filter_url_create_root_path ("unittests", "tests", "paula_2_result.usfm");
    string standard = filter_url_file_get_contents (path);
    
    string output = filter_merge_run_clever (mergeBaseData, userModificationData, serverModificationData, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 3, conflicts.size ());
    
    conflicts.clear ();
    output = filter_merge_run (mergeBaseData, userModificationData, serverModificationData, true, conflicts);
    evaluate (__LINE__, __func__, standard, output);
    evaluate (__LINE__, __func__, 3, conflicts.size ());
  }
}


void test_filter_tidy ()
{
  trace_unit_tests (__func__);
  
  string folder = filter_url_create_root_path ("unittests", "tests");
  string html = filter_url_file_get_contents (filter_url_create_path (folder, "/biblehub-john-1-1.html"));
  vector <string> tidy = filter_string_explode (html_tidy (html), '\n');
  evaluate (__LINE__, __func__, 752, (int)tidy.size());
}


void test_ipc_notes ()
{
  trace_unit_tests (__func__);
  
  // Initialize.
  refresh_sandbox (true);
  Webserver_Request request;
  request.database_users ()->create ();
  request.session_logic ()->setUsername ("phpunit");

  // There should be no note identifier.
  int identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test opening note.
  Ipc_Notes::open (&request, 123456789);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);
  
  // Test trimming.
  request.database_ipc()->trim ();
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 123456789, identifier);

  // Test deleting note once.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);

  // Test deleting two notes.
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::open (&request, 123456789);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
  Ipc_Notes::erase (&request);
  identifier = Ipc_Notes::get (&request);
  evaluate (__LINE__, __func__, 0, identifier);
}


void test_filter_date ()
{
  trace_unit_tests (__func__);
  
  // First Business Day Of Month
  {
    // Sunday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 0));
    // Monday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 1));
    // Tuesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 2));
    // Wednesday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 3));
    // Thirsday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 4));
    // Friday the 1st.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (1, 5));
    // Saturday the 1st.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (1, 6));
    // Sunday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 0));
    // Monday the 2nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (2, 1));
    // Tuesday the 2nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (2, 2));
    // Sunday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 0));
    // Monday the 3nd.
    evaluate (__LINE__, __func__, true, filter_date_is_first_business_day_of_month (3, 1));
    // Tuesday the 3nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (3, 2));
    // Sunday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 0));
    // Monday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 1));
    // Tuesday the 4nd.
    evaluate (__LINE__, __func__, false, filter_date_is_first_business_day_of_month (4, 2));
  }
  // Last Business Day Of Month
  {
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2013, 9));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 10));
    evaluate (__LINE__, __func__, 29, filter_date_get_last_business_day_of_month (2013, 11));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2013, 12));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 1));
    evaluate (__LINE__, __func__, 28, filter_date_get_last_business_day_of_month (2014, 2));
    evaluate (__LINE__, __func__, 31, filter_date_get_last_business_day_of_month (2014, 3));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 4));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 5));
    evaluate (__LINE__, __func__, 30, filter_date_get_last_business_day_of_month (2014, 6));
  }
  // Is Business Day
  {
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 1));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 2));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 3));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 4));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 5));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 6));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 7));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2013, 9, 8));
    evaluate (__LINE__, __func__, true, filter_date_is_business_day (2013, 9, 30));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 3, 1));
    evaluate (__LINE__, __func__, false, filter_date_is_business_day (2015, 2, 32));
    
  }
  // Seonds since Unix epoch.
  {
    int year, month, day, seconds;

    year = 2011;
    month = 2;
    day = 5;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));

    year = 2015;
    month = 3;
    day = 15;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
    
    year = 2030;
    month = 12;
    day = 31;
    seconds = filter_date_seconds_since_epoch (year, month, day);
    evaluate (__LINE__, __func__, year, filter_date_numerical_year (seconds));
    evaluate (__LINE__, __func__, month, filter_date_numerical_month (seconds));
    evaluate (__LINE__, __func__, day, filter_date_numerical_month_day (seconds));
  }
}


void test_filter_string ()
{
  trace_unit_tests (__func__);
  
  {
    string input = "<span>Praise the LORD&#xB6;, all &amp; you nations</span>";
    string output = convert_xml_character_entities_to_characters (input);
    string standard = filter_string_str_replace ("&#xB6;", "¶", input);
    evaluate (__LINE__, __func__, standard, output);

    input = "<span>Praise the LORD &#x5D0; all you nations</span>";
    output = convert_xml_character_entities_to_characters (input);
    standard = filter_string_str_replace ("&#x5D0;", "א", input);
    evaluate (__LINE__, __func__, standard, output);
    
    input = "Username";
    output = encrypt_decrypt ("key", input);
    output = encrypt_decrypt ("key", output);
    evaluate (__LINE__, __func__, input, output);

    input = "בְּרֵאשִׁ֖ית בָּרָ֣א אֱלֹהִ֑ים אֵ֥ת הַשָּׁמַ֖יִם וְאֵ֥ת הָאָֽרֶץ";
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", input);
    output = encrypt_decrypt ("בְּרֵאשִׁ֖ית", output);
    evaluate (__LINE__, __func__, input, output);
  }

  {
    string one = get_new_random_string ();
    this_thread::sleep_for (chrono::milliseconds (10));
    string two = get_new_random_string ();
    evaluate (__LINE__, __func__, 32, one.length ());
    evaluate (__LINE__, __func__, true, one != two);
  }

  {
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("test"));
    evaluate (__LINE__, __func__, 4, (int)unicode_string_length ("ᨁᨃᨅᨕ"));
  }

  {
    string hebrew = "אָבּגּדּהּ";
    evaluate (__LINE__, __func__, "st1234", unicode_string_substr ("test1234", 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 5));
    evaluate (__LINE__, __func__, "", unicode_string_substr (hebrew, 6));
    evaluate (__LINE__, __func__, "test", unicode_string_substr ("test123456", 0, 4));
    evaluate (__LINE__, __func__, "12", unicode_string_substr ("test123456", 4, 2));
    evaluate (__LINE__, __func__, "גּדּ", unicode_string_substr (hebrew, 2, 2));
    evaluate (__LINE__, __func__, "גּדּהּ", unicode_string_substr (hebrew, 2, 10));
  }
  
  {
    string hebrew = "אָבּגּדּהּ";
    string needle = "דּ";
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos ("012345", "3"));
    evaluate (__LINE__, __func__, 5, (int)unicode_string_strpos ("012345", "5"));
    evaluate (__LINE__, __func__, 0, (int)unicode_string_strpos ("012345", "0"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("012345", "6"));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle));
    evaluate (__LINE__, __func__, 3, (int)unicode_string_strpos (hebrew, needle, 3));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos (hebrew, needle, 4));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos ("", "3"));
  }
  
  {
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "c"));
    evaluate (__LINE__, __func__, 2, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "cD"));
    evaluate (__LINE__, __func__, -1, (int)unicode_string_strpos_case_insensitive ("AbCdEf", "ce"));
  }
  
  {
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("test1234"));
    evaluate (__LINE__, __func__, "test1234", unicode_string_casefold ("TEST1234"));
    evaluate (__LINE__, __func__, "θεος", unicode_string_casefold ("Θεος"));
    evaluate (__LINE__, __func__, "α α β β", unicode_string_casefold ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_casefold ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("test1234"));
    evaluate (__LINE__, __func__, "TEST1234", unicode_string_uppercase ("TEST1234"));
    evaluate (__LINE__, __func__, "ΘΕΟΣ", unicode_string_uppercase ("Θεος"));
    evaluate (__LINE__, __func__, "Α Α Β Β", unicode_string_uppercase ("Α α Β β"));
    evaluate (__LINE__, __func__, "אָבּגּדּהּ", unicode_string_uppercase ("אָבּגּדּהּ"));
  }

  {
    evaluate (__LINE__, __func__, "ABCDEFG", unicode_string_transliterate ("ABCDEFG"));
    evaluate (__LINE__, __func__, "Ιησου Χριστου", unicode_string_transliterate ("Ἰησοῦ Χριστοῦ"));
    evaluate (__LINE__, __func__, "אבגדה", unicode_string_transliterate ("אָבּגּדּהּ"));
  }
  
  {
    vector <string> needles;
    needles = filter_string_search_needles ("ABC", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
    needles = filter_string_search_needles ("abc", "one abc two ABc three aBc four");
    evaluate (__LINE__, __func__, { "abc", "ABc", "aBc" }, needles);
  }
  
  {
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("A"));
    evaluate (__LINE__, __func__, false, unicode_string_is_punctuation ("z"));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation ("."));
    evaluate (__LINE__, __func__, true, unicode_string_is_punctuation (","));
  }
  
  {
    evaluate (__LINE__, __func__, false, convert_to_bool ("0"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("false"));
    evaluate (__LINE__, __func__, false, convert_to_bool ("FALSE"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("1"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("true"));
    evaluate (__LINE__, __func__, true, convert_to_bool ("TRUE"));
  }
  
  {
    evaluate (__LINE__, __func__, 21109, unicode_string_convert_to_codepoint ("創"));
    evaluate (__LINE__, __func__, 97, unicode_string_convert_to_codepoint ("a"));
  }
  
  {
    // Check that the function to desanitize html no longer corrupts UTF-8.
    string html = "<p>“Behold”, from “הִנֵּה”, means look at</p>";
    string desanitized = filter_string_desanitize_html (html);
    evaluate (__LINE__, __func__, html, desanitized);
  }
}


void test_filter_shell ()
{
  trace_unit_tests (__func__);
  
  evaluate (__LINE__, __func__, true, filter_shell_is_present ("zip"));
  evaluate (__LINE__, __func__, false, filter_shell_is_present ("xxxxx"));
  
  string output;
  int result;

  result = filter_shell_vfork (output, "", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("unittest") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list files", output);
  }

  result = filter_shell_vfork (output, "/", "ls", "-l");
  evaluate (__LINE__, __func__, 0, result);
  if (output.find ("tmp") == string::npos) {
    evaluate (__LINE__, __func__, "Supposed to list folder /", output);
  }
}


void test_filter_memory ()
{
  trace_unit_tests (__func__);
  
  int available = filter_memory_percentage_available ();
  (void) available;
}


void test_filter_ldap ()
{
  trace_unit_tests (__func__);
  
  refresh_sandbox (true);

  // Copy the default LDAP server configuration into place.
  string ldap_txt = filter_url_create_root_path ("config", "ldap.txt");
  string ldap_conf = filter_url_create_root_path ("config", "ldap.conf");
  filter_url_file_cp (ldap_txt, ldap_conf);
  // Initialize LDAP configuration.
  ldap_logic_initialize ();
  
  // Authenticate a user and check the results.
  string user = "boyle";
  string password = "password";
  bool okay;
  bool access;
  string email;
  int role;
  okay = ldap_logic_fetch (user, password, access, email, role, false);
  evaluate (__LINE__, __func__, true, okay);
  evaluate (__LINE__, __func__, true, access);
  evaluate (__LINE__, __func__, "boyle@ldap.forumsys.com", email);
  evaluate (__LINE__, __func__, Filter_Roles::guest (), role);

  // Check there is one journal entry as a result of authenticating a user.
  string last = "0";
  vector <string> logs = Database_Logs::get (last);
  evaluate (__LINE__, __func__, 1, logs.size ());

  // Clear LDAP settings.
  ldap_logic_clear ();
  refresh_sandbox (false);
}


void test_filter_dev ()
{
  trace_unit_tests (__func__);
}
