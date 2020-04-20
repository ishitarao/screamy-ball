// Copyright (c) 2020 CS126SP20. All rights reserved.

#ifndef SNAKE_LOCATION_H_
#define SNAKE_LOCATION_H_

#include <cstddef>
#include <iostream>

namespace screamy_ball {

// Represents a location on the board.
class Location {
 public:
  Location(int row, int col);

  // Comparison operators.
  bool operator()(const Location& lhs, const Location& rhs) const;
  bool operator==(const Location& rhs) const;
  bool operator!=(const Location& rhs) const;
  bool operator<(const Location& rhs) const;
  bool operator<=(const Location& rhs) const;
  bool operator>(const Location& rhs) const;
  bool operator>=(const Location& rhs) const;

  // Vector operators.
  Location operator+(const Location& rhs) const;
  // Note: Always returns positive coordinates.
  Location operator%(const Location& rhs) const;
  Location operator-(const Location& rhs) const;
  Location& operator+=(const Location& rhs);
  Location operator-() const;

  // Accessors.
  int Row() const;
  int Col() const;

 private:
  int row_;
  int col_;
};

std::ostream& operator<<(std::ostream& os, const Location& location);

}  // namespace snake


#endif  // SNAKE_LOCATION_H_
