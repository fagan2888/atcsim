/* 
 * File:   Airplane.cpp
 * Author: michael
 * 
 * Created on 25. mars 2013, 21:39
 */

#include <iostream>

#include <cmath>
#include <string>

#include "SDL/SDL_mouse.h"

#include "Airplane.h"
#include "Airway.h"
#include "Cloud.h"
#include "Constants.h"
#include "ForbiddenZone.h"
#include "Surface.h"



Airplane::Airplane(unsigned int number, std::string identification, unsigned int
                   altitude, float cape, int velocity, Point initialPosition,
                   enum CardinalPoints in, enum CardinalPoints out)
:
Entity(cape, velocity, initialPosition), // Parent constructor.
number_(number),
identification_(identification), // Flight number.
altitude_(altitude),
predictedCollision_(),
in_(in),
out_(out)
{
}



void Airplane::compute(enum PosType posType, int gameFieldWidth, int gameFieldHeight) {
  
  // The cape of this entity can change (user input) : compute it.
  if (posType == realPosition)
    updateCape();
  // This entity can move : compute it.
  computeMovement(posType, gameFieldWidth, gameFieldHeight);
  
}



void Airplane::checkForCollisionDispatch(Entity& entity, enum PosType posType) const {
  entity.checkForCollision(this, posType);
}



void Airplane::checkForCollision(const Airplane* airplane, enum PosType posType) {
    
  const Point* airplane1Pos = getPosition(posType);
  const Point* airplane2Pos = airplane->getPosition(posType);
  
  // Collision if the distance between 2 airplanes is smaller than DMINCOL.
  if (std::sqrt(std::pow(airplane1Pos->x - airplane2Pos->x, 2) +
          std::pow(airplane1Pos->y - airplane2Pos->y, 2)) < DMINCOL &&
          static_cast<unsigned int>(std::abs(int(altitude_) -
          int(airplane->getAltitude()))) < DMINVERTCOL) {
    switch (posType) {
      case realPosition:
//        std::cout << "Collision with an airplane." << std::endl;
        break;
      case simPosition:
        predictedCollision_.airplane = true;
    }
  }
}



void Airplane::checkForCollision(const ForbiddenZone* forbiddenZone, enum PosType posType) {

  // Terminate the simulation if the airplane is inside a forbidden zone.
    
  if (forbiddenZone->isInside(*getPosition(posType), posType, false)) {
    switch (posType) {
      case realPosition:
//        std::cout << "Collision with a forbidden zone." << std::endl;
        break;
      case simPosition:
        predictedCollision_.forbiddenZone = true;
    }
  }
}



void Airplane::checkForCollision(const Airway* airway, enum PosType posType) {

  // Remove points if the airplane is outside an airway.
    
  if (airway->isInside(*getPosition(posType), posType, false)) {
    switch (posType) {
      case realPosition:
//        std::cout << "Collision with an airway." << std::endl;
        break;
      case simPosition:
        predictedCollision_.airway = true;
    }
  }
}



void Airplane::checkForCollision(const Cloud* cloud, enum PosType posType) {

  // Remove points if the airplane is inside a cloud.
  
  if (cloud->isInside(*getPosition(posType), posType, false)) {
    switch (posType) {
      case realPosition:
//        std::cout << "Collision with a cloud." << std::endl;
        break;
      case simPosition:
        predictedCollision_.cloud = true;
    }
  }  
}



const struct Point* Airplane::getPosition(enum PosType posType) const {
  const struct Point* position;
  switch (posType) {
    case realPosition:
      position = &realPosition_;
      break;
    case simPosition:
      position = &simPosition_;
  }
  return position;
}



unsigned int Airplane::getAltitude() const {
  return altitude_;
}



void Airplane::resetSimulation() {
  Entity::resetSimulation();
  predictedCollision_.reset();
}



bool Airplane::isInside(Point point, enum PosType posType, bool mouse) const {
  
  // Select the position from which to test.
  const Point* position;
  switch (posType) {
    case realPosition:
      position = &realPosition_;
      break;
    case simPosition:
      position = &simPosition_;
  }
  
  if (point.x >= position->x-10 && point.x <= position->x+10 &&
      point.y >= position->y-10 && point.y <= position->y+10)
    return true;
  else
    return false;
  
}



void Airplane::render(Surface& displaySurf) const {
  
  // Airplane labels.
  const std::string labelL1(identification_);
  const std::string labelL2(std::to_string(altitude_) + "m   " +
                            std::to_string(int(std::round(currentCape_))) + "°");
  const std::string labelL3(std::to_string(velocity_) + "km/h");
  // SDL_ttf is a text rendering engine, not a text layout engine.
  // So it doesn't print "\n".
  
  // Default text color is black, become orange if warning, red if danger.
  uint8_t red(0), green(0), blue(0);
  if (predictedCollision_.forbiddenZone || predictedCollision_.airplane) {
    red = 255;
  } else if (predictedCollision_.cloud) {
    red = 255;
    green = 128;
  }
  
  // Create surfaces out of text strings.
  Surface textSurf1(labelL1, red, green, blue, BOLDFONT, 14);
  Surface textSurf2(labelL2, 0, 0, 0, STDFONT, 14);
  Surface textSurf3(labelL3, 0, 0, 0, STDFONT, 14);
    
  // Change airplane color if selected by user.
  red = green = blue = 100;
  if (selected_) {
    red = 150;
    green = 0;
    blue = 150;
  }
  
  // Airplane is actually a 21x21 pixels rectangle with diagonals.
  Surface airplaneSurf(21, 21);
  airplaneSurf.drawRectangle(0, 0, 20, 20, red, green, blue, 255);
  airplaneSurf.drawRectangle(1, 1, 19, 19, red, green, blue, 255);
  airplaneSurf.drawLine(0, 0, 20, 20, red, green, blue, 255);
  airplaneSurf.drawLine(0, 20, 20, 0, red, green, blue, 255);
  
  // Draw the airplane and his related text informations at it's actual position.
  displaySurf.blit(airplaneSurf, int16_t(realPosition_.x-10), int16_t(realPosition_.y-10));
  displaySurf.blit(textSurf1,    int16_t(realPosition_.x-20), int16_t(realPosition_.y+15));
  displaySurf.blit(textSurf2,    int16_t(realPosition_.x-20), int16_t(realPosition_.y+30));
  displaySurf.blit(textSurf3,    int16_t(realPosition_.x-20), int16_t(realPosition_.y+45));
  
  // Draw a line which indicates the predicted airplane trajectory.
  traceLineFromAirplane(displaySurf, simPosition_, red, green, blue);
  
  // Draw a line which indicates the new cape a user would choose.
  if (selected_) {
    int x, y;
    SDL_GetMouseState(&x, &y);
    traceLineFromAirplane(displaySurf, Point(x,y), red, green, blue);
  }
  
  // Print airplane informations on the side panel.
  printSidePanelInfo(displaySurf);
  
}



void Airplane::printSidePanelInfo(Surface& displaySurf) const {
  
  const std::string labelL1("Airplane " + identification_);
  const std::string labelL2("in: " + in_.toString() + "    out: " + out_.toString());
  std::string labelL3("Status: OK");
  
  if (currentCape_ != targetCape_)
    labelL3 = "Airplane turning !";
  
  // Default text color is black, become red if warning.
  uint8_t redL2(0), greenL2(160);
  
  if (predictedCollision_.airplane) {
    labelL3 = "Danger: airplane collision !";
    redL2 = 255;
    greenL2 = 0;
  } else if (predictedCollision_.forbiddenZone) {
    labelL3 = "Danger: forbidden zone !";
    redL2 = 255;
    greenL2 = 0;
  } else if (predictedCollision_.airway) {
    labelL3 = "Warning: out of airway !";
    redL2 = 255;
    greenL2 = 128;
  } else if (predictedCollision_.cloud) {
    labelL3 = "Warning: turbulence zone !";
    redL2 = 255;
    greenL2 = 128;
  }
  
  Surface textSurf1(labelL1, 0, 0, 0, BOLDFONT, 14);
  Surface textSurf2(labelL2, 0, 0, 0, STDFONT, 14);
  Surface textSurf3(labelL3, redL2, greenL2, 0, STDFONT, 14);
  
  displaySurf.blit(textSurf1, 810, int16_t((number_-1) * 70 + 20));
  displaySurf.blit(textSurf2, 820, int16_t((number_-1) * 70 + 40));
  displaySurf.blit(textSurf3, 820, int16_t((number_-1) * 70 + 60));
}



void Airplane::traceLineFromAirplane(Surface& displaySurf, Point endPoint,
        uint8_t red, uint8_t green, uint8_t blue) const {

  const float xPred(endPoint.x - realPosition_.x);
  const float yPred(endPoint.y - realPosition_.y);
  
  Surface predTrajectorySurf(
          int16_t(std::abs(xPred))+1,
          int16_t(std::abs(yPred))+1);
  
  predTrajectorySurf.drawLine(
          int16_t(xPred>0?0:-xPred),
          int16_t(yPred>0?0:-yPred),
          int16_t(xPred>0?xPred:0),
          int16_t(yPred>0?yPred:0),
          red, green, blue, 255);
  
  displaySurf.blit(predTrajectorySurf,
          int16_t(xPred>0?realPosition_.x:realPosition_.x+xPred),
          int16_t(yPred>0?realPosition_.y:realPosition_.y+yPred));
  
}

