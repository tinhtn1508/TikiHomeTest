#include "BM25.hh"
#include <math.h> 

BM25::BM25(ParsingTextFile* p)
{
    this->parser = p;
    N = (float)p->getLineOfFile();

    auto& hashTable = parser->getHashTable();

    uint32_t sumWordInData = 0;
    for (auto& itr : hashTable)
    {
        sumWordInData += itr.second.second;
    }
    avgdl = (float)sumWordInData / N;
}

double BM25::IDF(const std::string& word, bool flag)
{
    std::map<std::string, std::vector<uint32_t>*>::iterator itr;
    auto& mapIndex = parser->getMapInvertedIndex(flag);
    itr = mapIndex.find(word);
    uint32_t n = 0;
    if (itr != mapIndex.end())
    {
        auto vec = *(itr->second);
        std::set<uint32_t> out(vec.begin(), vec.end());
        n = out.size();
    }
    double tmp = (N - n + 0.5) / (n + 0.5);

    return log(tmp);
}

double BM25::TF(const std::string& word, const uint32_t& uniqueID, const double& D, bool flag)
{
    auto& mapIndexBM25 = parser->getMapInvertedIndexBM25(flag);
    std::map<std::string, std::unordered_map<uint32_t, uint32_t>>::iterator itrBM25;
    itrBM25 = mapIndexBM25.find(word);
    if (itrBM25 != mapIndexBM25.end())
    {
        auto orderMap = itrBM25->second;
        return (double)orderMap[uniqueID] / D;
    }
    else
    {
        return 0.0;
    }
}

double BM25::numberOfWordInD(const uint32_t& uniqueID)
{
    auto& hashTable = parser->getHashTable();
    return (double)hashTable[uniqueID].second;
}

double BM25::score(const std::vector<std::string>& keys, const uint32_t& uniqueID, bool flag)
{
    double sum = 0.0;
    for (auto& key : keys)
    {
        double D = numberOfWordInD(uniqueID);
        double tf = TF(key, uniqueID, D, flag);
        double idf = IDF(key, flag);
        double b = 0.75 * idf;
        sum += idf*((tf * (k_1 + 1))/ (tf + k_1 * (1 - b + b * (D / avgdl))));
    }
    return sum;
}
