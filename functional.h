#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

template<typename F, typename ... Fs>
inline auto compose(F f, Fs ... fs) {
    return compose(f, compose(fs...));
}

template<typename F, typename G>
inline auto compose(F f, G g) {
    return [=](auto x) { return g(f(x)); };
}

inline auto compose() {
    return [](auto x) { return x; };
}

template<typename H, typename ... Fs>
inline auto lift(H h, Fs ... fs) {
    return [=](auto p) { return h(fs(p) ...); };
}

template<typename H>
inline auto lift(H h) {
    return [=](auto p) { return h(p); };
}


#endif //FUNCTIONAL_H
