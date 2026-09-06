#pragma once
#include <utility>
#include "vec.h"

template<int nrows, int ncols, typename T> struct matrix {
    vec<ncols,T> rows[nrows] = {};
    vec<ncols,T>&       operator[](int i)       { assert(i>=0 && i<nrows); return rows[i]; }
    const vec<ncols,T>& operator[](int i) const { assert(i>=0 && i<nrows); return rows[i]; }
};

template<int nrows, int ncols, typename T>
vec<nrows,T> operator*(const matrix<nrows,ncols,T> &m, const vec<ncols,T> &v) {
    vec<nrows,T> ret;
    for (int i = 0; i < nrows; i++) ret[i] = m[i]*v;
    return ret;
}

template<int R1, int C1, int C2, typename T>
matrix<R1,C2,T> operator*(const matrix<R1,C1,T> &a, const matrix<C1,C2,T> &b) {
    matrix<R1,C2,T> ret;
    for (int i = 0; i < R1; i++)
        for (int j = 0; j < C2; j++)
            for (int k = 0; k < C1; k++)
                ret[i][j] += a[i][k]*b[k][j];
    return ret;
}

template<int nrows, int ncols, typename T>
matrix<ncols,nrows,T> transpose(const matrix<nrows,ncols,T> &m) {
    matrix<ncols,nrows,T> ret;
    for (int i = 0; i < nrows; i++)
        for (int j = 0; j < ncols; j++)
            ret[j][i] = m[i][j];
    return ret;
}

template<int n, typename T> matrix<n,n,T> identity() {
    matrix<n,n,T> ret;
    for (int i = 0; i < n; i++) ret[i][i] = T(1);
    return ret;
}

template<int n, typename T> matrix<n,n,T> invert(matrix<n,n,T> m) {
    matrix<n,n,T> inv = identity<n,T>();
    for (int col = 0; col < n; col++) {
        int pivot = col;
        for (int r = col+1; r < n; r++)
            if (std::abs(m[r][col]) > std::abs(m[pivot][col])) pivot = r;
        assert(m[pivot][col] != T(0) && "matrix is singular");
        std::swap(m[col], m[pivot]);
        std::swap(inv[col], inv[pivot]);

        T d = m[col][col];
        m[col]   = m[col]   * (T(1)/d);
        inv[col] = inv[col] * (T(1)/d);
        for (int r = 0; r < n; r++) {
            if (r == col) continue;
            T f = m[r][col];
            m[r]   = m[r]   - m[col]*f;
            inv[r] = inv[r] - inv[col]*f;
        }
    }
    return inv;
}

template<int nrows, int ncols, typename T>
std::ostream& operator<<(std::ostream &out, const matrix<nrows,ncols,T> &m) {
    for (int i = 0; i < nrows; i++) out << m[i] << "\n";
    return out;
}

using matrix2 = matrix<2,2,float>;
using matrix3 = matrix<3,3,float>;
using matrix4 = matrix<4,4,float>;
