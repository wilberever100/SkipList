// Copyright
#ifndef SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_
#define SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_

#include <functional>
#include <iostream>
#include <random>
#include <vector>

namespace ADE {
namespace Concurrent {

template <typename Type, class Comparator = std::less<Type>>
class SkipList {
 public:
  typedef Type data_type;
  Comparator compare_;

  bool get_random_bool() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, 1);
    return distribution(gen);
  }

  struct Node {
    Node(unsigned int level, const data_type& data)
        : forward_(level, nullptr), value_(data) {}
    std::vector<Node*> forward_;
    data_type value_;
  };

  explicit SkipList(unsigned int max_level = 16)
      : max_level_(max_level),
        current_level_(1),
        header_(max_level_, data_type()) {}

  void debug_print() {
    Node* it = &header_;
    while (it->forward_[0] != nullptr) {
      for (unsigned int i = 0; i < it->forward_.size(); ++i) {
        std::cout << it->forward_[0]->value_ << " ";
      }
      std::cout << std::endl;
      it = it->forward_[0];
    }
  }

  unsigned int max_level_;
  unsigned int current_level_;
  Node header_;
};
}  // namespace Concurrent
}  // namespace ADE

#endif  // SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_
