// Copyright
#ifndef SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_
#define SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_

#include <functional>
#include <iostream>
#include <random>
#include <vector>
#include <mutex>


namespace ADE {
    namespace Concurrent {



        template <typename Type, class Comparator = std::less<Type>>
        class ConcurrentSkipList {
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
                std::mutex source_;
            };

            explicit ConcurrentSkipList(unsigned int max_level = 16)
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
            unsigned int size() const {
                unsigned int size = 0;
                Node* it = header_.forward_[0];
                while (it != nullptr) {
                    ++size;
                    it = it->forward_[0];
                }
                return size;
            }

            void unlocking_nodes(std::vector<Node*>& predecessor) {
                Node* back = nullptr;
                for (unsigned int i = 0; i < predecessor.size(); i++) {
                    if (predecessor[i]) {
                        if (predecessor[i] != back) {
                            predecessor[i]->source_.unlock();
                        }

                        back = predecessor[i];
                    }
                    else {
                        break;
                    }
                }
            }

            bool insert(const data_type& data) {
                //source.lock();
                std::vector<Node*> predecessor;
                bool node_locked;
                Node* it = find(data, &predecessor, node_locked);

                if (it != nullptr && it->value_ == data) {
                    //source.unlock();

                    unlocking_nodes(predecessor);
                    it->source_.unlock();

                    return false;
                }

                unsigned int level = 1;
                while (get_random_bool() && level < max_level_) {
                    ++level;
                }

                if (current_level_ < level) {
                    bool should_lock = true;
                    for (unsigned int i = 0; i < predecessor.size(); i++) {
                        if (predecessor[i] == &header_) {
                            should_lock = false;
                            break;
                        }
                    }

                    predecessor[current_level_] = &header_;

                    level = ++current_level_;

                    if (should_lock) {
                        header_.source_.lock();
                    }
                }

                Node* new_node = new Node(level, data_type(data));
                for (unsigned int i = 0; i < level; ++i) {
                    new_node->forward_[i] = predecessor[i]->forward_[i];
                    predecessor[i]->forward_[i] = new_node;
                }




                unlocking_nodes(predecessor);


                //source.unlock();
                return true;
            }
            bool remove(const data_type& data) {
                std::vector<Node*> predecessor;
                bool node_locked;
                Node* it = find(data, &predecessor, node_locked);

                if (it == nullptr || it->value_ != data) {
                    unlocking_nodes(predecessor);
                    return false;
                }

                for (unsigned int i = 0; i < current_level_; ++i) {
                    if (predecessor[i]->forward_[i] != it) {
                        break;
                    }
                    predecessor[i]->forward_[i] = it->forward_[i];
                }
                unlocking_nodes(predecessor);

                if (node_locked) {
                    it->source_.unlock();
                }

                delete it;
                while (1 < current_level_ && header_.forward_[current_level_ - 1] == 0) {
                    --current_level_;
                }
                return true;
            }
            bool contains(const data_type& data) {
                std::vector<Node*> predecessor;
                bool x;
                Node* it = find(data, &predecessor, x);

                return (it != nullptr && it->value_ == data);
            }
            void clear() {
                Node* it = header_.forward_[0];
                while (it != nullptr) {
                    Node* ptr = it->forward_[0];
                    delete it;
                    it = ptr;
                }
                for (unsigned int i = 0; i < current_level_; ++i) {
                    header_.forward_[i] = nullptr;
                }
                current_level_ = 1;
            }
            bool empty() const { return header_.forward_[0] == nullptr; }


            unsigned int max_level_;
            unsigned int current_level_;
            Node header_;

        private:
            Node* find(const data_type& data, std::vector<Node*>* predecessor, bool& found_first) {
                predecessor->resize(max_level_, nullptr);

                Node* it = &header_, * back = nullptr;
                found_first = false;
                for (int i = current_level_ - 1; 0 <= i; --i) {
                    //This node will be locking while traveling
                    // Node* looking_for = nullptr;
                    while (it->forward_[i] != nullptr &&
                        compare_(it->forward_[i]->value_, data)) {

                        it = it->forward_[i];
                    }

                    if (it->forward_[i] && it->forward_[i]->value_ == data && !found_first) {
                        found_first = true;

                        if (!(it->forward_[i]->source_.try_lock())) {
                            it = back;
                            i++;
                            found_first = false;
                            continue;
                        }
                    }
                    if (back != it && it) {

                        it->source_.lock();


                    }

                    (*predecessor)[i] = it;
                    back = it;
                }
                it = it->forward_[0];

                return it;
            }
        };
    }  // namespace Concurrent
}  // namespace ADE

#endif  // SOURCE_SKIP_LIST_CONCURRENT_SKIP_LIST_HPP_
