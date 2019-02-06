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
  Triangle();

  Triangle(Vect, Vect, Vect, Color);

  // method functions
  double getTriDist()
  {
    return getNormal().dotProduct(A);
  }
  Vect getNormal()
  {
    Vect Edge1(C.getVectX() - A.getVectX(), C.getVectY() - A.getVectY(), C.getVectZ() - A.getVectZ());
    Vect Edge2(B.getVectX() - A.getVectX(), B.getVectY() - A.getVectY(), B.getVectZ() - A.getVectZ());
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
      // ray is parallel to the plane
      return -1;
    }
    else
    {
      double b = normal.dotProduct(ray.getRayOrigin().vectAdd(normal.vectMult(distance).negative()));
      double distance_plane = -1 * b / a;
      double Qx = (ray_direction.vectMult(distance_plane)).getVectX() + ray_origin.getVectX();
      double Qy = (ray_direction.vectMult(distance_plane)).getVectY() + ray_origin.getVectY();
      double Qz = (ray_direction.vectMult(distance_plane)).getVectZ() + ray_origin.getVectZ();
      Vect Q(Qx, Qy, Qz);
      Vect CA(C.getVectX() - A.getVectX(), C.getVectY() - A.getVectY(), C.getVectZ() - A.getVectZ());
      Vect QA(Q.getVectX() - A.getVectX(), Q.getVectY() - A.getVectY(), Q.getVectZ() - A.getVectZ());
      double t1 = (CA.crossProduct(QA)).dotProduct(normal);
      Vect BC(B.getVectX() - C.getVectX(), B.getVectY() - C.getVectY(), B.getVectZ() - C.getVectZ());
      Vect QC(Q.getVectX() - C.getVectX(), Q.getVectY() - C.getVectY(), Q.getVectZ() - C.getVectZ());
      double t2 = (BC.crossProduct(QC)).dotProduct(normal);
      Vect AB(A.getVectX() - B.getVectX(), A.getVectY() - B.getVectY(), A.getVectZ() - B.getVectZ());
      Vect QB(Q.getVectX() - B.getVectX(), Q.getVectY() - B.getVectY(), Q.getVectZ() - B.getVectZ());
      double t3 = (AB.crossProduct(QB)).dotProduct(normal);
      if (t1 >= 0 && t2 >= 0 && t3 >= 0)
      {
        // return distance_plane;
        return -1 * b / a;
      }
      else
      {
        return -1;
      }
    }
  }
};

Triangle::Triangle()
{
  // normal = Vect(1, 0, 0);
  // distance = 0;
  A = Vect(1, 0, 0);
  B = Vect(0, 1, 0);
  C = Vect(0, 0, 1);
  color = Color(1, 0, 1, 0);
}

Triangle::Triangle(Vect A1, Vect B1, Vect C1, Color col)
{
  // normal = normalValue;
  A = A1;
  B = B1;
  C = C1;
  color = col;
  normal = getNormal();
}

#endif

