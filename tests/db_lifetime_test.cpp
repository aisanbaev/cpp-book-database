#include "book.hpp"
#include "book_database.hpp"
#include <gtest/gtest.h>
#include <string>

using namespace bookdb;

class DatabaseLifetimeTest : public ::testing::Test {
protected:
    void SetUp() override {}
};

TEST_F(DatabaseLifetimeTest, TemporaryStringAuthor) {
    BookDatabase db;

    {
        std::string temp_author = "Temporary Author";
        db.PushBack(Book("Temp Book", temp_author, 2000, Genre::Fiction, 4.0, 100));
    }  // temp_author уничтожается здесь

    // Проверяем что книга все еще доступна и автор корректный
    ASSERT_FALSE(db.empty());
    EXPECT_EQ(db.GetBooks()[0].title, "Temp Book");
    EXPECT_EQ(db.GetBooks()[0].author, "Temporary Author");

    // Проверяем что автор добавился в коллекцию авторов
    EXPECT_TRUE(db.GetAuthors().find("Temporary Author") != db.GetAuthors().end());
}

TEST_F(DatabaseLifetimeTest, MovedBookWithTemporaryAuthor) {
    BookDatabase db;

    Book book("Moved Book", "Moved Author", 2000, Genre::SciFi, 4.5, 150);
    db.PushBack(std::move(book));

    ASSERT_FALSE(db.empty());
    EXPECT_EQ(db.GetBooks()[0].author, "Moved Author");
}

TEST_F(DatabaseLifetimeTest, MultipleBooksSameTemporaryAuthor) {
    BookDatabase db;

    {
        std::string shared_author = "Shared Author";
        db.PushBack(Book("Book 1", shared_author, 2000, Genre::Fiction, 4.0, 100));
        db.PushBack(Book("Book 2", shared_author, 2001, Genre::Fiction, 4.2, 120));
        db.PushBack(Book("Book 3", shared_author, 2002, Genre::Fiction, 4.4, 140));
    }  // shared_author уничтожается здесь

    // Все книги должны иметь корректные ссылки на автора
    ASSERT_EQ(db.size(), 3);
    for (const auto &book : db) {
        EXPECT_EQ(book.author, "Shared Author");
        EXPECT_TRUE(book.author.data() != nullptr);
    }

    EXPECT_EQ(db.GetAuthors().size(), 1);
    EXPECT_TRUE(db.GetAuthors().find("Shared Author") != db.GetAuthors().end());
}

TEST_F(DatabaseLifetimeTest, EmplaceBackWithTemporaryStrings) {
    BookDatabase db;

    {
        std::string temp_title = "Emplaced Book";
        std::string temp_author = "Emplaced Author";
        db.EmplaceBack(std::move(temp_title), std::move(temp_author), 2000, Genre::Mystery, 4.3, 110);
    }  // temp_title и temp_author уничтожаются здесь

    ASSERT_FALSE(db.empty());
    EXPECT_EQ(db.GetBooks()[0].title, "Emplaced Book");
    EXPECT_EQ(db.GetBooks()[0].author, "Emplaced Author");

    const auto &author_ref = db.GetBooks()[0].author;
    EXPECT_FALSE(author_ref.empty());
    EXPECT_EQ(author_ref, "Emplaced Author");
}

TEST_F(DatabaseLifetimeTest, LongLivedDatabaseWithMultipleTemporaryAuthors) {
    BookDatabase db;

    // Добавляем много книг с временными авторами
    for (int i = 0; i < 100; ++i) {
        std::string temp_author = "Author " + std::to_string(i);
        db.PushBack(Book("Book " + std::to_string(i), temp_author, 1900 + i, Genre::Fiction, 4.0 + i * 0.01, i * 10));
        // temp_author уничтожается на каждой итерации
    }

    ASSERT_EQ(db.size(), 100);
    ASSERT_EQ(db.GetAuthors().size(), 100);

    for (size_t i = 0; i < db.size(); ++i) {
        const auto &book = db.GetBooks()[i];
        std::string expected_author = "Author " + std::to_string(i);

        EXPECT_EQ(book.author, expected_author);
        EXPECT_EQ(book.title, "Book " + std::to_string(i));

        EXPECT_FALSE(book.author.empty());
        EXPECT_EQ(book.author.size(), expected_author.size());
    }
}

TEST_F(DatabaseLifetimeTest, AuthorStringStability) {
    BookDatabase db;

    db.PushBack(Book("Book 1", "Same Author", 2000, Genre::Fiction, 4.0, 100));
    db.PushBack(Book("Book 2", "Same Author", 2001, Genre::Fiction, 4.1, 110));
    db.PushBack(Book("Book 3", "Same Author", 2002, Genre::Fiction, 4.2, 120));

    // Проверяем что все books.author ссылаются на одну и ту же строку в памяти
    const auto &books = db.GetBooks();
    EXPECT_EQ(books[0].author.data(), books[1].author.data());
    EXPECT_EQ(books[1].author.data(), books[2].author.data());

    // Проверяем что эта строка находится в authors_ контейнере
    const auto &authors = db.GetAuthors();
    auto author_it = authors.find("Same Author");
    ASSERT_NE(author_it, authors.end());
    EXPECT_EQ(books[0].author.data(), author_it->data());
}
