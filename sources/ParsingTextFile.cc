/**
 *
 * @file: ParsingTextFile.cc
 *
 * @brief: The class implement some behavior for parsing text file
 *         and build data structure
 *
 * @author Tinh Nguyen
 * Contact: trongtinh.bk14@gmail.com
 *
 */

#include "ParsingTextFile.hh"

// Support converting to usigned Vietnamese
std::map<std::string, std::string> ParsingTextFile::convertTable = {
    {"a", sign_a},
    {"A", sign_A},
    {"e", sign_e},
    {"E", sign_E},
    {"o", sign_o},
    {"O", sign_O},
    {"u", sign_u},
    {"U", sign_U},
    {"i", sign_i},
    {"I", sign_I},
    {"d", sign_d},
    {"D", sign_D},
    {"y", sign_y},
    {"Y", sign_Y}
};

// Support detecting to signed Vietnamese
std::vector<std::string> ParsingTextFile::sign = {
    "á", "à", "ạ", "ả", "ã", "â", "ấ", "ầ", "ậ", "ẩ", "ẫ", "ă", "ắ", "ằ", "ặ", "ẳ", "ẵ",
    "Á", "À", "Ạ", "Ả", "Ã", "Â", "Ấ", "Ầ", "Ậ", "Ẩ", "Ẫ", "Ă", "Ắ", "Ằ", "Ặ", "Ẳ", "Ẵ",
    "é", "è", "ẹ", "ẻ", "ẽ", "ê", "ế", "ề", "ệ", "ể", "ễ",
    "É", "È", "Ẹ", "Ẻ", "Ẽ", "Ê", "Ế", "Ề", "Ệ", "Ể", "Ễ",
    "ó", "ò", "ọ", "ỏ", "õ", "ô", "ố", "ồ", "ộ", "ổ", "ỗ", "ơ", "ớ", "ờ", "ợ", "ở", "ỡ",
    "Ó", "Ò", "Ọ", "Ỏ", "Õ", "Ô", "Ố", "Ồ", "Ộ", "Ổ", "Ỗ", "Ơ", "Ớ", "Ờ", "Ợ", "Ở", "Ỡ",
    "ú", "ù", "ụ", "ủ", "ũ", "ư", "ứ", "ừ", "ự", "ử", "ữ",
    "Ú", "Ù", "Ụ", "Ủ", "Ũ", "Ư", "Ứ", "Ừ", "Ự", "Ử", "Ữ",
    "í", "ì", "ị", "ỉ", "ĩ",
    "Í", "Ì", "Ị", "Ỉ", "Ĩ",
    "đ",
    "Đ",
    "ý", "ỳ", "ỵ", "ỷ", "ỹ",
    "Ý", "Ỳ", "Ỵ", "Ỷ", "Ỹ"
};

std::unordered_map<uint32_t, std::pair<std::string, uint32_t>> ParsingTextFile::hashTable;
std::queue <std::pair<uint32_t, std::string>*> ParsingTextFile::container;

std::mutex ParsingTextFile::mContainer;
std::mutex ParsingTextFile::mMapInvertedIndex;

std::condition_variable ParsingTextFile::condNotify;
std::thread ParsingTextFile::thWorker[MAX_THEARD];

std::map<std::string, std::unordered_map<uint32_t, uint32_t>> ParsingTextFile::mapInvertedIndexBM25;
std::map<std::string, std::unordered_map<uint32_t, uint32_t>> ParsingTextFile::mapSignedInvertedIndexBM25;

std::map<std::string, std::vector<uint32_t>*> ParsingTextFile::mapSignedInvertedIndex;
std::map<std::string, std::vector<uint32_t>*> ParsingTextFile::mapInvertedIndex;

bool ParsingTextFile::isRuning = false;

ParsingTextFile::ParsingTextFile(std::string pathFile)
{
    this->pathFile = pathFile;
    file = std::ifstream(pathFile);
}

ParsingTextFile::~ParsingTextFile()
{
    file.close();
}

uint32_t ParsingTextFile::parsingToMap(ParsingTextFile* p, const std::string& data, const uint32_t uniqueID, bool flag)
{
    std::string rawdata;

    if (flag)
    {
        rawdata = p->convertUnsignedString(data);
    }
    else
    {
        rawdata = data;
    }

    std::regex rgx ("\\s+");
    std::sregex_token_iterator iter(rawdata.begin(), rawdata.end(), rgx, -1);
    std::sregex_token_iterator end;

    std::unordered_map<uint32_t, uint32_t> uniqueIdBM25;

    uint32_t countWord = 0;
    while (iter != end)
    {
        countWord++;
        std::unique_lock<std::mutex> lock(mMapInvertedIndex);

        if (flag)
        {
            auto itr = mapInvertedIndex.find(*iter);
            auto itrBm25 = mapInvertedIndexBM25.find(*iter);
            if (itr != mapInvertedIndex.end())
            {
                itr->second->push_back(uniqueID);
                itrBm25->second[uniqueID]++;
            }
            else
            {
                std::vector<uint32_t>* vec = new std::vector<uint32_t>;
                vec->push_back(uniqueID);
                mapInvertedIndex.emplace(*iter, vec);

                uniqueIdBM25[uniqueID] = 1;
                mapInvertedIndexBM25.emplace(*iter, uniqueIdBM25);
            }
        }
        else
        {
            auto itrBm25 = mapSignedInvertedIndexBM25.find(*iter);
            auto itr = mapSignedInvertedIndex.find(*iter);
            if (itr != mapSignedInvertedIndex.end())
            {
                itrBm25->second[uniqueID]++;
                itr->second->push_back(uniqueID);
            }
            else
            {
                std::vector<uint32_t>* vec = new std::vector<uint32_t>;
                vec->push_back(uniqueID);
                mapSignedInvertedIndex.emplace(*iter, vec);
                uniqueIdBM25[uniqueID] = 1;
                mapSignedInvertedIndexBM25.emplace(*iter, uniqueIdBM25);
            }
        }
        lock.unlock();
        iter++;
    }
    return countWord;
}

void ParsingTextFile::worker(ParsingTextFile* p)
{
    while(1)
    {
        std::unique_lock<std::mutex> lock(mContainer);

        while (container.empty())
        {
            if (isRuning)
            {
                return;
            }
            condNotify.wait(lock);
        }
        std::pair<uint32_t, std::string>* pairData = container.front();
        container.pop();
        lock.unlock();

        std::pair<std::string, uint32_t> pairHashTable;
        pairHashTable.first = pairData->second;

        // Build maps for unsigned string
        pairHashTable.second = p->parsingToMap(p, pairData->second, pairData->first, true);
        // Build maps for signed string
        p->parsingToMap(p, pairData->second, pairData->first, false);

        lock.lock();
        hashTable[(uint32_t)pairData->first] = pairHashTable;
        lock.unlock();
        delete pairData;
    }
}

void ParsingTextFile::build()
{
    uint32_t count = 0;
    std::string rawdata;

    for (int i = 0; i < MAX_THEARD; ++i)
    {
        thWorker[i] = std::thread(worker, this);
    }

    while(file.good())
    {
        count++;
        std::getline(file, rawdata);
        std::unique_lock<std::mutex> lock(mContainer);
        std::pair<uint32_t, std::string>* p = new std::pair<uint32_t, std::string>;
        p->first = count;
        p->second = rawdata;

        container.push(p);
        condNotify.notify_all();
        lock.unlock();
    }
    isRuning = true;
    file.close();

    for (int i = 0; i < MAX_THEARD; ++i)
    {
        thWorker[i].join();
    }
}

std::string ParsingTextFile::convertUnsignedString(const std::string& input)
{
    std::string output = input;
    for (auto& itr : convertTable)
    {
        output = std::regex_replace(output, std::regex(itr.second), itr.first);
    }
    return output;
}

uint32_t ParsingTextFile::getLineOfFile()
{
    FILE *fp;
    char result[10];
    const std::string command = "wc -l " + pathFile + " | cut -d \" \" -f1";
    fp = popen(command.c_str(), "r");

    if (fp == nullptr)
    {
        std::cout << "Can not execute the command: " << command << std::endl;
        return 0;
    }

    fgets(result, 10, fp);
    return std::atoi(result);
}
