#ifndef SEARCHSERVER_H
#define SEARCHSERVER_H

#include "InvertedIndex.h"
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <map>

/**
 * @struct RelativeIndex
 * @brief Структура для хранения относительной релевантности документа
 */
struct RelativeIndex {
    size_t doc_id;     // Идентификатор документа
    float rank;        // Относительная релевантность (0..1)

    // Оператор сравнения для тестирования
    bool operator==(const RelativeIndex& other) const {
        return (doc_id == other.doc_id && fabs(rank - other.rank) < 1e-6);
    }

    // Оператор для удобного преобразования в pair
    operator std::pair<int, float>() const {
        return {static_cast<int>(doc_id), rank};
    }
};

/**
 * @class SearchServer
 * @brief Класс для выполнения поисковых запросов и ранжирования результатов
 */
class SearchServer {
public:
    /**
     * @brief Конструктор
     * @param idx Ссылка на инвертированный индекс
     */
    explicit SearchServer(InvertedIndex& idx) : index(idx) {}

    /**
     * @brief Выполняет поиск по запросам
     * @param queries_input Вектор поисковых запросов
     * @return Вектор результатов для каждого запроса (отсортированный по убыванию релевантности)
     */
    std::vector<std::vector<RelativeIndex>> search(const std::vector<std::string>& queries_input);

private:
    InvertedIndex& index;  // Ссылка на инвертированный индекс

    /**
     * @brief Обрабатывает один поисковый запрос
     * @param query Поисковый запрос
     * @return Вектор пар (doc_id, абсолютная_релевантность)
     */
    std::vector<std::pair<size_t, float>> processQuery(const std::string& query);

    /**
     * @brief Вычисляет относительную релевантность документов
     * @param found_docs Вектор пар (doc_id, абсолютная_релевантность)
     * @return Вектор RelativeIndex с относительной релевантностью (0..1)
     */
    std::vector<RelativeIndex> calculateRelativeRank(const std::vector<std::pair<size_t, float>>& found_docs);

    /**
     * @brief Преобразует результаты в формат для ConverterJSON
     * @param results Результаты поиска в формате RelativeIndex
     * @return Результаты в формате пар (int, float)
     */
    std::vector<std::vector<std::pair<int, float>>> convertResults(
        const std::vector<std::vector<RelativeIndex>>& results);
};

#endif // SEARCHSERVER_H
