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


#include "greek.h"


ustring greek_case_and_accent_fold (const ustring& phrase)
// Takes a Greek phrase and folds the case and any accents.
{
  ustring casefold;
  for (unsigned int i = 0; i < phrase.length(); i++) {
    ustring c = phrase.substr (i, 1).normalize();
    // There are bugs when comparing using a ustring, therefore we use a gunichar.
    gunichar uc = g_utf8_get_char (c.c_str());
    if ((uc == g_utf8_get_char ("ἀ")) || (uc == g_utf8_get_char ("ἁ")) || (uc == g_utf8_get_char ("ἀ")) || (uc == g_utf8_get_char ("ἁ")) || (uc == g_utf8_get_char ("ἄ")) || (uc == g_utf8_get_char ("ἅ")) || (uc == g_utf8_get_char ("ἆ")) || (uc == g_utf8_get_char ("Ἀ")) || (uc == g_utf8_get_char ("Ἄ")) || (uc == g_utf8_get_char ("ά")) || (uc == g_utf8_get_char ("Α")) || (uc == g_utf8_get_char ("ᾳ")) || (uc == g_utf8_get_char ("ᾴ")) || (uc == g_utf8_get_char ("ᾄ")) || (uc == g_utf8_get_char ("ᾅ")) || (uc == g_utf8_get_char ("ᾷ")) || (uc == g_utf8_get_char ("ὰ")) || (uc == g_utf8_get_char ("ᾶ")) || (uc == g_utf8_get_char ("ά"))) {
      casefold.append ("α");
    } else if (uc == g_utf8_get_char ("Β")) {
      casefold.append ("β");
    } else if (uc == g_utf8_get_char ("Γ")) {
      casefold.append ("γ");
    } else if (uc == g_utf8_get_char ("Δ")) {
      casefold.append ("δ");
    } else if ((uc == g_utf8_get_char ("Ε")) || (uc == g_utf8_get_char ("ἐ")) || (uc == g_utf8_get_char ("ἑ")) || (uc == g_utf8_get_char ("ἔ")) || (uc == g_utf8_get_char ("ἕ")) || (uc == g_utf8_get_char ("Ἐ")) || (uc == g_utf8_get_char ("Ἑ")) || (uc == g_utf8_get_char ("Ἔ")) || (uc == g_utf8_get_char ("Ἕ")) || (uc == g_utf8_get_char ("ὲ")) || (uc == g_utf8_get_char ("έ")) || (uc == g_utf8_get_char ("έ"))) {
      casefold.append ("ε");
    } else if (uc == g_utf8_get_char ("Ζ")) {
      casefold.append ("ζ");
    } else if ((uc == g_utf8_get_char ("ἠ")) || (uc == g_utf8_get_char ("ἡ")) || (uc == g_utf8_get_char ("ἤ")) || (uc == g_utf8_get_char ("ἥ")) || (uc == g_utf8_get_char ("ἦ")) || (uc == g_utf8_get_char ("ἧ")) || (uc == g_utf8_get_char ("Ἠ")) || (uc == g_utf8_get_char ("Ἡ")) || (uc == g_utf8_get_char ("Ἤ")) || (uc == g_utf8_get_char ("ή")) || (uc == g_utf8_get_char ("η")) || (uc == g_utf8_get_char ("ῄ")) || (uc == g_utf8_get_char ("ὴ")) || (uc == g_utf8_get_char ("ῆ")) || (uc == g_utf8_get_char ("ῇ")) || (uc == g_utf8_get_char ("ή"))) {
      casefold.append ("η");
    } else if (uc == g_utf8_get_char ("Θ")) {
      casefold.append ("θ");
    } else if ((uc == g_utf8_get_char ("ἰ")) || (uc == g_utf8_get_char ("ἱ")) || (uc == g_utf8_get_char ("ἴ")) || (uc == g_utf8_get_char ("ἵ")) || (uc == g_utf8_get_char ("ἶ")) || (uc == g_utf8_get_char ("ἷ")) || (uc == g_utf8_get_char ("Ἰ")) || (uc == g_utf8_get_char ("Ἱ")) || (uc == g_utf8_get_char ("ὶ")) || (uc == g_utf8_get_char ("ί")) || (uc == g_utf8_get_char ("ΐ")) || (uc == g_utf8_get_char ("ϊ")) || (uc == g_utf8_get_char ("ῖ")) || (uc == g_utf8_get_char ("ί")) || (uc == g_utf8_get_char ("ΐ"))) {
      casefold.append ("ι");
    } else if (uc == g_utf8_get_char ("Κ")) {
      casefold.append ("κ");
    } else if (uc == g_utf8_get_char ("Λ")) {
      casefold.append ("λ");
    } else if (uc == g_utf8_get_char ("Μ")) {
      casefold.append ("μ");
    } else if (uc == g_utf8_get_char ("Ν")) {
      casefold.append ("ν");
    } else if (uc == g_utf8_get_char ("Ξ")) {
      casefold.append ("ξ");
    } else if ((uc == g_utf8_get_char ("Ο")) || (uc == g_utf8_get_char ("ὀ")) || (uc == g_utf8_get_char ("ὁ")) || (uc == g_utf8_get_char ("ὄ")) || (uc == g_utf8_get_char ("ὅ")) || (uc == g_utf8_get_char ("Ὀ")) || (uc == g_utf8_get_char ("ό")) || (uc == g_utf8_get_char ("ό"))) {
      casefold.append ("ο");
    } else if (uc == g_utf8_get_char ("Π")) {
      casefold.append ("π");
    } else if ((uc == g_utf8_get_char ("Ρ")) || (uc == g_utf8_get_char ("ῤ")) || (uc == g_utf8_get_char ("ῥ")) || (uc == g_utf8_get_char ("Ῥ"))) {
      casefold.append ("ρ");
    } else if ((uc == g_utf8_get_char ("ϛ")) || (uc == g_utf8_get_char ("ς")) || (uc == g_utf8_get_char ("Σ"))) {
      casefold.append ("σ");
    } else if (uc == g_utf8_get_char ("Τ")) {
      casefold.append ("τ");
    } else if ((uc == g_utf8_get_char ("ὐ")) || (uc == g_utf8_get_char ("ὑ")) || (uc == g_utf8_get_char ("ὔ")) || (uc == g_utf8_get_char ("ὕ")) || (uc == g_utf8_get_char ("ὖ")) || (uc == g_utf8_get_char ("ὗ")) || (uc == g_utf8_get_char ("Ὑ")) || (uc == g_utf8_get_char ("ύ")) || (uc == g_utf8_get_char ("ΰ")) || (uc == g_utf8_get_char ("ϋ")) || (uc == g_utf8_get_char ("ῦ")) || (uc == g_utf8_get_char ("ύ")) || (uc == g_utf8_get_char ("ΰ"))) {
      casefold.append ("υ");
    } else if (uc == g_utf8_get_char ("Φ")) {
      casefold.append ("φ");
    } else if (uc == g_utf8_get_char ("Χ")) {
      casefold.append ("χ");
    } else if (uc == g_utf8_get_char ("Ψ")) {
      casefold.append ("ψ");
    } else if ((uc == g_utf8_get_char ("ὠ")) || (uc == g_utf8_get_char ("ὡ")) || (uc == g_utf8_get_char ("ὢ")) || (uc == g_utf8_get_char ("ὤ")) || (uc == g_utf8_get_char ("ὥ")) || (uc == g_utf8_get_char ("ὦ")) || (uc == g_utf8_get_char ("ὧ")) || (uc == g_utf8_get_char ("Ὠ")) || (uc == g_utf8_get_char ("Ὡ")) || (uc == g_utf8_get_char ("ῶ")) || (uc == g_utf8_get_char ("ᾠ")) || (uc == g_utf8_get_char ("ῷ")) || (uc == g_utf8_get_char ("ώ")) || (uc == g_utf8_get_char ("Ω")) || (uc == g_utf8_get_char ("ῳ")) || (uc == g_utf8_get_char ("ῴ")) || (uc == g_utf8_get_char ("ώ"))) {
      casefold.append ("ω");
    } else {
      casefold.append (c);
    }
  }
  return casefold;
}
