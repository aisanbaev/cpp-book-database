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
auto buildAuthorHistogramFlat(const BookDatabase<T> &db, Comparator comp = {}) {
    std::flat_map<std::string, int, Comparator> histogram(comp);

    for (const auto &book : db) {
        auto it = histogram.find(book.author);
        if (it != histogram.end()) {
            it->second++;
        } else {
            histogram.emplace(std::string(book.author), 1);
        }
    }

    return histogram;
}

class GenreStats {
public:
    void AddRating(double rating) {
        total_rating_ += rating;
        ++count_;
    }
    double Average() const { return count_ > 0 ? total_rating_ / count_ : 0.0; }

private:
    double total_rating_ = 0.0;
    size_t count_ = 0;
};

template <BookIterator I, BookSentinel<I> S>
auto calculateGenreRatings(I first, S last) {
    std::flat_map<Genre, GenreStats> genreStats;

    for (auto it = first; it != last; ++it) {
        const auto &book = *it;
        auto [it_stats, inserted] = genreStats.try_emplace(book.genre);
        it_stats->second.AddRating(book.rating);
    }

    std::flat_map<Genre, double> result;
    for (const auto &[genre, stats] : genreStats) {
        result.insert({genre, stats.Average()});
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
auto sampleRandomBooks(const BookDatabase<T> &db, typename BookDatabase<T>::size_type count) {
    if (count > db.size()) {
        throw std::invalid_argument("Sample count exceeds database size");
    }

    std::vector<std::reference_wrapper<const Book>> sampled_books;
    sampled_books.reserve(count);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::sample(db.begin(), db.end(), std::back_inserter(sampled_books), count, gen);
    return sampled_books;
}

template <BookContainerLike T, typename Comparator>
auto getTopNBy(BookDatabase<T> &db, size_t n, Comparator comp) {
    if (n == 0 || db.empty()) {
        return std::vector<std::reference_wrapper<const Book>>{};
    }

    std::sort(db.begin(), db.end(), [&comp](const Book &a, const Book &b) { return comp(b, a); });

    n = std::min(n, db.size());
    return std::vector<std::reference_wrapper<const Book>>(db.begin(), std::next(db.begin(), n));
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
