#include "book.hpp"
#include "book_database.hpp"
#include "concepts.hpp"
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace bookdb;

class BookDatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        testBooks = {Book("1984", "George Orwell", 1949, Genre::SciFi, 4.0, 190),
                     Book("Animal Farm", "George Orwell", 1945, Genre::Fiction, 4.4, 143),
                     Book("The Great Gatsby", "F. Scott Fitzgerald", 1925, Genre::Fiction, 4.5, 120),
                     Book("To Kill a Mockingbird", "Harper Lee", 1960, Genre::Fiction, 4.8, 156),
                     Book("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98)};
    }

    std::vector<Book> testBooks;
};

// Базовые тесты конструкторов
TEST_F(BookDatabaseTest, DefaultConstructor) {
    BookDatabase db;
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST_F(BookDatabaseTest, InitializerListConstructor) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2]};
    EXPECT_EQ(db.size(), 3);
    EXPECT_FALSE(db.empty());
    EXPECT_EQ(db.GetAuthors().size(), 2);
}

// Тесты концептов
TEST_F(BookDatabaseTest, ConceptsSatisfied) {
    static_assert(BookContainerLike<std::vector<Book>>);

    BookDatabase db;
    static_assert(BookIterator<decltype(db.begin())>);
    static_assert(BookIterator<decltype(db.end())>);
}

// Тесты методов добавления
TEST_F(BookDatabaseTest, PushBackCopy) {
    BookDatabase db;
    db.PushBack(testBooks[0]);
    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
    EXPECT_EQ(db.GetAuthors().size(), 1);
}

TEST_F(BookDatabaseTest, PushBackMove) {
    BookDatabase db;
    Book book = testBooks[0];
    db.PushBack(std::move(book));
    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "1984");
}

TEST_F(BookDatabaseTest, EmplaceBackWithGenreEnum) {
    BookDatabase db;
    auto &newBook = db.EmplaceBack("Brave New World", "Aldous Huxley", 1932, Genre::SciFi, 4.5, 98);

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(newBook.title, "Brave New World");
    EXPECT_EQ(newBook.author, "Aldous Huxley");
    EXPECT_EQ(newBook.genre, Genre::SciFi);
    EXPECT_EQ(db.GetAuthors().size(), 1);
}

TEST_F(BookDatabaseTest, EmplaceBackWithGenreString) {
    BookDatabase db;
    auto &newBook = db.EmplaceBack("The Hobbit", "J.R.R. Tolkien", 1937, "Fiction", 4.9, 203);

    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(newBook.title, "The Hobbit");
    EXPECT_EQ(newBook.author, "J.R.R. Tolkien");
    EXPECT_EQ(newBook.genre, Genre::Fiction);
}

TEST_F(BookDatabaseTest, MultipleBooksSameAuthor) {
    BookDatabase db;
    db.PushBack(testBooks[0]);  // George Orwell
    db.PushBack(testBooks[1]);  // George Orwell

    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetAuthors().size(), 1);
}

// Тесты итераторов
TEST_F(BookDatabaseTest, ForwardIteration) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2]};

    size_t count = 0;
    for (auto it = db.begin(); it != db.end(); ++it) {
        EXPECT_EQ(it->author, db.GetBooks()[count].author);
        ++count;
    }
    EXPECT_EQ(count, 3);
}

TEST_F(BookDatabaseTest, ConstIteration) {
    const BookDatabase db{testBooks[0], testBooks[1]};

    size_t count = 0;
    for (auto it = db.cbegin(); it != db.cend(); ++it) {
        EXPECT_FALSE(it->title.empty());
        ++count;
    }
    EXPECT_EQ(count, 2);
}

TEST_F(BookDatabaseTest, ReverseIteration) {
    BookDatabase db{testBooks[0], testBooks[1]};

    std::vector<std::string> titles;
    for (auto it = db.rbegin(); it != db.rend(); ++it) {
        titles.push_back(std::string(it->title));
    }

    EXPECT_EQ(titles.size(), 2);
    EXPECT_EQ(titles[0], "Animal Farm");
    EXPECT_EQ(titles[1], "1984");
}

// Тесты доступа к данным
TEST_F(BookDatabaseTest, GetBooksReference) {
    BookDatabase db{testBooks[0], testBooks[1]};

    const auto &books = db.GetBooks();
    EXPECT_EQ(books.size(), 2);
    EXPECT_EQ(books[0].title, "1984");
    EXPECT_EQ(books[1].title, "Animal Farm");

    static_assert(std::is_same_v<decltype(db.GetBooks()), const std::vector<Book> &>);
}

TEST_F(BookDatabaseTest, GetAuthorsWithHeterogeneousLookup) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2]};

    const auto &authors = db.GetAuthors();
    EXPECT_EQ(authors.size(), 2);

    EXPECT_TRUE(authors.find("George Orwell") != authors.end());
    EXPECT_TRUE(authors.find(std::string("George Orwell")) != authors.end());
    EXPECT_TRUE(authors.find(std::string_view("George Orwell")) != authors.end());

    EXPECT_TRUE(authors.find("F. Scott Fitzgerald") != authors.end());
    EXPECT_TRUE(authors.find("Unknown Author") == authors.end());
}

TEST_F(BookDatabaseTest, AuthorsUnique) {
    BookDatabase db;
    db.PushBack(testBooks[0]);  // George Orwell
    db.PushBack(testBooks[1]);  // George Orwell
    db.PushBack(testBooks[2]);  // F. Scott Fitzgerald
    db.PushBack(testBooks[4]);  // Aldous Huxley

    EXPECT_EQ(db.size(), 4);
    EXPECT_EQ(db.GetAuthors().size(), 3);
}

// Тесты управления состоянием
TEST_F(BookDatabaseTest, Clear) {
    BookDatabase db{testBooks[0], testBooks[1], testBooks[2]};
    EXPECT_EQ(db.size(), 3);
    EXPECT_EQ(db.GetAuthors().size(), 2);

    db.Clear();
    EXPECT_TRUE(db.empty());
    EXPECT_EQ(db.size(), 0);
    EXPECT_TRUE(db.GetAuthors().empty());
}

TEST_F(BookDatabaseTest, EmptyDatabaseOperations) {
    BookDatabase db;

    EXPECT_TRUE(db.begin() == db.end());
    EXPECT_TRUE(db.rbegin() == db.rend());
    EXPECT_TRUE(db.cbegin() == db.cend());
    EXPECT_TRUE(db.crbegin() == db.crend());
}

// Тесты форматирования
TEST_F(BookDatabaseTest, FormatterOutput) {
    BookDatabase db{testBooks[0]};
    std::string formatted = std::format("{}", db);

    EXPECT_TRUE(formatted.find("BookDatabase") != std::string::npos);
    EXPECT_TRUE(formatted.find("size = 1") != std::string::npos);
    EXPECT_TRUE(formatted.find("1984") != std::string::npos);
    EXPECT_TRUE(formatted.find("George Orwell") != std::string::npos);
    EXPECT_TRUE(formatted.find("Authors:") != std::string::npos);
}

TEST_F(BookDatabaseTest, FormatterEmptyDatabase) {
    BookDatabase db;
    std::string formatted = std::format("{}", db);

    EXPECT_TRUE(formatted.find("size = 0") != std::string::npos);
}

// Тесты граничных случаев
TEST_F(BookDatabaseTest, EmptyStringsHandling) {
    BookDatabase db;
    db.EmplaceBack("", "", 0, Genre::Unknown, 0.0, 0);

    EXPECT_EQ(db.size(), 1);
    EXPECT_TRUE(db.GetBooks()[0].title.empty());
    EXPECT_TRUE(db.GetBooks()[0].author.empty());
    EXPECT_EQ(db.GetAuthors().size(), 1);
    EXPECT_TRUE(db.GetAuthors().find("") != db.GetAuthors().end());
}

TEST_F(BookDatabaseTest, ExtremeValues) {
    BookDatabase db;
    db.EmplaceBack("Extreme", "Author", 9999, Genre::SciFi, 5.0, 1000000);
    db.EmplaceBack("Negative", "Author", -100, Genre::Unknown, -1.0, -50);

    EXPECT_EQ(db.size(), 2);
    EXPECT_EQ(db.GetAuthors().size(), 1);
}

// Тесты на корректность типов
TEST_F(BookDatabaseTest, TypeAliases) {
    BookDatabase db;

    static_assert(std::is_same_v<decltype(db)::value_type, Book>);
    static_assert(std::is_same_v<decltype(db)::reference, Book &>);
    static_assert(std::is_same_v<decltype(db)::const_reference, const Book &>);
    static_assert(std::is_same_v<decltype(db)::size_type, size_t>);
}

TEST_F(BookDatabaseTest, DataIntegrityAfterMultipleOperations) {
    BookDatabase db;

    db.PushBack(testBooks[0]);
    db.PushBack(testBooks[1]);
    db.EmplaceBack("New Book", "New Author", 2023, Genre::Mystery, 4.2, 50);

    EXPECT_EQ(db.size(), 3);
    EXPECT_EQ(db.GetAuthors().size(), 2);  // Orwell + New Author

    db.Clear();
    EXPECT_TRUE(db.empty());

    db.PushBack(testBooks[2]);
    EXPECT_EQ(db.size(), 1);
    EXPECT_EQ(db.GetAuthors().size(), 1);
    EXPECT_EQ(db.GetBooks()[0].title, "The Great Gatsby");
}

// Тесты для проверки гетерогенного поиска в Authors
TEST_F(BookDatabaseTest, HeterogeneousLookupDetailed) {
    BookDatabase db{testBooks[0], testBooks[2]};
    const auto &authors = db.GetAuthors();

    std::string str_author = "George Orwell";
    std::string_view sv_author = "F. Scott Fitzgerald";
    const char *cstr_author = "George Orwell";

    EXPECT_TRUE(authors.find(str_author) != authors.end());
    EXPECT_TRUE(authors.find(sv_author) != authors.end());
    EXPECT_TRUE(authors.find(cstr_author) != authors.end());

    EXPECT_TRUE(authors.find("Nonexistent Author") == authors.end());
}
