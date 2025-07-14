#include "SearchServer.h"
#include <algorithm>
#include <numeric>
#include <cmath>

std::vector<std::vector<RelativeIndex>> SearchServer::search(
    const std::vector<std::string>& queries_input) 
{
    std::vector<std::vector<RelativeIndex>> results;
    
    for (const auto& query : queries_input) {
        // Обрабатываем каждый запрос
        auto found_docs = processQuery(query);
        
        // Рассчитываем относительную релевантность
        auto relative_ranks = calculateRelativeRank(found_docs);
        
        // Сортируем по убыванию релевантности
        std::sort(relative_ranks.begin(), relative_ranks.end(),
            [](const RelativeIndex& a, const RelativeIndex& b) {
                return a.rank > b.rank;
            });
        
        results.push_back(std::move(relative_ranks));
    }
    
    return results;
}

std::vector<std::pair<size_t, float>> SearchServer::processQuery(
    const std::string& query) 
{
    std::vector<std::pair<size_t, float>> doc_relevance;
    std::map<size_t, float> doc_abs_rank; // doc_id -> суммарная релевантность
    
    std::stringstream ss(query);
    std::string word;
    std::vector<std::string> words;
    
    // Разбиваем запрос на уникальные слова
    while (ss >> word) {
        std::transform(word.begin(), word.end(), word.begin(),
            [](unsigned char c){ return std::tolower(c); });
        if (std::find(words.begin(), words.end(), word) == words.end()) {
            words.push_back(word);
        }
    }
    
    if (words.empty()) return {};
    
    // Ищем документы, содержащие все слова запроса
    std::vector<std::vector<Entry>> word_entries;
    for (const auto& word : words) {
        auto entries = index.GetWordCount(word);
        if (entries.empty()) {
            return {}; // Если хотя бы одно слово не найдено
        }
        word_entries.push_back(entries);
    }
    
    // Сортируем слова по возрастанию частоты (от самых редких)
    std::sort(word_entries.begin(), word_entries.end(),
        [](const std::vector<Entry>& a, const std::vector<Entry>& b) {
            return a.size() < b.size();
        });
    
    // Находим пересечение документов (где есть все слова)
    std::vector<size_t> common_docs;
    for (const auto& entry : word_entries[0]) {
        common_docs.push_back(entry.doc_id);
    }
    
    for (size_t i = 1; i < word_entries.size(); ++i) {
        std::vector<size_t> temp;
        std::set_intersection(
            common_docs.begin(), common_docs.end(),
            word_entries[i].begin(), word_entries[i].end(),
            std::back_inserter(temp),
            [](const Entry& a, size_t b) { return a.doc_id < b; },
            [](size_t a, const Entry& b) { return a < b.doc_id; }
        );
        common_docs = std::move(temp);
        if (common_docs.empty()) break;
    }
    
    // Рассчитываем абсолютную релевантность
    for (size_t doc_id : common_docs) {
        float rank = 0;
        for (const auto& entries : word_entries) {
            for (const auto& entry : entries) {
                if (entry.doc_id == doc_id) {
                    rank += entry.count;
                    break;
                }
            }
        }
        doc_abs_rank[doc_id] = rank;
    }
    
    // Преобразуем в вектор пар
    std::vector<std::pair<size_t, float>> result;
    for (const auto& [doc_id, rank] : doc_abs_rank) {
        result.emplace_back(doc_id, rank);
    }
    
    return result;
}

std::vector<RelativeIndex> SearchServer::calculateRelativeRank(
    const std::vector<std::pair<size_t, float>>& found_docs) 
{
    if (found_docs.empty()) {
        return {};
    }
    
    // Находим максимальную абсолютную релевантность
    float max_rank = 0;
    for (const auto& [doc_id, rank] : found_docs) {
        if (rank > max_rank) {
            max_rank = rank;
        }
    }
    
    if (max_rank == 0) {
        return {};
    }
    
    // Рассчитываем относительную релевантность
    std::vector<RelativeIndex> result;
    for (const auto& [doc_id, rank] : found_docs) {
        result.push_back({doc_id, rank / max_rank});
    }
    
    return result;
}
