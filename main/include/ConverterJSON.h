#ifndef CONVERTERJSON_H
#define CONVERTERJSON_H

#include <string>
#include <vector>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @class ConverterJSON
 * @brief Класс для работы с JSON-файлами конфигурации, запросов и ответов
 */
class ConverterJSON {
public:
    /**
     * @brief Конструктор по умолчанию
     * @throws std::runtime_error если config.json не найден или невалиден
     */
    ConverterJSON();

    /**
     * @brief Получает содержимое документов из файлов, указанных в config.json
     * @return Вектор строк с содержимым документов
     * @throws std::runtime_error если файлы документов не найдены
     */
    std::vector<std::string> GetTextDocuments();

    /**
     * @brief Получает максимальное количество ответов на один запрос
     * @return Максимальное количество ответов (по умолчанию 5)
     */
    int GetResponsesLimit() const;

    /**
     * @brief Получает поисковые запросы из файла requests.json
     * @return Вектор строк с запросами
     * @throws std::runtime_error если requests.json не найден
     */
    std::vector<std::string> GetRequests();

    /**
     * @brief Сохраняет результаты поиска в файл answers.json
     * @param answers Вектор результатов для каждого запроса
     *                (пары: doc_id, релевантность)
     */
    void putAnswers(const std::vector<std::vector<std::pair<int, float>>>& answers);

private:
    json config;  // Загруженная конфигурация
    std::string configPath = "config.json";
    std::string requestsPath = "requests.json";
    std::string answersPath = "answers.json";

    /**
     * @brief Загружает и проверяет config.json
     * @throws std::runtime_error при ошибках загрузки
     */
    void loadConfig();

    /**
     * @brief Проверяет обязательные поля в config.json
     * @throws std::runtime_error если поля отсутствуют или неверны
     */
    void validateConfig() const;
};

#endif // CONVERTERJSON_H
