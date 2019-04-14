#pragma once

#include <algorithm>
#include <cstring>
#include <numeric>

namespace kosh
{
namespace detail
{
template <class Fun>
auto get_n_radix(Fun fun, unsigned radix_digit)
{
        return [fun, radix_digit](auto value) { return fun(value, radix_digit); };
}

template <class RanIt, class Fun>
void calculate(RanIt first, RanIt last, unsigned* counters, Fun fun, unsigned max_digit_value,
               unsigned radix_count)
{
        for (auto i = first; i != last; ++i) {
                for (int j = 0; j < radix_count; ++j) {
                        ++counters[j * max_digit_value + fun(*i, j)];
                }
        }

        for (int i = 0; i < radix_count; ++i) {
                std::partial_sum(counters + i * max_digit_value,
                                 counters + i * max_digit_value + max_digit_value,
                                 counters + i * max_digit_value);
        }
}

template <class RanIt1, class RanIt2, class Map>
void collect(RanIt1 first, RanIt1 last, RanIt2 result, Map map, unsigned* counters)
{
        for (auto i = std::make_reverse_iterator(last); i != std::make_reverse_iterator(first);
             ++i) {
                const unsigned index = --counters[map(*i)];
                *(result + index) = *i;
        }
}

template <class RanIt1, class RanIt2, class Fun>
void radix_sort_impl(RanIt1 first, RanIt1 last, RanIt2 buffer_begin, Fun fun, unsigned radix_count,
                     unsigned max_digit_value)
{
        unsigned size_mem_counters = radix_count * max_digit_value;

        unsigned counters[size_mem_counters] = {};

        calculate(first, last, counters, fun, max_digit_value, radix_count);

        RanIt2 buffer_end = buffer_begin + (last - first);

        for (int i = 0; i < radix_count; i += 2) {
                collect(first, last, buffer_begin, get_n_radix(fun, i),
                        counters + i * max_digit_value);
                collect(buffer_begin, buffer_end, first, get_n_radix(fun, i + 1),
                        counters + (i + 1) * max_digit_value);
        }

        if (radix_count % 2 == 1) {
                std::move(buffer_begin, buffer_end, first);
        }
}

}  // namespace detail

template <class Ty>
struct make_mem {
        using type = std::conditional_t<
            sizeof(Ty) == sizeof(unsigned char), unsigned char,
            std::conditional_t<
                sizeof(Ty) == sizeof(unsigned short), unsigned short,
                std::conditional_t<sizeof(Ty) == sizeof(unsigned int), unsigned int,
                                   std::conditional_t<sizeof(Ty) == sizeof(unsigned long),
                                                      unsigned long, unsigned long long>>>>;
};

template <class Ty>
using make_mem_t = typename make_mem<Ty>::type;

template <class RanIt1, class RanIt2>
void radix_sort(RanIt1 first, RanIt1 last, RanIt2 buffer_begin)
{
        using value_type = typename std::iterator_traits<RanIt1>::value_type;
        static_assert(sizeof(value_type) > 1, "for one digit, use the sorting by counting");

        auto fun = [](value_type value, unsigned digit) -> unsigned {
                make_mem_t<value_type> buf;
                std::memcpy(&buf, &value, sizeof(value_type));
                return (buf >> (8 * digit) & 0xff);
        };

        unsigned radix_count = sizeof(value_type);
        unsigned max_byte_value = 256;

        if (std::is_unsigned<value_type>::value) {
                detail::radix_sort_impl(first, last, buffer_begin, fun, radix_count,
                                        max_byte_value);
        } else if (std::is_floating_point<value_type>::value) {
                RanIt1 buf = std::partition(
                    first, last, [](value_type i) { return (i <= -0.0) ? true : false; });
                auto fun2 = [](value_type value, unsigned digit) -> unsigned {
                        make_mem_t<value_type> buf;
                        std::memcpy(&buf, &value, sizeof(value_type));
                        return ((buf >> (8 * digit)) & 0xff) ^ 0xff;
                };
                detail::radix_sort_impl(first, buf, buffer_begin, fun2, radix_count,
                                        max_byte_value);
                // std::reverse(first, buf);
                detail::radix_sort_impl(buf, last, buffer_begin, fun, radix_count, max_byte_value);
        } else if (std::is_signed<value_type>::value) {
                RanIt1 buf = std::partition(first, last,
                                            [](value_type i) { return (i < 0) ? true : false; });
                detail::radix_sort_impl(first, buf, buffer_begin, fun, radix_count, max_byte_value);
                detail::radix_sort_impl(buf, last, buffer_begin, fun, radix_count, max_byte_value);
        }
}

}  // namespace kosh
