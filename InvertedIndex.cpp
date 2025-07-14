#include "InvertedIndex.h"
#include <thread>
#include <sstream>
#include <algorithm>
#include <mutex>

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
    docs = std::move(input_docs); // Перемещаем данные вместо копирования
    freq_dictionary.clear(); // Очищаем предыдущий индекс

    std::vector<std::thread> threads;
    threads.reserve(docs.size()); // Оптимизация выделения памяти

    // Запускаем потоки для индексации каждого документа
    for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
        threads.emplace_back([this, doc_id]() {
            indexDocument(doc_id, docs[doc_id]);
        });
    }

    // Ожидаем завершения всех потоков
    for (auto& thread : threads) {
        thread.join();
    }
}

void InvertedIndex::indexDocument(size_t doc_id, const std::string& doc_content) {
    std::map<std::string, size_t> word_counts; // Локальный счетчик слов
    std::stringstream ss(doc_content);
    std::string word;

    // Разбиваем текст на слова и считаем частоту
    while (ss >> word) {
        // Приводим слово к lowercase для регистронезависимого поиска
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c){ return std::tolower(c); });
        word_counts[word]++;
    }

    // Блокируем доступ к общему словарю
    std::lock_guard<std::mutex> lock(dict_mutex);
    
    // Обновляем глобальный индекс
    for (const auto& [word, count] : word_counts) {
        freq_dictionary[word].push_back({doc_id, count});
    }
}

std::vector<Entry> InvertedIndex::GetWordCount(const std::string& word) {
    std::string lowercase_word = word;
    std::transform(lowercase_word.begin(), lowercase_word.end(), lowercase_word.begin(),
        [](unsigned char c){ return std::tolower(c); });

    std::lock_guard<std::mutex> lock(dict_mutex); // Для потокобезопасного чтения
    
    auto it = freq_dictionary.find(lowercase_word);
    if (it != freq_dictionary.end()) {
        // Возвращаем копию данных (потокобезопасно)
        return it->second;
    }
    return {};
}
