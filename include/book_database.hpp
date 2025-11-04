#pragma once

#include <set>
#include <vector>

#include "book.hpp"
#include "concepts.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike BookContainer = std::vector<Book>>
class BookDatabase {
public:
    using value_type = typename BookContainer::value_type;
    using reference = typename BookContainer::reference;
    using const_reference = typename BookContainer::const_reference;
    using pointer = typename BookContainer::pointer;
    using const_pointer = typename BookContainer::const_pointer;
    using iterator = typename BookContainer::iterator;
    using const_iterator = typename BookContainer::const_iterator;
    using reverse_iterator = typename BookContainer::reverse_iterator;
    using const_reverse_iterator = typename BookContainer::const_reverse_iterator;
    using size_type = typename BookContainer::size_type;
    using difference_type = typename BookContainer::difference_type;

    using AuthorContainer = std::set<std::string, TransparentStringLess>;

    BookDatabase() = default;

    BookDatabase(std::initializer_list<Book> init_list) {
        books_.reserve(init_list.size());
        for (const auto &book : init_list) {
            PushBack(book);
        }
    }

    void Clear() {
        books_.clear();
        authors_.clear();
    }

    iterator begin() { return books_.begin(); }
    iterator end() { return books_.end(); }
    const_iterator begin() const { return books_.begin(); }
    const_iterator end() const { return books_.end(); }
    const_iterator cbegin() const { return books_.cbegin(); }
    const_iterator cend() const { return books_.cend(); }

    reverse_iterator rbegin() { return books_.rbegin(); }
    reverse_iterator rend() { return books_.rend(); }
    const_reverse_iterator rbegin() const { return books_.rbegin(); }
    const_reverse_iterator rend() const { return books_.rend(); }
    const_reverse_iterator crbegin() const { return books_.crbegin(); }
    const_reverse_iterator crend() const { return books_.crend(); }

    size_type size() const { return books_.size(); }
    bool empty() const { return books_.empty(); }

    void PushBack(const Book &book) {
        books_.push_back(book);
        auto [it, inserted] = authors_.insert(std::string(book.author));
        books_.back().author = *it;
    }

    void PushBack(Book &&book) {
        books_.push_back(std::move(book));
        auto [it, inserted] = authors_.insert(std::string(books_.back().author));
        books_.back().author = *it;
    }

    template <typename... Args>
    decltype(auto) EmplaceBack(Args &&...args) {
        auto &new_book = books_.emplace_back(std::forward<Args>(args)...);
        auto [it, inserted] = authors_.insert(std::string(new_book.author));
        new_book.author = *it;
        return new_book;
    }

    const BookContainer &GetBooks() const { return books_; }
    const AuthorContainer &GetAuthors() const { return authors_; }

private:
    BookContainer books_;
    AuthorContainer authors_;
};

}  // namespace bookdb

namespace std {
template <>
struct formatter<bookdb::BookDatabase<std::vector<bookdb::Book>>> {
    template <typename FormatContext>
    auto format(const bookdb::BookDatabase<std::vector<bookdb::Book>> &db, FormatContext &fc) const {

        format_to(fc.out(), "BookDatabase (size = {}): ", db.size());

        format_to(fc.out(), "Books:\n");
        for (const auto &book : db.GetBooks()) {
            format_to(fc.out(), "- {}\n", book);
        }

        format_to(fc.out(), "Authors:\n");
        for (const auto &author : db.GetAuthors()) {
            format_to(fc.out(), "- {}\n", author);
        }

        return fc.out();
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};
}  // namespace std