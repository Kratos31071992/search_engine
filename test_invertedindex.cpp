#include <gtest/gtest.h>
#include "InvertedIndex.h"

using namespace std;

// Тестовые данные
const vector<string> docs = {
    "milk milk milk milk water water water",
    "milk water water",
    "milk milk milk milk milk water water water water water",
    "americano cappuccino"
};

const vector<string> requests = {"milk", "water", "cappuccino", "sugar"};

const vector<vector<Entry>> expected = {
    {{0, 4}, {1, 1}, {2, 5}},  // milk
    {{0, 3}, {1, 2}, {2, 5}},   // water
    {{3, 1}},                    // cappuccino
    {}                           // sugar (not found)
};

// Тестовый класс
class InvertedIndexTest : public ::testing::Test {
protected:
    InvertedIndex idx;

    void SetUp() override {
        idx.UpdateDocumentBase(docs);
    }
};

// Проверка базовой функциональности
TEST_F(InvertedIndexTest, BasicWordCount) {
    vector<Entry> result = idx.GetWordCount("milk");
    ASSERT_EQ(result.size(), expected[0].size());
    for (size_t i = 0; i < result.size(); ++i) {
        EXPECT_EQ(result[i].doc_id, expected[0][i].doc_id);
        EXPECT_EQ(result[i].count, expected[0][i].count);
    }
}

// Проверка отсутствующего слова
TEST_F(InvertedIndexTest, MissingWord) {
    vector<Entry> result = idx.GetWordCount("sugar");
    EXPECT_TRUE(result.empty());
}

// Проверка многопоточной индексации
TEST_F(InvertedIndexTest, MultiThreadConsistency) {
    const int thread_count = 10;
    vector<thread> threads;
    vector<vector<Entry>> results(thread_count);

    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back([this, &results, i]() {
            results[i] = idx.GetWordCount(requests[i % requests.size()]);
        });
    }

    for (auto& t : threads) {
        t.join();
    }

    // Проверка согласованности результатов
    for (int i = 0; i < thread_count; ++i) {
        auto expected_result = expected[i % requests.size()];
        ASSERT_EQ(results[i].size(), expected_result.size());
    }
}

// Проверка обработки пустых документов
TEST_F(InvertedIndexTest, EmptyDocuments) {
    InvertedIndex empty_idx;
    empty_idx.UpdateDocumentBase({});
    EXPECT_TRUE(empty_idx.GetWordCount("milk").empty());
}

// Проверка регистронезависимости
TEST_F(InvertedIndexTest, CaseInsensitive) {
    vector<Entry> lower = idx.GetWordCount("milk");
    vector<Entry> upper = idx.GetWordCount("MILK");
    vector<Entry> mixed = idx.GetWordCount("MiLk");

    ASSERT_EQ(lower.size(), upper.size());
    ASSERT_EQ(lower.size(), mixed.size());
}

// Проверка обновления базы документов
TEST_F(InvertedIndexTest, UpdateDocumentBase) {
    idx.UpdateDocumentBase({"new document with unique words"});
    EXPECT_FALSE(idx.GetWordCount("unique").empty());
}

// Проверка корректности подсчета частот
TEST_F(InvertedIndexTest, FrequencyCalculation) {
    auto result = idx.GetWordCount("water");
    ASSERT_EQ(result.size(), 3);
    EXPECT_EQ(result[0].doc_id, 0);
    EXPECT_EQ(result[0].count, 3);
    EXPECT_EQ(result[1].doc_id, 1);
    EXPECT_EQ(result[1].count, 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
