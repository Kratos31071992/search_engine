#include <gtest/gtest.h>
#include "SearchServer.h"
#include "InvertedIndex.h"
#include "ConverterJSON.h"

using namespace std;

// Тестовые данные
const vector<string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuccino",
    "espresso latte with milk and sugar",
    "coffee with milk and milk foam"
};

class SearchServerTest : public ::testing::Test {
protected:
    InvertedIndex idx;
    SearchServer server;

    void SetUp() override {
        idx.UpdateDocumentBase(docs);
        server = SearchServer(idx);
    }
};

// Тест базового поиска
TEST_F(SearchServerTest, SimpleSearch) {
    vector<string> request = {"milk water"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].size(), 3);
    
    // Проверка порядка и релевантности
    EXPECT_EQ(result[0][0].doc_id, 2);
    EXPECT_FLOAT_EQ(result[0][0].rank, 1.0f);
    
    EXPECT_EQ(result[0][1].doc_id, 0);
    EXPECT_NEAR(result[0][1].rank, 0.7f, 0.01f);
    
    EXPECT_EQ(result[0][2].doc_id, 1);
    EXPECT_NEAR(result[0][2].rank, 0.3f, 0.01f);
}

// Тест поиска по отсутствующему слову
TEST_F(SearchServerTest, MissingWordSearch) {
    vector<string> request = {"milk", "unknownword"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    ASSERT_EQ(result.size(), 2);
    EXPECT_FALSE(result[0].empty());
    EXPECT_TRUE(result[1].empty());
}

// Тест ранжирования с несколькими словами
TEST_F(SearchServerTest, MultiWordRanking) {
    vector<string> request = {"milk and"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    ASSERT_EQ(result.size(), 1);
    ASSERT_EQ(result[0].size(), 2);
    
    // Документ 5 содержит "milk" дважды
    EXPECT_EQ(result[0][0].doc_id, 5);
    EXPECT_GT(result[0][0].rank, result[0][1].rank);
}

// Тест пустого запроса
TEST_F(SearchServerTest, EmptyQuery) {
    vector<string> request = {""};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    EXPECT_TRUE(result[0].empty());
}

// Тест максимального количества ответов
TEST_F(SearchServerTest, MaxResponsesLimit) {
    ConverterJSON converter;
    int max_responses = converter.GetResponsesLimit();
    
    vector<string> request = {"milk"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    ASSERT_LE(result[0].size(), max_responses);
}

// Тест регистронезависимости
TEST_F(SearchServerTest, CaseInsensitiveSearch) {
    vector<string> request = {"MILK", "Cappuccino"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    ASSERT_EQ(result.size(), 2);
    EXPECT_FALSE(result[0].empty());
    EXPECT_FALSE(result[1].empty());
}

// Тест обработки стоп-слов (опционально)
TEST_F(SearchServerTest, StopWordsHandling) {
    vector<string> request = {"coffee with milk"};
    vector<vector<RelativeIndex>> result = server.search(request);
    
    // Ожидаем найти по словам "coffee" и "milk", игнорируя "with"
    ASSERT_EQ(result[0].size(), 2);
}

// Тест многопоточной обработки запросов
TEST_F(SearchServerTest, MultiThreadQueryProcessing) {
    vector<string> requests(100, "milk"); // 100 одинаковых запросов
    
    vector<vector<vector<RelativeIndex>>> results;
    vector<thread> threads;
    
    for (int i = 0; i < 4; ++i) {
        threads.emplace_back([&]() {
            results.push_back(server.search(requests));
        });
    }
    
    for (auto& t : threads) t.join();
    
    // Проверка согласованности результатов
    for (const auto& res : results) {
        ASSERT_EQ(res.size(), requests.size());
        for (const auto& r : res) {
            ASSERT_EQ(r.size(), 4); // 4 документа содержат "milk"
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
