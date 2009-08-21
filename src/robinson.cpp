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


#include "robinson.h"


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
  bool remove_code = true;
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
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


void robinson_define_parsing_number (ustring& definition, ustring& parsing)
// Parse the number.
{
  ustring number = parsing.substr (0, 1);
  bool remove_code = true;
  if (number == "S") {
    definition.append (" singular");
  } else if (number == "P") {
    definition.append (" plural");
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


void robinson_define_parsing_gender (ustring& definition, ustring& parsing)
// Parse the gender.
{
  ustring number = parsing.substr (0, 1);
  bool remove_code = true;
  if (number == "M") {
    definition.append (" masculine");
  } else if (number == "F") {
    definition.append (" feminine");
  } else if (number == "N") {
    definition.append (" neuter");
  } else {
    remove_code = false;
  }
  if (remove_code) {
    parsing.erase (0, 1);
  }
}


void robinson_define_parsing_suffix (ustring& definition, ustring& parsing)
// Parse the suffix.
{
  if (parsing.substr (0, 1) == "-") {
    parsing.erase (0, 1);
  }
  if (parsing == "S") {
    definition.append (" superlative");
  } else if (parsing == "C") {
    definition.append (" comparative");
  } else if (parsing == "ABB") {
    definition.append (" Abbreviated form");
  } else if (parsing == "I") {
    definition.append (" interrogative");
  } else if (parsing == "N") {
    definition.append (" negative");
  } else if (parsing == "K") {
    definition.append (" \"Kai\" (conjunction), second person personal pronoun \"su\", or neuter definite article \"to\" merged by crasis with a second word; declension is that of the second word");
  } else if (parsing == "ATT") {
    definition.append (" Attic Greek form");
  } else {
    // A fall-back for a suffix that has not been defined in the key.
    if (!parsing.empty()) {
      definition.append (" suffix \"" + parsing + "\"");
    }
  }
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


ustring robinson_define_parsing (ustring parsing)
// Gives a definition of the Robinson Greek parsings.
{
  ustring definition;
  
  // UNDECLINED FORMS:
  if (parsing == "ADV") {
    definition = "adverb or adverb and particle combined"; 
  } else if (parsing == "CONJ") {
    definition = "conjunction or conjunctive particle";
  } else if (parsing == "COND") {
    definition = "conditional particle or conjunction";
  } else if (parsing == "PRT") {
    definition = "particle, disjunctive particle";
  } else if (parsing == "PREP") {
    definition = "preposition";
  } else if (parsing == "INJ") {
    definition = "interjection";
  } else if (parsing == "ARAM") {
    definition = "Aramaic transliterated word (indeclinable)";
  } else if (parsing == "HEB") {
    definition = "Hebrew transliterated word (indeclinable)";
  } else if (parsing == "N-PRI") {
    definition = "indeclinable proper noun";
  } else if (parsing == "A-NUI") {
    definition = "indeclinable numeral (adjective)";
  } else if (parsing == "N-LI") {
    definition = "indeclinable letter (noun)";
  } else if (parsing == "N-OI") {
    definition = "indeclinable noun of other type";
  } else {
    
    // DECLINED FORMS:
    ustring prefix = parsing.substr (0, 1);
    parsing.erase (0, 2);
    
    if (prefix == "N") {
      definition = "noun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "A") {
      definition = "adjective";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "R") {
      definition = "relative pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "C") {
      definition = "reciprocal pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "D") {
      definition = "demonstrative pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "T") {
      definition = "definite article";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "K") {
      definition = "correlative pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "I") {
      definition = "interrogative pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "X") {
      definition = "indefinite pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "Q") {
      definition = "correlative or interrogative pronoun";
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "F") {
      definition = "reflexive pronoun";
      // It has person 1,2,3 added, e.g. F-3ASF. 
      robinson_define_parsing_person (definition, parsing);
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "S") {
      definition = "possessive pronoun";
      // It has person 1,2,3 added, e.g. S-1DPM. 
      robinson_define_parsing_person (definition, parsing);
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    } else if (prefix == "P") {
      definition = "personal pronoun";
      // It may have person 1,2,3 added, e.g. P-GSM or P-1GS.
      // Note: 1st and 2nd personal pronouns have no gender.
      robinson_define_parsing_person (definition, parsing);
      robinson_define_parsing_case (definition, parsing);
      robinson_define_parsing_number (definition, parsing);
      robinson_define_parsing_gender (definition, parsing);
      robinson_define_parsing_suffix (definition, parsing);
    }
    
    // VERBS:
    
    else if (prefix == "V") {
      definition = "verb";
      // After the "V", remove the hyphen.
      if (parsing.substr (0, 1) == "-") {
        parsing.erase (0, 1);
      }
      // The verb always has tense voice mood, such as in, e.g., "V-2ADN".
      robinson_define_parsing_tense (definition, parsing);
      robinson_define_parsing_voice (definition, parsing);
      robinson_define_parsing_mood (definition, parsing);
      // Next element is a hyphen, remove it.
      if (parsing.substr (0, 1) == "-") {
        parsing.erase (0, 1);
      }
      // Next we can have two options. 
      // First we try person and number.
      if (robinson_define_parsing_person (definition, parsing)) {
        robinson_define_parsing_number (definition, parsing);
      } else {
        // Else it is case, number and gender.
        robinson_define_parsing_case (definition, parsing);
        robinson_define_parsing_number (definition, parsing);
        robinson_define_parsing_gender (definition, parsing);
      }
      // Finally parse the few cases that have a suffix.
      robinson_define_parsing_suffix (definition, parsing);
    }
    
    // Can't describe the morphology.
    else {
      definition = "Prefix " + prefix + " not recognized";
    }

  }
  return definition;
}


