#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <algorithm>
#include <stdexcept>

struct Entry {
    size_t doc_id;
    size_t count;

    bool operator==(const Entry& other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};

class InvertedIndex {
public:
    InvertedIndex() = default;

    /**
     * @brief Обновляет базу документов с обработкой исключений
     * @throws std::runtime_error если входные данные пусты
     */
    void UpdateDocumentBase(std::vector<std::string> input_docs);

    std::vector<Entry> GetWordCount(const std::string& word) const;

private:
    std::vector<std::string> docs;
    mutable std::mutex dict_mutex;
    std::map<std::string, std::vector<Entry>> freq_dictionary;

    void indexDocument(size_t doc_id, const std::string& doc_content);
};

#endif // INVERTEDINDEX_H
