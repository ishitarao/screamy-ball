//
// Created by Ishita Rao on 21/04/20.
//

#ifndef FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
#define FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_

namespace screamy_ball {

enum class ObstacleType { kHigh, kLow };

class Obstacle {
  const int kHeight;
  ObstacleType obstacle_type_;

 public:
  Obstacle();

};

}

#endif  // FINALPROJECT_INCLUDE_SCREAMY_BALL_OBSTACLE_H_
