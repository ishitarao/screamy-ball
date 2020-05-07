// Copyright (c) 2020 Ishita Rao. All rights reserved.

#include <screamy-ball/location.h>


namespace screamy_ball {

Location::Location() : row_(0), col_(0) {}

Location::Location(int row, int col) : row_(row), col_(col) {}

/**
 * Equality operator for Location. Both the row and column must be equal.
 * @param rhs the location to compare this location to.
 * @return true if the two Locations are equal, false otherwise.
 */
bool Location::operator==(const Location& rhs) const {
  return row_ == rhs.row_ && col_ == rhs.col_;
}

/**
 * Inequality operator for Location.
 * @param rhs the Location to compare this Location to.
 * @return true if the two Locations are unequal, false otherwise.
 */
bool Location::operator!=(const Location& rhs) const {
  return !(*this == rhs);
}

/**
 * Getter for the Location's row.
 * @return the row.
 */
int Location::Row() const { return row_; }

/**
 * Getter for the Location's column.
 * @return the column.
 */
int Location::Col() const { return col_; }

/**
 * Output operator for Location.
 * @param os the output operator to overload.
 * @param location the Location to output.
 * @return the overloaded output operator containing the printed Location.
 */
std::ostream& operator<<(std::ostream& os, const Location& location) {
  os << "{ row = " << location.Row() << ", col = " << location.Col() << " }";
  return os;
}

}  // namespace screamy_ball
