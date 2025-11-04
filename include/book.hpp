#pragma once

#include <format>
#include <stdexcept>
#include <string_view>

namespace bookdb {

enum class Genre { Fiction, NonFiction, SciFi, Biography, Mystery, Unknown };

constexpr Genre GenreFromString(std::string_view s) {
    // clang-format off
    if (s == "Fiction")    { return Genre::Fiction; }
    if (s == "NonFiction") { return Genre::NonFiction; }
    if (s == "SciFi")      { return Genre::SciFi; }
    if (s == "Biography")  { return Genre::Biography; }
    if (s == "Mystery")    { return Genre::Mystery; }
    // clang-format on
    return Genre::Unknown;
}

constexpr std::string_view StringFromGenre(Genre g) {
    // clang-format off
    switch (g) {
        case Genre::Fiction:    return "Fiction";
        case Genre::NonFiction: return "NonFiction";
        case Genre::SciFi:      return "SciFi";
        case Genre::Biography:  return "Biography";
        case Genre::Mystery:    return "Mystery";
        case Genre::Unknown:    return "Unknown";
        default:                return "Unknown";
    }
    // clang-format on
}

struct Book {
    std::string_view author;
    std::string title;

    int year;
    Genre genre;
    double rating;
    int read_count;

    constexpr Book(std::string t, std::string_view a, int y, Genre g, double r, int rc)
        : title(std::move(t)), author(a), year(y), genre(g), rating(r), read_count(rc) {}

    constexpr Book(std::string t, std::string_view a, int y, std::string_view g, double r, int rc)
        : title(std::move(t)), author(a), year(y), genre(GenreFromString(g)), rating(r), read_count(rc) {}
};
}  // namespace bookdb

namespace std {

template <>
struct formatter<bookdb::Genre, char> {
    template <typename FormatContext>
    auto format(const bookdb::Genre g, FormatContext &fc) const {
        std::string genre_str;

        // clang-format off
        using bookdb::Genre;
        switch (g) {
            case Genre::Fiction:    genre_str = "Fiction"; break;
            case Genre::Mystery:    genre_str = "Mystery"; break;
            case Genre::NonFiction: genre_str = "NonFiction"; break;
            case Genre::SciFi:      genre_str = "SciFi"; break;
            case Genre::Biography:  genre_str = "Biography"; break;
            case Genre::Unknown:    genre_str = "Unknown"; break;
            default:
                throw logic_error{"Unsupported bookdb::Genre"};
            }
        // clang-format on
        return format_to(fc.out(), "{}", genre_str);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Просто игнорируем пользовательский формат
    }
};

template <>
struct formatter<bookdb::Book, char> {
    template <typename FormatContext>
    auto format(const bookdb::Book &b, FormatContext &fc) const {
        // Форматируем книгу как "Author: Title (Year, Genre, Rating, ReadCount)"
        return format_to(fc.out(), "{}: {} ({}, {}, {}, {})", b.author, b.title, b.year, b.genre, b.rating,
                         b.read_count);
    }

    constexpr auto parse(format_parse_context &ctx) {
        return ctx.begin();  // Игнорируем пользовательский формат
    }
};

}  // namespace std
