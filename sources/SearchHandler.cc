/**
 *
 * @file: SearchHandler.cc
 *
 * @brief: The class implement some behavior for searching
 *
 * @author Tinh Nguyen
 * Contact: trongtinh.bk14@gmail.com
 *
 */

#include "SearchHandler.hh"
#include <ctime>
#include <chrono>

SearchHandler::SearchHandler(ParsingTextFile* p) : BM25(p)
{
    parsing = p;
}

std::multimap<double, std::string, std::greater<double>> SearchHandler::searchBM25(const std::string& key, bool flag)
{
    std::vector<uint32_t>&& matched = search(key, flag);
    std::multimap<double, std::string, std::greater<double>> sorted_map;
    std::queue<uint32_t, std::deque<uint32_t>> q(std::deque<uint32_t>(matched.begin(), matched.end()));
    std::thread worker[MAX_THEARD];
    std::mutex mutexQueue;

    for (int i = 0; i < MAX_THEARD; i++)
    {
        worker[i] = std::thread([&](){
            while(1)
            {
                std::unique_lock<std::mutex> lock(mutexQueue);
                if (q.empty())
                {
                    break;
                }

                uint32_t id = q.front();
                q.pop();
                lock.unlock();
                double priority = score(paramKey, id, flag);
                auto& hashTable = parsing->getHashTable();
                std::unordered_map<uint32_t, std::pair<std::string, uint32_t>>::iterator got;
                got = hashTable.find(id);

                lock.lock();
                sorted_map.insert(std::make_pair(priority, got->second.first));
                lock.unlock();
            }
        });
    }

    for (int i = 0; i < MAX_THEARD; i++)
    {
        worker[i].join();
    }
    paramKey.clear();
    return sorted_map;
}

std::vector<uint32_t> SearchHandler::search(const std::string& key, bool flag)
{
    std::string keyParam;
    // dsacsd
    if (flag)
    {
        keyParam = parsing->convertUnsignedString(key);
    }
    else
    {
        keyParam = key;
    }
    // cnasujdbcjs
    // cahsdbc
    
    
    std::regex rgx ("\\s+");
    std::sregex_token_iterator iter(keyParam.begin(), keyParam.end(), rgx, -1);
    std::sregex_token_iterator end;
    std::map<std::string, std::vector<uint32_t>*>::iterator itr;

    std::vector<uint32_t> matched;

    auto& mapIndex = parsing->getMapInvertedIndex(flag);

    for (int index = 0; iter != end; iter++, index++)
    {
        itr = mapIndex.find(*iter);
        if (itr != mapIndex.end())
        {
            auto& second = *itr->second;
            matched = intersection(matched, second, index);
            paramKey.push_back(*iter);
        }
    }

    return matched;
}

std::vector<std::string> SearchHandler::searchRevertIndex(const std::string& key, bool flag)
{
    auto& hashTable = parsing->getHashTable();
    std::vector<uint32_t>&& matched = search(key, flag);

    std::vector<std::string> result;
    std::unordered_map<uint32_t, std::pair<std::string, uint32_t>>::iterator got;

    for (auto& itr : matched)
    {
        got = hashTable.find(itr);
        if (got != hashTable.end())
        {
            result.push_back(got->second.first);
        }
    }
    paramKey.clear();
    return result;
}

std::vector<uint32_t> SearchHandler::intersection(std::vector<uint32_t>& first, std::vector<uint32_t>& second, uint32_t count)
{
    std::vector<uint32_t> match(second.size());
    std::vector<uint32_t>::iterator it;
    
    if (first.empty() && count == 0)
    {
        std::set<uint32_t> out(second.begin(), second.end());
        std::vector <uint32_t> output;
        std::copy(out.begin(), out.end(), std::back_inserter(output));
        return output;
    }
    else if (first.empty())
    {
        return {};
    }

    std::sort(second.begin(), second.end());

    it = std::set_intersection(first.begin(), first.end(),
                                second.begin(), second.end(), match.begin()); 

    match.resize(it - match.begin());
    return match;
}
