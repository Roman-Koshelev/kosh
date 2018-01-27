#ifndef KOSH_CONSTEXPR_FOR_EACH_HPP
#define KOSH_CONSTEXPR_FOR_EACH_HPP

#include "int_c.hpp"
#include "make_index_sequence_range.hpp"

namespace kosh
{
	namespace detail
	{
		template <class Function, std::size_t... I>
		void constexpr_for_each_impl(Function fun, std::index_sequence<I...>)
		{
			auto dummy = {(fun(uint_c<I>()), 0)...};
			(void)dummy;

			// Ñ++17   ((fun(uint_c<I>())), ...);
		}

	}  // namespace detail

	template <std::size_t First, std::size_t Last, std::size_t Step = 1, class Function>
	void constexpr_for_each(Function fun)
	{
		detail::constexpr_for_each_impl(fun,
										make_index_sequence_range<First, Last, Step>());
	}

}  // namespace kosh

#endif  // KOSH_CONSTEXPR_FOR_EACH_HPP
