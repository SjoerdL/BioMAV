/*
 * vector3d.cpp
 *
 *  Created on: 18.06.2010
 *      Author: paul
 */

#include "vector3d.hpp"

#include <iostream>
#include <math.h>

using namespace std;

Matrix :: Matrix() {
  eye();
}

Matrix :: Matrix(const matrix3d& m) {
  mat = m;
}

Matrix :: Matrix(const Matrix& m) {
  mat = matrix3d(m.mat);
}

Matrix :: Matrix(const vector_value* values) {
  setValues(values);
}

Matrix :: ~Matrix() {
//  cout << "Matrix destroyed" << endl;
}

Matrix& Matrix :: operator=(const Matrix& other) {
  if (this != &other) {
    mat = other.mat;
  }
  return *this;
}

vector_value* Matrix :: getValues() {
  int i = 0;
  for (int col = 0; col < 4; col++)
    for (int row = 0; row < 4; row++)
      matCopy[i++] = at(row, col);

  return matCopy;
}

void Matrix :: setValues(const vector_value* values) {
  int i = 0;
  for (int col = 0; col < 4; col++)
    for (int row = 0; row < 4; row++)
      set(row, col, values[i++]);
}

Matrix Matrix :: operator*(const Matrix& mat) const {
  return Matrix(this->mat * mat.mat);
}

Vector Matrix :: operator*(const Vector& vec) const {
  return Vector(mat * vec.vec);
}

Vector Matrix :: operator[](const int i) const {
  return at(i);
}

Vector Matrix :: at(const int i) const {
  return Vector(mat.col(i));;
}

vector_value Matrix :: at(const int i, const int j) const {
  return mat(i, j);
}

void Matrix :: set(const int i, const int j, const vector_value value) {
  mat(i, j) = value;
}

void Matrix :: inv() {
  arma::inv(mat);
}

void Matrix :: eye() {
  mat = arma::eye<matrix3d>(4, 4);
}

void Matrix :: zero() {
  mat = arma::zeros<matrix3d>(4, 4);
}

void Matrix :: trans() {
  mat = arma::trans(mat);
}

void Matrix :: setTranslate(const vector_value x, const vector_value y, const vector_value z) {
  eye();
  mat(0, 3) = x;
  mat(1, 3) = y;
  mat(2, 3) = z;
}

void Matrix :: setTranslate(const Vector& vec) {
  eye();
  mat(0, 3) = vec[0];
  mat(1, 3) = vec[1];
  mat(2, 3) = vec[2];
}

void Matrix :: setScale(const vector_value x, const vector_value y, const vector_value z) {
  eye();
  mat(0, 0) = x;
  mat(1, 1) = y;
  mat(2, 2) = z;
}

void Matrix :: setRotateMatrix(const Vector& vec, const vector_value rotation) {
  Vector v = vec;
  v.normalize();

  vector_value cos_val = cos(rotation);
  vector_value sin_val = sin(rotation);

  eye();
  mat(0, 0) = v[0] * v[0] * (1 - cos_val) + cos_val;
  mat(0, 1) = v[0] * v[1] * (1 - cos_val) - v[2] * sin_val;
  mat(0, 2) = v[0] * v[2] * (1 - cos_val) + v[1] * sin_val;

  mat(1, 0) = v[1] * v[0] * (1 - cos_val) + v[2] * sin_val;
  mat(1, 1) = v[1] * v[1] * (1 - cos_val) + cos_val;
  mat(1, 2) = v[1] * v[2] * (1 - cos_val) - v[0] * sin_val;

  mat(2, 0) = v[2] * v[0] * (1 - cos_val) - v[1] * sin_val;
  mat(2, 1) = v[2] * v[1] * (1 - cos_val) + v[0] * sin_val;
  mat(2, 2) = v[2] * v[2] * (1 - cos_val) + cos_val;
}

void Matrix :: setProjectionMatrix(const vector_value fov, const vector_value aspect, const vector_value near_plane, const vector_value far_plane) {
  eye();

  vector_value f = 1 / tan(fov / 2);

  mat(0, 0) = f / aspect;
  mat(1, 1) = f;
  mat(2, 2) = (far_plane + near_plane) / (near_plane - far_plane);
  mat(2, 3) = (2 * far_plane * near_plane) / (near_plane - far_plane);
  mat(3, 2) = -1;
  mat(3, 3) = 0;
}

void Matrix :: setViewMatrix(const Vector& from, const Vector& at, const Vector& up) {
  Vector n_up = up;
  Vector n_at = from - at;

  n_up.normalize();
  n_at.normalize();

  // Verticalize up-vector
  vector_value proj = n_up * n_at;
  n_up = n_up - (n_at * proj);
  n_up.normalize();

  Vector right = n_at.cross(n_up);

  eye();
  mat(0, 0) = right[0];
  mat(0, 1) = right[1];
  mat(0, 2) = right[2];
  mat(1, 0) = n_up[0];
  mat(1, 1) = n_up[1];
  mat(1, 2) = n_up[2];
  mat(2, 0) = n_at[0];
  mat(2, 1) = n_at[1];
  mat(2, 2) = n_at[2];

  Matrix from_translate = Matrix();
  from_translate.setTranslate(-from[0], -from[1], -from[2]);
  mat = mat * from_translate.mat;
}


Vector :: Vector() {
  vec = vector3d(4);
  vec(0) = 0.0;
  vec(1) = 0.0;
  vec(2) = 0.0;
  vec(3) = 1.0;
}

Vector :: Vector(const arma::fvec& vec) {
  this->vec = vector3d(4);
  this->vec(0) = vec(0);
  this->vec(1) = vec(1);
  this->vec(2) = vec(2);
  this->vec(3) = 1.0;
}

Vector :: Vector(const vector_value x, const vector_value y, const vector_value z) {
  vec = vector3d(4);
  setVector(*this, x, y, z);
}

Vector :: Vector(const Vector& vec) {
  this->vec = vec.vec;
}

Vector :: Vector(const vector_value* values) {
  setValues(values);
}

Vector :: ~Vector() {
//  cout << "Vector destroyed" << endl;
}

Vector& Vector :: operator=(const Vector& other) {
  if (this != &other) {
    vec = other.vec;
  }
  return *this;
}

vector_value* Vector :: getValues() {
  for (int i = 0; i < 4; i++)
    vecCopy[i] = at(i);

  return vecCopy;
}

void Vector :: setValues(const vector_value* values) {
  for (int i = 0; i < 4; i++)
    set(i, values[i]);
}

Vector Vector :: operator+(const Vector& vec2) const {
  Vector res = Vector(vec + vec2.vec);
  res.vec(3) = vec(3);
  return res;
}

Vector Vector :: operator-(const Vector& vec2) const {
  Vector res = Vector(vec - vec2.vec);
  res.vec(3) = vec(3);
  return res;
}

Vector Vector :: operator*(const vector_value value) const {
  Vector res = Vector(value * vec);
  res.vec(3) = vec(3);
  return res;
}

vector_value Vector :: operator*(const Vector& vec) const {
  return arma::dot(this->vec, vec.vec) - this->vec(3) * vec.vec(3);
}

vector_value Vector :: operator[](const int i) const {
  return at(i);
}

vector_value Vector :: at(const int i) const {
  return vec(i);
}

void Vector :: set(const int i, vector_value value) {
  vec(i) = value;
}

vector_value Vector :: abs() const {
  return sqrt(*this * *this);
}

Vector Vector :: cross(const Vector& vec2) const {
  vector3d v1 (3);
  v1(0) = vec(0);
  v1(1) = vec(1);
  v1(2) = vec(2);

  vector3d v2 (3);
  v2(0) = vec2.vec(0);
  v2(1) = vec2.vec(1);
  v2(2) = vec2.vec(2);

  return Vector(arma::cross(v1, v2));
}

void Vector :: normalize() {
  vector_value l = abs();
  if (l > 0.0) {
    vec(0) = vec(0) / l;
    vec(1) = vec(1) / l;
    vec(2) = vec(2) / l;
  }
}




gl_vector3d setGLVector(vector_value x, vector_value y, vector_value z) {
  gl_vector3d res;
  res.x = x;
  res.y = y;
  res.z = z;
  return res;
}

void setVector(Vector& vec, vector_value x, vector_value y, vector_value z) {
  vec.vec(0) = x;
  vec.vec(1) = y;
  vec.vec(2) = z;
  vec.vec(3) = 1.0;
}

void glToVec(Vector& vec, const gl_vector3d* v) {
  vec.vec(0) = v->x;
  vec.vec(1) = v->y;
  vec.vec(2) = v->z;
  vec.vec(3) = 1.0;
}

gl_vector3d vecToGl(const Vector& v) {
  gl_vector3d vec;
  vec.x = v.at(1);
  vec.y = v.at(2);
  vec.z = v.at(3);
  return vec;
}
