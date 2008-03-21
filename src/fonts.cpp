/*
** Copyright (©) 2003-2008 Teus Benschop.
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
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
**  
*/


#include "fonts.h"
#include <fontconfig/fontconfig.h>
#include "gwrappers.h"
#include "dialogfont.h"
#include "gtkwrappers.h"
#include "settings.h"


#define HELVETICA "Helvetica"
#define TIMES "Times"
#define COURIER "Courier"


vector<ustring> font_get_families_on_system ();
void font_get_for_xep (vector<ustring>& family, vector<ustring>& path, 
                       vector<ustring>& metrics, vector<FontVariant>& variant);


int pango_fc_convert_slant_to_fc (PangoStyle pango_style)
// This function was copied from Pango.
{
  switch (pango_style) {
    case PANGO_STYLE_NORMAL:
      return FC_SLANT_ROMAN;
    case PANGO_STYLE_ITALIC:
      return FC_SLANT_ITALIC;
    case PANGO_STYLE_OBLIQUE:
      return FC_SLANT_OBLIQUE;
  }
  return FC_SLANT_ROMAN;
}


int pango_fc_convert_weight_to_fc (PangoWeight pango_weight)
// This function was copied from Pango and adapted.
{
  if (pango_weight <= PANGO_WEIGHT_NORMAL)
    return FC_WEIGHT_NORMAL;
  else
    return FC_WEIGHT_BOLD;
}


void font_fontinformation (PangoFontDescription *font_desc, 
                           ustring& ufile, ustring& ufamily, ustring& ustyle)
// Retrieves font information.
{
  // Get family name, slant and weight.
  ustring fcname = pango_font_description_get_family (font_desc);
  PangoWeight pangoweight = pango_font_description_get_weight (font_desc);
  int fcweight = pango_fc_convert_weight_to_fc (pangoweight);
  PangoStyle pangostyle = pango_font_description_get_style (font_desc);
  int fcslant = pango_fc_convert_slant_to_fc (pangostyle);
  // Create pattern and initialize it.
  FcPattern *pattern;
  pattern = FcPatternBuild (NULL,
                            FC_FAMILY, FcTypeString, (FcChar8 *) fcname.c_str(),
                            FC_WEIGHT, FcTypeInteger, fcweight,
                            FC_SLANT,  FcTypeInteger, fcslant,
                            NULL);
  FcConfigSubstitute (0, pattern, FcMatchPattern);
  FcDefaultSubstitute (pattern);
  // Create a font set.
  FcFontSet *fs;
  fs = FcFontSetCreate ();
  // Look up font by matching.
  FcPattern *match;
  FcResult result;
  match = FcFontMatch (0, pattern, &result);
  if (match)
    FcFontSetAdd (fs, match);
  // Print the font and free it.
  ustring fontpath;
  if (fs) {
    FcChar8 *file;
    FcChar8 *family;
    FcChar8 *style;
    FcPatternGetString (fs->fonts[0], FC_FILE, 0, &file);
    ufile = (char *) file; 
    FcPatternGetString (fs->fonts[0], FC_FAMILY, 0, &family);
    ufamily = (char *) family; 
    FcPatternGetString (fs->fonts[0], FC_STYLE, 0, &style);
    ustyle = (char *) style; 
    FcFontSetDestroy (fs);
  }
  // Free the pattern.
  if (pattern)
    FcPatternDestroy (pattern);
}


bool font_get_afm_file (ustring& file)
// Given a font filename, puts the corresponding .afm file in "file"
// and returns true if that file exists.
{
  size_t length = file.length();
  file.replace (length - 3, 3, "afm");
  if (g_file_test (file.c_str(), G_FILE_TEST_IS_REGULAR))
    return true;
  else 
    return false;
}


bool font_is_ttf_or_pfa_slash_pfb_with_afm (const ustring& filename)
// This verifies whether the font filename is a True Type font, or 
// whether it has a .pfm font metrics file.
{
  // Check whether .ttf file.
  bool ttf = g_str_has_suffix (filename.c_str(), ".ttf");
  // Check whether this filename has a corresponding .afm file.
  ustring path (filename);
  bool afm = font_get_afm_file (path);
  // Check whether it is a pfa file.
  bool pfa = g_str_has_suffix (filename.c_str(), ".pfa");
  // Check whether it is a pfb file.
  bool pfb = g_str_has_suffix (filename.c_str(), ".pfb");
  // Decide whether acceptable.
  if (ttf)
    return true;
  if (afm && pfa)
    return true;
  if (afm && pfb)
    return true;
  // Not acceptable
  return false;  
}


FontVariant font_variant_get (const ustring& style)
// Returns the variant.
{
  bool italic (style.find ("Italic") != string::npos);
  if (style.find ("Oblique") != string::npos)
    italic = true;
  bool bold = (style.find("Bold") != string::npos);
  if (italic && !bold) {
    return fvItalic;
  }
  if (!italic && bold) {
    return fvBold;
  }
  if (italic && bold) {
    return fvBoldItalic;
  }
  return fvRegular;
}


void font_variants_get (PangoFontDescription *description,
                        ustring& file_regular, ustring& file_italic,
                        ustring& file_bold, ustring& file_bold_italic)
// This function checks whether the font in the description has the four 
// variants (regular/roman, italic/oblique, bold, bold + italic/oblique).
// It gives the four filenames for these variants.
// If a variant does not exist, the filename will be empty.
{
  // Make a copy of the description, to work on.
  PangoFontDescription * font_desc;
  font_desc = pango_font_description_copy (description);
  // Storage for family and style.
  ustring family, style;
  // We start off with the regular/roman font.
  pango_font_description_set_style (font_desc, PANGO_STYLE_NORMAL);
  pango_font_description_set_weight (font_desc, PANGO_WEIGHT_NORMAL);
  font_fontinformation (font_desc, file_regular, family, style);
  if (font_variant_get (style) != fvRegular)
    file_regular.clear();
  // Next the italic font.
  pango_font_description_set_style (font_desc, PANGO_STYLE_ITALIC);
  font_fontinformation (font_desc, file_italic, family, style);
  if (font_variant_get (style) != fvItalic)
    file_italic.clear();
  // Then the bold font.
  pango_font_description_set_style (font_desc, PANGO_STYLE_NORMAL);
  pango_font_description_set_weight (font_desc, PANGO_WEIGHT_BOLD);
  font_fontinformation (font_desc, file_bold, family, style);
  if (font_variant_get (style) != fvBold)
    file_bold.clear();
  // Lastly the bold / italic font.
  pango_font_description_set_style (font_desc, PANGO_STYLE_ITALIC);
  font_fontinformation (font_desc, file_bold_italic, family, style);
  if (font_variant_get (style) != fvBoldItalic)
    file_bold_italic.clear();
  // Free memory.
  pango_font_description_free (font_desc);
}


vector<ustring> font_get_families_on_system ()
// Produce a list of the families of the fonts installed on the system.
{
  int          j;
  FcObjectSet *os = 0;
  FcFontSet   *fs;
  FcPattern   *pat;
  FcChar8     *family;
  vector<ustring> list;
  if (FcInit()) {
    pat = FcNameParse((FcChar8 *)":");
    os  = FcObjectSetCreate();
    FcObjectSetAdd(os, "family"); 
    fs = FcFontList(0, pat, os);
    if(pat)
      FcPatternDestroy(pat);
    if(fs) {
      for (j = 0; j < fs->nfont; j++) {
        if (FcPatternGetString (fs->fonts[j], FC_FAMILY, 0, &family) == FcResultMatch) {
          list.push_back ((char*)family);
        }
      }
      FcFontSetDestroy(fs);
      FcObjectSetDestroy(os);
    }
  }
  sort (list.begin(), list.end());
  return list;
}


void font_get_for_xep_internal (const ustring& fontfamily, const ustring& fontpath,
                                FontVariant fontvariant,
                                vector<ustring>& family, vector<ustring>& path, 
                                vector<ustring>& metrics, vector<FontVariant>& variant)
// Checks font and stores if acceptable.
{
  // If we've no filename, quit.
  if (fontpath.empty())
    return;
  // If it is no True Type files or is not a .pfa/.pfb files with .afm metrics, quit.
  if (!font_is_ttf_or_pfa_slash_pfb_with_afm (fontpath))
    return;
  // Font acceptable. Get additional details, and store everything.
  ustring metricsfilename;
  metricsfilename = fontpath;
  if (!font_get_afm_file (metricsfilename))
    metricsfilename = fontpath;
  family.push_back (fontfamily);
  path.push_back (fontpath);
  metrics.push_back (metricsfilename);
  variant.push_back (fontvariant);
}


void font_get_for_xep (vector<ustring>& family, vector<ustring>& path, 
                       vector<ustring>& metrics, vector<FontVariant>& variant)
// This function gives a list of fonts acceptable for XEP.
// family: contains the font family.
// path: contains the path to the font file.
// metrics: contains the path to the metrics file.
// variant: contains the variant, e.g. Regular, Italic, etc.
{
  // Get list of families of all fonts in fontconfig.
  vector<ustring> families_all = font_get_families_on_system ();
  // Go through each family.
  for (unsigned int i = 0; i < families_all.size(); i++) {
    // Make a pango description of it.
    PangoFontDescription *font_desc;
    font_desc = pango_font_description_from_string (families_all[i].c_str());
    // Get the four variants: regular, italic, bold, bold-italic.
    ustring file_regular, file_italic, file_bold, file_bold_italic;    
    font_variants_get (font_desc, file_regular, file_italic, file_bold, file_bold_italic);
    font_get_for_xep_internal (families_all[i], file_regular, fvRegular, family, path, metrics, variant);
    font_get_for_xep_internal (families_all[i], file_italic, fvItalic, family, path, metrics, variant);
    font_get_for_xep_internal (families_all[i], file_bold, fvBold, family, path, metrics, variant);
    font_get_for_xep_internal (families_all[i], file_bold_italic, fvBoldItalic, family, path, metrics, variant);
    // Free memory.  
    pango_font_description_free (font_desc);
  }    
}


vector<ustring> font_get_sizes ()
{
  vector<ustring> sizes;
  sizes.push_back ("8");
  sizes.push_back ("9");
  sizes.push_back ("10");
  sizes.push_back ("11");
  sizes.push_back ("12");
  sizes.push_back ("13");
  sizes.push_back ("14");
  sizes.push_back ("16");
  sizes.push_back ("18");
  sizes.push_back ("20");
  sizes.push_back ("22");
  sizes.push_back ("24");
  sizes.push_back ("26");
  sizes.push_back ("28");
  sizes.push_back ("32");
  sizes.push_back ("36");
  sizes.push_back ("40");
  sizes.push_back ("48");
  sizes.push_back ("56");
  sizes.push_back ("64");
  sizes.push_back ("72");
  return sizes;
}


PrintingFonts::PrintingFonts (const ustring& project)
{
  /*
  Get all families on the system that are acceptable to XEP.
  It gets only those acceptable to XEP, though the system might have more.
  Note that the families will be repeated, as there are bold, italics, etc.,
  all stated separately with their family.
  */
  font_get_for_xep (xep_families, xep_paths, xep_metrics, xep_variants);
  for (unsigned int i = 0; i < xep_families.size(); i++) {
    families_set.insert (xep_families[i]);
  }
  available_families.assign (families_set.begin(), families_set.end());
  // Ensure that some relevant fonts, provided by XEP, are listed too.
  ensure_font_available (HELVETICA);
  ensure_font_available (TIMES);
  ensure_font_available (COURIER);
  // Sort the families.
  sort (available_families.begin(), available_families.end());
  
  /*
  Get the font families to be used in printing for "project".
  */
  
  // If project given, take project's printing fonts.
  extern Settings * settings;
  if (!project.empty()) {
    printing_families = settings->projectconfig (project)->printing_fonts_get();
  }
  // If no printing fonts found, take them of the general config.
  if (printing_families.empty()) {
    printing_families = settings->genconfig.printing_fonts_get ();
  }
  // Remove fonts that are not acceptable for XEP or are not on the system.
  {
    vector<ustring> new_fonts;
    for (unsigned int i = 0; i < printing_families.size(); i++) {
      if (families_set.find (printing_families[i]) != families_set.end())
        new_fonts.push_back (printing_families[i]);
    }
    if (new_fonts.size() != printing_families.size()) {
      printing_families.clear();
      printing_families.assign (new_fonts.begin(), new_fonts.end());    
    }
  }
  // Ensure that at least one printing font is given.
  if (printing_families.empty()) {
    printing_families.push_back (HELVETICA);
  }
}


void PrintingFonts::ensure_font_available (const ustring& font)
{
  if (families_set.find (font) == families_set.end()) {
    available_families.push_back (font);
    families_set.insert (font);
  }
}


ustring PrintingFonts::printing_families_comma_separated ()
{
  ustring font;
  for (unsigned int i = 0; i < printing_families.size(); i++) {
    if (!font.empty())
      font.append (",");
    font.append (printing_families[i]);
  }
  return font;
}


void PrintingFonts::printing_families_store (const vector<ustring>& fonts)
{
  extern Settings * settings;
  // Save the fonts in the configuration.
  settings->genconfig.printing_fonts_set (fonts);
  // If a project is open, save it there too.
  if (!settings->genconfig.project_get().empty()) {
    settings->projectconfig (settings->genconfig.project_get())->printing_fonts_set (fonts);
  }
}
