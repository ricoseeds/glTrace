#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"

class Triangle : public Object
{
  Vect A, B, C, normal;
  Color color;

public:
Triangle(Vect A1, Vect B1, Vect C1, Color col) : A(A1), B(B1), C(C1), color(col), normal(getNormal()) 
{}

  double getTriDist()
  {
    return getNormal().dotProduct(B);
  }
  Vect getNormal()
  {
    Vect Edge1(C.vectAdd(A.negative()));
    Vect Edge2(B.vectAdd(A.negative()));
    normal = Edge1.crossProduct(Edge2).normalize();
    return normal;
  }
  virtual Color getColor() { return color; }

  virtual Vect getNormalAt(Vect point)
  {

    return getNormal();
  }

  virtual double findIntersection(Ray ray)
  {
    Vect ray_direction = ray.getRayDirection();
    Vect ray_origin = ray.getRayOrigin();
    double a = ray_direction.dotProduct(normal);
    double distance = getTriDist();
    if (a == 0)
    {
      return -1;
    }
    else
    {
      double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
      double distance_plane = -1 * b / a;
      Vect Q(ray_direction.vectMult(distance_plane).vectAdd(ray_origin));
      Vect CA(C.vectAdd(A.negative()));
      Vect QA(Q.vectAdd(A.negative()));
      double t1 = (CA.crossProduct(QA)).dotProduct(normal);
      Vect BC(B.vectAdd(C.negative()));
      Vect QC(Q.vectAdd(C.negative()));
      double t2 = (BC.crossProduct(QC)).dotProduct(normal);
      Vect AB(A.vectAdd(B.negative()));
      Vect QB(Q.vectAdd(B.negative()));
      double t3 = (AB.crossProduct(QB)).dotProduct(normal);
      return (t1 >= 0 && t2 >= 0 && t3 >= 0) ? (double)-1 * b / a : -1;
    }
  }
};
#endif

