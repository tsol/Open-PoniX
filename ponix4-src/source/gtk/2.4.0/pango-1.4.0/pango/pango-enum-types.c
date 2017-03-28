
/* Generated data (by glib-mkenums) */

#include <pango.h>

/* enumerations from "pango-attributes.h" */
GType
pango_attr_type_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_ATTR_INVALID, "PANGO_ATTR_INVALID", "invalid" },
      { PANGO_ATTR_LANGUAGE, "PANGO_ATTR_LANGUAGE", "language" },
      { PANGO_ATTR_FAMILY, "PANGO_ATTR_FAMILY", "family" },
      { PANGO_ATTR_STYLE, "PANGO_ATTR_STYLE", "style" },
      { PANGO_ATTR_WEIGHT, "PANGO_ATTR_WEIGHT", "weight" },
      { PANGO_ATTR_VARIANT, "PANGO_ATTR_VARIANT", "variant" },
      { PANGO_ATTR_STRETCH, "PANGO_ATTR_STRETCH", "stretch" },
      { PANGO_ATTR_SIZE, "PANGO_ATTR_SIZE", "size" },
      { PANGO_ATTR_FONT_DESC, "PANGO_ATTR_FONT_DESC", "font-desc" },
      { PANGO_ATTR_FOREGROUND, "PANGO_ATTR_FOREGROUND", "foreground" },
      { PANGO_ATTR_BACKGROUND, "PANGO_ATTR_BACKGROUND", "background" },
      { PANGO_ATTR_UNDERLINE, "PANGO_ATTR_UNDERLINE", "underline" },
      { PANGO_ATTR_STRIKETHROUGH, "PANGO_ATTR_STRIKETHROUGH", "strikethrough" },
      { PANGO_ATTR_RISE, "PANGO_ATTR_RISE", "rise" },
      { PANGO_ATTR_SHAPE, "PANGO_ATTR_SHAPE", "shape" },
      { PANGO_ATTR_SCALE, "PANGO_ATTR_SCALE", "scale" },
      { PANGO_ATTR_FALLBACK, "PANGO_ATTR_FALLBACK", "fallback" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoAttrType", values);
  }
  return etype;
}

GType
pango_underline_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_UNDERLINE_NONE, "PANGO_UNDERLINE_NONE", "none" },
      { PANGO_UNDERLINE_SINGLE, "PANGO_UNDERLINE_SINGLE", "single" },
      { PANGO_UNDERLINE_DOUBLE, "PANGO_UNDERLINE_DOUBLE", "double" },
      { PANGO_UNDERLINE_LOW, "PANGO_UNDERLINE_LOW", "low" },
      { PANGO_UNDERLINE_ERROR, "PANGO_UNDERLINE_ERROR", "error" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoUnderline", values);
  }
  return etype;
}


/* enumerations from "pango-coverage.h" */
GType
pango_coverage_level_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_COVERAGE_NONE, "PANGO_COVERAGE_NONE", "none" },
      { PANGO_COVERAGE_FALLBACK, "PANGO_COVERAGE_FALLBACK", "fallback" },
      { PANGO_COVERAGE_APPROXIMATE, "PANGO_COVERAGE_APPROXIMATE", "approximate" },
      { PANGO_COVERAGE_EXACT, "PANGO_COVERAGE_EXACT", "exact" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoCoverageLevel", values);
  }
  return etype;
}


/* enumerations from "pango-font.h" */
GType
pango_style_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_STYLE_NORMAL, "PANGO_STYLE_NORMAL", "normal" },
      { PANGO_STYLE_OBLIQUE, "PANGO_STYLE_OBLIQUE", "oblique" },
      { PANGO_STYLE_ITALIC, "PANGO_STYLE_ITALIC", "italic" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoStyle", values);
  }
  return etype;
}

GType
pango_variant_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_VARIANT_NORMAL, "PANGO_VARIANT_NORMAL", "normal" },
      { PANGO_VARIANT_SMALL_CAPS, "PANGO_VARIANT_SMALL_CAPS", "small-caps" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoVariant", values);
  }
  return etype;
}

GType
pango_weight_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_WEIGHT_ULTRALIGHT, "PANGO_WEIGHT_ULTRALIGHT", "ultralight" },
      { PANGO_WEIGHT_LIGHT, "PANGO_WEIGHT_LIGHT", "light" },
      { PANGO_WEIGHT_NORMAL, "PANGO_WEIGHT_NORMAL", "normal" },
      { PANGO_WEIGHT_BOLD, "PANGO_WEIGHT_BOLD", "bold" },
      { PANGO_WEIGHT_ULTRABOLD, "PANGO_WEIGHT_ULTRABOLD", "ultrabold" },
      { PANGO_WEIGHT_HEAVY, "PANGO_WEIGHT_HEAVY", "heavy" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoWeight", values);
  }
  return etype;
}

GType
pango_stretch_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_STRETCH_ULTRA_CONDENSED, "PANGO_STRETCH_ULTRA_CONDENSED", "ultra-condensed" },
      { PANGO_STRETCH_EXTRA_CONDENSED, "PANGO_STRETCH_EXTRA_CONDENSED", "extra-condensed" },
      { PANGO_STRETCH_CONDENSED, "PANGO_STRETCH_CONDENSED", "condensed" },
      { PANGO_STRETCH_SEMI_CONDENSED, "PANGO_STRETCH_SEMI_CONDENSED", "semi-condensed" },
      { PANGO_STRETCH_NORMAL, "PANGO_STRETCH_NORMAL", "normal" },
      { PANGO_STRETCH_SEMI_EXPANDED, "PANGO_STRETCH_SEMI_EXPANDED", "semi-expanded" },
      { PANGO_STRETCH_EXPANDED, "PANGO_STRETCH_EXPANDED", "expanded" },
      { PANGO_STRETCH_EXTRA_EXPANDED, "PANGO_STRETCH_EXTRA_EXPANDED", "extra-expanded" },
      { PANGO_STRETCH_ULTRA_EXPANDED, "PANGO_STRETCH_ULTRA_EXPANDED", "ultra-expanded" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoStretch", values);
  }
  return etype;
}

GType
pango_font_mask_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GFlagsValue values[] = {
      { PANGO_FONT_MASK_FAMILY, "PANGO_FONT_MASK_FAMILY", "family" },
      { PANGO_FONT_MASK_STYLE, "PANGO_FONT_MASK_STYLE", "style" },
      { PANGO_FONT_MASK_VARIANT, "PANGO_FONT_MASK_VARIANT", "variant" },
      { PANGO_FONT_MASK_WEIGHT, "PANGO_FONT_MASK_WEIGHT", "weight" },
      { PANGO_FONT_MASK_STRETCH, "PANGO_FONT_MASK_STRETCH", "stretch" },
      { PANGO_FONT_MASK_SIZE, "PANGO_FONT_MASK_SIZE", "size" },
      { 0, NULL, NULL }
    };
    etype = g_flags_register_static ("PangoFontMask", values);
  }
  return etype;
}


/* enumerations from "pango-layout.h" */
GType
pango_alignment_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_ALIGN_LEFT, "PANGO_ALIGN_LEFT", "left" },
      { PANGO_ALIGN_CENTER, "PANGO_ALIGN_CENTER", "center" },
      { PANGO_ALIGN_RIGHT, "PANGO_ALIGN_RIGHT", "right" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoAlignment", values);
  }
  return etype;
}

GType
pango_wrap_mode_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_WRAP_WORD, "PANGO_WRAP_WORD", "word" },
      { PANGO_WRAP_CHAR, "PANGO_WRAP_CHAR", "char" },
      { PANGO_WRAP_WORD_CHAR, "PANGO_WRAP_WORD_CHAR", "word-char" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoWrapMode", values);
  }
  return etype;
}


/* enumerations from "pango-script.h" */
GType
pango_script_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_SCRIPT_INVALID_CODE, "PANGO_SCRIPT_INVALID_CODE", "invalid-code" },
      { PANGO_SCRIPT_COMMON, "PANGO_SCRIPT_COMMON", "common" },
      { PANGO_SCRIPT_INHERITED, "PANGO_SCRIPT_INHERITED", "inherited" },
      { PANGO_SCRIPT_ARABIC, "PANGO_SCRIPT_ARABIC", "arabic" },
      { PANGO_SCRIPT_ARMENIAN, "PANGO_SCRIPT_ARMENIAN", "armenian" },
      { PANGO_SCRIPT_BENGALI, "PANGO_SCRIPT_BENGALI", "bengali" },
      { PANGO_SCRIPT_BOPOMOFO, "PANGO_SCRIPT_BOPOMOFO", "bopomofo" },
      { PANGO_SCRIPT_CHEROKEE, "PANGO_SCRIPT_CHEROKEE", "cherokee" },
      { PANGO_SCRIPT_COPTIC, "PANGO_SCRIPT_COPTIC", "coptic" },
      { PANGO_SCRIPT_CYRILLIC, "PANGO_SCRIPT_CYRILLIC", "cyrillic" },
      { PANGO_SCRIPT_DESERET, "PANGO_SCRIPT_DESERET", "deseret" },
      { PANGO_SCRIPT_DEVANAGARI, "PANGO_SCRIPT_DEVANAGARI", "devanagari" },
      { PANGO_SCRIPT_ETHIOPIC, "PANGO_SCRIPT_ETHIOPIC", "ethiopic" },
      { PANGO_SCRIPT_GEORGIAN, "PANGO_SCRIPT_GEORGIAN", "georgian" },
      { PANGO_SCRIPT_GOTHIC, "PANGO_SCRIPT_GOTHIC", "gothic" },
      { PANGO_SCRIPT_GREEK, "PANGO_SCRIPT_GREEK", "greek" },
      { PANGO_SCRIPT_GUJARATI, "PANGO_SCRIPT_GUJARATI", "gujarati" },
      { PANGO_SCRIPT_GURMUKHI, "PANGO_SCRIPT_GURMUKHI", "gurmukhi" },
      { PANGO_SCRIPT_HAN, "PANGO_SCRIPT_HAN", "han" },
      { PANGO_SCRIPT_HANGUL, "PANGO_SCRIPT_HANGUL", "hangul" },
      { PANGO_SCRIPT_HEBREW, "PANGO_SCRIPT_HEBREW", "hebrew" },
      { PANGO_SCRIPT_HIRAGANA, "PANGO_SCRIPT_HIRAGANA", "hiragana" },
      { PANGO_SCRIPT_KANNADA, "PANGO_SCRIPT_KANNADA", "kannada" },
      { PANGO_SCRIPT_KATAKANA, "PANGO_SCRIPT_KATAKANA", "katakana" },
      { PANGO_SCRIPT_KHMER, "PANGO_SCRIPT_KHMER", "khmer" },
      { PANGO_SCRIPT_LAO, "PANGO_SCRIPT_LAO", "lao" },
      { PANGO_SCRIPT_LATIN, "PANGO_SCRIPT_LATIN", "latin" },
      { PANGO_SCRIPT_MALAYALAM, "PANGO_SCRIPT_MALAYALAM", "malayalam" },
      { PANGO_SCRIPT_MONGOLIAN, "PANGO_SCRIPT_MONGOLIAN", "mongolian" },
      { PANGO_SCRIPT_MYANMAR, "PANGO_SCRIPT_MYANMAR", "myanmar" },
      { PANGO_SCRIPT_OGHAM, "PANGO_SCRIPT_OGHAM", "ogham" },
      { PANGO_SCRIPT_OLD_ITALIC, "PANGO_SCRIPT_OLD_ITALIC", "old-italic" },
      { PANGO_SCRIPT_ORIYA, "PANGO_SCRIPT_ORIYA", "oriya" },
      { PANGO_SCRIPT_RUNIC, "PANGO_SCRIPT_RUNIC", "runic" },
      { PANGO_SCRIPT_SINHALA, "PANGO_SCRIPT_SINHALA", "sinhala" },
      { PANGO_SCRIPT_SYRIAC, "PANGO_SCRIPT_SYRIAC", "syriac" },
      { PANGO_SCRIPT_TAMIL, "PANGO_SCRIPT_TAMIL", "tamil" },
      { PANGO_SCRIPT_TELUGU, "PANGO_SCRIPT_TELUGU", "telugu" },
      { PANGO_SCRIPT_THAANA, "PANGO_SCRIPT_THAANA", "thaana" },
      { PANGO_SCRIPT_THAI, "PANGO_SCRIPT_THAI", "thai" },
      { PANGO_SCRIPT_TIBETAN, "PANGO_SCRIPT_TIBETAN", "tibetan" },
      { PANGO_SCRIPT_CANADIAN_ABORIGINAL, "PANGO_SCRIPT_CANADIAN_ABORIGINAL", "canadian-aboriginal" },
      { PANGO_SCRIPT_YI, "PANGO_SCRIPT_YI", "yi" },
      { PANGO_SCRIPT_TAGALOG, "PANGO_SCRIPT_TAGALOG", "tagalog" },
      { PANGO_SCRIPT_HANUNOO, "PANGO_SCRIPT_HANUNOO", "hanunoo" },
      { PANGO_SCRIPT_BUHID, "PANGO_SCRIPT_BUHID", "buhid" },
      { PANGO_SCRIPT_TAGBANWA, "PANGO_SCRIPT_TAGBANWA", "tagbanwa" },
      { PANGO_SCRIPT_BRAILLE, "PANGO_SCRIPT_BRAILLE", "braille" },
      { PANGO_SCRIPT_CYPRIOT, "PANGO_SCRIPT_CYPRIOT", "cypriot" },
      { PANGO_SCRIPT_LIMBU, "PANGO_SCRIPT_LIMBU", "limbu" },
      { PANGO_SCRIPT_OSMANYA, "PANGO_SCRIPT_OSMANYA", "osmanya" },
      { PANGO_SCRIPT_SHAVIAN, "PANGO_SCRIPT_SHAVIAN", "shavian" },
      { PANGO_SCRIPT_LINEAR_B, "PANGO_SCRIPT_LINEAR_B", "linear-b" },
      { PANGO_SCRIPT_TAI_LE, "PANGO_SCRIPT_TAI_LE", "tai-le" },
      { PANGO_SCRIPT_UGARITIC, "PANGO_SCRIPT_UGARITIC", "ugaritic" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoScript", values);
  }
  return etype;
}


/* enumerations from "pango-tabs.h" */
GType
pango_tab_align_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_TAB_LEFT, "PANGO_TAB_LEFT", "left" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoTabAlign", values);
  }
  return etype;
}


/* enumerations from "pango-types.h" */
GType
pango_direction_get_type (void)
{
  static GType etype = 0;
  if (etype == 0) {
    static const GEnumValue values[] = {
      { PANGO_DIRECTION_LTR, "PANGO_DIRECTION_LTR", "ltr" },
      { PANGO_DIRECTION_RTL, "PANGO_DIRECTION_RTL", "rtl" },
      { PANGO_DIRECTION_TTB_LTR, "PANGO_DIRECTION_TTB_LTR", "ttb-ltr" },
      { PANGO_DIRECTION_TTB_RTL, "PANGO_DIRECTION_TTB_RTL", "ttb-rtl" },
      { PANGO_DIRECTION_WEAK_LTR, "PANGO_DIRECTION_WEAK_LTR", "weak-ltr" },
      { PANGO_DIRECTION_WEAK_RTL, "PANGO_DIRECTION_WEAK_RTL", "weak-rtl" },
      { PANGO_DIRECTION_NEUTRAL, "PANGO_DIRECTION_NEUTRAL", "neutral" },
      { 0, NULL, NULL }
    };
    etype = g_enum_register_static ("PangoDirection", values);
  }
  return etype;
}


/* Generated data ends here */

