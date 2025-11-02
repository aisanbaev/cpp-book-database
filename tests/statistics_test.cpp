#include "book.hpp"
#include "book_database.hpp"
#include "comparators.hpp"
#include "statistics.hpp"
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

using namespace bookdb;

BookDatabase<std::vector<Book>> createTestDatabase() {
    BookDatabase<std::vector<Book>> db;
    db.EmplaceBack("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    db.EmplaceBack("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 120);
    db.EmplaceBack("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.7, 150);
    db.EmplaceBack("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.8, 200);
    db.EmplaceBack("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Mystery, 4.3, 80);
    db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.6, 90);
    return db;
}

TEST(StatisticsTest, BuildAuthorHistogramFlat) {
    auto db = createTestDatabase();
    auto histogram = buildAuthorHistogramFlat(db);

    EXPECT_EQ(histogram.size(), 5);  // 5 уникальных авторов

    EXPECT_EQ(histogram.at("Aldous Huxley"), 1);
    EXPECT_EQ(histogram.at("F. Scott Fitzgerald"), 1);
    EXPECT_EQ(histogram.at("George Orwell"), 2);  // Две книги
    EXPECT_EQ(histogram.at("Jane Austen"), 1);
    EXPECT_EQ(histogram.at("J.D. Salinger"), 1);
}

TEST(StatisticsTest, CalculateGenreRatings) {
    auto db = createTestDatabase();
    auto genre_ratings = calculateGenreRatings(db.begin(), db.end());

    EXPECT_EQ(genre_ratings.size(), 3);  // SciFi, Fiction, Mystery

    // Проверим средние рейтинги
    // SciFi: (4.5 + 4.6) / 2 = 4.55
    EXPECT_NEAR(genre_ratings.at(Genre::SciFi), 4.55, 1e-6);
    // Fiction: (4.4 + 4.7 + 4.8) / 3 = 4.6333...
    EXPECT_NEAR(genre_ratings.at(Genre::Fiction), 4.6333333, 1e-6);
    // Mystery: 4.3 / 1 = 4.3
    EXPECT_NEAR(genre_ratings.at(Genre::Mystery), 4.3, 1e-6);
}

TEST(StatisticsTest, CalculateAverageRating) {
    auto db = createTestDatabase();
    double avg_rating = calculateAverageRating(db);

    // Общий рейтинг: 4.5 + 4.4 + 4.7 + 4.8 + 4.3 + 4.6 = 27.3
    // Количество книг: 6
    // Среднее: 27.3 / 6 = 4.55
    EXPECT_DOUBLE_EQ(avg_rating, 27.3 / 6);
}

TEST(StatisticsTest, CalculateAverageRatingEmptyDatabase) {
    BookDatabase<std::vector<Book>> empty_db;
    EXPECT_THROW(calculateAverageRating(empty_db), std::domain_error);
}

TEST(StatisticsTest, SampleRandomBooks) {
    auto db = createTestDatabase();
    size_t sample_size = 3;

    auto sampled = sampleRandomBooks(db, sample_size);

    EXPECT_EQ(sampled.size(), sample_size);
    EXPECT_FALSE(sampled.empty());

    EXPECT_THROW(sampleRandomBooks(db, db.size() + 1), std::invalid_argument);
}

TEST(StatisticsTest, GetTopNByRating) {
    auto db = createTestDatabase();
    size_t n = 3;

    // Создаём копию, так как getTopNBy модифицирует контейнер
    auto db_copy = db;
    auto top_books = getTopNBy(db_copy, n, comp::LessByRating{});
    EXPECT_EQ(top_books.size(), n);

    // Проверим порядок (рейтинги: 4.8, 4.7, 4.6, 4.5, 4.4, 4.3)
    // После сортировки по убыванию, первые 3 должны быть 4.8, 4.7, 4.6
    EXPECT_DOUBLE_EQ(top_books[0].get().rating, 4.8);  // Pride and Prejudice
    EXPECT_DOUBLE_EQ(top_books[1].get().rating, 4.7);  // The Great Gatsby
    EXPECT_DOUBLE_EQ(top_books[2].get().rating, 4.6);  // Brave New World
}

TEST(StatisticsTest, GetTopNByRatingZero) {
    auto db = createTestDatabase();
    auto db_copy = db;
    auto top_books = getTopNBy(db_copy, 0, comp::LessByRating{});

    EXPECT_TRUE(top_books.empty());
}

TEST(StatisticsTest, GetTopNByRatingMoreThanAvailable) {
    auto db = createTestDatabase();
    auto db_copy = db;
    auto top_books = getTopNBy(db_copy, 10, comp::LessByRating{});

    // Должно вернуть все 6 книг, отсортированные
    EXPECT_EQ(top_books.size(), db.size());
    EXPECT_DOUBLE_EQ(top_books[0].get().rating, 4.8);
    EXPECT_DOUBLE_EQ(top_books.back().get().rating, 4.3);
}

TEST(StatisticsTest, GetTopNByRatingEmptyDatabase) {
    BookDatabase<std::vector<Book>> empty_db;
    auto top_books = getTopNBy(empty_db, 3, comp::LessByRating{});
    EXPECT_TRUE(top_books.empty());
}
