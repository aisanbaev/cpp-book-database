#include "book.hpp"
#include "book_database.hpp"
#include "filters.hpp"
#include <gtest/gtest.h>
#include <vector>

using namespace bookdb;

class FiltersTest : public ::testing::Test {
protected:
    void SetUp() override {
        testBooks = {Book("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190),
                     Book("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143),
                     Book("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120),
                     Book("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98),
                     Book("Do Androids Dream", "Philip K. Dick", 1968, Genre::SciFi, 4.2, 85),
                     Book("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178)};
    }

    std::vector<Book> testBooks;
};

TEST_F(FiltersTest, YearBetween) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2], testBooks[5]};

    auto filter = YearBetween(1900, 1950);
    EXPECT_TRUE(filter(testBooks[0]));   // 1949
    EXPECT_TRUE(filter(testBooks[1]));   // 1945
    EXPECT_TRUE(filter(testBooks[2]));   // 1925
    EXPECT_FALSE(filter(testBooks[5]));  // 1813
}

TEST_F(FiltersTest, RatingAbove) {
    auto filter = RatingAbove(4.5);
    EXPECT_FALSE(filter(testBooks[0]));  // 4.0
    EXPECT_FALSE(filter(testBooks[1]));  // 4.4
    EXPECT_TRUE(filter(testBooks[2]));   // 4.5
    EXPECT_TRUE(filter(testBooks[3]));   // 4.5
    EXPECT_TRUE(filter(testBooks[5]));   // 4.7
}

TEST_F(FiltersTest, GenreIs) {
    auto fiction_filter = GenreIs(Genre::Fiction);
    auto scifi_filter = GenreIs(Genre::SciFi);

    EXPECT_FALSE(fiction_filter(testBooks[0]));  // SciFi
    EXPECT_TRUE(fiction_filter(testBooks[1]));   // Fiction
    EXPECT_TRUE(fiction_filter(testBooks[2]));   // Fiction
    EXPECT_TRUE(scifi_filter(testBooks[0]));     // SciFi
    EXPECT_FALSE(scifi_filter(testBooks[1]));    // Fiction
}

TEST_F(FiltersTest, FilterBooksWithIterators) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2], testBooks[3]};

    auto result = filterBooks(db.begin(), db.end(), all_of(YearBetween(1930, 1960), RatingAbove(4.0)));

    // Должны найти:
    // 1984 (1949, 4.0)
    // Brave New World (1932, 4.5)
    // Animal Farm (1945, 4.4)
    EXPECT_EQ(result.size(), 3);
}
