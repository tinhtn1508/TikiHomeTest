#ifndef SEARCHHANDLER_HH
#define SEARCHHANDLER_HH

#include <iostream>
#include "ParsingTextFile.hh"
#include "BM25.hh"

class SearchHandler : public BM25
{
public:
    /**
     * Constructor
     */
    SearchHandler(ParsingTextFile* p);

    /**
     * Default destuctor
     */
    ~SearchHandler() = default;

    /**
     *  searchBM25
     */
    std::multimap<double, std::string, std::greater<double>> searchBM25(const std::string& key, bool flag);

    /**
     *  searchRevertIndex
     */
    std::vector<std::string> searchRevertIndex(const std::string& key, bool flag);

    /**
     *  search
     */
    std::vector<uint32_t> search(const std::string& key, bool flag);

private:

    /**
     *  Intersection
     */
    std::vector<uint32_t> intersection(std::vector<uint32_t>& first,
                                       std::vector<uint32_t>& second, uint32_t count);

    /**
     *  instance for parsing
     */
    ParsingTextFile* parsing;

    /**
     *  vector for sotring param key
     */
    std::vector<std::string> paramKey;
};

#endif //SEARCHHANDLER_HH
