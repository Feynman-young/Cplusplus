#include <iostream>
#include <type_traits>

template<typename T1, typename T2>
auto max_decay(T1 a, T2 b) -> typename std::decay<decltype(true ? a : b)>::type {
	return b < a ? a : b;
}

template<typename T1, typename T2>
std::common_type_t<T1, T2> max_common(T1 a, T2 b) {
	return b < a ? a : b;
}

int main() {
	std::cout << ::max_decay(4, 7.2) << std::endl;
	std::cout << ::max_common(7.2, 4) << std::endl;
}