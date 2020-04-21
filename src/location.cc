// Copyright (c) 2020 CS126SP20. All rights reserved.

#include <screamy-ball/location.h>


namespace screamy_ball {

Location::Location() : row_(0), col_(0) {}

Location::Location(int row, int col) : row_(row), col_(col) {}

bool Location::operator==(const Location& rhs) const {
  return row_ == rhs.row_ && col_ == rhs.col_;
}

bool Location::operator()(const Location& lhs, const Location& rhs) const {
  return lhs <= rhs;
}

bool Location::operator!=(const Location& rhs) const {
  return !(*this == rhs);
}

bool Location::operator<(const Location& rhs) const {
  return row_ < rhs.row_ || (rhs.row_ >= row_ && col_ < rhs.col_);
}

bool Location::operator<=(const Location& rhs) const {
  return !(rhs < *this);
}

bool Location::operator>(const Location& rhs) const {
  return rhs < *this;
}

bool Location::operator>=(const Location& rhs) const {
  return !(*this < rhs);
}

Location Location::operator+(const Location& rhs) const {
  return { row_ + rhs.row_, col_ + rhs.col_ };
}

Location Location::operator-(const Location& rhs) const {
  return *this + (-rhs);
}

Location Location::operator-() const { return {-row_, -col_}; }

Location& Location::operator+=(const Location& rhs) {
  *this = *this + rhs;
  return *this;
}

int mod(int a, int b) {
  int c = a % b;
  return c + (c < 0 ? b : 0);
}

Location Location::operator%(const Location& rhs) const {
  return {mod(row_, rhs.row_), mod(col_, rhs.col_)};
}

int Location::Row() const { return row_; }

int Location::Col() const { return col_; }

std::ostream& operator<<(std::ostream& os, const Location& location) {
  os << "{row=" << location.Row() << ", col=" << location.Col() << "}";
  return os;
}

}  // namespace snake
