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
struct all_of<typelist<ELEMENTs...>, PREDICATE> : std::integral_constant<bool, (... && (PREDICATE<ELEMENTs>::value == true))> {};

// is the true type if for any of the elements the predicate has a value of true
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct any_of;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr bool any_of_v = any_of<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE, typename... ELEMENTs>
struct any_of<typelist<ELEMENTs...>, PREDICATE> : std::integral_constant<bool, (... || (PREDICATE<ELEMENTs>::value == true))> {};

// is the true type if for any of the elements the predicate has a value of true
template<concepts::typelist LIST,  template<typename> typename PREDICATE>
struct none_of;

template<concepts::typelist LIST, template<typename> typename PREDICATE>
constexpr bool none_of_v = none_of<LIST, PREDICATE>::value;

template<template<typename> typename PREDICATE, typename... ELEMENTs>
struct none_of<typelist<ELEMENTs...>, PREDICATE> : std::integral_constant<bool, (... && (PREDICATE<ELEMENTs>::value == false))> {};


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

} // namespace tmp
