#include <array>
#include <ostream>

template<typename T>
struct vec2 {
    explicit vec2(const T& px, const T& py) : x(px), y(py) {}
    T x,y;
};

template<int N,typename T> const T& get(const vec2<T>& a,std::enable_if_t<N==0>* = 0) { return a.x; };
template<int N,typename T> const T& get(const vec2<T>& a,std::enable_if_t<N==1>* = 0) { return a.y; };

template<typename T>
struct vec3 {
    explicit vec3(const T& px, const T& py, const T& pz) : x(px), y(py), z(pz) {}
    T x, y, z;
};

template<int N,typename T> const T& get(const vec3<T>& a,std::enable_if_t<N==0>* =0) { return a.x; };
template<int N,typename T> const T& get(const vec3<T>& a,std::enable_if_t<N==1>* =0) { return a.y; };
template<int N,typename T> const T& get(const vec3<T>& a,std::enable_if_t<N==2>* =0) { return a.z; };

template<typename T> T dot(const vec3<T>& a,const vec3<T>& b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
template<typename T> vec3<T> operator/(const vec3<T>& a,const T& b) { return vec3<T>(a.x / b,a.y / b,a.z / b); }

template<typename T>
struct mat22 {
    explicit mat22(
        const T& x0,const T& y0,
        const T& x1,const T& y1
    ) : r0(x0,y0),r1(x1,y1) {}

    vec2<T> r0,r1;
};

template<int N,typename T> const vec2<T>& get(const mat22<T>& a,std::enable_if_t<N==0>* =0) { return a.r0; };
template<int N,typename T> const vec2<T>& get(const mat22<T>& a,std::enable_if_t<N==1>* =0) { return a.r1; };
template<int N,int M,typename T> const T& get(const mat22<T>& a) { return get<N>(get<M>(a)); };
template<int N,int M,typename T> T& get(mat22<T>& a) { return get<N>(get<M>(a)); };

template<typename T>
struct mat33 {
    explicit mat33(
        const T& x0,const T& y0,const T& z0,
        const T& x1,const T& y1,const T& z1,
        const T& x2,const T& y2,const T& z2
    ) : r0(x0,y0,z0),r1(x1,y1,z1),r2(x2,y2,z2) {}

    explicit mat33(const vec3<T>& pr0,const vec3<T>& pr1,const vec3<T>& pr2) 
        : r0(pr0),r1(pr1),r2(pr2) {}

    vec3<T> r0,r1,r2;
};

template<int N,typename T> const vec3<T>& get(const mat33<T>& a,std::enable_if_t<N==0>* =0) { return a.r0; };
template<int N,typename T> const vec3<T>& get(const mat33<T>& a,std::enable_if_t<N==1>* =0) { return a.r1; };
template<int N,typename T> const vec3<T>& get(const mat33<T>& a,std::enable_if_t<N==2>* =0) { return a.r2; };
template<int N,int M,typename T> const T& get(const mat33<T>& a) { return get<N>(get<M>(a)); };
template<int N,int M,typename T> T& get(mat33<T>& a) { return get<N>(get<M>(a)); };

template<int N,typename T> vec3<T> row(const mat33<T>& a) { return get<N>(a); };
template<int N,typename T> vec3<T> col(const mat33<T>& a) { return vec3<T>(get<N>(a.r0),get<N>(a.r1),get<N>(a.r2)); };

template<typename T> mat33<T> operator+(const mat33<T>& a,const mat33<T>& b) { return mat33<T>(a.r0+b.r0,a.r1+b.r1,a.r2+b.r2); }
template<typename T> mat33<T> operator/(const mat33<T>& a,const T& b) { return mat33<T>(a.r0/b,a.r1/b,a.r2/b); }

template<typename T> mat33<T> operator*(const mat33<T>& a,const mat33<T>& b) {
    return type44<T>(
        dot(row<0>(a),col<0>(b)),dot(row<0>(a),col<1>(b)),dot(row<0>(a),col<2>(b)),
        dot(row<1>(a),col<0>(b)),dot(row<1>(a),col<1>(b)),dot(row<1>(a),col<2>(b)),
        dot(row<2>(a),col<0>(b)),dot(row<2>(a),col<1>(b)),dot(row<2>(a),col<2>(b))
    );
}

template<typename T>  vec3<T> operator*(const mat33<T>& a,const vec3<T>& b) {
    return vec3<T>(dot(row<0>(a),b),dot(row<1>(a),b),dot(row<2>(a),b));
}

template<int I,typename T>
mat22<T> sub22(const mat33<T>& a){
    static const int X = I % 3;
    static const int Y = I / 3;
    static const int X1 = X==0 ? 1 : X==1 ? 0 : 0;
    static const int X2 = X==0 ? 2 : X==1 ? 2 : 1;
    static const int Y1 = Y==0 ? 1 : Y==1 ? 0 : 0;
    static const int Y2 = Y==0 ? 2 : Y==1 ? 2 : 1;
    return mat22<T>(
        get<X1,Y1>(a),get<X2,Y1>(a),
        get<X1,Y2>(a),get<X2,Y2>(a)
    );
}

template<typename T> T determinant(const mat22<T>& a) { 
    return get<0,0>(a)*get<1,1>(a) - get<1,0>(a)*get<0,1>(a);
}

template<typename T> T determinant(const mat33<T>& a) { 
    return get<0,0>(a)*determinant(sub22<0>(a)) - get<1,0>(a)*determinant(sub22<1>(a)) + get<2,0>(a)*determinant(sub22<2>(a));
}

template<typename T> mat22<T> adjugate(const mat22<T>& a) { 
    return mat22<T>(get<1,1>(a),-get<1,0>(a),-get<0,1>(a),get<0,0>(a));
}

template<typename T> mat33<T> adjugate(const mat33<T>& a) {
    return mat33<T>(
        determinant(sub22<0>(a)), -determinant(sub22<3>(a)),  determinant(sub22<6>(a)),
        -determinant(sub22<1>(a)),  determinant(sub22<4>(a)), -determinant(sub22<7>(a)),
        determinant(sub22<2>(a)), -determinant(sub22<5>(a)),  determinant(sub22<8>(a))
    );
}

template<typename T> mat33<T> inverse(const mat33<T>& a) { return adjugate(a) / determinant(a); }

using fvec3 = vec3<float>;
using fmat33 = mat33<float>;

using dvec3 = vec3<double>;
using dmat33 = mat33<double>;


template<typename T,int N>
struct vec_n {
    T at(int col) const { return data[col]; }
    T& at(int col) { return data[col]; }

    static const int nN = N;
    std::array<T, N> data;
};

template<typename T,int N>
std::ostream& operator<<(std::ostream& os, const vec_n<T,N>& vec){
    for(int col=0; col<N; ++col){
        os << vec.at(col) << ", ";
    }
    return os;
}


template<typename T,int N>
struct matrix_nn {
    T at(int row, int col) const { return data[row*nN + col]; }
    T& at(int row, int col) { return data[row*nN + col]; }

    static const int nN = N;
    std::array<T, N*N> data;
};

template<typename T,int N>
matrix_nn<T,N> operator/(const matrix_nn<T,N>& mat, T div){
    matrix_nn<T,N> ret;
    for(int row=0; row < N; row++){
        for(int col=0; col < N; col++){
            ret.at(row, col) = mat.at(row, col) / div;
        }
    }
    return ret;
}

template<typename T,int N>
vec_n<T,N> operator*(const matrix_nn<T,N>& mat, const vec_n<T,N>& vec){
    vec_n<T,N> ret;
    for(int row=0; row < N; row++){
        ret.at(row) = T(0);
        for(int col=0; col < N; col++){
            ret.at(row) += mat.at(row, col) * vec.at(col);
        }
    }
    return ret;
}

template<typename T,int N>
matrix_nn<T,N> minor(const matrix_nn<T,N>& mat, int colMatrix){
    matrix_nn<T,N> ret;
    int row2 = 0, col2 = 0;
    for(int row=1; row<N; row++){
        for(int col=0; col<N; col++){
            if(col == colMatrix){
                continue;
            }
            ret.at(row2, col2) = mat.at(row, col);
            col2++;
            if(col2 == (N - 1)){
                row2++;
                col2 = 0;
            }
        }
    }
    return ret;
}

template<typename T,int N>
T determinate(const matrix_nn<T,N>& mat, int size){
    T sum = 0;
    matrix_nn<T,N> ret;
    if (size == 1){
        return mat.at(0,0);
    }else if(size == 2){
        return mat.at(0,0) * mat.at(1,1) - mat.at(0,1) * mat.at(1,0);
    }else{
        for(int col=0; col<size; col++){
            ret = minor(mat, col);
            sum += mat.at(0, col) * std::pow(-1.0, col) * determinate(ret, size-1);
        }
    }
    return sum;
}

template<typename T,int N>
matrix_nn<T,N> cofactor(const matrix_nn<T,N>& mat){
    matrix_nn<T,N> minor_mat, co_factor;
    int col3, row3, row2, col2, row, col;
    for (row3=0; row3 < N; row3++){
        for (col3=0; col3 < N; col3++){
            row2 = 0; col2 = 0;
            for (row=0; row < N; row++){
                for (col=0; col < N; col++){
                    if (row != row3 && col != col3){
                        minor_mat.at(row2, col2) = mat.at(row, col);
                        if (col2 < (N - 2)){
                            col2++;
                        }
                        else {
                            col2 = 0;
                            row2++;
                        }
                    }
                }
            }
            co_factor.at(row3,col3) = std::pow(-1, (row3 + col3)) * determinate(minor_mat, (N - 1));
        }
    }
    return transpose(co_factor); 

}

template<typename T,int N>
matrix_nn<T,N> inverse(const matrix_nn<T,N>& mat) {
    T det = determinate(mat, N);
    if(det == 0){
        throw std::runtime_error("non invertible matrix");
    }
    
    return cofactor(mat) / det;   
}

template<typename T,int N>
matrix_nn<T,N> transpose(const matrix_nn<T,N>& mat){
    T det = determinate(mat, N);
    matrix_nn<T,N> ret;
    int row, col;
    for (row=0; row < N; row++){
        for (col=0; col < N; col++){
            ret.at(row,col) = mat.at(col,row);
        }
    }
    return ret;
}

template<typename T,int N>
std::ostream& operator<<(std::ostream& os, const matrix_nn<T,N>& mat){
    for(int row=0; row<N; ++row){
        for(int col=0; col<N; ++col){
            os << mat.at(row, col) << ", ";
        }
        os << std::endl;
    }
    return os;
}

using dvec6 = vec_n<double, 6>;
using dmat66 = matrix_nn<double, 6>;
