#include "book.hpp"
#include "comparators.hpp"
#include <algorithm>
#include <gtest/gtest.h>
#include <set>
#include <vector>

using namespace bookdb;
using namespace bookdb::comp;

class ComparatorsTest : public ::testing::Test {
protected:
    void SetUp() override {
        books = {Book("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190),
                 Book("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143),
                 Book("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120),
                 Book("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156),
                 Book("Pride and Prejudice", "Jane Austen", 1813, Genre::Fiction, 4.7, 178),
                 Book("The Catcher in the Rye", "J.D. Salinger", 1951, Genre::Fiction, 4.3, 112),
                 Book("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98),
                 Book("Jane Eyre", "Charlotte Brontë", 1847, Genre::Fiction, 4.6, 110)};
    }

    std::vector<Book> books;
};

// Тесты для LessByAuthor
TEST_F(ComparatorsTest, LessByAuthor_BookToString_Correct) {
    LessByAuthor comp;

    Book book("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190);

    EXPECT_TRUE(comp(book, "Harper Lee"));
    EXPECT_FALSE(comp(book, "Aldous Huxley"));
    EXPECT_FALSE(comp(book, "George Orwell"));
}

TEST_F(ComparatorsTest, LessByAuthor_StringToBook_Correct) {
    LessByAuthor comp;

    Book book("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190);

    EXPECT_TRUE(comp("Aldous Huxley", book));
    EXPECT_FALSE(comp("Harper Lee", book));
    EXPECT_FALSE(comp("George Orwell", book));
}

TEST_F(ComparatorsTest, LessByAuthor_Sorting_Correct) {
    std::vector<Book> test_books = {Book("Book Z", "Zeta Author", 2000, Genre::Fiction, 4.0, 100),
                                    Book("Book A", "Alpha Author", 2000, Genre::Fiction, 4.0, 100),
                                    Book("Book M", "Middle Author", 2000, Genre::Fiction, 4.0, 100)};
    std::sort(test_books.begin(), test_books.end(), LessByAuthor{});
    EXPECT_EQ(test_books[0].author, "Alpha Author");
    EXPECT_EQ(test_books[1].author, "Middle Author");
    EXPECT_EQ(test_books[2].author, "Zeta Author");
}

TEST_F(ComparatorsTest, LessByAuthor_Sorting) {
    std::vector<Book> sorted_books = books;
    std::sort(sorted_books.begin(), sorted_books.end(), LessByAuthor{});

    for (size_t i = 1; i < sorted_books.size(); ++i) {
        EXPECT_LE(sorted_books[i - 1].author, sorted_books[i].author);
    }
}

// Тесты для LessByTitle
TEST_F(ComparatorsTest, LessByTitle_BookToBook) {
    LessByTitle comp;
    EXPECT_TRUE(comp(books[0], books[1]));  // "1984" < "Animal Farm"
}

TEST_F(ComparatorsTest, LessByTitle_BookToString) {
    LessByTitle comp;
    EXPECT_TRUE(comp(books[0], "Animal Farm"));  // "1984" < "Animal Farm"
}

TEST_F(ComparatorsTest, LessByTitle_StringToBook) {
    LessByTitle comp;

    EXPECT_TRUE(comp("1984", books[1]));  // "1984" < "Animal Farm"
}

TEST_F(ComparatorsTest, LessByTitle_Sorting) {
    std::vector<Book> sorted_books = books;
    std::sort(sorted_books.begin(), sorted_books.end(), LessByTitle{});

    for (size_t i = 1; i < sorted_books.size(); ++i) {
        EXPECT_LE(sorted_books[i - 1].title, sorted_books[i].title);
    }
}

// Тесты для LessByYear
TEST_F(ComparatorsTest, LessByYear_BookToBook) {
    LessByYear comp;

    EXPECT_TRUE(comp(books[1], books[0]));  // 1945 < 1949

    Book book1("Book1", "Author", 2000, Genre::Fiction, 4.0, 100);
    Book book2("Book2", "Author", 2000, Genre::Fiction, 4.0, 100);
    EXPECT_FALSE(comp(book1, book2));
    EXPECT_FALSE(comp(book2, book1));
}

TEST_F(ComparatorsTest, LessByYear_BookToInt) {
    LessByYear comp;
    EXPECT_TRUE(comp(books[0], 1950));  // 1949 < 1950
}

TEST_F(ComparatorsTest, LessByYear_IntToBook) {
    LessByYear comp;

    EXPECT_TRUE(comp(1940, books[0]));  // 1940 < 1949
}

TEST_F(ComparatorsTest, LessByYear_Sorting) {
    std::vector<Book> sorted_books = books;
    std::sort(sorted_books.begin(), sorted_books.end(), LessByYear{});

    for (size_t i = 1; i < sorted_books.size(); ++i) {
        EXPECT_LE(sorted_books[i - 1].year, sorted_books[i].year);
    }
}

// Тесты для LessByRating
TEST_F(ComparatorsTest, LessByRating_BookToBook) {
    LessByRating comp;

    EXPECT_TRUE(comp(books[0], books[1]));  // 4.0 < 4.4
}

TEST_F(ComparatorsTest, LessByRating_BookToDouble) {
    LessByRating comp;

    EXPECT_TRUE(comp(books[0], 4.5));  // 4.0 < 4.5
}

TEST_F(ComparatorsTest, LessByRating_DoubleToBook) {
    LessByRating comp;

    EXPECT_TRUE(comp(3.5, books[0]));  // 3.5 < 4.0
}

TEST_F(ComparatorsTest, LessByRating_Sorting) {
    std::vector<Book> sorted_books = books;
    std::sort(sorted_books.begin(), sorted_books.end(), LessByRating{});

    for (size_t i = 1; i < sorted_books.size(); ++i) {
        EXPECT_LE(sorted_books[i - 1].rating, sorted_books[i].rating);
    }
}

// Тесты для LessByPopularity
TEST_F(ComparatorsTest, LessByPopularity_BookToBook) {
    LessByPopularity comp;

    // Более популярные книги идут первыми (обратный порядок)
    EXPECT_TRUE(comp(books[0], books[6]));  // 190 > 98

    Book popular("Popular", "Author", 2000, Genre::Fiction, 5.0, 200);
    Book less_popular("Less Popular", "Author", 2000, Genre::Fiction, 5.0, 100);

    EXPECT_TRUE(comp(popular, less_popular));
    EXPECT_FALSE(comp(less_popular, popular));
}

TEST_F(ComparatorsTest, LessByPopularity_BookToInt) {
    LessByPopularity comp;

    EXPECT_TRUE(comp(books[0], 100));
}

TEST_F(ComparatorsTest, LessByPopularity_IntToBook) {
    LessByPopularity comp;

    EXPECT_TRUE(comp(200, books[0]));   // 190 < 200
    EXPECT_FALSE(comp(100, books[0]));  // 190 > 100
}

TEST_F(ComparatorsTest, LessByPopularity_Sorting) {
    std::vector<Book> sorted_books = books;
    std::sort(sorted_books.begin(), sorted_books.end(), LessByPopularity{});

    for (size_t i = 1; i < sorted_books.size(); ++i) {
        EXPECT_GE(sorted_books[i - 1].read_count, sorted_books[i].read_count);
    }
}

TEST_F(ComparatorsTest, TransparentComparator_SetFind) {
    std::set<Book, LessByAuthor> book_set(books.begin(), books.end());

    auto it = book_set.find("George Orwell");
    ASSERT_NE(it, book_set.end());
    EXPECT_EQ(it->author, "George Orwell");

    it = book_set.find("Unknown Author");
    EXPECT_EQ(it, book_set.end());
}

TEST_F(ComparatorsTest, TransparentComparator_SetFindByTitle) {
    std::set<Book, LessByTitle> book_set(books.begin(), books.end());

    auto it = book_set.find("1984");
    ASSERT_NE(it, book_set.end());
    EXPECT_EQ(it->title, "1984");
}

TEST_F(ComparatorsTest, TransparentComparator_SetFindByYear) {
    std::set<Book, LessByYear> book_set(books.begin(), books.end());

    auto it = book_set.find(1949);
    ASSERT_NE(it, book_set.end());
    EXPECT_EQ(it->year, 1949);
}

// Тесты на граничные случаи
TEST_F(ComparatorsTest, EdgeCases_EmptyStrings) {
    Book empty_book("", "", 0, Genre::Unknown, 0.0, 0);
    Book normal_book("Title", "Author", 2000, Genre::Fiction, 4.0, 100);

    LessByAuthor author_comp;
    LessByTitle title_comp;

    EXPECT_TRUE(author_comp(empty_book, normal_book));
    EXPECT_TRUE(title_comp(empty_book, normal_book));
}

TEST_F(ComparatorsTest, EdgeCases_SameBooks) {
    Book book1("Same", "Author", 2000, Genre::Fiction, 4.0, 100);
    Book book2("Same", "Author", 2000, Genre::Fiction, 4.0, 100);

    LessByAuthor author_comp;
    LessByTitle title_comp;
    LessByYear year_comp;
    LessByRating rating_comp;
    LessByPopularity popularity_comp;

    EXPECT_FALSE(author_comp(book1, book2));
    EXPECT_FALSE(title_comp(book1, book2));
    EXPECT_FALSE(year_comp(book1, book2));
    EXPECT_FALSE(rating_comp(book1, book2));
    EXPECT_FALSE(popularity_comp(book1, book2));
}