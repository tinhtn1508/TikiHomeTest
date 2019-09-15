#ifndef BM25_HH
#define BM25_HH

#include "ParsingTextFile.hh"

constexpr float k_1 = 2;

class BM25
{
public:

    /**
     * Constructor
     */
    BM25(ParsingTextFile* p);

    /**
     * Disable copy constructor
     */
    BM25(const BM25& p) = delete;

    /**
     * Get score of BM25 algorithm
     */
    double score(const std::vector<std::string>& keys,
                 const uint32_t& uniqueID, bool flag);

    /**
     * Default constructor
     */
    ~BM25() = default;

private:

    /**
     * Get inverse document frequency
     */
    double IDF(const std::string& word, bool flag);

    /**
     * Get term frequency
     */
    double TF(const std::string& word, const uint32_t& uniqueID, const double& D, bool flag);

    /**
     * Get the length of the document D in word
     */
    double numberOfWordInD(const uint32_t& uniqueID);

    ParsingTextFile* parser;

    float avgdl; // the average document length
    float N;     // number of name (line) 50.000
};

#endif //BM25_HH
