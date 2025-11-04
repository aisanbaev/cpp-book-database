#pragma once

#include "book.hpp"

namespace bookdb::comp {

// Компаратор для сортировки/сравнения книг по автору
struct LessByAuthor {
    using is_transparent = void;

    bool operator()(const bookdb::Book &lhs, const bookdb::Book &rhs) const { return lhs.author < rhs.author; }
    bool operator()(const bookdb::Book &lhs, std::string_view author) const { return lhs.author < author; }
    bool operator()(std::string_view author, const bookdb::Book &rhs) const { return author < rhs.author; }
};

// Компаратор для сортировки/сравнения книг по названию
struct LessByTitle {
    using is_transparent = void;

    bool operator()(const bookdb::Book &lhs, const bookdb::Book &rhs) const { return lhs.title < rhs.title; }
    bool operator()(const bookdb::Book &lhs, std::string_view title) const { return lhs.title < title; }
    bool operator()(std::string_view title, const bookdb::Book &rhs) const { return title < rhs.title; }
};

// Компаратор для сортировки/сравнения книг по году выпуска
struct LessByYear {
    using is_transparent = void;

    bool operator()(const bookdb::Book &lhs, const bookdb::Book &rhs) const { return lhs.year < rhs.year; }
    bool operator()(const bookdb::Book &lhs, int year) const { return lhs.year < year; }
    bool operator()(int year, const bookdb::Book &rhs) const { return year < rhs.year; }
};

// Компаратор для сортировки/сравнения книг по рейтингу
struct LessByRating {
    using is_transparent = void;

    bool operator()(const bookdb::Book &lhs, const bookdb::Book &rhs) const { return lhs.rating < rhs.rating; }
    bool operator()(const bookdb::Book &lhs, double rating) const { return lhs.rating < rating; }
    bool operator()(double rating, const bookdb::Book &rhs) const { return rating < rhs.rating; }
};

// Компаратор для сортировки/сравнения книг по популярности
// Наиболее популярные книги идут первыми
struct LessByPopularity {
    using is_transparent = void;

    bool operator()(const bookdb::Book &lhs, const bookdb::Book &rhs) const { return rhs.read_count < lhs.read_count; }
    bool operator()(const bookdb::Book &lhs, int read_count) const { return read_count < lhs.read_count; }
    bool operator()(int read_count, const bookdb::Book &rhs) const { return rhs.read_count < read_count; }
};

}  // namespace bookdb::comp