#include "book.hpp"
#include <gtest/gtest.h>

using namespace bookdb;

TEST(BookTest, ConstructorWithGenreEnum) {
    Book book("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);

    EXPECT_EQ(book.title, "1984");
    EXPECT_EQ(book.author, "George Orwell");
    EXPECT_EQ(book.year, 1949);
    EXPECT_EQ(book.genre, Genre::SciFi);
    EXPECT_DOUBLE_EQ(book.rating, 4.5);
    EXPECT_EQ(book.read_count, 100);
}

TEST(BookTest, ConstructorWithGenreString) {
    Book book("The Great Gatsby", "F. Scott Fitzgerald", 1925, "Fiction", 4.7, 150);

    EXPECT_EQ(book.title, "The Great Gatsby");
    EXPECT_EQ(book.author, "F. Scott Fitzgerald");
    EXPECT_EQ(book.year, 1925);
    EXPECT_EQ(book.genre, Genre::Fiction);
    EXPECT_DOUBLE_EQ(book.rating, 4.7);
    EXPECT_EQ(book.read_count, 150);
}

TEST(BookTest, ConstructorWithGenreStringUnknown) {
    Book book("Unknown Book", "Unknown Author", 2000, "InvalidGenre", 3.0, 10);

    EXPECT_EQ(book.genre, Genre::Unknown);
}

TEST(BookTest, StringViewAuthorAndStringTitle) {
    std::string author_name = "J.K. Rowling";
    Book book("Harry Potter", author_name, 1997, Genre::Fiction, 4.8, 200);
    EXPECT_EQ(book.author, author_name);
    EXPECT_EQ(book.title, "Harry Potter");
}

TEST(BookTest, BookWithExtremeValues) {
    Book min_book("Min Book", "Min Author", 0, Genre::Unknown, 0.0, 0);
    EXPECT_EQ(min_book.year, 0);
    EXPECT_DOUBLE_EQ(min_book.rating, 0.0);
    EXPECT_EQ(min_book.read_count, 0);

    Book max_book("Max Book", "Max Author", 9999, Genre::Biography, 5.0, 1000000);
    EXPECT_EQ(max_book.year, 9999);
    EXPECT_DOUBLE_EQ(max_book.rating, 5.0);
    EXPECT_EQ(max_book.read_count, 1000000);
}

TEST(BookTest, NegativeValuesHandling) {
    Book negative_book("Negative", "Author", -100, Genre::NonFiction, -1.0, -50);

    EXPECT_EQ(negative_book.year, -100);
    EXPECT_DOUBLE_EQ(negative_book.rating, -1.0);
    EXPECT_EQ(negative_book.read_count, -50);
}

TEST(BookTest, MultipleBooksSameAuthor) {
    Book book1("Book One", "Same Author", 2001, Genre::Fiction, 4.0, 100);
    Book book2("Book Two", "Same Author", 2002, Genre::Mystery, 4.2, 150);

    EXPECT_EQ(book1.author, book2.author);
    EXPECT_NE(book1.title, book2.title);
    EXPECT_NE(book1.genre, book2.genre);
}

TEST(BookTest, GenreConsistencyBetweenConstructors) {
    Book book1("Test Book", "Test Author", 2000, Genre::SciFi, 4.0, 100);
    Book book2("Test Book", "Test Author", 2000, "SciFi", 4.0, 100);

    EXPECT_EQ(book1.genre, book2.genre);
    EXPECT_EQ(book1.title, book2.title);
    EXPECT_EQ(book1.author, book2.author);
}

TEST(BookTest, EmptyStringsHandling) {
    Book empty_book("", "", 0, Genre::Unknown, 0.0, 0);

    EXPECT_TRUE(empty_book.title.empty());
    EXPECT_TRUE(empty_book.author.empty());
}

TEST(BookTest, FormatterGenre) {
    std::formatter<bookdb::Genre, char> formatter;

    EXPECT_EQ(std::format("{}", Genre::Fiction), "Fiction");
    EXPECT_EQ(std::format("{}", Genre::NonFiction), "NonFiction");
    EXPECT_EQ(std::format("{}", Genre::SciFi), "SciFi");
    EXPECT_EQ(std::format("{}", Genre::Biography), "Biography");
    EXPECT_EQ(std::format("{}", Genre::Mystery), "Mystery");
    EXPECT_EQ(std::format("{}", Genre::Unknown), "Unknown");
}

TEST(BookTest, FormatterBook) {
    Book book("1984", "George Orwell", 1949, Genre::SciFi, 4.5, 100);
    std::string formatted = std::format("{}", book);

    EXPECT_TRUE(formatted.find("George Orwell") != std::string::npos);
    EXPECT_TRUE(formatted.find("1984") != std::string::npos);
    EXPECT_TRUE(formatted.find("1949") != std::string::npos);
    EXPECT_TRUE(formatted.find("SciFi") != std::string::npos);
    EXPECT_TRUE(formatted.find("4.5") != std::string::npos);
    EXPECT_TRUE(formatted.find("100") != std::string::npos);
}

TEST(BookTest, FormatterBookCompleteFormat) {
    Book book("The Hobbit", "J.R.R. Tolkien", 1937, Genre::Fiction, 4.9, 203);

    std::string expected = "J.R.R. Tolkien: The Hobbit (1937, Fiction, 4.9, 203)";
    std::string actual = std::format("{}", book);

    EXPECT_EQ(actual, expected);
}

TEST(BookTest, ConstexprConstructors) {
    static constexpr Book book1("Title1", "Author1", 2000, Genre::Fiction, 4.0, 100);
    static constexpr Book book2("Title2", "Author2", 2001, "SciFi", 4.5, 200);

    static_assert(book1.year == 2000);
    static_assert(book1.genre == Genre::Fiction);
    static_assert(book2.genre == Genre::SciFi);

    static_assert(GenreFromString("Fiction") == Genre::Fiction);
    static_assert(GenreFromString("NonExistent") == Genre::Unknown);
}