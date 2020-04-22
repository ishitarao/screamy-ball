//
// Created by Ishita Rao on 21/04/20.
//

#include <screamy-ball/obstacle.h>

namespace screamy_ball {

Obstacle::Obstacle() {}

Obstacle::Obstacle(ObstacleType type, Location location):
    kHeight(2),
    obstacle_type_(type),
    location_(location) {}

ObstacleType Obstacle::GetObstacleType() {
  return obstacle_type_;
}

}