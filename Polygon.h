/* 
 * File:   Polygons.h
 * Author: michael
 *
 * Created on 26. mars 2013, 07:08
 */

#ifndef POLYGONS_H
#define	POLYGONS_H


#include <cstddef>   // Definition of size_t.
#include <cstdint>   // Definitions of uint8_t and int16_t.
#include <vector>

#include "Entity.h"


namespace ATCsim {  // Project ATCsim namespace.


// Forward declarations (no header includes) (namespace pollution, build time).
class Surface;


class Polygon : public Entity {
  
public:
  
  // Constructor takes x and y coordinates, color, cape and velocity of a new polygon.
  Polygon(const std::vector<int16_t>& xPoints,
          const std::vector<int16_t>& yPoints,
          uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha,
          float cape, unsigned int velocity);
  
  // Use the default (member to member) copy ctor and copy assignment operator.
  Polygon(const Polygon& orig) = default;
  Polygon& operator=(const Polygon& orig) = default;
  // Use the default (member to member) move ctor and move assignment operator.
  Polygon(Polygon&& orig) = default;
  Polygon& operator=(Polygon&& orig) = default;
  
  // Default : call base class'es destructor and destructors of all members.
  // Do not throw any exception (which is what we want for a dtor).
  virtual ~Polygon() noexcept(true) = default;
  
  // Redeclaration of virtual methods inherited from IEntity interface class.
  virtual void render(Surface& displaySurf) const final;
  
  // Check if a point is inside an entity.
  virtual bool isInside(Point point, PosTypes posType = realPosition,
                        bool mouse = false) const final;
  
protected:
  
private:
  
  // Polygon corners.
  std::vector<int16_t> xCorners_, yCorners_;
  
  // Maximum depth on the 2 axis, to determine SDL Surface minimum size.
  int16_t xMax_, yMax_;
  
  // Polygon color.
  uint8_t red_, green_, blue_, alpha_;
 
  // Helper method for Polygon::isInside.
  int isLeft(const Point& point, size_t p0, size_t p1) const;

  // Prohibit default constructor usage.
  // Mark methods that won’t be implemented with '= delete' (C++11).
  Polygon() = delete;
  
};
  

}  // End of project ATCsim namespace.


#endif	/* POLYGONS_H */

