// Copyright
#include <iostream>
#include <random>

#include "skip_list.hpp"

unsigned int get_random() {
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

  std::cout << std::endl;

  while (!test.empty()) {
    if (test.remove(get_random())) {
      --counter;
    }
  }

  std::cout << "Counter: " << counter << std::endl;

  test.insert(1);
  test.insert(2);
  test.insert(3);
  std::cout << test.size() << std::endl;

  test.clear();
  test.insert(1);
  test.insert(2);
  test.insert(3);
  test.insert(4);
  test.insert(5);
  std::cout << std::endl << std::endl;

  test.debug_print();
  test.remove(3);
  if (test.contains(3)) {
    std::cout << "El elemento 2 no debería estar" << std::endl;
  }
  std::cout << std::endl;
  test.debug_print();

  return 0;
}

