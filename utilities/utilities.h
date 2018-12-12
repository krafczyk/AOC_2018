#include <algorithm>

// BOOORING
template<typename iter, typename type> bool hasElementHelper(const iter& begin, const iter& end, const type& el) {
	if(std::find(begin, end, el) == end) {
		return false;
	} else {
		return true;
	}
}

template<typename container, typename type> bool hasElement(const container& cont, const type& el) {
	return hasElementHelper(cont.begin(), cont.end(), el);
}

