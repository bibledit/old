/*
** Copyright (©) 2003-2009 Teus Benschop.
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 3 of the License, or
** (at your option) any later version.
**  
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**  
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**  
*/

#include "languages.h"
#include "utilities.h"
#include "gwrappers.h"
#include <sqlite3.h>
#include "directories.h"
#include "books.h"
#include "bible.h"

/*
Following information was taken from the BibleTime source, file clanguagemgr.cpp.

	Chris explained in an eMail how language codes are build:
	
	Preference order for locale codes are:
	
	ISO 639-1
	ISO 639-2
	Ethnologue (http:www.ethnologue.org/)
	
	We intend to always follow OSIS locale conventions, which state that
	Ethnologue codes will be encoded in the format x-E-??? where ???
	represents the 3-letter Ethnologue code in capital letters (though
	capitalization really doesn't matter here).
	
	Some older modules maintain a former format for Ethnolgoue codes of
	xx-???, including the AleWiesler module.
	
	--Chris
*/

#define LANGUAGE_COUNT 380

const char *languages[LANGUAGE_COUNT] = {
  "aa", "Afar",
  "ab", "Abkhazian",
  "ae", "Avestan",
  "af", "Afrikaans",
  "am", "Amharic",
  "ang", "English, Old (ca.450-1100)",
  "ar", "Arabic",
  "as", "Assamese",
  "az", "Azerbaijani",
  "ba", "Bashkir",
  "be", "Belarusian",
  "bg", "Bulgarian",
  "bh", "Bihari",
  "bi", "Bislama",
  "bn", "Bengali",
  "bo", "Tibetan",
  "br", "Breton",
  "bs", "Bosnian",
  "ca", "Catalan",
  "ce", "Chechen",
  "ceb", "Cebuano",
  "ch", "Chamorro",
  "co", "Corsican",
  "cop", "Coptic",
  "cs", "Czech",
  "cu", "Church Slavic",
  "cv", "Chuvash",
  "cy", "Welsh",
  "da", "Danish",
  "de", "German",
  "dz", "Dzongkha",
  "el", "Greek, Modern (1453-)",
  "en", "English", "English" "en_US", "American English",
  "enm", "English, Middle (1100-1500)",
  "eo", "Esperanto",
  "es", "Spanish",
  "et", "Estonian",
  "eu", "Basque",
  "fa", "Persian",
  "fi", "Finnish",
  "fj", "Fijian",
  "fo", "Faroese",
  "fr", "French",
  "fy", "Frisian",
  "ga", "Irish",
  "gd", "Gaelic (Scots)",
  "gl", "Gallegan",
  "gn", "Guarani",
  "gn", "Gujarati",
  "got", "Gothic",
  "gv", "Manx",
  "grc", "Greek, Ancient (to 1453)",
  "he", "Hebrew",
  "hau", "Hausa",
  "haw", "Hawaiian",
  "hi", "Hindi",
  "ho", "Hiri Motu",
  "hr", "Croatian",
  "hu", "Hungarian",
  "hy", "Armenian",
  "hz", "Herero",
  "ia", "Interlingua",
  "id", "Indonesian",
  "ie", "Interlingue",
  "ik", "Inupiaq",
  "is", "Icelandic",
  "it", "Italian",
  "iu", "Inuktitut",
  "ja", "Japanese",
  "ka", "Georgian",
  "x-E-KAB", "Kabyle",
  "ki", "Kikuyu",
  "kj", "Kuanyama",
  "kk", "Kazakh",
  "kl", "Kalaallisut",
  "km", "Khmer",
  "kn", "Kannada",
  "ko", "Korean",
  "ks", "Kashmiri",
  "ku", "Kurdish",
  "kv", "Komi",
  "kw", "Cornish",
  "ky", "Kirghiz",
  "la", "Latin",
  "lb", "Letzeburgesch",
  "ln", "Lingala",
  "lo", "Lao",
  "lt", "Lithuanian",
  "lv", "Latvian",
  "mg", "Malagasy",
  "mh", "Marshall",
  "mi", "Maori",
  "mk", "Macedonian",
  "ml", "Malayalam",
  "mn", "Mongolian",
  "mo", "Moldavian",
  "mos", "More",
  "mr", "Marathi",
  "ms", "Malay",
  "mt", "Maltese",
  "my", "Burmese",
  "na", "Nauru",
  "nb", "Norwegian Bokmål",
  "nd", "Ndebele, North",
  "nds", "Low German; Low Saxon",
  "ne", "Nepali",
  "ng", "Ndonga",
  "nl", "Dutch",
  "nn", "Norwegian Nynorsk",
  "no", "Norwegian",
  "nr", "Ndebele, South",
  "nv", "Navajo",
  "ny", "Chichewa; Nyanja",
  "oc", "Occitan (post 1500); Provençal",
  "om", "Oromo",
  "or", "Oriya",
  "os", "Ossetian; Ossetic",
  "pa", "Panjabi",
  "pap", "Papiamento",
  "pi", "Pali",
  "pl", "Polish",
  "ps", "Pushto",
  "pt", "Portuguese",
  "pt_BR", "Brasilian Portuguese",
  "qu", "Quechua",
  "rm", "Raeto-Romance",
  "rn", "Rundi",
  "ro", "Romanian",
  "ru", "Russian",
  "rw", "Kinyarwanda",
  "sa", "Sanskrit",
  "sc", "Sardinian",
  "sco", "Scots",
  "sd", "Sindhi",
  "se", "Northern Sami",
  "sg", "Sango",
  "si", "Sinhalese",
  "sk", "Slovak",
  "sl", "Slovenian",
  "sm", "Samoan",
  "sn", "Shona",
  "so", "Somali",
  "sq", "Albanian",
  "sr", "Serbian",
  "ss", "Swati",
  "st", "Sotho, Southern",
  "su", "Sundanese",
  "sv", "Swedish",
  "sw", "Swahili",
  "syr", "Syriac",
  "ta", "Tamil",
  "te", "Telugu",
  "tg", "Tajik",
  "th", "Thai",
  "tk", "Turkmen",
  "tl", "Tagalog",
  "tn", "Tswana",
  "tr", "Turkish",
  "ts", "Tsonga",
  "tt", "Tatar",
  "tw", "Twi",
  "ty", "Tahitian",
  "ug", "Uighur",
  "uk", "Ukrainian",
  "ur", "Urdu",
  "uz", "Uzbek",
  "vi", "Vietnamese",
  "vo", "Volapük",
  "wo", "Wolof",
  "xh", "Xhosa",
  "x-E-BAR", "Bavarian",
  "x-E-DJE", "Zarma",
  "x-E-GSW", "Alemannisch",
  "x-E-HAT", "Haitian Creole French",
  "x-E-ITZ", "Itzá",
  "x-E-JIV", "Shuar",
  "x-E-KEK", "Kekchí",
  "x-E-KAB", "Kabyle",
  "x-E-LMO", "Lombard",
  "x-E-MKJ", "Macedonian",
  "x-E-PDG", "Tok Pisin",
  "x-E-PPK", "Uma",
  "x-E-RMY", "Romani, Vlax",
  "x-E-SAJ", "Sango",
  "x-E-SRN", "Sranan",
  "yi", "Yiddish",
  "za", "Zhuang",
  "zh", "Chinese",
  "zu", "Zulu"
};

vector < ustring > languages_get_sword()
{
  vector < ustring > language;
  for (unsigned int i = 0; i < LANGUAGE_COUNT; i++) {
    language.push_back(languages[++i]);
  }
  sort(language.begin(), language.end());
  return language;
}

ustring language_encode_sword(const ustring & language)
{
  ustring code;
  for (unsigned int i = 0; i < LANGUAGE_COUNT; i++) {
    if (language == languages[++i]) {
      code = languages[i - 1];
      break;
    }
  }
  return code;
}
