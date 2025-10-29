#pragma once

#include <algorithm>
#include <flat_map>
#include <functional>
#include <iterator>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "book_database.hpp"
#include "heterogeneous_lookup.hpp"

namespace bookdb {

template <BookContainerLike T, typename Comparator = TransparentStringLess>
auto buildAuthorHistogramFlat(const BookDatabase<T> &cont, Comparator comp = {}) {
    std::flat_map<std::string, int, Comparator> histogram_flat(comp);

    for (const auto &book : cont.GetBooks()) {
        histogram_flat[std::string(book.author)]++;
    }

    return histogram_flat;
}

template <BookIterator I, BookSentinel<I> S>
auto calculateGenreRatings(I first, S last) {
    std::flat_map<Genre, std::pair<double, size_t>> genreStats;

    for (auto it = first; it != last; ++it) {
        const auto &book = *it;
        auto it_stats = genreStats.find(book.genre);
        if (it_stats != genreStats.end()) {
            it_stats->second.first += book.rating;
            it_stats->second.second++;
        } else {
            genreStats.insert({book.genre, {book.rating, 1}});
        }
    }

    std::flat_map<Genre, double> result;
    for (const auto &[genre, stats] : genreStats) {
        if (stats.second > 0) {
            result.insert({genre, stats.first / stats.second});
        }
    }

    return result;
}

template <BookContainerLike T>
double calculateAverageRating(const BookDatabase<T> &cont) {
    if (cont.empty()) {
        throw std::domain_error("Cannot calculate average rating of an empty database");
    }
    double total_rating =
        std::accumulate(cont.begin(), cont.end(), 0.0, [](double sum, const Book &book) { return sum + book.rating; });
    return total_rating / cont.size();
}

template <BookContainerLike T>
auto sampleRandomBooks(const BookDatabase<T> &cont, typename BookDatabase<T>::size_type count) {
    if (count > cont.size()) {
        throw std::invalid_argument("Sample count exceeds database size");
    }

    std::vector<std::reference_wrapper<const Book>> all_books;
    all_books.reserve(cont.size());
    for (const auto &book : cont) {
        all_books.emplace_back(book);
    }

    std::random_device rd;
    std::mt19937 gen(rd());

    std::vector<std::reference_wrapper<const Book>> sampled_books;
    sampled_books.reserve(count);
    std::sample(all_books.begin(), all_books.end(), std::back_inserter(sampled_books), count, gen);

    return sampled_books;
}

template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &db, size_t n, Comparator comp) {
    if (n == 0 || db.empty()) {
        return std::vector<std::reference_wrapper<const Book>>{};
    }

    std::sort(db.begin(), db.end(), [&comp](const Book &a, const Book &b) { return comp(b, a); });

    std::vector<std::reference_wrapper<const Book>> top_books;
    auto end_it = std::next(db.begin(), std::min(n, db.size()));
    top_books.reserve(std::distance(db.begin(), end_it));
    for (auto it = db.begin(); it != end_it; ++it) {
        top_books.emplace_back(*it);
    }

    return top_books;
}

}  // namespace bookdb

namespace std {

template <typename Key, typename Value, typename Compare>
struct formatter<std::flat_map<Key, Value, Compare>, char> {
    template <typename FormatContext>
    auto format(const std::flat_map<Key, Value, Compare> &fm, FormatContext &fc) const {
        auto out = fc.out();
        out = format_to(out, "[");
        bool first = true;
        for (const auto &[key, value] : fm) {
            if (!first) {
                out = format_to(out, ", ");
            }
            out = format_to(out, "({}, {})", key, value);
            first = false;
        }
        out = format_to(out, "]");
        return out;
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

template <>
struct formatter<flat_map<bookdb::Genre, double>> {
    template <typename FormatContext>
    auto format(const flat_map<bookdb::Genre, double> &m, FormatContext &fc) const {
        auto out = fc.out();
        format_to(out, "[");
        bool first = true;
        for (const auto &[genre, rating] : m) {
            if (!first)
                format_to(out, ", ");
            format_to(out, "{}: {:.2f}", genre, rating);
            first = false;
        }
        format_to(out, "]");
        return out;
    }

    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }
};

}  // namespace std
