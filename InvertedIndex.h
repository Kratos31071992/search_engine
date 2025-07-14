#ifndef INVERTEDINDEX_H
#define INVERTEDINDEX_H

#include <string>
#include <vector>
#include <map>
#include <mutex>
#include <thread>
#include <algorithm>

/**
 * @brief Структура для хранения информации о вхождении слова в документ
 */
struct Entry {
    size_t doc_id;    // Идентификатор документа
    size_t count;     // Количество вхождений слова в документ

    // Оператор для сравнения (необходим для тестов)
    bool operator==(const Entry& other) const {
        return (doc_id == other.doc_id && count == other.count);
    }
};

/**
 * @brief Класс для построения инвертированного индекса документов
 */
class InvertedIndex {
public:
    InvertedIndex() = default;

    /**
     * @brief Обновляет или заполняет базу документов для поиска
     * @param input_docs Содержимое документов для индексации
     */
    void UpdateDocumentBase(std::vector<std::string> input_docs);

    /**
     * @brief Получает количество вхождений слова в документы
     * @param word Искомое слово
     * @return Вектор вхождений слова в документы
     */
    std::vector<Entry> GetWordCount(const std::string& word);

private:
    std::vector<std::string> docs;                    // Хранит содержимое документов
    std::map<std::string, std::vector<Entry>> freq_dictionary; // Частотный словарь
    std::mutex dict_mutex;                           // Мьютекс для потокобезопасности

    /**
     * @brief Индексирует отдельный документ
     * @param doc_id Идентификатор документа
     * @param doc_content Содержимое документа
     */
    void indexDocument(size_t doc_id, const std::string& doc_content);
};

#endif // INVERTEDINDEX_H
