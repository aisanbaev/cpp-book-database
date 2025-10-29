#pragma once

#include "book.hpp"
#include "concepts.hpp"
#include <algorithm>
#include <functional>
#include <vector>

namespace bookdb {

namespace detail {
constexpr double EPSILON = 1e-9;
}

constexpr auto YearBetween(int from, int to) noexcept {
    return [from, to](const Book &book) { return book.year >= from && book.year <= to; };
}

constexpr auto RatingAbove(double threshold) noexcept {
    return [threshold](const Book &book) { return book.rating > threshold - detail::EPSILON; };
}

constexpr auto GenreIs(Genre g) noexcept {
    return [g](const Book &book) { return book.genre == g; };
}

template <typename... Predicates>
constexpr auto all_of(Predicates... predicates) noexcept {
    return [predicates...](const Book &book) { return (predicates(book) && ...); };
}

template <typename... Predicates>
constexpr auto any_of(Predicates... predicates) noexcept {
    return [predicates...](const Book &book) { return (predicates(book) || ...); };
}

template <BookIterator Iterator, BookSentinel<Iterator> Sentinel, BookPredicate Predicate>
constexpr auto filterBooks(Iterator first, Sentinel last, const Predicate &pred) noexcept {
    std::vector<std::reference_wrapper<const Book>> result;
    std::copy_if(first, last, std::back_inserter(result), [&pred](const auto &book) { return pred(book); });
    return result;
}

}  // namespace bookdb