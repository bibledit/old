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


#include <resource/external.h>
#include <filter/string.h>
#include <filter/url.h>
#include <webserver/request.h>
#include "json/json.h"
#include "assets/view.h"
#include "resource/logic.h"


// Local forward declarations:


unsigned int resource_external_count ();
int gbs_digitaal_json_callback (void *userdata, int type, const char *data, uint32_t length);
string gbs_digitaal_processor (string url, int chapter, int verse);
string gbs_digitaal_plus_processor (string url, int verse);
string resource_external_studylight_processor (string directory, int book, int chapter, int verse);
string bibleserver_processor (string directory, int book, int chapter, int verse);
string resource_external_get_statenbijbel_gbs (int book, int chapter, int verse);
string resource_external_get_statenbijbel_plus_gbs (int book, int chapter, int verse);
string resource_external_get_king_james_version_gbs (int book, int chapter, int verse);
string resource_external_get_king_james_version_plus_gbs (int book, int chapter, int verse);
string resource_external_get_biblehub_interlinear (int book, int chapter, int verse);
string resource_external_get_biblehub_scrivener (int book, int chapter, int verse);
string resource_external_get_biblehub_westminster (int book, int chapter, int verse);
string resource_external_get_net_bible (int book, int chapter, int verse);
// Start of Studylight commentaries forward declarations
string resource_external_studylight_commentary_get_acc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ain (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_bbc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_bcc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_bch (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_bnb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_box (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_bul (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_cal (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_cgt (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_cob (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_cpc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_csc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dcb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dcc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dcp (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dsb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dsn (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_dun (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ebc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_egt (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_fam (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_fbh (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_fbm (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_gab (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_gcc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_gcm (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_geb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ges (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ghc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_gmt (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_gsb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_guz (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hac (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hal (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hcc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hdg (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hmc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_hms (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ice (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_isn (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jab (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jbc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jbm (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jec (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jfb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jfu (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jgc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jlc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_joc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jsc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_jtc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_kdo (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_kpc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_lmg (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_lwc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_mac (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_mhm (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_mhn (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_mlg (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_mpc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_nfp (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_oca (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_pet (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_pfc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_phc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_pmc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_pnt (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_rbc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_rsc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_rwp (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ryl (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_sbc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_scn (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_shh (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_sor (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_spe (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_srn (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tbi (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tcc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_teb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tfg (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tgc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tod (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tpc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_tsk (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_ttb (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_vnt (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_wbc (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_wen (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_whe (int book, int chapter, int verse);
string resource_external_studylight_commentary_get_wnc (int book, int chapter, int verse);
// End of Studylight commentaries forward declarations
string resource_external_get_blue_letter_bible (int book, int chapter, int verse);
string resource_external_get_elberfelder_bibel (int book, int chapter, int verse);
string resource_external_convert_book_biblehub (int book);
string resource_external_convert_book_netbible (int book);
string resource_external_convert_book_blueletterbible (int book);
string resource_external_convert_book_bibleserver (int book);


typedef struct
{
  const char *name;
  const char *versification;
  const char *mapping;
  const char *type;
  string (* func) (int, int, int);
} resource_record;


#define ORIGINAL "o"
#define BIBLE "b"
#define COMMENTARY "c"
#define GENERAL "g"


// Implementation:


resource_record resource_table [] =
{
  { "Statenbijbel GBS", "Dutch Traditional", "Dutch Traditional", BIBLE, & resource_external_get_statenbijbel_gbs },
  { "Statenbijbel Plus GBS", "Dutch Traditional", "Dutch Traditional", BIBLE, & resource_external_get_statenbijbel_plus_gbs },
  { "King James Version GBS", "English", "English", BIBLE, & resource_external_get_king_james_version_gbs },
  { "King James Version Plus GBS", "English", "English", BIBLE, & resource_external_get_king_james_version_plus_gbs },
  { resource_external_biblehub_interlinear_name (), "English", "English", ORIGINAL, & resource_external_get_biblehub_interlinear },
  { "Scrivener Greek", "English", "English", ORIGINAL, & resource_external_get_biblehub_scrivener },
  { "Westminster Hebrew", "English", "English", ORIGINAL, & resource_external_get_biblehub_westminster },
  { resource_external_net_bible_name (), "English", "English", BIBLE, & resource_external_get_net_bible },
  // Start of Studylight commentaries resources table entries
  { "Clarke's Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_acc },
  { "Abbott's Illustrated New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ain },
  { "Bridgeway Bible Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_bbc },
  { "Coffman's Commentaries on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_bcc },
  { "Light & Truth: Bible Thoughts and Themes on Revelation", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_bch },
  { "Barnes' Notes on the Whole Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_bnb },
  { "Box's Commentary on Seleted Books of the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_box },
  { "Bullinger's Companion Bible Notes", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_bul },
  { "Calvin's Commentaries on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_cal },
  { "Cambridge Greek Testament for Schools and Colleges", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_cgt },
  { "Cobb's Commentary on Philemon", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_cob },
  { "Nisbet's Church Pulpit Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_cpc },
  { "Smith's Bible Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_csc },
  { "Dummelow's Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dcb },
  { "Expository Notes of Dr. Constable", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dcc },
  { "F. B. Meyer's - A Devotional Commentary on Philippians", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dcp },
  { "Barclay's Daily Study Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dsb },
  { "Darby's Synopsis of the Whole Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dsn },
  { "Dunagan's Commentary on Selected Books", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_dun },
  { "Ellicott's Commentary for English Readers", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ebc },
  { "The Expositor's Greek Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_egt },
  { "Edwards' Family Bible New Testament Notes", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_fam },
  { "Hole's Old and New Testament Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_fbh },
  { "Meyer's \"Through the Bible\" Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_fbm },
  { "Gaebelein's \"The Annotated Bible\"", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_gab },
  { "Golden Chain Commentary on the Gospels", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_gcc },
  { "Morgan's Exposition on the Whole Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_gcm },
  { "Gill's Exposition of the Whole Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_geb },
  { "Godbey's Commentary on the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ges },
  { "Hampton's Commentary on Selected Books", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ghc },
  { "George Milligan - Paul's Epistle to the Thessalonians", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_gmt },
  { "Geneva Study Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_gsb },
  { "Guzik's Commentaries on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_guz },
  { "Greek Testament Critical Exegetical Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hac },
  { "Haldane's Commentary on Romans", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hal },
  { "Haydock's Catholic Bible Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hcc },
  { "Hodge's Commentary on Romans, Ephesians and First Corintians", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hdg },
  { "Meyer's Critical and Exegetical Commentary on the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hmc },
  { "Henry Mahan's Commentary on Selected Books of the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_hms },
  { "The Bible Study New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ice },
  { "Ironside's Notes on Selected Books", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_isn },
  { "Bengel's Gnomon of the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jab },
  { "Beet's Commentary on Selected Books of the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jbc },
  { "Broadus' Commentary on Matthew", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jbm },
  { "Eadie's Commentary on Galatians, Ephesians, Philippians and Colossians", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jec },
  { "Commentary Critical and Explanatory on the Whole Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jfb },
  { "Commentary Critical and Explanatory on the Whole Bible - Unabridged", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jfu },
  { "Gray's Concise Bible Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jgc },
  { "Lightfoot's Commentary on the Gospels", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jlc },
  { "Owen's Exposition of Hebrews", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_joc },
  { "Sutcliffe's Commentary on the Old and New Testaments", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jsc },
  { "Trapp's Complete Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_jtc },
  { "Keil and Delitzsch Commentary on the Old Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_kdo },
  { "Kretzmann's Popular Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_kpc },
  { "Grant's Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_lmg },
  { "Neighbour's Living Water Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_lwc },
  { "MacLaren's Exposition of the Holy Scripture", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_mac },
  { "Henry's Complete Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_mhm },
  { "Henry's Concise Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_mhn },
  { "Luther's Commentary on Galatians", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_mlg },
  { "Poole's English Annotations on the Holy Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_mpc },
  { "Macintosh's Notes on the Pentateuch", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_nfp },
  { "McGarvey's Original Commentary on Acts", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_oca },
  { "Pett's Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_pet },
  { "Peake's Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_pfc },
  { "Preacher's Complete Homiletical Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_phc },
  { "Hawker's Poor Man's Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_pmc },
  { "People's New Testament ", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_pnt },
  { "Benson's Commentary on the Old and New Testaments", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_rbc },
  { "Stedman's Expository Studies", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_rsc },
  { "Robertson's Word Pictures in the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_rwp },
  { "J. C. Ryle's Expository Thoughts on the Gospels", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ryl },
  { "Sermon Bible Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_sbc },
  { "Schaff's Popular Commentary on the New Testament", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_scn },
  { "Simeon's Horae Homileticae", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_shh },
  { "Scott's Commentary on Revelation", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_sor },
  { "Spurgeon's Verse Expositions of the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_spe },
  { "Scofield's Reference Notes", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_srn },
  { "The Biblical Illustrator", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tbi },
  { "Coke's Commentary on the Holy Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tcc },
  { "The Expositor's Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_teb },
  { "The Fourfold Gospel", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tfg },
  { "The Gospels Compared", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tgc },
  { "Spurgeon's The Treasury of David", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tod },
  { "The Pulpit Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tpc },
  { "The Treasury of Scripture Knowlege", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_tsk },
  { "McGee's \"Thru the Bible\"", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_ttb },
  { "Vincent's Word Studies", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_vnt },
  { "Burkitt's Commentary", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_wbc },
  { "Wesley's Explanatory Notes", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_wen },
  { "Whedon's Commentary on the Bible", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_whe },
  { "Newell's Commentary on Romans and Revelation", "English", "English", COMMENTARY, & resource_external_studylight_commentary_get_wnc },
  // End of Studylight commentaries resources table entries
  { "Blue Letter Bible", "English", "English", ORIGINAL, & resource_external_get_blue_letter_bible },
  { "Elberfelder Bibel", "English", "English", BIBLE, & resource_external_get_elberfelder_bibel },
};


int gbs_digitaal_json_callback (void *userdata, int type, const char *data, uint32_t length)
{
  if (length) {};
  vector <string> * history = (vector <string> *) userdata;
  switch (type) {
    case JSON_OBJECT_BEGIN:
    case JSON_ARRAY_BEGIN:
    case JSON_OBJECT_END:
    case JSON_ARRAY_END:
      break;
    case JSON_KEY:
    case JSON_STRING:
    case JSON_INT:
    case JSON_FLOAT:
    {
      history->push_back (data);
      break;
    }
    case JSON_NULL:
    {
      history->push_back ("");
      break;
    }
    case JSON_TRUE:
    case JSON_FALSE:
      break;
  }
  return 0;
}


// This function displays the canonical text from gbsdigitaal.nl.
string gbs_digitaal_processor (string url, int verse)
{
  string text;

  string json = resource_logic_web_cache_get (url, text);

  vector <string> history;
  
  json_parser parser;
  if (json_parser_init (&parser, NULL, gbs_digitaal_json_callback, &history)) {
    text.append ("Could not initialize");
  }
  if (json_parser_string (&parser, json.c_str(), json.length(), NULL)) {
    text.append ("Could not parse the data");
  }
  json_parser_free (&parser);

  // The history sequence to get a verse is this:
  // uid -> 0 -> number -> <verse number> -> text -> "verse text"
  for (unsigned int i = 0; i < history.size (); i++) {
    if (history[i] == "text") {
      if (i >= 5) {
        if (history [i - 4] == "uid") {
          if (history [i - 3] == "0") {
            if (history [i - 2] == "number") {
              if (history [i - 1] == convert_to_string (verse)) {
                if (history.size () > i + 1) {
                  text = history [i + 1];
                }
              }
            }
          }
        }
      }
    }
  }

  // Take out breaks.
  text = filter_string_str_replace ("<br />", "", text);
  // Remove the note callers.
  filter_string_replace_between (text, "<sup", "</sup>", "");
  // Remove the pilcrow sign / paragraph sign if it's there.
  text = filter_string_str_replace ("¶", "", text);
  // Add new line.
  text += "\n";
  
  // Done.
  return text;
}


// This funcion processes and displays Bible data from gbsdigitaal.nl
// plus extra content like headers, introductions, cross references, and notes.
// It is supposed to be called by a more specific script.
string gbs_digitaal_plus_processor (string url, int chapter, int verse) // Todo
{
  string text;
  
  string json = resource_logic_web_cache_get (url, text);

  vector <string> history;
  
  json_parser parser;
  if (json_parser_init (&parser, NULL, gbs_digitaal_json_callback, &history)) {
    text.append ("Could not initialize");
  }
  if (json_parser_string (&parser, json.c_str(), json.length(), NULL)) {
    text.append ("Could not parse the data");
  }
  json_parser_free (&parser);
  
  string historyblock;
  bool collect_notes = false;
  int text_block_counter = 0;
  for (unsigned int i = 0; i < history.size (); i++) {
    // Add possible book title.
    if (chapter == 0) {
      if (history[i] == "title") {
        if (history.size () > i + 1) {
          text.append (" " + history [i + 1]);
        }
      }
    }
    // Add possible book summary.
    if (chapter == 0) {
      if (history[i] == "summary") {
        if (history.size () > i + 1) {
          text.append (" " + history [i + 1]);
        }
      }
    }
    // Add possible chapter title.
    if (chapter == 0) {
      if (history[i] == "headerTitle") {
        if (history.size () > i + 1) {
          text.append (" " + history [i + 1]);
        }
      }
    }
    // Add possible book summary.
    if (chapter == 0) {
      if (i > 2) {
        if (history[i] == "content") {
          if (history[i-1] == "lines") {
            if (history.size () > i + 1) {
              text.append (" " + history [i + 1]);
            }
          }
        }
      }
    }
    // Add the chapter summary located at verse 0.
    if (verse == 0) {
      if (i > 2) {
        if (history[i] == "originalText") {
          if (history[i-1] == "summary") {
            if (history.size () > i + 1) {
              text.append (" " + history [i + 1]);
            }
          }
        }
      }
    }
    // History sequence to get the header above a verse:
    // text -> <header text> -> isPerikoop -> isHeader -> uid -> 0 -> number -> <verse number>
    if (history[i] == "text") {
      if (history.size () > i + 7) {
        if (history[i+2] == "isPerikoop") {
          if (history[i+7] == convert_to_string (verse)) {
            text.append ("<p><i>" + history [i+1] + "</i></p>");
          }
        }
      }
    }
    // The history sequence to get the verse text is this:
    // uid -> 0 -> number -> <verse number> -> text -> "verse text"
    if (history[i] == "text") {
      if (i >= 5) {
        if (history [i - 4] == "uid") {
          if (history [i - 3] == "0") {
            if (history [i - 2] == "number") {
              if (history [i - 1] == convert_to_string (verse)) {
                if (history.size () > i + 1) {
                  text.append (" " + history [i + 1]);
                }
                // Enable notes collection.
                collect_notes = true;
              } else {
                // Disable notes collection
                collect_notes = false;
              }
            }
          }
        }
      }
    }
    // Add notes.
    if (collect_notes && i && history [i-1] == "originalText") {
      text.append ("<p>" + history [i] + "</p>");
    }
    // Add text of Bible references.
    if (collect_notes && i && history [i-1] == "text") {
      if (text_block_counter) text.append ("<p>" + history [i] + "</p>");
      text_block_counter++;
    }
  }

  text = filter_string_str_replace ("\n", " ", text);
  text = filter_string_str_replace ("\r", " ", text);
  text = filter_string_str_replace ("\t", "", text);
  text = filter_string_str_replace ("  ", " ", text);
  text = filter_string_str_replace ("<br />", "", text);
  text = filter_string_trim (text);

  // Add new line.
  text += "\n";

  // Done.
  return text;
}


// This function retrieves commentaries from www.studylight.org.
string resource_external_studylight_processor (string directory, int book, int chapter, int verse)
{
  // On StudyLight.org, Genesis equals book 0, Exodus book 1, and so on.
  book--;
  
  string url = "http://www.studylight.org/com/" + directory + "/view.cgi?bk=" + convert_to_string (book) + "&ch=" + convert_to_string (chapter);
  
  // Get the html from the server, and tidy it up.
  string error;
  string html = resource_logic_web_cache_get (url, error);
  string tidy = html_tidy (html);
  vector <string> tidied = filter_string_explode (tidy, '\n');
 
  vector <string> relevant_lines;
  bool relevant_flag = false;
  
  for (auto & line : tidied) {
    
    if (relevant_flag) relevant_lines.push_back (line);
    
    size_t pos = line.find ("</div>");
    if (pos != string::npos) relevant_flag = false;
    
    pos = line.find ("name=\"" + convert_to_string (verse) + "\"");
    if (pos != string::npos) relevant_flag = true;
  }
  
  html = filter_string_implode (relevant_lines, "\n");
  html += "<p><a href=\"" + url + "\">" + url + "</a></p>\n";
  
  return html;
}


// This filters Bibles from www.bibleserver.com.
string bibleserver_processor (string directory, int book, int chapter, int verse)
{
  string bookname = resource_external_convert_book_bibleserver (book);
  
  string url = "http://www.bibleserver.com/text/" + directory + "/" + bookname + convert_to_string (chapter);
  
  string error;
  string text = resource_logic_web_cache_get (url, error);
  string tidy = html_tidy (text);
  vector <string> tidied = filter_string_explode (tidy, '\n');

  text.clear ();
  bool relevant_line = false;
  for (auto & line : tidied) {
    size_t pos = line.find ("noscript");
    if (pos != string::npos) relevant_line = false;
    if (relevant_line) {
      if (!text.empty ()) text.append (" ");
      text.append (line);
    }
    pos = line.find ("no=\"" + convert_to_string (verse) + "," + convert_to_string (verse) + "\"");
    if (pos != string::npos) relevant_line = true;
  }
  filter_string_replace_between (text, "<", ">", "");
  text = filter_string_trim (text);
  
  text += "<p><a href=\"" + url + "\">" + url + "</a></p>\n";
  
  return text;
}


// This script fetches the Statenbijbel from the Dutch GBS.
string resource_external_get_statenbijbel_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/Statenvertaling/1.json
  // Books NT: http://gbsdigitaal.nl/Data/Statenvertaling/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/Statenvertaling/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/Statenvertaling/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/Statenvertaling/2/40/2.json
  
  // Old Testament = 1 and New Testament = 2.
  int testament = book >= 40 ? 2 : 1;
  
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = convert_to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/Statenvertaling/" + convert_to_string (testament) + "/" + convert_to_string (book) + "/" + urlchapter + ".json";
  
  return gbs_digitaal_processor (url, verse);
}


// This displays the Statenbijbel from the Dutch GBS.
// It also includes headers, introductions, and notes.
string resource_external_get_statenbijbel_plus_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/Statenvertaling/1.json
  // Books NT: http://gbsdigitaal.nl/Data/Statenvertaling/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/Statenvertaling/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/Statenvertaling/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/Statenvertaling/2/40/2.json
  
  // Old (1) or New (2) Testament.
  int testament = book >= 40 ? 2 : 1;
  
  // Introduction is chapter 1 in the URL.
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = convert_to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/Statenvertaling/" + convert_to_string (testament) + "/" + convert_to_string (book) + "/" + urlchapter + ".json";
  
  return gbs_digitaal_plus_processor (url, chapter, verse);
}


// This script displays the King James Bible published by the Dutch GBS.
string resource_external_get_king_james_version_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/AuthorizedVersion/1.json
  // Books NT: http://gbsdigitaal.nl/Data/AuthorizedVersion/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/AuthorizedVersion/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/AuthorizedVersion/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/AuthorizedVersion/2/40/2.json
  
  // Old (1) or New (2) Testament.
  int testament = book >= 40 ? 2 : 1;
  
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = convert_to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/AuthorizedVersion/" + convert_to_string (testament) + "/" + convert_to_string (book) + "/" + urlchapter + ".json";
  
  return gbs_digitaal_processor (url, verse);
}


// This script displays the Statenbijbel from the Dutch GBS.
// It also includes headers, introductions, and notes.
string resource_external_get_king_james_version_plus_gbs (int book, int chapter, int verse)
{
  // Books OT: http://gbsdigitaal.nl/Data/Statenvertaling/1.json
  // Books NT: http://gbsdigitaal.nl/Data/Statenvertaling/2.json
  // Genesis intro http://gbsdigitaal.nl/Data/Statenvertaling/1/1/1.json
  // Genesis 1 http://gbsdigitaal.nl/Data/Statenvertaling/1/1/2.json
  // Matthew 1 http://gbsdigitaal.nl/Data/Statenvertaling/2/40/2.json
  
  // Old (1) or New (2) Testament.
  int testament = book >= 40 ? 2 : 1;
  
  // Introduction is chapter 1 in the URL.
  // Chapter 1 of the text is chapter 2 in the URL.
  string urlchapter = convert_to_string (chapter + 1);
  
  string url = "http://gbsdigitaal.nl/Data/AuthorizedVersion/" + convert_to_string (testament) + "/" + convert_to_string (book) + "/" + urlchapter + ".json";
  
  return gbs_digitaal_plus_processor (url, chapter, verse);
}


// This displays the interlinear Bible from biblehub.com.
string resource_external_get_biblehub_interlinear (int book, int chapter, int verse)
{
  // Sample URL:
  // http://biblehub.com/interlinear/genesis/1-1.htm
 
  string bookname = resource_external_convert_book_biblehub (book);
  
  string url = "http://biblehub.com/interlinear/" + bookname + "/" + convert_to_string (chapter) + "-" + convert_to_string (verse) + ".htm";
  
  // Get the html from the server, and tidy it up.
  string error;
  string html = resource_logic_web_cache_get (url, error);
  string tidy = html_tidy (html);
  vector <string> tidied = filter_string_explode (tidy, '\n');
  
  vector <string> filtered_lines;
  
  int relevant_line = 0;
  for (auto & line : tidied) {
    if (line.find ("<div") != string::npos) {
      relevant_line = 0;
    }
    if (relevant_line) {
      relevant_line++;
    }
    if (relevant_line > 3) {
      if (line.find ("iframe") == string::npos) {
        filtered_lines.push_back (line);
      }
    }
    if (line.find ("\"vheading\"") != string::npos) {
      relevant_line++;
    }
  }
  
  html = filter_string_implode (filtered_lines, "\n");
  
  html = filter_string_str_replace ("/abbrev.htm", "http://biblehub.com/abbrev.htm", html);
  html = filter_string_str_replace ("/hebrew/", "http://biblehub.com/hebrew/", html);
  html = filter_string_str_replace ("/hebrewparse.htm", "http://biblehub.com/hebrewparse.htm", html);
  html = filter_string_str_replace ("/greek/", "http://biblehub.com/greek/", html);
  html = filter_string_str_replace ("/grammar/", "http://biblehub.com/grammar/", html);
  //html = filter_string_str_replace ("height=\"165\"", "", html);
  html = filter_string_str_replace ("height=\"160\"", "", html);
  html = filter_string_str_replace ("height=\"145\"", "", html);
  html = filter_string_str_replace (non_breaking_space_entity () + non_breaking_space_entity (), non_breaking_space_entity (), html);
  
  // Stylesheet for using web fonts,
  // because installing fonts on some tablets is very hard.
  string stylesheet =
  "<style>\n"
  ".tablefloat {\n"
  "  float: left;\n"
  "}\n"
  ".tablefloatheb\n"
  "{\n"
  "  float : right;\n"
  "}\n"
  "span[class*='ref'] {\n"
  "display: none;\n"
  "}\n"
  "</style>\n";
  string output = stylesheet;
  // The following line prevents the Hebrew and Greek from floating around
  // the name of the Resource, which would disturb the order of the words in Hebrew.
  output += "<p></p>\n";
  output += html;
  
  return output;
}


// Filters the Scrivener Greek text from biblehub.com.
string resource_external_get_biblehub_scrivener (int book, int chapter, int verse)
{
  string bookname = resource_external_convert_book_biblehub (book);
  
  string url = "http://biblehub.com/text/" + bookname + "/" + convert_to_string (chapter) + "-" + convert_to_string (verse) + ".htm";
  
  // Get the html from the server, and tidy it up.
  string error;
  string html = resource_logic_web_cache_get (url, error);
  string tidy = html_tidy (html);
  vector <string> tidied = filter_string_explode (tidy, '\n');

  html.clear ();
  int hits = 0;
  for (auto & line : tidied) {
    /* This is the text block we are looking at:
    <p>
    <span class="versiontext">
    <a href="/tr94/matthew/1.htm">Scrivener's Textus Receptus 1894
    </a>
    <br>
    </span>
    <span class="greek">Βίβλος γενέσεως Ἰησοῦ Χριστοῦ, υἱοῦ Δαβὶδ, υἱοῦ Ἀβραάμ.
    </span>
    */
    if (line.find ("Scrivener") != string::npos) hits = 1;
    if (hits == 1) if (line.find ("greek") != string::npos) hits = 2;
    if ((hits == 2) || (hits == 3)) {
      html.append (line);
      hits++;
    }
  }
  
  if (html.empty ()) return html;
  
  string stylesheet =
  "<style>\n"
  "</style>\n";
  
  return stylesheet + "\n" + html;
}


// This displays the Westminster Leningrad Codex from biblehub.com.
string resource_external_get_biblehub_westminster (int book, int chapter, int verse)
{
  // No New Testament in the Westminster Leningrad Codex.
  if (book >= 40) {
    return "";
  }
  
  string bookname = resource_external_convert_book_biblehub (book);
  
  // Sample URL:
  // http://biblehub.com/text/genesis/1-1.htm
  string url = "http://biblehub.com/text/" + bookname + "/" + convert_to_string (chapter) + "-" + convert_to_string (verse) + ".htm";
  
  // Get the html from the server, and tidy it up.
  string error;
  string html = resource_logic_web_cache_get (url, error);
  string tidy = html_tidy (html);
  vector <string> tidied = filter_string_explode (tidy, '\n');
  
  html.clear ();
  int hits = 0;
  for (auto & line : tidied) {
    /* This is the text block we are looking at:
     <div align="right">
     <span class="versiontext">
     <a href="/wlc/genesis/1.htm">בראשית 1:1 Hebrew OT: Westminster Leningrad Codex
     </a>
     <br>
     </span>
     <span class="heb">בְּרֵאשִׁ֖ית בָּרָ֣א אֱלֹהִ֑ים אֵ֥ת הַשָּׁמַ֖יִם וְאֵ֥ת הָאָֽרֶץ׃
     </span>
     <p>
    */
    if (line.find ("Westminster") != string::npos) hits = 1;
    if (hits == 1) if (line.find ("class=\"heb\"") != string::npos) hits = 2;
    if (hits == 2) {
      html.append (line);
      if (line.find ("</span>") != string::npos) hits = 0;
    }
  }
  
  if (html.empty ()) return html;
  
  // Change class "heb" to "hebrew", because that is what Bibledit uses for all Hebrew text.
  html = filter_string_str_replace ("heb", "hebrew", html);
  
  
  // Stylesheet for using web fonts,
  // because installing fonts on some tablets is very hard.
  string stylesheet =
  "<style>\n"
  "</style>\n";

  string output = stylesheet;
  
  // The following line prevents the Hebrew from floating around the name of the Resource,
  // which would disturb the order of the words.
  output += "<p></p>\n";
  
  output += html;

  return output;

  // In Job 24:14, it lacks a space between two words, making it hard to read.
  // In Job 31:36, one letter is joined to the wrong word.
}


// This displays the text and the notes of the NET Bible.
string resource_external_get_net_bible (int book, int chapter, int verse)
{
  string bookname = resource_external_convert_book_netbible (book);
  
  string url = bookname + " " + convert_to_string (chapter) + ":" + convert_to_string (verse);
  url = filter_url_urlencode (url);
  url.insert (0, "https://net.bible.org/resource/netTexts/");
  
  string error;
  string text = resource_logic_web_cache_get (url, error);
  
  string output = text;
  
  url = bookname + " " + convert_to_string (chapter) + ":" + convert_to_string (verse);
  url = filter_url_urlencode (url);
  url.insert (0, "https://net.bible.org/resource/netNotes/");
  
  string notes = resource_logic_web_cache_get (url, error);
  // If notes fail with an error, don't include the note text.
  if (!error.empty ()) notes.clear ();
  // It the verse contains no notes, the website returns an unusual message.
  if (notes.find ("We are currently offline") != string::npos) notes.clear ();
  
  // The "bibleref" class experiences interference from other resources,
  // so that the reference would become invisible.
  // Remove this class, and the references will remain visible.
  notes = filter_string_str_replace ("class=\"bibleref\"", "", notes);
  
  output += notes;
  
  return output;
}


// Start of Studylight commentaries implementation functions
// Clarke's Commentary
string resource_external_studylight_commentary_get_acc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "acc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Abbott's Illustrated New Testament
string resource_external_studylight_commentary_get_ain (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ain";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Bridgeway Bible Commentary
string resource_external_studylight_commentary_get_bbc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "bbc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Coffman's Commentaries on the Bible
string resource_external_studylight_commentary_get_bcc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "bcc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Light & Truth: Bible Thoughts and Themes on Revelation
string resource_external_studylight_commentary_get_bch (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "bch";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Barnes' Notes on the Whole Bible
string resource_external_studylight_commentary_get_bnb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "bnb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Box's Commentary on Seleted Books of the Bible
string resource_external_studylight_commentary_get_box (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "box";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Bullinger's Companion Bible Notes
string resource_external_studylight_commentary_get_bul (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "bul";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Calvin's Commentaries on the Bible
string resource_external_studylight_commentary_get_cal (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "cal";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Cambridge Greek Testament for Schools and Colleges
string resource_external_studylight_commentary_get_cgt (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "cgt";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Cobb's Commentary on Philemon
string resource_external_studylight_commentary_get_cob (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "cob";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Nisbet's Church Pulpit Commentary
string resource_external_studylight_commentary_get_cpc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "cpc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Smith's Bible Commentary
string resource_external_studylight_commentary_get_csc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "csc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Dummelow's Commentary on the Bible
string resource_external_studylight_commentary_get_dcb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dcb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Expository Notes of Dr. Constable
string resource_external_studylight_commentary_get_dcc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dcc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// F. B. Meyer's - A Devotional Commentary on Philippians
string resource_external_studylight_commentary_get_dcp (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dcp";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Barclay's Daily Study Bible
string resource_external_studylight_commentary_get_dsb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dsb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Darby's Synopsis of the Whole Bible
string resource_external_studylight_commentary_get_dsn (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dsn";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Dunagan's Commentary on Selected Books
string resource_external_studylight_commentary_get_dun (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "dun";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Ellicott's Commentary for English Readers
string resource_external_studylight_commentary_get_ebc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ebc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Expositor's Greek Testament
string resource_external_studylight_commentary_get_egt (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "egt";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Edwards' Family Bible New Testament Notes
string resource_external_studylight_commentary_get_fam (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "fam";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Hole's Old and New Testament Commentary
string resource_external_studylight_commentary_get_fbh (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "fbh";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Meyer's "Through the Bible" Commentary
string resource_external_studylight_commentary_get_fbm (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "fbm";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Gaebelein's "The Annotated Bible"
string resource_external_studylight_commentary_get_gab (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "gab";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Golden Chain Commentary on the Gospels
string resource_external_studylight_commentary_get_gcc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "gcc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Morgan's Exposition on the Whole Bible
string resource_external_studylight_commentary_get_gcm (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "gcm";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Gill's Exposition of the Whole Bible
string resource_external_studylight_commentary_get_geb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "geb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Godbey's Commentary on the New Testament
string resource_external_studylight_commentary_get_ges (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ges";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Hampton's Commentary on Selected Books
string resource_external_studylight_commentary_get_ghc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ghc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// George Milligan - Paul's Epistle to the Thessalonians
string resource_external_studylight_commentary_get_gmt (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "gmt";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Geneva Study Bible
string resource_external_studylight_commentary_get_gsb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "gsb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Guzik's Commentaries on the Bible
string resource_external_studylight_commentary_get_guz (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "guz";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Greek Testament Critical Exegetical Commentary
string resource_external_studylight_commentary_get_hac (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hac";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Haldane's Commentary on Romans
string resource_external_studylight_commentary_get_hal (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hal";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Haydock's Catholic Bible Commentary
string resource_external_studylight_commentary_get_hcc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hcc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Hodge's Commentary on Romans, Ephesians and First Corintians
string resource_external_studylight_commentary_get_hdg (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hdg";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Meyer's Critical and Exegetical Commentary on the New Testament
string resource_external_studylight_commentary_get_hmc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hmc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Henry Mahan's Commentary on Selected Books of the New Testament
string resource_external_studylight_commentary_get_hms (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "hms";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Bible Study New Testament
string resource_external_studylight_commentary_get_ice (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ice";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Ironside's Notes on Selected Books
string resource_external_studylight_commentary_get_isn (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "isn";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Bengel's Gnomon of the New Testament
string resource_external_studylight_commentary_get_jab (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jab";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Beet's Commentary on Selected Books of the New Testament
string resource_external_studylight_commentary_get_jbc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jbc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Broadus' Commentary on Matthew
string resource_external_studylight_commentary_get_jbm (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jbm";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Eadie's Commentary on Galatians, Ephesians, Philippians and Colossians
string resource_external_studylight_commentary_get_jec (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jec";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Commentary Critical and Explanatory on the Whole Bible
string resource_external_studylight_commentary_get_jfb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jfb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Commentary Critical and Explanatory on the Whole Bible - Unabridged
string resource_external_studylight_commentary_get_jfu (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jfu";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Gray's Concise Bible Commentary
string resource_external_studylight_commentary_get_jgc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jgc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Lightfoot's Commentary on the Gospels
string resource_external_studylight_commentary_get_jlc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jlc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Owen's Exposition of Hebrews
string resource_external_studylight_commentary_get_joc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "joc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Sutcliffe's Commentary on the Old and New Testaments
string resource_external_studylight_commentary_get_jsc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jsc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Trapp's Complete Commentary
string resource_external_studylight_commentary_get_jtc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "jtc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Keil and Delitzsch Commentary on the Old Testament
string resource_external_studylight_commentary_get_kdo (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "kdo";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Kretzmann's Popular Commentary
string resource_external_studylight_commentary_get_kpc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "kpc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Grant's Commentary on the Bible
string resource_external_studylight_commentary_get_lmg (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "lmg";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Neighbour's Living Water Commentary
string resource_external_studylight_commentary_get_lwc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "lwc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// MacLaren's Exposition of the Holy Scripture
string resource_external_studylight_commentary_get_mac (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "mac";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Henry's Complete Commentary on the Bible
string resource_external_studylight_commentary_get_mhm (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "mhm";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Henry's Concise Commentary on the Bible
string resource_external_studylight_commentary_get_mhn (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "mhn";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Luther's Commentary on Galatians
string resource_external_studylight_commentary_get_mlg (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "mlg";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Poole's English Annotations on the Holy Bible
string resource_external_studylight_commentary_get_mpc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "mpc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Macintosh's Notes on the Pentateuch
string resource_external_studylight_commentary_get_nfp (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "nfp";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// McGarvey's Original Commentary on Acts
string resource_external_studylight_commentary_get_oca (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "oca";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Pett's Commentary on the Bible
string resource_external_studylight_commentary_get_pet (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "pet";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Peake's Commentary on the Bible
string resource_external_studylight_commentary_get_pfc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "pfc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Preacher's Complete Homiletical Commentary
string resource_external_studylight_commentary_get_phc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "phc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Hawker's Poor Man's Commentary
string resource_external_studylight_commentary_get_pmc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "pmc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// People's New Testament
string resource_external_studylight_commentary_get_pnt (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "pnt";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Benson's Commentary on the Old and New Testaments
string resource_external_studylight_commentary_get_rbc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "rbc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Stedman's Expository Studies
string resource_external_studylight_commentary_get_rsc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "rsc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Robertson's Word Pictures in the New Testament
string resource_external_studylight_commentary_get_rwp (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "rwp";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// J. C. Ryle's Expository Thoughts on the Gospels
string resource_external_studylight_commentary_get_ryl (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ryl";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Sermon Bible Commentary
string resource_external_studylight_commentary_get_sbc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "sbc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Schaff's Popular Commentary on the New Testament
string resource_external_studylight_commentary_get_scn (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "scn";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Simeon's Horae Homileticae
string resource_external_studylight_commentary_get_shh (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "shh";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Scott's Commentary on Revelation
string resource_external_studylight_commentary_get_sor (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "sor";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Spurgeon's Verse Expositions of the Bible
string resource_external_studylight_commentary_get_spe (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "spe";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Scofield's Reference Notes
string resource_external_studylight_commentary_get_srn (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "srn";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Biblical Illustrator
string resource_external_studylight_commentary_get_tbi (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tbi";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Coke's Commentary on the Holy Bible
string resource_external_studylight_commentary_get_tcc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tcc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Expositor's Bible
string resource_external_studylight_commentary_get_teb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "teb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Fourfold Gospel
string resource_external_studylight_commentary_get_tfg (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tfg";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Gospels Compared
string resource_external_studylight_commentary_get_tgc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tgc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Spurgeon's The Treasury of David
string resource_external_studylight_commentary_get_tod (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tod";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Pulpit Commentary
string resource_external_studylight_commentary_get_tpc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tpc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// The Treasury of Scripture Knowlege
string resource_external_studylight_commentary_get_tsk (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "tsk";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// McGee's "Thru the Bible"
string resource_external_studylight_commentary_get_ttb (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "ttb";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Vincent's Word Studies
string resource_external_studylight_commentary_get_vnt (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "vnt";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Burkitt's Commentary
string resource_external_studylight_commentary_get_wbc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "wbc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Wesley's Explanatory Notes
string resource_external_studylight_commentary_get_wen (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "wen";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Whedon's Commentary on the Bible
string resource_external_studylight_commentary_get_whe (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "whe";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// Newell's Commentary on Romans and Revelation
string resource_external_studylight_commentary_get_wnc (int book, int chapter, int verse)
{
  // The directory on studylight.org
  string directory = "wnc";
  return resource_external_studylight_processor (directory, book, chapter, verse);
}
// End of Studylight commentaries implementation functions


// Blue Letter Bible.
string resource_external_get_blue_letter_bible (int book, int chapter, int verse)
{
  if (verse) {};
  
  string bookname = resource_external_convert_book_blueletterbible (book);
  
  string output;
  
  string url = "http://www.blueletterbible.org/Bible.cfm?b=" + filter_url_urlencode (bookname) + "&c=$" + convert_to_string (chapter) + "&t=KJV&ss=1";
  
  output += "<a href=\"" + url + "\">KJV</a>";
  
  output += " | ";
  
  url = "http://www.blueletterbible.org/Bible.cfm?b=" + filter_url_urlencode (bookname) + "&c=" + convert_to_string (chapter) + "&t=WLC";
  
  output += "<a href=\"" + url + "\">WLC</a>";
  
  output += " | ";
  
  url = "http://www.blueletterbible.org/Bible.cfm?b=" + filter_url_urlencode (bookname) + "&c=" + convert_to_string (chapter) + "&t=mGNT";
  
  output += "<a href=\"" + url + "\">mGNT</a>";

  return output;
}


// This displays the text of the Elberfelder Bibel.
string resource_external_get_elberfelder_bibel (int book, int chapter, int verse)
{
  // The directory on bibleserver.com
  string directory = "ELB";
  return bibleserver_processor (directory, book, chapter, verse);
}


// The number of available external resource scripts.
unsigned int resource_external_count ()
{
  return sizeof (resource_table) / sizeof (*resource_table);
}


// Gets the names of all the known Web resources.
vector <string> resource_external_names ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    names.push_back (resource_table [i].name);
  }
  sort (names.begin (), names.end ());
  return names;
}


// Get the names of the Web resources which are original language resources.
vector <string> resource_external_get_original_language_resources ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    if (strcmp (resource_table [i].type, ORIGINAL) == 0) {
      names.push_back (resource_table [i].name);
    }
  }
  sort (names.begin (), names.end ());
  return names;
}


// Get the names of the Web resources which are Bibles.
vector <string> resource_external_get_bibles ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    if (strcmp (resource_table [i].type, BIBLE) == 0) {
      names.push_back (resource_table [i].name);
    }
  }
  sort (names.begin (), names.end ());
  return names;
}


// Get the names of the Web resources which are commentaries.
vector <string> resource_external_get_commentaries ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    if (strcmp (resource_table [i].type, COMMENTARY) == 0) {
      names.push_back (resource_table [i].name);
    }
  }
  sort (names.begin (), names.end ());
  return names;
}


// Get the names of the Web resources which are other resources.
vector <string> resource_external_get_general_resources ()
{
  vector <string> names;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    if (strcmp (resource_table [i].type, GENERAL) == 0) {
      names.push_back (resource_table [i].name);
    }
  }
  sort (names.begin (), names.end ());
  return names;
}


// Returns the versification for the resource.
string resource_external_versification (string name)
{
  string versification = "English";
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      versification = resource_table [i].versification;
    }
  }
  return versification;
}


// Returns the versification for the resource.
string resource_external_mapping (string name)
{
  string versification = "English";
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      versification = resource_table [i].mapping;
    }
  }
  return versification;
}


// Returns what type the Web resource $name is.
string resource_external_type (string name)
{
  string type = GENERAL;
  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      type = resource_table [i].type;
    }
  }
  return type;
}


// Fetches the html data for the resource / book / chapter / verse.
// This function runs on the Cloud.
// It fetches data either from the cache or from the web via http(s),
// while optionally updating the cache with the raw web page content.
// It extracts the relevant snipped from the larger http(s) content.
string resource_external_cloud_fetch_cache_extract (string name, int book, int chapter, int verse)
{
  string (* func) (int, int, int) = NULL;

  for (unsigned int i = 0; i < resource_external_count (); i++) {
    string resource = resource_table [i].name;
    if (name == resource) {
      func = resource_table [i].func;
    }
  }
  
  if (func == NULL) {
    return "";
  }

  string result = func (book, chapter, verse);
  
  return result;
}


string resource_external_convert_book_biblehub (int book)
{
  // Map Bibledit books to biblehub.com books.
  map <int, string> mapping = {
    make_pair (1, "genesis"),
    make_pair (2, "exodus"),
    make_pair (3, "leviticus"),
    make_pair (4, "numbers"),
    make_pair (5, "deuteronomy"),
    make_pair (6, "joshua"),
    make_pair (7, "judges"),
    make_pair (8, "ruth"),
    make_pair (9, "1_samuel"),
    make_pair (10, "2_samuel"),
    make_pair (11, "1_kings"),
    make_pair (12, "2_kings"),
    make_pair (13, "1_chronicles"),
    make_pair (14, "2_chronicles"),
    make_pair (15, "ezra"),
    make_pair (16, "nehemiah"),
    make_pair (17, "esther"),
    make_pair (18, "job"),
    make_pair (19, "psalms"),
    make_pair (20, "proverbs"),
    make_pair (21, "ecclesiastes"),
    make_pair (22, "songs"),
    make_pair (23, "isaiah"),
    make_pair (24, "jeremiah"),
    make_pair (25, "lamentations"),
    make_pair (26, "ezekiel"),
    make_pair (27, "daniel"),
    make_pair (28, "hosea"),
    make_pair (29, "joel"),
    make_pair (30, "amos"),
    make_pair (31, "obadiah"),
    make_pair (32, "jonah"),
    make_pair (33, "micah"),
    make_pair (34, "nahum"),
    make_pair (35, "habakkuk"),
    make_pair (36, "zephaniah"),
    make_pair (37, "haggai"),
    make_pair (38, "zechariah"),
    make_pair (39, "malachi"),
    make_pair (40, "matthew"),
    make_pair (41, "mark"),
    make_pair (42, "luke"),
    make_pair (43, "john"),
    make_pair (44, "acts"),
    make_pair (45, "romans"),
    make_pair (46, "1_corinthians"),
    make_pair (47, "2_corinthians"),
    make_pair (48, "galatians"),
    make_pair (49, "ephesians"),
    make_pair (50, "philippians"),
    make_pair (51, "colossians"),
    make_pair (52, "1_thessalonians"),
    make_pair (53, "2_thessalonians"),
    make_pair (54, "1_timothy"),
    make_pair (55, "2_timothy"),
    make_pair (56, "titus"),
    make_pair (57, "philemon"),
    make_pair (58, "hebrews"),
    make_pair (59, "james"),
    make_pair (60, "1_peter"),
    make_pair (61, "2_peter"),
    make_pair (62, "1_john"),
    make_pair (63, "2_john"),
    make_pair (64, "3_john"),
    make_pair (65, "jude"),
    make_pair (66, "revelation")
  };
  return mapping [book];
}


string resource_external_convert_book_netbible (int book)
{
  // Map Bibledit books to net.bible.com books as used at the web service.
  map <int, string> mapping = {
    make_pair (1, "Genesis"),
    make_pair (2, "Exodus"),
    make_pair (3, "Leviticus"),
    make_pair (4, "Numbers"),
    make_pair (5, "Deuteronomy"),
    make_pair (6, "Joshua"),
    make_pair (7, "Judges"),
    make_pair (8, "Ruth"),
    make_pair (9, "1 Samuel"),
    make_pair (10, "2 Samuel"),
    make_pair (11, "1 Kings"),
    make_pair (12, "2 Kings"),
    make_pair (13, "1 Chronicles"),
    make_pair (14, "2 Chronicles"),
    make_pair (15, "Ezra"),
    make_pair (16, "Nehemiah"),
    make_pair (17, "Esther"),
    make_pair (18, "Job"),
    make_pair (19, "Psalms"),
    make_pair (20, "Proverbs"),
    make_pair (21, "Ecclesiastes"),
    make_pair (22, "Song of Solomon"),
    make_pair (23, "Isaiah"),
    make_pair (24, "Jeremiah"),
    make_pair (25, "Lamentations"),
    make_pair (26, "Ezekiel"),
    make_pair (27, "Daniel"),
    make_pair (28, "Hosea"),
    make_pair (29, "Joel"),
    make_pair (30, "Amos"),
    make_pair (31, "Obadiah"),
    make_pair (32, "Jonah"),
    make_pair (33, "Micah"),
    make_pair (34, "Nahum"),
    make_pair (35, "Habakkuk"),
    make_pair (36, "Zephaniah"),
    make_pair (37, "Haggai"),
    make_pair (38, "Zechariah"),
    make_pair (39, "Malachi"),
    make_pair (40, "Matthew"),
    make_pair (41, "Mark"),
    make_pair (42, "Luke"),
    make_pair (43, "John"),
    make_pair (44, "Acts"),
    make_pair (45, "Romans"),
    make_pair (46, "1 Corinthians"),
    make_pair (47, "2 Corinthians"),
    make_pair (48, "Galatians"),
    make_pair (49, "Ephesians"),
    make_pair (50, "Philippians"),
    make_pair (51, "Colossians"),
    make_pair (52, "1 Thessalonians"),
    make_pair (53, "2 Thessalonians"),
    make_pair (54, "1 Timothy"),
    make_pair (55, "2 Timothy"),
    make_pair (56, "Titus"),
    make_pair (57, "Philemon"),
    make_pair (58, "Hebrews"),
    make_pair (59, "James"),
    make_pair (60, "1 Peter"),
    make_pair (61, "2 Peter"),
    make_pair (62, "1 John"),
    make_pair (63, "2 John"),
    make_pair (64, "3 John"),
    make_pair (65, "Jude"),
    make_pair (66, "Revelation")
  };
  return mapping [book];
}


string resource_external_convert_book_blueletterbible (int book)
{
  // This array maps the the book identifiers from Bibledit-web
  // to the book names as used by the blueletterbible.org web service.
  map <int, string> mapping = {
    make_pair (1, "Gen"),
    make_pair (2, "Exd"),
    make_pair (3, "Lev"),
    make_pair (4, "Num"),
    make_pair (5, "Deu"),
    make_pair (6, "Jos"),
    make_pair (7, "Jdg"),
    make_pair (8, "Rth"),
    make_pair (9, "1Sa"),
    make_pair (10, "2Sa"),
    make_pair (11, "1Ki"),
    make_pair (12, "2Ki"),
    make_pair (13, "1Ch"),
    make_pair (14, "2Ch"),
    make_pair (15, "Ezr"),
    make_pair (16, "Neh"),
    make_pair (17, "Est"),
    make_pair (18, "Job"),
    make_pair (19, "Psa"),
    make_pair (20, "Pro"),
    make_pair (21, "Ecc"),
    make_pair (22, "Sgs"),
    make_pair (23, "Isa"),
    make_pair (24, "Jer"),
    make_pair (25, "Lam"),
    make_pair (26, "Eze"),
    make_pair (27, "Dan"),
    make_pair (28, "Hsa"),
    make_pair (29, "Joe"),
    make_pair (30, "Amo"),
    make_pair (31, "Oba"),
    make_pair (32, "Jon"),
    make_pair (33, "Mic"),
    make_pair (34, "Nah"),
    make_pair (35, "Hab"),
    make_pair (36, "Zep"),
    make_pair (37, "Hag"),
    make_pair (38, "Zec"),
    make_pair (39, "Mal"),
    make_pair (40, "Mat"),
    make_pair (41, "Mar"),
    make_pair (42, "Luk"),
    make_pair (43, "Jhn"),
    make_pair (44, "Act"),
    make_pair (45, "Rom"),
    make_pair (46, "1Cr"),
    make_pair (47, "2Cr"),
    make_pair (48, "Gal"),
    make_pair (49, "Eph"),
    make_pair (50, "Phl"),
    make_pair (51, "Col"),
    make_pair (52, "1Th"),
    make_pair (53, "2Th"),
    make_pair (54, "1Ti"),
    make_pair (55, "2Ti"),
    make_pair (56, "Tts"),
    make_pair (57, "Phm"),
    make_pair (58, "Hbr"),
    make_pair (59, "Jam"),
    make_pair (60, "1Pe"),
    make_pair (61, "2Pe"),
    make_pair (62, "1Jo"),
    make_pair (63, "2Jo"),
    make_pair (64, "3Jo"),
    make_pair (65, "Jud"),
    make_pair (66, "Rev")
  };
  return mapping [book];
}


string resource_external_convert_book_bibleserver (int book)
{
  // On bibleserver.com, Genesis is "1.Mose", Exodus is "2.Mose", and so on.
  map <int, string> mapping = {
    make_pair (1, "1.Mose"),
    make_pair (2, "2.Mose"),
    make_pair (3, "3.Mose"),
    make_pair (4, "4.Mose"),
    make_pair (5, "5.Mose"),
    make_pair (6, "Josua"),
    make_pair (7, "Richter"),
    make_pair (8, "Rut"),
    make_pair (9, "1.Samuel"),
    make_pair (10, "2.Samuel"),
    make_pair (11, "1.Könige"),
    make_pair (12, "2.Könige"),
    make_pair (13, "1.Chronik"),
    make_pair (14, "2.Chronik"),
    make_pair (15, "Esra"),
    make_pair (16, "Nehemia"),
    make_pair (17, "Esther"),
    make_pair (18, "Hiob"),
    make_pair (19, "Psalm"),
    make_pair (20, "Sprüche"),
    make_pair (21, "Prediger"),
    make_pair (22, "Hoheslied"),
    make_pair (23, "Jesaja"),
    make_pair (24, "Jeremia"),
    make_pair (25, "Klagelieder"),
    make_pair (26, "Hesekiel"),
    make_pair (27, "Daniel"),
    make_pair (28, "Hosea"),
    make_pair (29, "Joel"),
    make_pair (30, "Amos"),
    make_pair (31, "Obadja"),
    make_pair (32, "Jona"),
    make_pair (33, "Micha"),
    make_pair (34, "Nahum"),
    make_pair (35, "Habakuk"),
    make_pair (36, "Zefanja"),
    make_pair (37, "Haggai"),
    make_pair (38, "Sacharja"),
    make_pair (39, "Maleachi"),
    make_pair (40, "Matthäus"),
    make_pair (41, "Markus"),
    make_pair (42, "Lukas"),
    make_pair (43, "Johannes"),
    make_pair (44, "Apostelgeschichte"),
    make_pair (45, "Römer"),
    make_pair (46, "1.Korinther"),
    make_pair (47, "2.Korinther"),
    make_pair (48, "Galater"),
    make_pair (49, "Epheser"),
    make_pair (50, "Philipper"),
    make_pair (51, "Kolosser"),
    make_pair (52, "1.Thessalonicher"),
    make_pair (53, "2.Thessalonicher"),
    make_pair (54, "1.Timotheus"),
    make_pair (55, "2.Timotheus"),
    make_pair (56, "Titus"),
    make_pair (57, "Philemon"),
    make_pair (58, "Hebräer"),
    make_pair (59, "Jakobus"),
    make_pair (60, "1.Petrus"),
    make_pair (61, "2.Petrus"),
    make_pair (62, "1.Johannes"),
    make_pair (63, "2.Johannes"),
    make_pair (64, "3.Johannes"),
    make_pair (65, "Judas"),
    make_pair (66, "Offenbarung")
  };
  return mapping [book];
}


string resource_external_studylight_code ()
{
  string error;
  string html = resource_logic_web_cache_get ("http://www.studylight.org/commentaries", error);
  if (!error.empty ()) return error;
  map <string, string> commentaries;
  vector <string> lines = filter_string_explode (html, '\n');
  for (auto & line : lines) {
    if (line.find ("<!--") != string::npos) continue;
    if (line.find ("<h2>") == string::npos) continue;
    line = filter_string_str_replace ("<br />", " ", line);
    size_t pos = line.find ("href");
    if (pos == string::npos) continue;
    line.erase (0, pos + 6);
    string href = line.substr (0, 3);
    pos = line.find (">");
    line.erase (0, pos + 1);
    pos = line.find ("<");
    string title = line.substr (0, pos);
    commentaries [href] = title;
  }
  
  string function_1 = "resource_external_studylight_commentary_get_";
  string function_2 = " (int book, int chapter, int verse)";
  vector <string> code;
  code.push_back ("// Start of Studylight commentaries forward declarations");
  for (auto element : commentaries) {
    string line = "string " + function_1 + element.first + function_2 + ";";
    code.push_back (line);
  }
  code.push_back ("// End of Studylight commentaries forward declarations");
  code.push_back ("");
  code.push_back ("");
  code.push_back ("// Start of Studylight commentaries resources table entries");
  for (auto element : commentaries) {
    string href = element.first;
    string title = element.second;
    title = filter_string_str_replace ("\"", "\\\"", title);
    string line = "{ \"" + title + "\", \"English\", \"English\", & " + function_1 + href + " },";
    code.push_back (line);
  }
  code.push_back ("// End of Studylight commentaries resources table entries");
  code.push_back ("");
  code.push_back ("");
  code.push_back ("// Start of Studylight commentaries implementation functions");
  for (auto element : commentaries) {
    string href = element.first;
    string title = element.second;
    code.push_back ("// " + title);
    code.push_back ("string " + function_1 + href + function_2);
    code.push_back ("{");
    code.push_back ("  // The directory on studylight.org");
    code.push_back ("  string directory = \"" + href + "\";");
    code.push_back ("  return resource_external_studylight_processor (directory, book, chapter, verse);");
    code.push_back ("}");
  }
  code.push_back ("// End of Studylight commentaries implementation functions");

  return filter_string_implode (code, "\n");
}


const char * resource_external_net_bible_name ()
{
  return "NET Bible";
}


const char * resource_external_biblehub_interlinear_name ()
{
  return "Biblehub Interlinear";
}
