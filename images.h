#ifndef IMAGES_H
#define IMAGES_H

#include <functional>
#include <cmath>
#include "functional.h"
#include "coordinate.h"
#include "color.h"

using Fraction = double;

template<typename T>
using Base_image = std::function<T(const Point)>;

using Region = Base_image<bool>;

using Image = Base_image<Color>;

using Blend = Base_image<Fraction>;

template<typename T>
inline Base_image<T> constant(const T t) {
    return [=]([[gnu::unused]] const Point p) { return t; };
}

template<typename T>
Base_image<T> rotate(const Base_image<T> &image, const double &phi) {
    auto transform = [=](const Point p) {
        return Point(p.first * cos(phi) + p.second * sin(phi), p.second * cos(phi) - p.first * sin(phi));
    };

    return [=](const Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> translate(const Base_image<T> &image, const Vector &v) {
    auto transform = [=](const Point p) { return Point(p.first - v.first, p.second - v.second); };

    return [=](const Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> scale(const Base_image<T> &image, const double &s) {
    auto transform = [=](const Point p) { return Point(p.first / s, p.second / s); };

    return [=](const Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> circle(const Point &q, const double &r, const T &inner, const T &outer) {
    return [=](const Point p) { return distance(p, q) <= r ? inner : outer; };
}

template<typename T>
Base_image<T> checker(const double &d, const T &this_way, const T &that_way) {
    return [=](const Point p) {
        return (static_cast<int>(std::floor(p.first / d) + std::floor(p.second / d)) & 1) ? that_way : this_way;
    };
}

template<typename T>
Base_image<T> polar_checker(const double &d, const int &n, const T &this_way, const T &that_way) {
    return [=](const Point p) {
        auto sc = [=](const Point p) { return Point(p.first, (p.second * n * d) / (2 * M_PI)); };
        return compose(to_polar, sc, checker(d, this_way, that_way))(p);
    };
}

template<typename T>
Base_image<T> rings(const Point &q, const double &d, const T &this_way, const T &that_way) {
    return [=](const Point p) {
        return (static_cast<int>(std::floor(distance(p, q) / d)) & 1) ? that_way : this_way;
    };
}

template<typename T>
Base_image<T> vertical_stripe(const double &d, const T &this_way, const T &that_way) {
    return [=](const Point p) { return (std::abs(p.first) <= d / 2) ? this_way : that_way; };
}

inline Image cond(const Region &region, const Image &this_way, const Image &that_way) {
    auto h = [=](auto a, auto b, auto c) { return a ? b : c; };

    return [=](const Point p) { return lift(h, region, this_way, that_way)(p); };
}

inline Image lerp(const Blend &blend, const Image &this_way, const Image &that_way) {
    auto h = [=](auto a, auto b, auto c) { return a.weighted_mean(b, c); };

    return [=](const Point p) { return lift(h, this_way, that_way, blend)(p); };
}

inline Image darken(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant(Colors::black));
}

inline Image lighten(const Image &image, const Blend &blend) {
    return lerp(blend, image, constant(Colors::white));
}


#endif //IMAGES_H
