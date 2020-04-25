//
// Created by Ishita Rao on 21/04/20.
//

#include <screamy-ball/obstacle.h>

namespace screamy_ball {

Obstacle::Obstacle():
    obstacle_type_(ObstacleType::kHigh),
    location_({0, 0}) {}

Obstacle::Obstacle(ObstacleType type, const Location& location):
    obstacle_type_(type),
    location_(location) {}

ObstacleType Obstacle::GetObstacleType() {
  return obstacle_type_;
}

int Obstacle::GetHeight() { return kHeight; }



}