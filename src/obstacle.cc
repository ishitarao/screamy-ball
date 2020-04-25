//
// Created by Ishita Rao on 21/04/20.
//

#include <screamy-ball/obstacle.h>

namespace screamy_ball {

Obstacle::Obstacle():
    type_(ObstacleType::kHigh),
    location_({0, 0}),
    length_(3) {}

Obstacle::Obstacle(ObstacleType type, const Location& location):
    type_(type),
    location_(location),
    length_(3) {}

ObstacleType Obstacle::GetObstacleType() {
  return type_;
}

int Obstacle::GetHeight() { return kHeight; }



}