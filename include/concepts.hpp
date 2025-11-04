#pragma once

#include <concepts>
#include <iterator>
#include <string>
#include <string_view>

#include "book.hpp"

namespace bookdb {

template <typename T>
concept BookContainerLike = requires(T t) {
    typename T::value_type;
    requires std::same_as<typename T::value_type, bookdb::Book>;
    { t.begin() } -> std::input_or_output_iterator;
    { t.end() } -> std::input_or_output_iterator;
    { t.size() } -> std::integral;
    { t.empty() } -> std::convertible_to<bool>;
    {
        t.emplace_back(std::declval<std::string_view>(), std::declval<std::string>(), int{}, bookdb::Genre{}, double{},
                       int{})
    } -> std::same_as<typename T::reference>;
};

template <typename T>
concept BookIterator = std::input_iterator<T> && std::convertible_to<std::iter_reference_t<T>, const bookdb::Book &>;

template <typename S, typename I>
concept BookSentinel = BookIterator<I> && std::sentinel_for<S, I>;

template <typename P>
concept BookPredicate = requires(const P &pred, const bookdb::Book &book) {
    { pred(book) } -> std::convertible_to<bool>;
};

template <typename C>
concept BookComparator = requires(const C &comp, const bookdb::Book &book1, const bookdb::Book &book2) {
    { comp(book1, book2) } -> std::convertible_to<bool>;
    typename C::is_transparent;
};

}  // namespace bookdb