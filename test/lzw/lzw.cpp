///
/// @file
/// @author Julius Pettersson
/// @copyright MIT/Expat License.
/// @brief LZW file compressor
/// @version 5
/// @remark This version borrows heavily from Juha Nieminen's work.
///
/// This is the C++11 implementation of a Lempel-Ziv-Welch single-file command-line compressor.
/// It uses the simpler fixed-width code compression method.
/// It was written with Doxygen comments.
///
/// @see http://en.wikipedia.org/wiki/Lempel%E2%80%93Ziv%E2%80%93Welch
/// @see http://marknelson.us/2011/11/08/lzw-revisited/
/// @see http://www.cs.duke.edu/csed/curious/compression/lzw.html
/// @see http://warp.povusers.org/EfficientLZW/index.html
/// @see http://en.cppreference.com/
/// @see http://www.doxygen.org/
///

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <istream>
#include <limits>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

/// Type used to store and retrieve codes.
using CodeType = std::uint16_t;

namespace globals {

/// Dictionary Maximum Size (when reached, the dictionary will be reset)
const CodeType dms {std::numeric_limits<CodeType>::max()};

} // namespace globals

///
/// @brief Encoder's custom dictionary type.
///
class EncoderDictionary {

    ///
    /// @brief Binary search tree node.
    ///
    struct Node {

        ///
        /// @brief Default constructor.
        /// @param c    byte that the Node will contain
        ///
        explicit Node(char c): first(globals::dms), c(c), left(globals::dms), right(globals::dms)
        {
        }

        CodeType    first;  ///< Code of first child string.
        char        c;      ///< Byte.
        CodeType    left;   ///< Code of child node with byte < `c`.
        CodeType    right;  ///< Code of child node with byte > `c`.
    };

public:

    ///
    /// @brief Default constructor.
    /// @details It builds the `initials` cheat sheet.
    ///
    EncoderDictionary()
    {
        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();
        CodeType k {0};

        for (long int c = minc; c <= maxc; ++c)
            initials[static_cast<unsigned char> (c)] = k++;

        vn.reserve(globals::dms);
        reset();
    }

    ///
    /// @brief Resets dictionary to its initial contents.
	/// @see `EncoderDictionary::EncoderDictionary()`
    ///
    void reset()
    {
        vn.clear();

        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();

        for (long int c = minc; c <= maxc; ++c)
            vn.push_back(Node(c));
    }

    ///
    /// @brief Searches for a pair (`i`, `c`) and inserts the pair if it wasn't found.
    /// @param i                code to search for
    /// @param c                attached byte to search for
    /// @returns The index of the pair, if it was found.
    /// @retval globals::dms    if the pair wasn't found
    ///
    CodeType search_and_insert(CodeType i, char c)
    {
        // dictionary's maximum size was reached
        if (vn.size() == globals::dms)
            reset();

        if (i == globals::dms)
            return search_initials(c);

        const CodeType vn_size = vn.size();
        CodeType ci {vn[i].first}; // Current Index

        if (ci != globals::dms)
        {
            while (true)
                if (c < vn[ci].c)
                {
                    if (vn[ci].left == globals::dms)
                    {
                        vn[ci].left = vn_size;
                        break;
                    }
                    else
                        ci = vn[ci].left;
                }
                else
                if (c > vn[ci].c)
                {
                    if (vn[ci].right == globals::dms)
                    {
                        vn[ci].right = vn_size;
                        break;
                    }
                    else
                        ci = vn[ci].right;
                }
                else // c == vn[ci].c
                    return ci;
        }
        else
            vn[i].first = vn_size;

        vn.push_back(Node(c));
        return globals::dms;
    }

    ///
    /// @brief Fakes a search for byte `c` in the one-byte area of the dictionary.
    /// @param c    byte to search for
    /// @returns The code associated to the searched byte.
    ///
    CodeType search_initials(char c) const
    {
        return initials[static_cast<unsigned char> (c)];
    }

private:

    /// Vector of nodes on top of which the binary search tree is implemented.
    std::vector<Node> vn;

    /// Cheat sheet for mapping one-byte strings to their codes.
    std::array<CodeType, 1u << CHAR_BIT> initials;
};

///
/// @brief Compresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
void compress(std::istream &is, std::ostream &os)
{
    EncoderDictionary ed;
    CodeType i {globals::dms}; // Index
    char c;

    while (is.get(c))
    {
        const CodeType temp {i};

        if ((i = ed.search_and_insert(temp, c)) == globals::dms)
        {
            os.write(reinterpret_cast<const char *> (&temp), sizeof (CodeType));
            i = ed.search_initials(c);
        }
    }

    if (i != globals::dms)
        os.write(reinterpret_cast<const char *> (&i), sizeof (CodeType));
}

///
/// @brief Decompresses the contents of `is` and writes the result to `os`.
/// @param [in] is      input stream
/// @param [out] os     output stream
///
void decompress(std::istream &is, std::ostream &os)
{
    std::vector<std::pair<CodeType, char>> dictionary;

    // "named" lambda function, used to reset the dictionary to its initial contents
    const auto reset_dictionary = [&dictionary] {
        dictionary.clear();
        dictionary.reserve(globals::dms);

        const long int minc = std::numeric_limits<char>::min();
        const long int maxc = std::numeric_limits<char>::max();

        for (long int c = minc; c <= maxc; ++c)
            dictionary.push_back({globals::dms, static_cast<char> (c)});
    };

    const auto rebuild_string = [&dictionary](CodeType k) -> const std::vector<char> * {
        static std::vector<char> s; // String

        s.clear();

        // the length of a string cannot exceed the dictionary's number of entries
        s.reserve(globals::dms);

        while (k != globals::dms)
        {
            s.push_back(dictionary[k].second);
            k = dictionary[k].first;
        }

        std::reverse(s.begin(), s.end());
        return &s;
    };

    reset_dictionary();

    CodeType i {globals::dms}; // Index
    CodeType k; // Key

    while (is.read(reinterpret_cast<char *> (&k), sizeof (CodeType)))
    {
        // dictionary's maximum size was reached
        if (dictionary.size() == globals::dms)
            reset_dictionary();

        if (k > dictionary.size())
            throw std::runtime_error("invalid compressed code");

        const std::vector<char> *s; // String

        if (k == dictionary.size())
        {
            dictionary.push_back({i, rebuild_string(i)->front()});
            s = rebuild_string(k);
        }
        else
        {
            s = rebuild_string(k);

            if (i != globals::dms)
                dictionary.push_back({i, s->front()});
        }

        os.write(&s->front(), s->size());
        i = k;
    }

    if (!is.eof() || is.gcount() != 0)
        throw std::runtime_error("corrupted compressed file");
}

///
/// @brief Prints usage information and a custom error message.
/// @param s    custom error message to be printed
/// @param su   Show Usage information
///
void print_usage(const std::string &s = "", bool su = true)
{
    if (!s.empty())
        std::cerr << "\nERROR: " << s << '\n';

    if (su)
    {
        std::cerr << "\nUsage:\n";
        std::cerr << "\tprogram -flag input_file output_file\n\n";
        std::cerr << "Where `flag' is either `c' for compressing, or `d' for decompressing, and\n";
        std::cerr << "`input_file' and `output_file' are distinct files.\n\n";
        std::cerr << "Examples:\n";
        std::cerr << "\tlzw_v5.exe -c license.txt license.lzw\n";
        std::cerr << "\tlzw_v5.exe -d license.lzw new_license.txt\n";
    }

    std::cerr << std::endl;
}

///
/// @brief Actual program entry point.
/// @param argc             number of command line arguments
/// @param [in] argv        array of command line arguments
/// @retval EXIT_FAILURE    for failed operation
/// @retval EXIT_SUCCESS    for successful operation
///
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        print_usage("Wrong number of arguments.");
        return EXIT_FAILURE;
    }

    enum class Mode {
        Compress,
        Decompress
    };

    Mode m;

    if (std::string(argv[1]) == "-c")
        m = Mode::Compress;
    else
    if (std::string(argv[1]) == "-d")
        m = Mode::Decompress;
    else
    {
        print_usage(std::string("flag `") + argv[1] + "' is not recognized.");
        return EXIT_FAILURE;
    }

    const std::size_t buffer_size {1024 * 1024};

    // these custom buffers should be larger than the default ones
    const std::unique_ptr<char[]> input_buffer(new char[buffer_size]);
    const std::unique_ptr<char[]> output_buffer(new char[buffer_size]);

    std::ifstream input_file;
    std::ofstream output_file;

    input_file.rdbuf()->pubsetbuf(input_buffer.get(), buffer_size);
    input_file.open(argv[2], std::ios_base::binary);

    if (!input_file.is_open())
    {
        print_usage(std::string("input_file `") + argv[2] + "' could not be opened.");
        return EXIT_FAILURE;
    }

    output_file.rdbuf()->pubsetbuf(output_buffer.get(), buffer_size);
    output_file.open(argv[3], std::ios_base::binary);

    if (!output_file.is_open())
    {
        print_usage(std::string("output_file `") + argv[3] + "' could not be opened.");
        return EXIT_FAILURE;
    }

    try
    {
        input_file.exceptions(std::ios_base::badbit);
        output_file.exceptions(std::ios_base::badbit | std::ios_base::failbit);

        if (m == Mode::Compress)
            compress(input_file, output_file);
        else
        if (m == Mode::Decompress)
            decompress(input_file, output_file);
    }
    catch (const std::ios_base::failure &f)
    {
        print_usage(std::string("File input/output failure: ") + f.what() + '.', false);
        return EXIT_FAILURE;
    }
    catch (const std::exception &e)
    {
        print_usage(std::string("Caught exception: ") + e.what() + '.', false);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
