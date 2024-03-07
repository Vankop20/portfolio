#pragma once

int* LowerBound(int* first, int* last, int value) {
  int* start = first;
  int* end = last;
  int* middle = start + (end - start) / 2;
  while (end - start > 0) {
    if (*middle == value) {
      return middle;
    }
    if (*middle > value) {
      end = middle;
    } else {
      start = middle + 1;
    }
    middle = start + (end - start) / 2;
  }
  return end;
}
