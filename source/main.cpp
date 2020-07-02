// Copyright
#include <iostream>
#include <random>

#include "skip_list.hpp"

int get_random() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distribution(1, 100);
  return distribution(gen);
}

int main() {
  ADE::Sequential::SkipList<int> test;

  unsigned int counter = 100;

  for (unsigned int i = 0; i < counter;) {
    if (test.insert(get_random())) {
      ++i;
    }
  }

  test.debug_print();

  return 0;
}
