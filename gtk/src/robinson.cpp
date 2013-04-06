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


bool robinson_define_parsing_person (ustring& definition, ustring& parsing)
// This looks in the "parsing" whether the person is given. 
// If so, it adds the description to the "definition" and removes the relevant code from the "parsing".
// It returns true if a person was found.
{
  ustring person = parsing.substr (0, 1);
  bool person_found = true;
  if (person == "1") {
    definition.append (" first person");
  } else if (person == "2") {
    definition.append (" second person");
  } else if (person == "3") {
    definition.append (" third person");
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
    definition.append (" nominative");
  } else if (case_code == "V") {
    definition.append (" vocative");
  } else if (case_code == "G") {
    definition.append (" genitive");
  } else if (case_code == "D") {
    definition.append (" dative");
  } else if (case_code == "A") {
    definition.append (" accusative");
  } else {
    definition.append (" case \"" + case_code + "\"");
  }
}


void robinson_define_parsing_number (ustring& definition, ustring& parsing)
// Parse the number.
{
  ustring number = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (number == "S") {
    definition.append (" singular");
  } else if (number == "P") {
    definition.append (" plural");
  } else {
    if (!number.empty()) {
      definition.append (" number \"" + number + "\"");
    }
  }
}


void robinson_define_parsing_gender (ustring& definition, ustring& parsing)
// Parse the gender.
{
  ustring gender = parsing.substr (0, 1);
  parsing.erase (0, 1);
  if (gender == "M") {
    definition.append (" masculine");
  } else if (gender == "F") {
    definition.append (" feminine");
  } else if (gender == "N") {
    definition.append (" neuter");
  } else {
    if (!gender.empty()) {
      definition.append (" gender \"" + gender + "\"");
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
    definition.append (" superlative");
  } 
  else if (parsing == "C") {
    definition.append (" comparative");
  } 
  else if (parsing == "ABB") {
    definition.append (" abbreviated");
  } 
  else if (parsing == "I") {
    definition.append (" interrogative");
  } 
  else if (parsing == "N") {
    definition.append (" negative");
  }
  else if (parsing == "K") {
    definition.append (" merged by crasis with a second word; declension is that of the second word");
  } 
  else if (parsing == "ATT") {
    definition.append (" Attic Greek form");
  } 
  else if (parsing == "LI") {
    definition.append (" letter indeclinable");
  } 
  else if (parsing == "PRI") {
    definition.append (" proper indeclinable");
  } 
  else if (parsing == "OI") {
    definition.append (" other indeclinable");
  }
  else if (parsing == "NUI") {
    definition.append (" numeral indeclinable");
  }
  else if (parsing == "AP") {
    definition.append (" apocopated form");
  }
  else if (parsing == "A") {
    definition.append (" Aeolic");
  }
  else if (parsing == "M") {
    definition.append (" middle significance");
  }
  else {
    if (!silent) {
      if (!parsing.empty()) {
        definition.append (" unknown suffix \"" + parsing + "\"");
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
    definition.append (" second");
    parsing.erase (0, 1);
  }
  ustring tense = parsing.substr (0, 1);  
  bool remove_code = true;
  if (tense == "P") {
    definition.append (" present");
  } else if (tense == "I") {
    definition.append (" imperfect");
  } else if (tense == "F") {
    definition.append (" future");
  } else if (tense == "A") {
    definition.append (" aorist");
  } else if (tense == "R") {
    definition.append (" perfect");
  } else if (tense == "L") {
    definition.append (" pluperfect");
  } else if (tense == "X") {
    definition.append (" no tense stated (adverbial imperative)");
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
    definition.append (" active");
  } else if (voice == "M") {
    definition.append (" middle");
  } else if (voice == "P") {
    definition.append (" passive");
  } else if (voice == "E") {
    definition.append (" middle or passive");
  } else if (voice == "D") {
    definition.append (" middle deponent");
  } else if (voice == "O") {
    definition.append (" passive deponent");
  } else if (voice == "N") {
    definition.append (" middle or passive deponent");
  } else if (voice == "X") {
    definition.append (" no voice stated");
  } else if (voice == "Q") {
    definition.append (" impersonal active");
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
    definition.append (" indicative");
  } else if (mood == "S") {
    definition.append (" subjunctive");
  } else if (mood == "O") {
    definition.append (" optative");
  } else if (mood == "M") {
    definition.append (" imperative");
  } else if (mood == "N") {
    definition.append (" infinitive");
  } else if (mood == "P") {
    definition.append (" participle");
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
    definition = "adjective";
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
    definition = "adverb or adverb and particle combined"; 
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  }   

  if (parsing == "ARAM") {
    definition = "indeclinable Aramaic transliterated word";
    return true;
  } 

  if (bit0 == "C") {
    definition = "reciprocal pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "COND") {
    definition = "conditional particle or conjunction";
    if (bit1 == "C") {
      bit1 = "K";
    }
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  } 

  if (bit0 == "CONJ") {
    definition = "conjunction or conjunctive particle";
    return true;
  } 

  if (bit0 == "D") {
    definition = "demonstrative pronoun";
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
    definition = "reflexive pronoun";
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
    definition = "indeclinable Hebrew transliterated word";
    return true;
  } 
  
  if (bit0 == "I") {
    definition = "interrogative pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "INJ") {
    definition = "interjection";
    return true;
  } 

  if (bit0 == "K") {
    definition = "correlative pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "N") {
    definition = "noun";
    if (!robinson_define_parsing_suffix (definition, bit1, true)) {
      robinson_define_parsing_case (definition, bit1);
      robinson_define_parsing_number (definition, bit1);
      robinson_define_parsing_gender (definition, bit1);
      robinson_define_parsing_suffix (definition, bit2, false);
    }
    return true;
  }

  if (bit0 == "P") {
    definition = "personal pronoun";
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
    definition = "preposition";
    return true;
  } 

  if (bit0 == "PRT") {
    definition = "particle, disjunctive particle";
    robinson_define_parsing_suffix (definition, bit1, false);
    return true;
  } 

  if (bit0 == "Q") {
    definition = "correlative or interrogative pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "R") {
    definition = "relative pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "S") {
    definition = "possessive pronoun";
    // It has person 1,2,3 added, e.g. S-1DPM. 
    robinson_define_parsing_person (definition, bit1);
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  if (bit0 == "T") {
    definition = "definite article";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }

  if (bit0 == "V") {
    definition = "verb";
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
    definition = "indefinite pronoun";
    robinson_define_parsing_case (definition, bit1);
    robinson_define_parsing_number (definition, bit1);
    robinson_define_parsing_gender (definition, bit1);
    robinson_define_parsing_suffix (definition, bit2, false);
    return true;
  }
  
  // Can't describe the parsing.
  return false;
}


