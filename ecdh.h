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
int mod_inverse(int a, int n) {
    if (n == 1) {
        return 1;
    }

    int t, q;
    int x0 = 0, x1 = 1;

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

template<int p /* prime */>
class PF {
public:
    static constexpr int characteristic = p;
    static constexpr int size = p;

    PF(int a) : a_(a % p + p * (a % p < 0)) {}

    int operator()() const { return a_; }
    operator int() const { return a_; }

    PF<p> inverse() const {
        assert(a_ != 0);
        return mod_inverse(a_, p);
    }

    PF<p> operator-() const { return -a_; }

private:
    int a_;
};


template<int p>
PF<p> operator+(const PF<p> x, const PF<p> y) {
    return PF<p>(x() + y());
}

template<int p>
PF<p> operator-(const PF<p> x, const PF<p> y) {
    return PF<p>(x() - y());
}

template<int p>
PF<p> operator*(const PF<p> x, const PF<p> y) {
    return PF<p>(x() * y());
}

template<int p>
PF<p> operator/(const PF<p> x, const PF<p> y) {
    return PF<p>(x() * y.inverse());
}

template<int p>
bool operator==(const PF<p> x, const PF<p> y) {
    return x() == y();
}

template<int p>
std::ostream& operator<<(std::ostream& os, const PF<p> x) {
    return os << x();
}

//
// Elliptic curve
//

template<typename F /* field of char not 2 or 3 */>
class EllipticCurve {
public:
    class Element {
    public:
        explicit Element(const EllipticCurve<F>* curve)
            : curve_(curve), identity_(true), x_(0), y_(0) {}
        Element(const EllipticCurve<F>* curve, F x, F y)
            : curve_(curve), identity_(false), x_(x), y_(y) {}

        bool operator==(const Element& other) const {
            return identity_ == other.identity_
                && x_ == other.x_
                && y_ == other.y_;
        }

        bool is_identity() const { return identity_; }
        F x() const { assert(!identity_); return x_; };
        F y() const { assert(!identity_); return y_; };

        // group law
        const Element operator+(const Element& other) const {
            assert(*curve_ == *(other.curve_));

            // trivial
            if (identity_) {
                return other;
            } else if (other.identity_) {
                return *this;
            }

            // mirrored
            if (x_ == other.x_ && y_ == -other.y_) {
                return (*curve_)();
            }

            F lambda(0);
            if (x_ == other.x_ && y_ == other.y_) {
                lambda = (3*x_*x_ + curve_->a_)/(2*y_);
            } else {
                // Elliptic curves given by the equation y^2 = x^3 + ax + b are
                // y-axes symmetric, therefore in this case we have x_ !=
                // other.x_.
                lambda = (y_ - other.y_)/(x_ - other.x_);
            }

            auto x3 = lambda*lambda - x_ - other.x_;
            auto nu = y_ - lambda*x_;
            auto y3 = -lambda*x3 - nu;
            return (*curve_)(x3, y3);
        }

        // inverse element
        const Element operator-() const {
            if (identity_) {
                return *this;
            }
            return Element(curve_, x_, -y_);
        }

        // diffence
        const Element operator-(const Element& other) const {
            return *this + (-other);
        }

    private:
        const EllipticCurve<F>* curve_;
        const bool identity_;
        const F x_, y_;
    };

    EllipticCurve(const F a, const F b) : a_(a), b_(b) {
        assert(F::characteristic > 3);
        assert(discriminant() != 0);
    }

    // calculate the discriminant
    F discriminant() const { return -16 * (4*a_*a_*a_ + 27*b_*b_); }

    // calculate number of points
    template<typename L /* finite extension of F */>
    size_t size() const {
        size_t count = 1;  // first point is the infinity
        for (int x = 0; x < L::size; ++x) {
            for (int y = 0; y < L::size; ++y) {
                if (contains(x, y)) {
                    count +=1;
                }
            }
        }
        return count;
    }

    // compare the curve to another one
    bool operator==(const EllipticCurve<F>& other) const {
        return a_ == other.a_ && b_ == other.b_;
    }

    // check if the given point lies on the curve
    bool contains(const F x, const F y) const {
        return y*y == x*x*x + a_*x + b_;
    }

    // return identity
    const Element operator()() const {
        return Element(this);
    }

    // return point lying on the curve defined by (x, y)
    const Element operator()(const F x, const F y) const {
        return Element(this, x, y);
    }

private:
    F a_, b_;
};
