#ifndef FUNCTIONAL_H
#define FUNCTIONAL_H

template<typename ... Fs>
inline auto compose(const Fs &... fs);

template<typename F, typename ... Fs>
inline auto compose(const F &f, const Fs &... fs) {
    return [=](auto x) { return compose(fs ...)(f(x)); };
}

template<>
inline auto compose() {
    return [](auto x) { return x; };
}

template<typename H, typename ... Fs>
inline auto lift(const H &h, const Fs &... fs) {
    return [=](auto p) { return h(fs(p) ...); };
}

#endif //FUNCTIONAL_H
