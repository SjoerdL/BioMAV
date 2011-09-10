/*
 * vector3d.h
 *
 *  Created on: 18.06.2010
 *      Author: paul
 */

#ifndef VECTOR3D_H_
#define VECTOR3D_H_

#include <armadillo>

typedef arma::fvec vector3d;
typedef arma::fmat matrix3d;

typedef float vector_value; // tolua_export

struct gl_vector3d {
  vector_value x, y, z;
};

class Vector;

void setVector(Vector& vec, vector_value x, vector_value y, vector_value z);
void glToVec(Vector& vec, const gl_vector3d* v);
gl_vector3d setGLVector(float x, float y, float z);
gl_vector3d vecToGl(const vector3d* v);

class Matrix { // tolua_export
  public:
    Matrix(); // tolua_export
    Matrix(const matrix3d& m);
    Matrix(const Matrix& m); // tolua_export
    Matrix(const vector_value* values);

    virtual ~Matrix(); // tolua_export

    Matrix& operator=(const Matrix& other);

    virtual vector_value* getValues();
    virtual void setValues(const vector_value* values);

    // tolua_begin
    virtual Matrix operator*(const Matrix& mat) const;
    virtual Vector operator[](const int i) const;
    virtual Vector operator*(const Vector& vec) const;

    virtual Vector at(const int i) const;
    virtual vector_value at(const int i, const int j) const;
    virtual void set(const int i, const int j, const vector_value value);

    virtual void inv();
    virtual void eye();
    virtual void zero();
    virtual void trans();

    virtual void setTranslate(const vector_value x, const vector_value y, const vector_value z);
    virtual void setTranslate(const Vector& vec);
    virtual void setScale(const vector_value x, const vector_value y, const vector_value z);
    virtual void setRotateMatrix(const Vector& vec, const vector_value rotation);

    virtual void setProjectionMatrix(const vector_value fov, const vector_value aspect, const vector_value near_plane, const vector_value far_plane);
    virtual void setViewMatrix(const Vector& from, const Vector& at, const Vector& up);
    // tolua_end
  protected:
    matrix3d mat;

    vector_value matCopy[16];

    friend class Vector;
}; // tolua_export

class Vector { // tolua_export
  public:
    Vector(); // tolua_export
    Vector(const arma::fvec& vec);
    Vector(const vector_value x, const vector_value y, const vector_value z); // tolua_export
    Vector(const Vector& vec); // tolua_export
    Vector(const vector_value* values);

    virtual ~Vector(); // tolua_export

    Vector& operator=(const Vector& other);

    virtual vector_value* getValues();
    virtual void setValues(const vector_value* values);

    // tolua_begin
    virtual Vector operator+(const Vector& vec2) const;
    virtual Vector operator-(const Vector& vec2) const;
    virtual Vector operator*(const vector_value value) const;
    virtual vector_value operator*(const Vector& vec) const;
    virtual vector_value operator[](const int i) const;

    virtual vector_value at(const int i) const;
    virtual void set(const int i, vector_value value);

    virtual vector_value abs() const;
    virtual Vector cross(const Vector& vec2) const;
    virtual void normalize();

    // tolua_end
  protected:
    vector3d vec;

    vector_value vecCopy[4];

    friend class Matrix;
    friend void setVector(Vector& vec, vector_value x, vector_value y, vector_value z);
    friend void glToVec(Vector& vec, const gl_vector3d* v);
}; // tolua_export

#endif /* VECTOR3D_H_ */
