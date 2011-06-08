/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 *
 * @author Daniel
 */
public final class GBCToolkit {

   /**
	 * Follows the Unicode standard for determining if a code point is
	 * a Unicode space character
	 */
	public static boolean isSpace(char ch)
	{
		boolean result = false;
		switch (ch)
		{
			case 0x0020: // SPACE
			case 0x1680: // OGHAM SPACE MARK
			case 0x2000: // EN QUAD
			case 0x2001: // EM QUAD
			case 0x2002: // EN SPACE
			case 0x2003: // EM SPACE
			case 0x2004: // THREE-PER-EM SPACE
			case 0x2005: // FOUR-PER-EM SPACE
			case 0x2006: // SIX-PER-EM SPACE
			case 0x2007: // FIGURE SPACE
			case 0x2008: // PUNCTUATION SPACE
			case 0x2009: // THIN SPACE
			case 0x200A: // HAIR SPACE
			case 0x200B: // ZERO WIDTH SPACE
			case 0x205F: // MEDIUM MATHEMATICAL SPACE
/* Two versions of the Chinese CUV exists. One version uses two words
 * <shang> <di> to mean "God". The other version uses <IDEOGRAPHIC SPACE> <shen>
 * to mean "God". (A vestige from the days of moveable types - preserving the
 * character count by creating a space was easier than deleting a word and
 * messing up the layout.) However, in the latter case, it is obviously not
 * meant to be wrapped at the <IDEOGRAPHIC SPACE>.
 *
 * For the benefit of the Chinese translations, we break unicode rules and
 * treat the ideographic space as a normal character.
 */
//			case 0x3000: // IDEOGRAPHIC SPACE
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

	/**
	 * Follows the Unicode standard for determining if a code point is
	 * a Unicode non-breaking space (NBSP)
	 */
	public static boolean isNonBreakingSpace(char ch)
	{
		boolean result = false;
		switch ((int)ch)
		{
			case 0x00A0: // NO-BREAK SPACE
			case 0x202F: // NARROW NO-BREAK SPACE
			case 0x2060: // WORD JOINER (a zero width non-breaking space)
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

	/**
	 * Is this a breaking character?
	 * - See if it's a space, tab, etc...
	 */
	public static boolean isBreakingCharacter(char ch)
	{
		boolean result = isSpace(ch);
		if (result)
		{
			return result;
		}

		switch ((int)ch)
		{
			case 0x0009: // TAB
			case 0x00AD: // SOFT HYPHEN (SHY)
				result = true;
				break;
			default:
				break;
		}
		return result;
	}

	/**
	 * Convert from a Surrogate Pair to the int Code Point
	 *
	 * @param sp1 Surrogate Pair - 1st UTF-16 char
	 * @param sp2 Surrogate Pair - 2nd UTF-16 char
	 * @return integer code point value
	 */
	public static int SPtoCP(char sp1, char sp2)
	{
		int cp = ((sp1 - 0xd800) << 10) + (sp2 - 0xDC00) + 0x10000;
		return cp;
	}

	/**
	 * Returns a surrogate pair number (1 or 2) OR zero if not a surrogate pair
	 */
	public static int isSurrogatePair(char ch)
	{
		if (ch >= 0xD800 && ch <= 0xDBFF) 	// 0xD800-0xDBFF
		{
			return 1;
		}

		if (ch >= 0xDC00 && ch <= 0xDFFF)	// 0xDC00-0xDFFF
		{
			return 2;
		}

		return 0;
	}

        /**
         * Reverses RTL characters for devices that do not support them.
         *
         * Note that this will frequently reverse characters that should not
         * be reversed, e.g. numeric digits.
         *
         * Detection of these characters can be left to a later implementation.
         * 
         * @return
         */
        public static String reverseCharacters(String s) {
            return new StringBuffer(s).reverse().toString();
        }
        public static void myAssert(boolean expression, String message) {
            if (!expression) {
                System.err.println("ASSERT FAILED");
                System.err.println(message);
                throw new RuntimeException(message);
            }
        }
}
