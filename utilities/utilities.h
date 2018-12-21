#include <algorithm>

//template<typename T>
//concept Iterable 

#if defined(__GNUC__) && not defined(YCM)
// Shweeeeeeeeet

template<typename T>
concept bool ConstIterable = requires(T cont) {
	{ cont.cbegin() } -> typename T::const_iterator;
	{ cont.cend() } -> typename T::const_iterator;
};

template<typename T>
concept bool Iterable = requires(T cont) {
	{ cont.begin() } -> typename T::iterator;
	{ cont.end() } -> typename T::iterator;
};

template<ConstIterable container, typename type>
bool hasElement(const container& cont, const type& el) {
	if(std::find(cont.begin(), cont.end(), el) == cont.end()) {
		return false;
	} else {
		return true;
	}
}

#else
// BOOORING
template<typename iter, typename type>
bool hasElementHelper(const iter& begin, const iter& end, const type& el) {
	if(std::find(begin, end, el) == end) {
		return false;
	} else {
		return true;
	}
}

template<typename container, typename type>
bool hasElement(const container& cont, const type& el) {
	return hasElementHelper(cont.begin(), cont.end(), el);
}
#endif

template<typename C, typename F>
void ConstForEach(const C& container, F functor) {
	std::for_each(container.cbegin(), container.cend(), functor);
}

template<typename C, typename F>
void ForEach(C& container, F functor) {
	std::for_each(container.begin(), container.end(), functor);
}

template<typename C, typename type>
void removeFirst(C& container, const type& el) {
	for(auto it = container.begin(); it != container.end(); ++it) {
		if(*it == el) {
			container.erase(it);
			break;
		}
	}
}

template<typename C, typename type>
void removeAll(C& container, const type& el) {
	for(auto it = container.begin(); it != container.end();) {
		if(*it == el) {
			it = container.erase(it);
		} else {
			++it;
		}
	}
}

template<typename C, typename F>
void removeIf(C& container, F functor) {
    std::remove_if(container.begin(), container.end(), functor);
}
