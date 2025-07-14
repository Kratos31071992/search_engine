#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

class ConverterJSON {
public:
    ConverterJSON() = default;

    /**
     * Получение содержимого документов
     * @return Список строк с содержимым файлов из config.json
     */
    std::vector<std::string> GetTextDocuments() {
        json config = readConfigFile();
        std::vector<std::string> documents;
        
        if (!config.contains("files")) {
            std::cerr << "Warning: No 'files' section in config.json" << std::endl;
            return documents;
        }

        for (const auto& filePath : config["files"]) {
            try {
                std::ifstream file(filePath);
                if (!file.is_open()) {
                    std::cerr << "File not found: " << filePath << std::endl;
                    continue;
                }
                std::string content((std::istreambuf_iterator<char>(file)), 
                            std::istreambuf_iterator<char>());
                documents.push_back(content);
            } catch (...) {
                std::cerr << "Error reading file: " << filePath << std::endl;
            }
        }
        return documents;
    }

    /**
     * Получение максимального количества ответов
     * @return Максимальное количество ответов (по умолчанию 5)
     */
    int GetResponsesLimit() {
        json config = readConfigFile();
        if (config.contains("config") && config["config"].contains("max_responses")) {
            return config["config"]["max_responses"];
        }
        return 5; // Значение по умолчанию
    }

    /**
     * Получение запросов из файла
     * @return Вектор строк с запросами
     */
    std::vector<std::string> GetRequests() {
        std::ifstream requestsFile("requests.json");
        if (!requestsFile.is_open()) {
            throw std::runtime_error("requests.json file is missing");
        }

        json requestsData;
        requestsFile >> requestsFile;
        requestsFile.close();

        if (!requestsData.contains("requests")) {
            return {};
        }

        std::vector<std::string> requests;
        for (const auto& request : requestsData["requests"]) {
            requests.push_back(request);
        }
        return requests;
    }

    /**
     * Запись ответов в файл
     * @param answers Вектор пар (doc_id, rank)
     */
    void putAnswers(std::vector<std::vector<std::pair<int, float>>> answers) {
        json result;
        size_t requestNum = 1;

        for (const auto& requestAnswers : answers) {
            std::string requestId = "request" + std::to_string(requestNum);
            if (requestNum < 10) requestId.insert(7, "00");
            else if (requestNum < 100) requestId.insert(7, "0");

            if (requestAnswers.empty()) {
                result["answers"][requestId]["result"] = "false";
            } else {
                result["answers"][requestId]["result"] = "true";
                for (const auto& [docId, rank] : requestAnswers) {
                    result["answers"][requestId]["relevance"].push_back({
                        {"docid", docId},
                        {"rank", rank}
                    });
                }
            }
            requestNum++;
        }

        std::ofstream answersFile("answers.json");
        if (!answersFile.is_open()) {
            throw std::runtime_error("Cannot create answers.json");
        }
        answersFile << result.dump(4);
        answersFile.close();
    }

private:
    /**
     * Чтение конфигурационного файла с проверками
     * @return json объект с конфигурацией
     */
    json readConfigFile() {
        std::ifstream configFile("config.json");
        if (!configFile.is_open()) {
            throw std::runtime_error("config file is missing");
        }

        json config;
        configFile >> config;
        configFile.close();

        if (!config.contains("config")) {
            throw std::runtime_error("config file is empty");
        }

        if (config["config"].contains("version") && 
            config["config"]["version"] != "0.1") {
            throw std::runtime_error("config.json has incorrect file version");
        }

        return config;
    }
};

