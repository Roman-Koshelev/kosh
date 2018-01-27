#ifndef KOSH_DETAIL_RADIX_SORT_D_HPP
#define KOSH_DETAIL_RADIX_SORT_D_HPP

#include <numeric>
#include <algorithm>
#include <cstring>
#include "heap_and_stack_memory.hpp"
#include "constexpr_for_each.hpp"
#include "int_c.hpp"

namespace kosh
{
	namespace detail
	{
		template <class Iterator>
		using iter_value_t = typename std::iterator_traits<Iterator>::value_type;

		template <unsigned radix_digit, class Fun>
		auto get_n_radix(Fun fun)
		{
			return [fun](auto value) { return fun(value, uint_c<radix_digit>{}); };
		}

		template <class RanIt, class Fun, unsigned max_digit_value, unsigned radix_count>
		void calculate(RanIt first, RanIt last, unsigned* counters, Fun fun,
					   uint_c<max_digit_value>, uint_c<radix_count>)
		{
			using value_type = iter_value_t<RanIt>;
			std::for_each(first, last, [counters, fun](const value_type i) -> void {
				constexpr_for_each<0, radix_count, 1>([counters, fun, i](auto p) {
					constexpr unsigned I = (decltype(p)::value);
					++counters[I * max_digit_value + get_n_radix<I>(fun)(i)];
				});
			});

			constexpr_for_each<0, radix_count>([counters](auto p) -> void {
				constexpr unsigned I = (decltype(p)::value);
				std::partial_sum(counters + I * max_digit_value,
								 counters + I * max_digit_value + max_digit_value,
								 counters + I * max_digit_value);
			});
		}

		template <class RanIt1, class RanIt2, class Map>
		void
		collect(RanIt1 first, RanIt1 last, RanIt2 result, Map map, unsigned* counters)
		{
			using value_type = iter_value_t<RanIt1>;
			std::for_each(std::make_reverse_iterator(last),
						  std::make_reverse_iterator(first),
						  [counters, map, result](value_type i) {
							  const unsigned index = --counters[map(i)];
							  *(result + index) = i;
						  });
		}

		template <class RanIt1, class RanIt2, class Fun, unsigned radix_count,
				  unsigned max_digit_value>
		void radix_sort_impl(RanIt1 first, RanIt1 last, RanIt2 buffer_begin, Fun fun,
							 uint_c<radix_count>, uint_c<max_digit_value>)
		{
			constexpr unsigned size_mem_counters = radix_count * max_digit_value;

			unsigned* counters;
			memory_allocation<unsigned, size_mem_counters> mem(counters);

			std::memset(counters, 0, size_mem_counters * sizeof(unsigned));

			calculate(first, last, counters, fun, uint_c<max_digit_value>{},
					  uint_c<radix_count>{});

			RanIt2 buffer_end = buffer_begin + (last - first);

			using value_type = iter_value_t<RanIt1>;
			constexpr_for_each<0, radix_count, 2>(
				[first, last, buffer_begin, buffer_end, counters, fun](auto p) -> void {
					constexpr unsigned i = (decltype(p)::value);
					collect(first, last, buffer_begin, get_n_radix<i>(fun),
							counters + i * max_digit_value);
					collect(buffer_begin, buffer_end, first, get_n_radix<(i + 1)>(fun),
							counters + (i + 1) * max_digit_value);
				});

			if (radix_count % 2 == 1)
			{
				std::move(buffer_begin, buffer_end, first);
			}
		}

	}  // namespace detail

}  // namespace kosh

#endif // KOSH_DETAIL_RADIX_SORT_D_HPP
