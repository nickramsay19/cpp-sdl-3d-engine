#pragma once
#include <vector>
#include <tuple>
#include <utility>
#include <concepts>
#include <iostream>
#include <cmath>
#include <cstdint>
#include <cassert>

constexpr double PI = std::numbers::pi;

template<typename N>
concept Numeric = requires(N n)
{
    requires std::is_integral_v<N> || std::is_floating_point_v<N>;
    requires !std::is_same_v<bool, N>;
    requires std::is_arithmetic_v<decltype(n + 1)>;
    requires !std::is_pointer_v<N>;
};

template <Numeric N>
struct Point {
    N x;
    N y;
    N z;

    Point(N x, N y, N z) : x{x}, y{y}, z{z} {}

    template <Numeric M>
    Point(M x, M y, M z) : x{static_cast<N>(x)}, y{static_cast<N>(y)}, z{static_cast<N>(z)} {}

    template <Numeric M>
    operator Point<M>() const {
        return Point<M>(
            static_cast<M>(x),
            static_cast<M>(y),
            static_cast<M>(z)
        );
    }

    template <Numeric M>
    Point& operator+=(const Point<M>& rhs) {
        this->x += static_cast<N>(rhs.x);
        this->y += static_cast<N>(rhs.y);
        this->z += static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M>
    Point& operator-=(const Point<M>& rhs) {
        this->x -= static_cast<N>(rhs.x);
        this->y -= static_cast<N>(rhs.y);
        this->z -= static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M>
    Point& operator*=(const Point<M>& rhs) {
        this->x *= static_cast<N>(rhs.x);
        this->y *= static_cast<N>(rhs.y);
        this->z *= static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M>
    friend Point<decltype(std::declval<N>() + std::declval<M>())> operator+(const Point<N>& lhs, const Point<M>& rhs);

    template <Numeric M>
    friend Point<decltype(std::declval<N>() - std::declval<M>())> operator-(const Point<N>& lhs, const Point<M>& rhs);

    template <Numeric M>
    friend Point<decltype(std::declval<N>() * std::declval<M>())> operator*(const Point<N>& lhs, const Point<M>& rhs);

    template <Numeric M>
    friend Point<decltype(std::declval<N>() / std::declval<M>())> operator/(const Point<N>& lhs, const Point<M>& rhs);

    void Display() const {
        std::cout << "(" << x << "," << y << "," << z << ")";
    }
};

template <Numeric N, Numeric M>
Point<decltype(std::declval<N>() + std::declval<M>())> operator+(const Point<N>& lhs, const Point<M>& rhs) {
    using O = decltype(std::declval<N>() + std::declval<M>());
    return Point<O>(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

//template <Numeric N, Numeric M>
template <Numeric N, Numeric M, Numeric O = decltype(std::declval<N>() - std::declval<M>())>
Point<O> operator+(const Point<N>& lhs, const Point<M>& rhs) {
    //using O = decltype(std::declval<N>() - std::declval<M>());
    return Point<O>(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

template <Numeric N>
struct Triangle {
    Point<N> a;
    Point<N> b;
    Point<N> c;

    Triangle(Point<N> a, Point<N> b, Point<N> c) : a{a}, b{b}, c{c} {}

    template <Numeric M>
    Triangle(Point<M> a, Point<M> b, Point<M> c) : a(static_cast<Point<N>>(a)), b(static_cast<Point<N>>(b)), c(static_cast<Point<N>>(c)) {}

    // addition with a Point
    template <Numeric M>
    Triangle& operator+=(const Point<M>& rhs) {
        this->a.x += rhs.x; 
        this->b.x += rhs.x; 
        this->c.x += rhs.x; 

        this->a.y += rhs.y; 
        this->b.y += rhs.y; 
        this->c.y += rhs.y; 

        this->a.z += rhs.z; 
        this->b.z += rhs.z; 
        this->c.z += rhs.z; 
        return *this;
    }

    template <Numeric M>
    Triangle& operator-=(const Point<M>& rhs) {
        this->a.x -= rhs.x; 
        this->b.x -= rhs.x; 
        this->c.x -= rhs.x; 

        this->a.y -= rhs.y; 
        this->b.y -= rhs.y; 
        this->c.y -= rhs.y; 

        this->a.z -= rhs.z; 
        this->b.z -= rhs.z; 
        this->c.z -= rhs.z; 
        return *this;
    }

    Triangle& operator*=(const Point<N>& rhs) {
        this->a.x *= rhs.x; 
        this->b.x *= rhs.x; 
        this->c.x *= rhs.x; 

        this->a.y *= rhs.y; 
        this->b.y *= rhs.y; 
        this->c.y *= rhs.y; 

        this->a.z *= rhs.z; 
        this->b.z *= rhs.z; 
        this->c.z *= rhs.z; 
        return *this;
    }

    // arithmetic with other triangles
    template <Numeric M = N>
    Triangle& operator+=(const Triangle<M>& rhs) {
        this->a += static_cast<N>(rhs.x);
        this->b += static_cast<N>(rhs.y);
        this->c += static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M = N>
    Triangle& operator-=(const Triangle<M>& rhs) {
        this->a -= static_cast<N>(rhs.x);
        this->b -= static_cast<N>(rhs.y);
        this->c -= static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M = N>
    Triangle& operator*=(const Triangle<M>& rhs) {
        this->a *= static_cast<N>(rhs.x);
        this->b *= static_cast<N>(rhs.y);
        this->c *= static_cast<N>(rhs.z);
        return *this;
    }

    template <Numeric M>
    friend Triangle<decltype(std::declval<N>() + std::declval<M>())> operator+(const Triangle<N>& lhs, const Triangle<M>& rhs);

    template <Numeric M>
    friend Triangle<decltype(std::declval<N>() - std::declval<M>())> operator-(const Triangle<N>& lhs, const Triangle<M>& rhs);

    template <Numeric M>
    friend Triangle<decltype(std::declval<N>() * std::declval<M>())> operator*(const Triangle<N>& lhs, const Triangle<M>& rhs);

    template <Numeric M>
    friend Triangle<decltype(std::declval<N>() / std::declval<M>())> operator/(const Triangle<N>& lhs, const Triangle<M>& rhs);

    void Display() const {
        std::cout << "[";
        a.Display();
        std::cout << ",";
        b.Display();
        std::cout << ",";
        c.Display();
        std::cout << "]";
    }
};

template <Numeric N>
struct Mesh {
    std::vector<Triangle<N>> tris;
    Point<N> origin;

    int red = 0, green = 0, blue = 0;

    Mesh(std::vector<Triangle<N>> tris)
        : origin{Point<N>(0,0,0)}, tris{tris} {}

    Mesh(Point<N> offset, N size, std::vector<Triangle<N>> tris) 
        : tris{tris}, origin{offset}
    {
        for (Triangle<N>& t : this->tris) {
            t.a.x *= size;
            t.a.y *= size;
            t.a.z *= size;

            t.b.x *= size;
            t.b.y *= size;
            t.b.z *= size;

            t.c.x *= size;
            t.c.y *= size;
            t.c.z *= size;

            t.a.x += origin.x;
            t.a.y += origin.y;
            t.a.z += origin.z;

            t.b.x += origin.x;
            t.b.y += origin.y;
            t.b.z += origin.z;

            t.c.x += origin.x;
            t.c.y += origin.y;
            t.c.z += origin.z;
        }
    }

    template <Numeric M = N>
    void set_origin(const Point<M>& p) {
        const Point<N> pN = static_cast<Point<N>>(p);

        // calculate change
        Point<N> translation = pN;
        translation -= origin;

        origin = pN;
        for (Triangle<N>& t : tris) {
            t += translation;
        }
    }

    template <Numeric M = N>
    void translate(const Point<M>& p) {
        const Point<N> pN = static_cast<Point<N>>(p);
        for (Triangle<N>& t : tris) {
            t += pN;
        }
        origin += pN;
    }

    template <Numeric M = N>
    void scale(M s) {
        const N sN = static_cast<N>(s);
        for (Triangle<N>& t : tris) {
            t -= origin;
            t *= Point<N>(sN,sN,sN);
            t += origin;
        }
    }

    void rotate(double pitch, double yaw, double roll) {
        for (Triangle<N>& t : this->tris) {

            t -= this->origin;

            // pitch
            double r_zy = 0.0;
            double a0_zy = 0.0;
            r_zy = sqrtl(powl(t.a.z, 2) + powl(t.a.y, 2));
            a0_zy = atan2l(t.a.y, t.a.z);
            t.a.z = r_zy * cosl(a0_zy + pitch);
            t.a.y = r_zy * sinl(a0_zy + pitch);

            r_zy = sqrtl(powl(t.b.z, 2) + powl(t.b.y, 2));
            a0_zy = atan2l(t.b.y, t.b.z);
            t.b.z = r_zy * cosl(a0_zy + pitch);
            t.b.y = r_zy * sinl(a0_zy + pitch);

            r_zy = sqrtl(powl(t.c.z, 2) + powl(t.c.y, 2));
            a0_zy = atan2l(t.c.y, t.c.z);
            t.c.z = r_zy * cosl(a0_zy + pitch);
            t.c.y = r_zy * sinl(a0_zy + pitch);

            // yaw
            double r_xz = 0.0;
            double a0_xz = 0.0;

            r_xz = sqrtl(powl(t.a.x, 2) + powl(t.a.z, 2));
            a0_xz = atan2l(t.a.z, t.a.x);
            t.a.x = r_xz * cosl(a0_xz + yaw);
            t.a.z = r_xz * sinl(a0_xz + yaw);

            r_xz = sqrtl(powl(t.b.x, 2) + powl(t.b.z, 2));
            a0_xz = atan2l(t.b.z, t.b.x);
            t.b.x = r_xz * cosl(a0_xz + yaw);
            t.b.z = r_xz * sinl(a0_xz + yaw);

            r_xz = sqrtl(powl(t.c.x, 2) + powl(t.c.z, 2));
            a0_xz = atan2l(t.c.z, t.c.x);
            t.c.x = r_xz * cosl(a0_xz + yaw);
            t.c.z = r_xz * sinl(a0_xz + yaw);

            t += this->origin;
        }
    }
};

template <Numeric N>
Mesh<N> MakeCube(Point<N> origin, N scale) {
    return Mesh<N> (origin, scale, std::vector<Triangle<N>> {
        // south
        Triangle<N>(Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(-0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f)),
        Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f)),

        // east
        Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f)),
        Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(0.5f,-0.5f,0.5f)),

        // north
        Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f)),
        Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(-0.5f,-0.5f,0.5f)),

        // west
        Triangle<N>(Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(-0.5f,0.5f,-0.5f)),
        Triangle<N>(Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,-0.5f), Point<N>(-0.5f,-0.5f,-0.5f)),

        // top
        Triangle<N>(Point<N>(-0.5f,0.5f,-0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(0.5f,0.5f,0.5f)),
        Triangle<N>(Point<N>(-0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(0.5f,0.5f,-0.5f)),

        // bottomN
        Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,-0.5f)),
        Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(0.5f,-0.5f,-0.5f)),
    });
}

/*template <Numeric N>
struct CubeMesh : Mesh<N> {
    CubeMesh(N size, Point<N> offset) : 
        Mesh<N>(offset, size, std::vector<Triangle<N>> {
            // south
            Triangle<N>(Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(-0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f)),
            Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f)),

            // east
            Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f)),
            Triangle<N>(Point<N>(0.5f,-0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(0.5f,-0.5f,0.5f)),

            // north
            Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f)),
            Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(-0.5f,-0.5f,0.5f)),

            // west
            Triangle<N>(Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(-0.5f,0.5f,-0.5f)),
            Triangle<N>(Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,0.5f,-0.5f), Point<N>(-0.5f,-0.5f,-0.5f)),

            // top
            Triangle<N>(Point<N>(-0.5f,0.5f,-0.5f), Point<N>(-0.5f,0.5f,0.5f), Point<N>(0.5f,0.5f,0.5f)),
            Triangle<N>(Point<N>(-0.5f,0.5f,-0.5f), Point<N>(0.5f,0.5f,0.5f), Point<N>(0.5f,0.5f,-0.5f)),

            // bottomN
            Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,-0.5f)),
            Triangle<N>(Point<N>(0.5f,-0.5f,0.5f), Point<N>(-0.5f,-0.5f,-0.5f), Point<N>(0.5f,-0.5f,-0.5f)),
        }) {}

    template <Numeric M>
    CubeMesh<N>& operator+=(const Point<M>& rhs) {
        this->origin += static_cast<Point<N>>(rhs);

        for (Triangle<N>& t : this->tris) {
            t += rhs;
        }
        return *this;
    }
};*/
