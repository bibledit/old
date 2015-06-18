/*
** Copyright (©) 2003-2013 Teus Benschop.
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


#include "robinson.h"
#include "utilities.h"
#include <glib/gi18n.h>

bool robinson_define_parsing_person (ustring& definition, ustring& parsing)
// This looks in the "parsing" whether the person is given. 
// If so, it adds the description to the "definition" and removes the relevant code from the "parsing".
// It returns true if a person was found.
{
  ustring person = parsing.substr (0, 1);
  bool person_found = true;
  if (person == "1") {
    definition.append (_(" first person"));
  } else if (person == "2") {
    definition.append (_(" second person"));
  } else if (person == "3") {
    definition.append (_(" third person"));
  } else {
    person_found = false;
  }
  if (person_found) {
    parsing.erase (0, 1);
  }
  return person_found;
}


void robinson_define_parsing_case (ustring& definition, ustring& parsing)
// Parse the case.
{
  ustring case_code = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (case_code == "N") {
    definition.append (_(" nominative"));
  } else if (case_code == "V") {
    definition.append (_(" vocative"));
  } else if (case_code == "G") {
    definition.append (_(" genitive"));
  } else if (case_code == "D") {
    definition.append (_(" dative"));
  } else if (case_code == "A") {
    definition.append (_(" accusative"));
  } else {
    definition.append (_(" case \"") + case_code + "\"");
  }
}


void robinson_define_parsing_number (ustring& definition, ustring& parsing)
// Parse the number.
{
  ustring number = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (number == "S") {
    definition.append (_(" singular"));
  } else if (number == "P") {
    definition.append (_(" plural"));
  } else {
    if (!number.empty()) {
      definition.append (_(" number \"") + number + "\"");
    }
  }
}


void robinson_define_parsing_gender (ustring& definition, ustring& parsing)
// Parse the gender.
{
  ustring gender = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (gender == "M") {
    definition.append (_(" masculine"));
  } else if (gender == "F") {
    definition.append (_(" feminine"));
  } else if (gender == "N") {
    definition.append (_(" neuter"));
  } else {
    if (!gender.empty()) {
      definition.append (_(" gender \"") + gender + "\"");
    }
  }
}


bool robinson_define_parsing_suffix (ustring& definition, ustring& parsing, bool silent)
// Parse the suffix.
// Returns true if it interpreted the parsing.
{
  /*
Between the 2000 version of  and the , he changed this. 
In the 2000 Maurice Robinson's coding manual version, there were two interpretations of a final -C (comparative or crasis).
In the 2004 version -C should always indicate comparative and -K should indicate crasis.
There may be old content that uses -C for crasis, but at the time of writing this Bibledit does not use it.
  */
  bool known_parsing = true;
  if (parsing == "S") {
    definition.append (_(" superlative"));
  } 
  else if (parsing == "C") {
    definition.append (_(" comparative"));
  } 
  else if (parsing == "ABB") {
    definition.append (_(" abbreviated"));
  } 
  else if (parsing == "I") {
    definition.append (_(" interrogative"));
  } 
  else if (parsing == "N") {
    definition.append (_(" negative"));
  }
  else if (parsing == "K") {
    definition.append (_(" merged by crasis with a second word; declension is that of the second word"));
  } 
  else if (parsing == "ATT") {
    definition.append (_(" Attic Greek form"));
  } 
  else if (parsing == "LI") {
    definition.append (_(" letter indeclinable"));
  } 
  else if (parsing == "PRI") {
    definition.append (_(" proper indeclinable"));
  } 
  else if (parsing == "OI") {
    definition.append (_(" other indeclinable"));
  }
  else if (parsing == "NUI") {
    definition.append (_(" numeral indeclinable"));
  }
  else if (parsing == "AP") {
    definition.append (_(" apocopated form"));
  }
  else if (parsing == "A") {
    definition.append (_(" Aeolic"));
  }
  else if (parsing == "M") {
    definition.append (_(" middle significance"));
  }
  else {
    if (!silent) {
      if (!parsing.empty()) {
        definition.append (_(" unknown suffix \"") + parsing + "\"");
      }
    }
    known_parsing = false;
  }
  return known_parsing;
}


void robinson_define_parsing_tense (ustring& definition, ustring& parsing)
// Parse the tense of verbs.
{
  ustring number = parsing.substr (0, 1);
  if (number == "2") {
    definition.append (_(" second"));
    parsing.erase (0, 1);
  }
  ustring tense = parsing.substr (0, 1);  
  bool remove_code = true;
  if (tense == "P") {
    definition.append (_(" present"));
  } else if (tense == "I") {
    definition.append (_(" imperfect"));
  } else if (tense == "F") {
    definition.append (_(" future"));
  } else if (tense == "A") {
    definition.append (_(" aorist"));
  } else if (tense == "R") {
    definition.append (_(" perfect"));
  } else if (tense == "L") {
    definition.append (_(" pluperfect"));
  } else if (tense == "X") {
    definition.append (_(" no tense stated (adverbial imperative)"));
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


void robinson_define_parsing_voice (ustring& definition, ustring& parsing)
// Parse the voice of verbs.
{
  ustring voice = parsing.substr (0, 1);  
  bool remove_code = true;
  if (voice == "A") {
    definition.append (_(" active"));
  } else if (voice == "M") {
    definition.append (_(" middle"));
  } else if (voice == "P") {
    definition.append (_(" passive"));
  } else if (voice == "E") {
    definition.append (_(" middle or passive"));
  } else if (voice == "D") {
    definition.append (_(" middle deponent"));
  } else if (voice == "O") {
    definition.append (_(" passive deponent"));
  } else if (voice == "N") {
    definition.append (_(" middle or passive deponent"));
  } else if (voice == "X") {
    definition.append (_(" no voice stated"));
  } else if (voice == "Q") {
    definition.append (_(" impersonal active"));
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


void robinson_define_parsing_mood (ustring& definition, ustring& parsing)
// Parse the mood of verbs.
{
  ustring mood = parsing.substr (0, 1);  
  bool remove_code = true;
  if (mood == "I") {
    definition.append (_(" indicative"));
  } else if (mood == "S") {
    definition.append (_(" subjunctive"));
  } else if (mood == "O") {
    definition.append (_(" optative"));
  } else if (mood == "M") {
    definition.append (_(" imperative"));
  } else if (mood == "N") {
    definition.append (_(" infinitive"));
  } else if (mood == "P") {
    definition.append (_(" participle"));
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


bool robinson_define_parsing (ustring parsing, ustring& definition)
// Tries to define a Robinson parsing.
// Returns true if it managed.
{
  // The parsing should be long enough.
  if (parsing.length() < 3)
    return false;
  // Cut the parsing on the hyphens.
  Parse parse (parsing, false, "-");

  // Get the various bits of the parsing.
  ustring bit0, bit1, bit2, bit3;
  if (parse.words.size() > 0)
    bit0 = parse.words[0];
  if (parse.words.size() > 1)
    bit1 = parse.words[1];
  if (parse.words.size() > 2)
    bit2 = parse.words[2];
  if (parse.words.size() > 3)
    bit3 = parse.words[3];
    
  if (bit0 == "A") {
    definition = _("adjective");
    if (!robinson_define_parsing_suffix (definition, bit1, true)) {
      robinson_define_parsing_case (definition, bit1);
      robinson_define_parsing_number (definition, bit1);
      robinson_define_parsing_gender (definition, bit1);
      robinson_define_parsing_suffix (definition, bit2, false);
    } else {
      robinson_define_parsing_suffix (definition, bit2, true);
    }
    return true;
  }
  
  if (bit0 == "ADV") {
    definition =_( "adverb or adverb and particle combined"); 
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  }   

  if (parsing == "ARAM") {
    definition = _("indeclinable Aramaic transliterated word");
    return true;
  } 

  if (bit0 == "C") {
    definition = _("reciprocal pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "COND") {
    definition = _("conditional particle or conjunction");
    if (bit1 == "C") {
      bit1 = "K";
    }
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  } 

  if (bit0 == "CONJ") {
    definition = _("conjunction or conjunctive particle");
    return true;
  } 

  if (bit0 == "D") {
    definition = _("demonstrative pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    if (bit2 == "C") {
      bit2 = "K";
    }
    robinson_define_parsing_suffix (definition, bit2, false);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "F") {
    definition = _("reflexive pronoun");
    // It has person 1,2,3 added, e.g. F-3ASF. 
    robinson_define_parsing_person (definition, bit1);
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    if (bit2 == "C") {
      bit2 = "K";
    }
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "HEB") {
    definition = _("indeclinable Hebrew transliterated word");
    return true;
  } 
  
  if (bit0 == "I") {
    definition = _("interrogative pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "INJ") {
    definition = _("interjection");
    return true;
  } 

  if (bit0 == "K") {
    definition = _("correlative pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "N") {
    definition = _("noun");
    if (!robinson_define_parsing_suffix (definition, bit1, true)) {
      robinson_define_parsing_case (definition, bit1);
      robinson_define_parsing_number (definition, bit1);
      robinson_define_parsing_gender (definition, bit1);
      robinson_define_parsing_suffix (definition, bit2, false);
    }
    return true;
  }

  if (bit0 == "P") {
    definition = _("personal pronoun");
    // It optionally can have person 1,2,3 added, e.g. 
    // P-GSM (personal pronoun genetive singular masculine)
    // P-1GS (personal pronoun first person genetive singular)
    // Note: 1st and 2nd personal pronouns have no gender.
    robinson_define_parsing_person (definition, bit1);
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    if (bit2 == "C") {
      bit2 = "K";
    }
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "PREP") {
    definition = _("preposition");
    return true;
  } 

  if (bit0 == "PRT") {
    definition = _("particle, disjunctive particle");
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  } 

  if (bit0 == "Q") {
    definition = _("correlative or interrogative pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "R") {
    definition = _("relative pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "S") {
    definition = _("possessive pronoun");
    // It has person 1,2,3 added, e.g. S-1DPM. 
    robinson_define_parsing_person (definition, bit1);
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "T") {
    definition = _("definite article");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "V") {
    definition = _("verb");
    // The verb always has tense voice mood, such as in, e.g., "V-2ADN".
    robinson_define_parsing_tense (definition, bit1);
    robinson_define_parsing_voice (definition, bit1);
    robinson_define_parsing_mood (definition, bit1);
    // Optionally we have only a suffix.
    if (robinson_define_parsing_suffix (definition, bit2, true)) {
      bit2.clear();
    }
    // Or we can have two options. 
    // First we try person and number.
    if (!bit2.empty()) {
      if (robinson_define_parsing_person (definition, bit2)) {
        robinson_define_parsing_number (definition, bit2);
      } else {
        // Else it is case, number and gender.
        robinson_define_parsing_case (definition, bit2);
        robinson_define_parsing_number (definition, bit2);
        robinson_define_parsing_gender (definition, bit2);
      }
    }
    // Finally parse the few cases that have a suffix.
    robinson_define_parsing_suffix (definition, bit3, false);
    return true;
  }
  
  if (bit0 == "X") {
    definition = _("indefinite pronoun");
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  // Can't describe the parsing.
  return false;
}


