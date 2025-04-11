#pragma once

#include "Typelist.hpp"
#include <type_traits>

namespace tmp {

// A type that is the result if an algorithm has no result
struct nil_type {};

// is the true type if for all of the elements the predicate has a value of true
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct all_of;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr bool all_of_v = all_of<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE, typename... ELEMENTs>
struct all_of<typelist<ELEMENTs...>, PREDICATE> : std::conjunction<PREDICATE<ELEMENTs>...> {};

// is the true type if for any of the elements the predicate has a value of true
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct any_of;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr bool any_of_v = any_of<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE, typename... ELEMENTs>
struct any_of<typelist<ELEMENTs...>, PREDICATE> : std::disjunction<PREDICATE<ELEMENTs>...> {};

// is the true type if for any of the elements the predicate has a value of true
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct none_of;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr bool none_of_v = none_of<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE, typename... ELEMENTs>
struct none_of<typelist<ELEMENTs...>, PREDICATE> : std::conjunction<std::negation<PREDICATE<ELEMENTs>>...> {};


// The list contains at least on of T
template<concepts::typelist LIST, typename T>
struct has_a;

template<concepts::typelist LIST, typename T>
constexpr bool has_a_v = has_a<LIST, T>::value;

template<typename T, typename... ELEMENTs>
struct has_a<typelist<ELEMENTs...>, T> : std::disjunction<std::is_same<T, ELEMENTs>...> {};

// type is the element of the typelist where the predicate matches first. If no element matches, type is the nil_type
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct find_if;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
using find_if_t = typename find_if<LIST, PREDICATE>::type;

template<template<typename> typename PREDICATE>
struct find_if<typelist<>, PREDICATE>
{
    using type = nil_type;
};

template<template<typename> typename PREDICATE, typename FIRST, typename... RESTs>
struct find_if<typelist<FIRST, RESTs...>, PREDICATE>
{
    using type = std::conditional_t<PREDICATE<FIRST>::value, FIRST, find_if_t<typelist<RESTs...>, PREDICATE>>;
};

// type is a typelist with the elements of the list that satisfies the predicate
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct filter;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
using filter_t = typename filter<LIST, PREDICATE>::type;

template<template<typename> typename PREDICATE>
struct filter<typelist<>, PREDICATE> : std::type_identity<typelist<>> {};

template<template<typename> typename PREDICATE, typename FIRST, typename... RESTs>
struct filter<typelist<FIRST, RESTs...>, PREDICATE>
{
    using type = std::conditional_t<PREDICATE<FIRST>::value, prepend_t<FIRST, filter_t<typelist<RESTs...>, PREDICATE>>, filter_t<typelist<RESTs...>, PREDICATE>>;
};

// count the number of elements that satisfies the predicate
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct count_if;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr std::size_t count_if_v = count_if<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE>
struct count_if<typelist<>, PREDICATE> : std::integral_constant<std::size_t, 0> {};

template<template<typename> typename PREDICATE, typename FIRST, typename... RESTs>
struct count_if<typelist<FIRST, RESTs...>, PREDICATE> : std::integral_constant<std::size_t, count_if_v<typelist<RESTs...>, PREDICATE> + (PREDICATE<FIRST>::value ? 1U : 0U)> {};

// Apply the operation on each element and make a new list from the operation::type
template<concepts::typelist LIST, template<typename> typename OPERATION>
struct transform;

template<concepts::typelist LIST, template<typename> typename OPERATION>
using transform_t = typename transform<LIST, OPERATION>::type;

template<template<typename> typename OPERATION, typename... ELEMENTs>
struct transform<typelist<ELEMENTs...>, OPERATION>
{
    using type = typelist<typename OPERATION<ELEMENTs>::type...>;
};

} // namespace tmp
