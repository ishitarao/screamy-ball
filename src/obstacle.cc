//
// Created by Ishita Rao on 21/04/20.
//

#include <screamy-ball/obstacle.h>

namespace screamy_ball {

Obstacle::Obstacle():
    kMinLength(2),
    kMaxLength(4),
    type_(ObstacleType::kHigh),
    location_({0, 0}),
    length_(1) {}

Obstacle::Obstacle(ObstacleType type, const Location& location):
    kMinLength(2),
    kMaxLength(4),
    type_(type),
    location_(location),
    length_(1) {}

ObstacleType Obstacle::GetObstacleType() {
  return type_;
}

int Obstacle::GetHeight() { return kHeight; }



}