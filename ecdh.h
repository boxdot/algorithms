#include <ostream>
#include <assert.h>

//
// Used algorithms
//

constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1) {
    return exp < 1
        ? result
        : ipow(base*base, exp/2, (exp % 2) ? result*base : result);
}

// integral exponent
constexpr int64_t pow(int64_t base, int exp) {
    return ipow(base, exp, 1);
}

// inverse mod n
// Note: gcc >= 5 needed, clang 3.6 is fine
// cf. https://gcc.gnu.org/gcc-5/changes.html#cxx, extended constexpr.
constexpr int64_t mod_inverse(int64_t a, int64_t n) {
    if (n == 1) {
        return 1;
    }

    int64_t t = 0, q = 0;
    int64_t x0 = 0, x1 = 1;

    while (a > 1) {
        q = a / n;
        t = n, n = a % n, a = t;
        t = x0, x0 = x1 - q * x0, x1 = t;
    }
    return x1;
}

//
// Finite prime field (of characteristic p)
//
template<int64_t p /* prime */>
class PF {
public:
    static constexpr int64_t characteristic = p;
    static constexpr int64_t size = p;

    constexpr PF(int64_t a) : a_(a % p + p * (a % p < 0)) {}

    constexpr int64_t operator()() const { return a_; }

    constexpr PF<p> inverse() const {
        assert(a_ != 0);
        return mod_inverse(a_, p);
    }

    constexpr PF<p> operator-() const { return -a_; }

private:
    const int64_t a_;
};


template<int64_t p>
constexpr PF<p> operator+(const PF<p> x, const PF<p> y) {
    return PF<p>(x() + y());
}

// Z-module structure
template<int64_t p>
constexpr PF<p> operator*(int64_t a, const PF<p> y) {
    return PF<p>(a*y());
}

template<int64_t p>
constexpr PF<p> operator-(const PF<p> x, const PF<p> y) {
    return PF<p>(x() - y());
}

template<int64_t p>
constexpr PF<p> operator*(const PF<p> x, const PF<p> y) {
    return PF<p>(x() * y());
}

template<int64_t p>
constexpr PF<p> operator/(const PF<p> x, const PF<p> y) {
    return PF<p>(x * y.inverse());
}

template<int64_t p>
constexpr bool operator==(const PF<p> x, const PF<p> y) {
    return x() == y();
}

template<int64_t p>
std::ostream& operator<<(std::ostream& os, const PF<p> x) {
    return os << x();
}

// generic neq
template<typename T>
constexpr bool operator!=(const T& a, const T& b) {
    return !(a == b);
}

//
// Elliptic curve
//

template<typename F /* field of char != 2, 3 */, int64_t a, int64_t b>
class EllipticCurve {
public:
    class Point {
    public:
        // identity point (at infinity)
        Point() : identity_(true), x_(0), y_(0) {}
        // affine point
        Point(const F& x, const F& y) : identity_(false), x_(x), y_(y) {
            assert(contains(x, y));
        }

        bool identity() const { return identity_; }
        const F x() const { return x_; }
        const F y() const { return y_; }

        bool operator==(const Point& p) const {
            return (identity_ && p.identity_ )|| (x_ == p.x_ && y_ == p.y_);
        }

        // Group Law
        const Point operator+(const Point& p) const  {
            // trivial
            if (identity_) {
                return p;
            } else if (p.identity_) {
                return *this;
            }

            // mirrored
            if (x_ == p.x_ && y_ == -p.y_) {
                return Point();
            }

            F lambda =
                x_ == p.x_ && y_ == p.y_
                ? (3*x_*x_ + F(a))/(2*y_)
                : (y_ - p.y_)/(x_ - p.x_);

            auto x3 = lambda * lambda - x_ - p.x_;
            auto nu = y_ - lambda * x_;
            auto y3 = -lambda * x3 - nu;
            return Point(x3, y3);
        }

        // inverse
        const Point operator-() const {
            if (identity_) {
                return *this;
            }
            return Point(x_, -y_);
        }

        // difference
        const Point operator-(const Point& p) const {
            return (*this) + (-p);
        }

        // Z-module structure
        // TODO: This can be done better.
        friend const Point operator*(uint64_t n, const Point& p) {
            return mult(n, p, p);
        }

        // output
        friend std::ostream& operator<<(std::ostream& os, const Point& p) {
            if (p.identity_) {
                return os << "O";
            }
            return os << "(" << p.x_ << ", " << p.y_ << ")";
        }

    private:
        friend const Point mult(uint64_t n, const Point& p, const Point& res) {
            if (n == 0) {
                return Point();
            } else if (n == 1) {
                return res;
            }
            return mult(n - 1, p, res + p);
        }

        const bool identity_;
        const F x_, y_;
    };

    // Curve properties

    constexpr static F discriminant() { return -16 * (4*a*a*a + 27*b*b); }

    static bool contains(const F& x, const F& y) {
        return y*y == x*x*x + a*x + F(b);
    }

    // number of points (or order of the group E(F))
    // brute force, TODO: Replace by Schoof's algorithm
    template<typename L /* finite extension of F */ = F>
    static size_t size() {
        size_t count = 1;  // first point is the infinity
        for (int64_t x = 0; x < L::size; ++x) {
            for (int64_t y = 0; y < L::size; ++y) {
                if (contains(x, y)) {
                    count += 1;
                }
            }
        }
        return count;
    }

    static_assert(F::characteristic != 2, "Char 2 is not supported");
    static_assert(F::characteristic != 3, "Char 3 is not supported");
    static_assert(discriminant() != F(0), "Discriminant is zero");
};
