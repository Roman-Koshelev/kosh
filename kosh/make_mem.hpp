#ifndef KOSH_MAKE_MEM_HPP
#define KOSH_MAKE_MEM_HPP

#include <type_traits>

namespace kosh
{
	template <class Ty>
	struct make_mem
	{
		using type = std::conditional_t<
			sizeof(Ty) == sizeof(unsigned char), unsigned char,
			std::conditional_t<
				sizeof(Ty) == sizeof(unsigned short), unsigned short,
				std::conditional_t<
					sizeof(Ty) == sizeof(unsigned int), unsigned int,
					std::conditional_t<sizeof(Ty) == sizeof(unsigned long), unsigned long,
									   unsigned long long>>>>;
	};

	template <class Ty>
	using make_mem_t = typename make_mem<Ty>::type;

}  // namespace kosh
#endif  // KOSH_MAKE_MEM_HPP
