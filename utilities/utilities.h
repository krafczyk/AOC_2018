#include <algorithm>
#include <ostream>

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

template<typename T>
class array_2d {
    public:
        array_2d(size_t nx=0, size_t ny=0) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        ~array_2d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny) {
            this->nx = nx;
            this->ny = ny;
            initialize_internal();
        }
        T& assign(size_t i, size_t j) {
            return this->array[j*nx+i];
        }
        T operator()(size_t i, size_t j) const {
            return this->array[j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t j = 0; j < ny; ++j) {
                for(size_t i = 0; i < nx; ++i) {
                    out << (*this)(i,j);
                }
                out << std::endl;
            }
        }
    private:
        size_t nx;
        size_t ny;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny];
            }
        }
};

template<typename T>
class array_3d {
    public:
        array_3d(size_t nx=0, size_t ny=0, size_t nz = 0) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        ~array_3d() {
            if(array != nullptr) {
                delete [] array;
            }
        }
        void init(size_t nx, size_t ny, size_t nz) {
            this->nx = nx;
            this->ny = ny;
            this->nz = nz;
            initialize_internal();
        }
        T& assign(size_t i, size_t j, size_t k) {
            return this->array[k*nx*ny+j*nx+i];
        }
        T operator()(size_t i, size_t j, size_t k) const {
            return this->array[k*nx*ny+j*nx+i];
        }
        void print(std::ostream& out) const {
            for(size_t k = 0; k < nz; ++k) {
                for(size_t j = 0; j < ny; ++j) {
                    for(size_t i = 0; i < nx; ++i) {
                        out << (*this)(i,j);
                    }
                    out << std::endl;
                }
                out << std::endl;
            }
        }
    private:
        size_t nx;
        size_t ny;
        size_t nz;
        T* array;

        void initialize_internal() {
            if((this->nx == 0)||(this->ny == 0)||(this->nz == 0)) {
                array = nullptr;
            } else {
                array = new T[nx*ny*nz];
            }
        }
};

template<class T>
class map_val {
    public:
        map_val() {
            this->value = T();
            this->set = false;
        }
        map_val& operator=(const T& new_val) {
            this->value = new_val;
            this->set = true;
            return *this;
        }
        T value;
        bool set;
};

int pair_hash(int x, int y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned int A = (x >= 0 ? 2*x : -2*x-1);
    unsigned int B = (y >= 0 ? 2*y : -2*y-1);
    int C = (int)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}

long pair_hash_l(long x, long y) {
    // Here we use something similar to cantor pairing.
    // https://stackoverflow.com/questions/919612/mapping-two-integers-to-one-in-a-unique-and-deterministic-way
    unsigned long A = (x >= 0 ? 2*x : -2*x-1);
    unsigned long B = (y >= 0 ? 2*y : -2*y-1);
    long C = (long)((A >= B ? A*A+A+B : A+B*B)/2);
    return ((x < 0 && y < 0) || (x >= 0 && y >= 0) ? C : -C - 1);
}
