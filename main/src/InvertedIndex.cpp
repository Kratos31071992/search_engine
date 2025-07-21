#include "InvertedIndex.h"
#include <sstream>
#include <algorithm>
#include <future>
#include <vector>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    if (input_docs.empty()) {
        throw std::runtime_error("Input documents cannot be empty");
    }

    docs = std::move(input_docs);
    freq_dictionary.clear();

    std::vector<std::future<void>> futures;
    std::vector<std::exception_ptr> exceptions;

    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        if (docs[doc_id].empty()) {
            continue; // Пропускаем пустые документы
        }

        futures.emplace_back(std::async(std::launch::async, [this, doc_id, &exceptions]() {
            try {
                indexDocument(doc_id, docs[doc_id]);
            } catch (...) {
                std::lock_guard<std::mutex> lock(dict_mutex);
                exceptions.push_back(std::current_exception());
            }
        }));
    }

    // Ожидаем завершения всех потоков
    for (auto& future : futures) {
        future.get();
    }

    // Проверяем исключения
    if (!exceptions.empty()) {
        std::rethrow_exception(exceptions.front());
    }
}

void InvertedIndex::indexDocument(size_t doc_id, const std::string& doc_content) {
    if (doc_content.empty()) {
        return;
    }

    std::map<std::string, size_t> word_counts;
    std::istringstream iss(doc_content);
    std::string word;

    while (iss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c){ return std::tolower(c); });
        ++word_counts[word];
    }

    std::lock_guard<std::mutex> lock(dict_mutex);
    for (const auto& [word, count] : word_counts) {
        freq_dictionary[word].push_back({doc_id, count});
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) const {
    std::string lowercase_word = word;
    std::transform(lowercase_word.begin(), lowercase_word.end(), lowercase_word.begin(),
        [](unsigned char c){ return std::tolower(c); });

    std::lock_guard<std::mutex> lock(dict_mutex);
    if (auto it = freq_dictionary.find(lowercase_word); it != freq_dictionary.end()) {
        return it->second;
    }
    return {};
}
