#include <iostream>
#include <chrono>
#include "ConverterJSON.h"
#include "InvertedIndex.h"
#include "SearchServer.h"

using namespace std;
using namespace std::chrono;

/**
 * @brief Точка входа в программу
 * @return Код завершения: 0 - успех, 1 - ошибка конфигурации, 2 - ошибка выполнения
 */
int main() {
    try {
        // 1. Инициализация и загрузка конфигурации
        auto start_time = steady_clock::now();
        cout << "=== Starting Search Engine ===" << endl;
        
        ConverterJSON converter;
        cout << "✓ Config loaded: " << converter.readConfig()["config"]["name"] << endl;

        // 2. Индексация документов
        cout << "\n[1/3] Indexing documents..." << endl;
        InvertedIndex index;
        
        auto docs_start = steady_clock::now();
        vector<string> documents = converter.GetTextDocuments();
        index.UpdateDocumentBase(documents);
        
        auto docs_ms = duration_cast<milliseconds>(steady_clock::now() - docs_start).count();
        cout << "✓ Indexed " << documents.size() << " docs in " << docs_ms << " ms" << endl;

        // 3. Обработка поисковых запросов
        cout << "\n[2/3] Processing requests..." << endl;
        SearchServer server(index);
        
        auto queries_start = steady_clock::now();
        vector<string> requests = converter.GetRequests();
        auto results = server.search(requests);
        
        auto queries_ms = duration_cast<milliseconds>(steady_clock::now() - queries_start).count();
        cout << "✓ Processed " << requests.size() << " queries in " << queries_ms << " ms" << endl;

        // 4. Сохранение результатов
        cout << "\n[3/3] Saving answers..." << endl;
        converter.putAnswers(results);
        cout << "✓ Results saved to answers.json" << endl;

        // 5. Статистика работы
        auto total_ms = duration_cast<milliseconds>(steady_clock::now() - start_time).count();
        cout << "\n=== Completed in " << total_ms << " ms ===" << endl;
        
        return 0;

    } catch (const exception& e) {
        cerr << "\n! ERROR: " << e.what() << endl;
        return 1;
    } catch (...) {
        cerr << "\n! Unknown error occurred" << endl;
        return 2;
    }
}
