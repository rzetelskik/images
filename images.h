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
    return [=]([[gnu::unused]] Point p) { return t; };
}

template<typename T>
Base_image<T> rotate(Base_image<T> image, double phi) {
    auto transform = [=](Point p) {
        return Point(p.first * cos(phi) + p.second * sin(phi), p.second * cos(phi) - p.first * sin(phi));
    };

    return [=](Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> translate(Base_image<T> image, Vector v) {
    auto transform = [=](Point p) { return Point(p.first - v.first, p.second - v.second); };

    return [=](Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> scale(Base_image<T> image, double s) {
    auto transform = [=](Point p) { return Point(p.first / s, p.second / s); };

    return [=](Point p) { return lift(image, transform)(p); };
}

template<typename T>
Base_image<T> circle(Point q, double r, T inner, T outer) {
    return [=](Point p) { return distance(p, q) <= r ? inner : outer; };
}

template<typename T>
Base_image<T> checker(double d, T this_way, T that_way) {
    return [=](Point p) {
        return (static_cast<int>(std::floor(p.first / d) + std::floor(p.second / d)) & 1) ? that_way : this_way;
    };
}

template<typename T>
Base_image<T> polar_checker(double d, int n, T this_way, T that_way) {
    return [=](Point p) {
        auto sc = [=](Point p) { return Point(p.first, (p.second * n * d) / (2 * M_PI)); };
        return compose(to_polar, sc, checker(d, this_way, that_way))(p);
    };
}

template<typename T>
Base_image<T> rings(Point q, double d, T this_way, T that_way) {
    return [=](Point p) {
        return (static_cast<int>(std::floor(distance(p, q) / d)) & 1) ? that_way : this_way;
    };
}

template<typename T>
Base_image<T> vertical_stripe(double d, T this_way, T that_way) {
    return [=](Point p) { return (std::abs(p.first) <= d / 2) ? this_way : that_way; };
}

inline Image cond(Region region, Image this_way, Image that_way) {
    auto h = [=](auto a, auto b, auto c) { return a ? b : c; };

    return [=](Point p) { return lift(h, region, this_way, that_way)(p); };
}

inline Image lerp(Blend blend, Image this_way, Image that_way) {
    auto h = [=](auto a, auto b, auto c) { return a.weighted_mean(b, c); };

    return [=](Point p) { return lift(h, this_way, that_way, blend)(p); };
}

inline Image darken(Image image, Blend blend) {
    auto h = [=](auto a, auto b) { return Colors::black.weighted_mean(a, b); };

    return [=](Point p) { return lift(h, image, blend)(p); };
}

inline Image lighten(Image image, Blend blend) {
    auto h = [=](auto a, auto b) { return Colors::white.weighted_mean(a, b); };

    return [=](Point p) { return lift(h, image, blend)(p); };
}


#endif //IMAGES_H
