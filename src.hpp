#include <iostream>
#include <stdexcept>

// Exception type as specified
struct divided_by_zero final : std::exception {
public:
    divided_by_zero() = default;
    ~divided_by_zero() = default;
    const char *what() const noexcept override { return "Divided by zero!"; }
};

// Helper utilities within an anonymous namespace to avoid symbol clashes
namespace {
    template <typename _Tp>
    inline _Tp abs_val(_Tp x) {
        return x < _Tp() ? -x : x;
    }

    template <typename _Tp>
    inline void do_swap(_Tp &a, _Tp &b) {
        _Tp t = a; a = b; b = t;
    }

    template <typename _Tp>
    inline _Tp gcd_nonneg(_Tp x, _Tp y) {
        // x, y assumed non-negative
        while (y != _Tp()) {
            x %= y;
            do_swap(x, y);
        }
        return x;
    }
}

template <typename _Tp>
struct fraction {
private:
    _Tp num; // numerator
    _Tp den; // denominator, kept positive

    inline void normalize() {
        if (num == _Tp()) { den = _Tp(1); return; }
        _Tp a = abs_val(num);
        _Tp b = abs_val(den);
        _Tp g = gcd_nonneg(a, b);
        if (g != _Tp(0)) { num /= g; den /= g; }
        if (den < _Tp()) { num = -num; den = -den; }
    }

public:
    // Constructors
    fraction(): num(_Tp()), den(_Tp(1)) {}
    fraction(_Tp x): num(x), den(_Tp(1)) { /* already normalized */ }
    fraction(_Tp x, _Tp y) {
        if (y == _Tp()) throw divided_by_zero();
        num = x; den = y; normalize();
    }

    // Arithmetic operators (return new fraction)
    fraction operator + (const fraction &rhs) const {
        // a/b + c/d = (a*(d/g) + c*(b/g)) / (b*(d/g)), where g = gcd(b, d)
        if (num == _Tp()) return rhs; if (rhs.num == _Tp()) return *this;
        _Tp g = gcd_nonneg(abs_val(den), abs_val(rhs.den));
        _Tp l = rhs.den / g; // d/g
        _Tp r = den / g;     // b/g
        fraction res;
        res.num = num * l + rhs.num * r;
        res.den = den * l;   // b * (d/g)
        res.normalize();
        return res;
    }

    fraction operator - (const fraction &rhs) const {
        if (rhs.num == _Tp()) return *this;
        _Tp g = gcd_nonneg(abs_val(den), abs_val(rhs.den));
        _Tp l = rhs.den / g; // d/g
        _Tp r = den / g;     // b/g
        fraction res;
        res.num = num * l - rhs.num * r;
        res.den = den * l;
        res.normalize();
        return res;
    }

    fraction operator * (const fraction &rhs) const {
        // (a/g1 * c/g2) / (b/g2 * d/g1) with g1=gcd(|a|,|d|), g2=gcd(|c|,|b|)
        if (num == _Tp() || rhs.num == _Tp()) return fraction();
        _Tp g1 = gcd_nonneg(abs_val(num), abs_val(rhs.den));
        _Tp g2 = gcd_nonneg(abs_val(rhs.num), abs_val(den));
        fraction res;
        res.num = (num / g1) * (rhs.num / g2);
        res.den = (den / g2) * (rhs.den / g1);
        res.normalize();
        return res;
    }

    fraction operator / (const fraction &rhs) const {
        // a/b / (c/d) = (a*d)/(b*c)
        if (rhs.num == _Tp()) throw divided_by_zero();
        if (num == _Tp()) return fraction();
        _Tp g1 = gcd_nonneg(abs_val(num), abs_val(rhs.num));
        _Tp g2 = gcd_nonneg(abs_val(den), abs_val(rhs.den));
        fraction res;
        res.num = (num / g1) * (rhs.den / g2);
        res.den = (den / g2) * (rhs.num / g1);
        if (res.den == _Tp()) throw divided_by_zero();
        res.normalize();
        return res;
    }

    fraction operator ^ (long long e) const {
        // 0^0 = 1, 0^neg -> throw
        if (e == 0) return fraction(_Tp(1));
        if (num == _Tp()) {
            if (e < 0) throw divided_by_zero();
            return fraction();
        }
        fraction base = *this;
        long long k = e;
        if (k < 0) {
            // reciprocal
            _Tp n = base.num, d = base.den;
            if (n == _Tp()) throw divided_by_zero();
            base.num = d; base.den = n; base.normalize();
            k = -k;
        }
        fraction res(_Tp(1));
        while (k) {
            if (k & 1) res = res * base;
            base = base * base;
            k >>= 1;
        }
        return res;
    }

    // Compound assignment operators
    fraction &operator += (const fraction &rhs) { return *this = *this + rhs; }
    fraction &operator -= (const fraction &rhs) { return *this = *this - rhs; }
    fraction &operator *= (const fraction &rhs) { return *this = *this * rhs; }
    fraction &operator /= (const fraction &rhs) { return *this = *this / rhs; }
    fraction &operator ^= (long long e) { return *this = *this ^ e; }

    // Conversions
    explicit operator double() const {
        // Assumes _Tp is convertible to double
        return static_cast<double>(num) / static_cast<double>(den);
    }
    explicit operator bool() const { return num != _Tp(); }

    // Friends (declare templates as friends)
    template <typename U> friend bool operator==(const fraction<U> &, const fraction<U> &);
    template <typename U> friend bool operator<(const fraction<U> &, const fraction<U> &);
    template <typename U> friend std::ostream & operator<<(std::ostream &, const fraction<U> &);
};

// Equality: class maintains normalized form, so direct compare works
template <typename _Tp>
inline bool operator == (const fraction<_Tp> &lhs, const fraction<_Tp> &rhs) {
    return lhs.num == rhs.num && lhs.den == rhs.den;
}

// Less-than using cross multiplication; guaranteed safe per problem statement
template <typename _Tp>
inline bool operator < (const fraction<_Tp> &lhs, const fraction<_Tp> &rhs) {
    // a/b < c/d  <=> a*d < c*b
    return lhs.num * rhs.den < rhs.num * lhs.den;
}

// Output as "num/den"
template <typename _Tp>
inline std::ostream & operator << (std::ostream &os, const fraction<_Tp> &x) {
    os << x.num << '/' << x.den; return os;
}
