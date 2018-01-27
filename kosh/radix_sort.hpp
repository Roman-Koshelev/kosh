#ifndef KOSH_RADIX_SORT_HPP
#define KOSH_RADIX_SORT_HPP

#include  <type_traits>

#include "int_c.hpp"
#include "make_mem.hpp"
#include "radix_sort_d.hpp"

namespace kosh
{
	template <class RanIt1, class RanIt2>
	void radix_sort(RanIt1 first, RanIt1 last, RanIt2 buffer_begin)
	{
		using value_type = typename std::iterator_traits<RanIt1>::value_type;
		static_assert(sizeof(value_type) > 1,
					  "for one digit, use the sorting by counting");

		auto fun = [](value_type value, auto digit) -> unsigned {
			make_mem_t<value_type> buf;
			std::memcpy(&buf, &value, sizeof(value_type));
			return ((buf >> (8 * decltype(digit)::value)) & 0xff);
		};

		auto radix_count = uint_c<sizeof(value_type)>{};
		auto max_byte_value = 256_c;

		if (std::is_unsigned_v<value_type>)
		{
			detail::radix_sort_impl(first, last, buffer_begin, fun, radix_count,
									max_byte_value);
		}
		else if (std::is_floating_point<value_type>::value)
		{
			RanIt1 buf = std::partition(
				first, last, [](value_type i) { return (i <= -0.0) ? true : false; });
			auto fun2 = [](value_type value, auto digit) -> unsigned {
				make_mem_t<value_type> buf;
				std::memcpy(&buf, &value, sizeof(value_type));
				return ((buf >> (8 * decltype(digit)::value)) & 0xff) ^ 0xff;
			};
			detail::radix_sort_impl(first, buf, buffer_begin, fun2, radix_count,
									max_byte_value);
			// std::reverse(first, buf);
			detail::radix_sort_impl(buf, last, buffer_begin, fun, radix_count,
									max_byte_value);
		}
		else
		{
			RanIt1 buf = std::partition(
				first, last, [](value_type i) { return (i < 0) ? true : false; });
			detail::radix_sort_impl(first, buf, buffer_begin, fun, radix_count,
									max_byte_value);
			detail::radix_sort_impl(buf, last, buffer_begin, fun, radix_count,
									max_byte_value);
		}
	}

}  // namespace kosh

#endif  // KOSH_RADIX_SORT_HPP
