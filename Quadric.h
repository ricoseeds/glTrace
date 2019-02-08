#ifndef _QUADRIC_H
#define _QUADRIC_H

#include "math.h"
#include "Object.h"
#include "Vect.h"
#include "Color.h"
#include <iostream>

class Quadric : public Object
{
  double A, B, C, D, E, F, G, H, I, J;
  Color color;

public:
  Quadric();
  Quadric(double a, double b, double c, double d, double e, double f, double g, double h, double i, double j, Color col ){
    A = a; B = b; C = c; D = d; E = e; F = f; I = i, J = j;
    color = col;
  }

  Vect getNormalAt(Vect point) override
  {
    // point = (Xi, Yi, Zi)
    // xn = 2 *A *xi + D *yi + E *zi + G
    // yn = 2 *B *yi + D *xi + F *zi + H
    // z n = 2 * C * zi + E * xi + F * yi + I
    double xn, yn, zn, xi, yi, zi;
    xi = point.getVectX();
    yi = point.getVectY();
    zi = point.getVectZ();
    xn = (2 * A * xi) + (D * yi) + (E * zi) + G;
    yn = (2 * B * yi) + (D * xi) + (F * zi) + H;
    zn = (2 * C * zi) + (E * xi) + (F * yi) + I;
    return Vect(xn, yn, zn).normalize();
  }

  double findIntersection(Ray ray) override
  {
    double Aq, Bq, Cq, xo, yo, zo, xd, yd, zd;
    xo = ray.getRayOrigin().getVectX();
    yo = ray.getRayOrigin().getVectY();
    zo = ray.getRayOrigin().getVectZ();
    xd = ray.getRayDirection().getVectX();
    yd = ray.getRayDirection().getVectY();
    zd = ray.getRayDirection().getVectZ();
    // Aqt^2 + Bqt + Cq = 0 with
    // Aq = Axd^2 + Byd^2 + Czd^2 + Dxdyd + Exdzd + Fydzd
    // Bq = 2*Axoxd + 2*Byoyd + 2*Czozd + D(xoyd + yoxd) + Exozd + F(yozd + ydzo) + Gxd + Hyd + Izd
    // Cq = Axo^2 + Byo^2 + Czo^2 + Dxoyo + Exozo + Fyozo + Gxo + Hyo + Izo + J
    Aq = (A * pow(xd, 2)) + (B * pow(yd, 2)) + (C * pow(zd, 2)) + (D * xd * yd) + (E * xd * zd) + (F * yd * zd);
    Bq = 2 * ((A * xo * xd) + (B * yo * yd) + (C * zo * zd)) + (D * ((xo * yd) + (yo * xd))) + (E * ((xo * zd) + (zo * xd))) + (F * ((yo * zd) + (yd * zo))) + (G * xd) + (H * yd) + (I * zd);
    Cq = (A * pow(xo, 2)) + (B * pow(yo, 2)) + (C * pow(zo, 2)) + (D * xo * yo) + (E * xo * zo) + (F * yo * zo) + (G * xo) + (H * yo) + (I * zo) + J;
    if (Aq != 0)
    {
      double DISCRIMINANT = (Bq * Bq) - (4 * Aq * Cq);
      if (DISCRIMINANT < 0)
      {
        return -1;
      }
      if (DISCRIMINANT == 0)
      {
        return -B / (2 * A);
      }
      else if (DISCRIMINANT > 0)
      {
        double t0 = (double)(-Bq + sqrt(DISCRIMINANT)) / (2 * Aq);
        double t1 = (double)(-Bq - sqrt(DISCRIMINANT)) / (2 * Aq);
        if (t0 > t1)
        {
          return (t1 < 0) ? -1 : t1;
        }
        else
        {
          return (t0 < 0) ? -1 : t0;
        }
      }
    }
    else if (Aq == 0)
    {
      return (double)(-Cq / Bq);
    }
  }
  Color getColor() override { return color; }
};

Quadric::Quadric()
{
  double scale = 10.0;
  A = (double)(100.0 / scale);
  B = (double)(5.0 / scale);
  C = (double)(4.0 / scale);
  D = 0;
  E = 0;
  F = 0;
  G = (double)(-200.0 / scale);
  H = (double)(-100.0 / scale);
  I = (double)(-8.0 / scale);
  J = (double)(104.0 / scale);
  color = Color(1.0, 1.0, 0.0, 0.4);
}

#endif

