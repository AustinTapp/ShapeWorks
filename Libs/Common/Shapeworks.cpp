#include "Shapeworks.h"

namespace shapeworks {

Point toPoint(const Dims &d) { return Point({static_cast<double>(d[0]), static_cast<double>(d[1]), static_cast<double>(d[2])}); }
Point toPoint(const Coord &c) { return Point({static_cast<double>(c[0]), static_cast<double>(c[1]), static_cast<double>(c[2])}); }
Vector toVector(const Dims &d) { return makeVector({static_cast<double>(d[0]), static_cast<double>(d[1]), static_cast<double>(d[2])}); }
Vector toVector(const Point &p) { return makeVector({p[0], p[1], p[2]}); } 
Point toPoint(const Vector &v) { return Point({v[0], v[1], v[2]}); }

/// Enables construction using an initializer list: `Vector3 f() { return makeVector({1,2,3}); }`
//itkVector doesn't have this handy ctor like itkPoint; `Point p({a,b,c})` works, but `Vector3 v({1,2,3})` doesn't.
Vector3 makeVector(std::array<double, 3>&& arr) { return Vector3(arr.data()); }

template<>
Vector3 invert(Vector3 &&v) { return makeVector({1.0/v[0], 1.0/v[1], 1.0/v[2]}); }

Vector3 cross(const Vector3 &a, const Vector3 &b)
{
  return makeVector({a[1]*b[2] - a[2]*b[1], -(a[0]*b[2] - a[2]*b[0]), a[0]*b[1] - a[1]*b[0]});
}

Vector3 dot(const Vector3 &a, const Vector3 &b)
{
  return makeVector({a[0]*b[0], a[1]*b[1], a[2]*b[2]});
}

Point3 operator+(const Point3 &p, const Point3 &q)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] + q[i];
  return ret;
}

Point3& operator+=(Point3 &p, const Point3 &q)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] += q[i];
  return p;
}

Point3 operator/(const Point3 &p, const double x)
{
  Point3 ret;
  for (unsigned i = 0; i < 3; i++)
    ret[i] = p[i] / x;
  return ret;
}

Point3& operator/=(Point3 &p, const double x)
{
  for (unsigned i = 0; i < 3; i++)
    p[i] /= x;
  return p;
}

itk::Index<3> toIndex(const IPoint3 &pt) 
{ 
  return itk::Index<3>({pt[0], pt[1], pt[2]}); 
}

itk::Size<3> toSize(const IPoint3 &pt) 
{ 
  return itk::Size<3>({static_cast<itk::SizeValueType>(pt[0]), static_cast<itk::SizeValueType>(pt[1]), static_cast<itk::SizeValueType>(pt[2])}); 
}

bool axis_is_valid(const Vector3 &axis)
{
  const double eps = 1E-6;
  return axis.GetSquaredNorm() > eps;
}

double degToRad(const double deg)
{
  return deg * Pi / 180.0;
}

AffineTransformPtr createAffineTransform(const Matrix33 &mat, const Vector3 &translate) {
  AffineTransformPtr xform(AffineTransform::New());
  xform->SetMatrix(mat);
  xform->SetOffset(translate);
  return xform;
}

}; //shapeworks
