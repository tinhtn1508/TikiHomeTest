/**
 *
 * @file: main.cc
 *
 * @brief: This is main function to execute search algorithms (reverse index & BM25)
 *
 * @author Tinh Nguyen
 * Contact: trongtinh.bk14@gmail.com
 *
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctime>
#include <chrono>
#include <algorithm>
#include <iterator> 
#include <map> 
#include "SearchHandler.hh"
#include "BM25.hh"

#define PATH_DATA_FILE  "./../data/product_names.txt"
#define PATH_KEY_FILE   "./../data/100_query.txt"

// Get key from 100_query.txt
std::vector<std::pair<std::string, bool>> getKey(std::string key)
{
    std::ifstream file = std::ifstream(key);
    std::string rawKey;
    std::pair<std::string, bool> pairKey;
    std::vector<std::pair<std::string, bool>> vecKey;
    pairKey.second = true;
    while(file.good())
    {
        std::getline(file, rawKey);
        for (auto& itr : ParsingTextFile::sign)
        {
            if (rawKey.find(itr) != std::string::npos)
            {
                pairKey.second = false;
                break;
            }
        }
        pairKey.first = rawKey;
        vecKey.push_back(pairKey);
    }
    file.close();
    return vecKey;
}

int main () {
    // Build data structure

    ParsingTextFile* parser = new ParsingTextFile(PATH_DATA_FILE);
    parser->build();
    SearchHandler handler(parser);
    auto&& keys = getKey(PATH_KEY_FILE);

#ifdef __BM25__
    
    for (auto& itr : keys)
    {
        std::cout << "\n-------- Kết quả tìm kiếm cho '"<< itr.first.substr(0, itr.first.length() - 1) << "' ";
        auto t0 = std::chrono::high_resolution_clock::now();
        std::multimap<double, std::string, std::greater<double>>&& result = handler.searchBM25(itr.first, itr.second);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration< double > fs = t1 - t0;
        std::chrono::milliseconds d = std::chrono::duration_cast< std::chrono::milliseconds >( fs );
        std::cout << result.size() << " kết quả (" << d.count() << " mili giây)" << "\n\n";

        for (auto& itr : result)
        {
            std::cout << itr.second << std::endl;
        }
    }

#else

    for (auto& itr : keys)
    {
        std::cout << "\n-------- Kết quả tìm kiếm cho '"<< itr.first.substr(0, itr.first.length() - 1) << "' ";
        auto t0 = std::chrono::high_resolution_clock::now();
        std::vector<std::string>&& result = handler.searchRevertIndex(itr.first, itr.second);
        auto t1 = std::chrono::high_resolution_clock::now();
        std::chrono::duration< double > fs = t1 - t0;
        std::chrono::milliseconds d = std::chrono::duration_cast< std::chrono::milliseconds >( fs );
        std::cout << result.size() << " kết quả (" << d.count() << " mili giây)" << "\n\n";

        for (auto& itr : result)
        {
            std::cout << itr << std::endl;
        }
    }

#endif
    delete parser;
    return 0;
}
