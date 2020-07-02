// Copyright
#ifndef SOURCE_SKIP_LIST_SKIP_LIST_HPP_
#define SOURCE_SKIP_LIST_SKIP_LIST_HPP_

#include <functional>
#include <iostream>
#include <random>
#include <vector>

namespace ADE {
namespace Sequential {

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

  bool insert(const data_type& data) {
    std::vector<Node*> predecessor;
    Node* it = find(data, &predecessor);

    if (it != nullptr && it->value_ == data) {
      return false;
    }

    unsigned int level = 1;
    while (get_random_bool() && level < max_level_) {
      ++level;
    }

    if (current_level_ < level) {
      predecessor[current_level_] = &header_;
      level = ++current_level_;
    }

    Node* new_node = new Node(level, data_type(data));
    for (unsigned int i = 0; i < level; ++i) {
      new_node->forward_[i] = predecessor[i]->forward_[i];
      predecessor[i]->forward_[i] = new_node;
    }

    return true;
  }

  unsigned int size() const {
    unsigned int size = 0;
    Node* it = header_.forward_[0];
    while (it != nullptr) {
      ++size;
      it = it->forward_[0];
    }
    return size;
  }

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

  virtual ~SkipList() {
    Node* it = header_.forward_[0];
    while (it) {
      Node* ptr = it->forward_[0];
      delete it;
      it = ptr;
    }
  }

 private:
  Node* find(const data_type& data, std::vector<Node*>* predecessor) {
    predecessor->resize(max_level_, nullptr);
    Node* it = &header_;
    for (int i = current_level_ - 1; 0 <= i; --i) {
      while (it->forward_[i] != nullptr &&
             compare_(it->forward_[i]->value_, data)) {
        it = it->forward_[i];
      }
      (*predecessor)[i] = it;
    }
    it = it->forward_[0];

    return it;
  }

  unsigned int max_level_;
  unsigned int current_level_;
  Node header_;
};
}  // namespace Sequential
}  // namespace ADE

#endif  // SOURCE_SKIP_LIST_SKIP_LIST_HPP_
