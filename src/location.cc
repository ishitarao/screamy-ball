// Copyright (c) 2020 CS126SP20. All rights reserved.

#include <screamy-ball/location.h>


namespace screamy_ball {

Location::Location() : row_(0), col_(0) {}

Location::Location(int row, int col) : row_(row), col_(col) {}

bool Location::operator==(const Location& rhs) const {
  return row_ == rhs.row_ && col_ == rhs.col_;
}

bool Location::operator!=(const Location& rhs) const {
  return !(*this == rhs);
}

int Location::Row() const { return row_; }

int Location::Col() const { return col_; }

std::ostream& operator<<(std::ostream& os, const Location& location) {
  os << "{row=" << location.Row() << ", col=" << location.Col() << "}";
  return os;
}

}  // namespace screamy_ball
