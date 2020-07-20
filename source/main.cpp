// Copyright
#include <cassert>
#include <iostream>
#include <random>
#include <thread>
#include <vector>
#include <ctime>
#include <sstream>



#include "concurrent_skip_list.hpp"
#include "skip_list.hpp"


using namespace std;

int get_random(int lower, int higher) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(lower, higher);
    return distribution(gen);
}

struct IntWithObjCounter {
    explicit IntWithObjCounter(int n = 0) : num(n) { ++counter; }
    IntWithObjCounter(const IntWithObjCounter& o) : num(o.num) { ++counter; }
    ~IntWithObjCounter() { --counter; }
    int num;
    static unsigned int counter;
};
bool operator<(const IntWithObjCounter& left, const IntWithObjCounter& right) {
    return left.num < right.num;
}
bool operator==(const IntWithObjCounter& left, const IntWithObjCounter& right) {
    return left.num == right.num;
}
bool operator!=(const IntWithObjCounter& left, const IntWithObjCounter& right) {
    return left.num != right.num;
}

unsigned int IntWithObjCounter::counter = 0;
/*
bool operator<(const IntWithObjCounter& left, const IntWithObjCounter& right) {
  return left.num < right.num;
}
bool operator==(const IntWithObjCounter& left, const IntWithObjCounter& right) {
  return left.num == right.num;
}*/

void TestSequential() {
    std::cout << "Basic test on consurrent SkipListSet" << std::endl
        << "====================================" << std::endl;
    {
        // no dangling pointers test
        int num_iter = 1000;
        ADE::Concurrent::ConcurrentSkipList<IntWithObjCounter> test;
        for (int i = 1; i < num_iter; ++i) test.insert(IntWithObjCounter(num_iter));
        IntWithObjCounter::counter;
        for (int i = 1; i < num_iter; ++i) test.remove(IntWithObjCounter(num_iter));

        assert(IntWithObjCounter::counter == 1);  // sentinelas left y right
    }

    {
        // inserción y eliminación aleatoria
        ADE::Concurrent::ConcurrentSkipList<int> test(10);

        int max_value = 1000;
        int num_iter = 1000;

        for (int i = 1; i < num_iter; ++i) test.insert(get_random(0, max_value));
        for (int i = 1; i < num_iter; ++i) test.remove(get_random(0, max_value));

        test.debug_print();
        std::cout << std::endl;
    }

    {
        // validación de la no-doble eliminación
        ADE::Concurrent::ConcurrentSkipList<int> test(10);
        test.insert(1);
        test.insert(2);
        test.insert(3);
        test.remove(3);
        test.remove(2);
        test.remove(1);
        assert(!test.contains(1));
        test.debug_print();
    }

    {
        // validación de la no-doble inserción
        ADE::Concurrent::ConcurrentSkipList<int> test(10);
        test.insert(1);
        test.insert(2);
        test.insert(3);
        test.insert(4);
        test.insert(5);
        test.debug_print();
        assert(!test.insert(3));
        test.debug_print();

        // validación de la inserción y eliminación
        test.remove(3);
        assert(test.insert(3));
        test.remove(3);
        assert(!test.contains(3));
        assert(test.contains(5));
        test.debug_print();
    }
}

class ListTester {
public:
    static const int MAX_NUM_OPERATIONS = 10000;
    static int MIN_VALS;
    static int MAX_VALS;

public:
    ListTester(ADE::Concurrent::ConcurrentSkipList<int>& lista, int modifier)
        : lista_(lista), modifier_(modifier) {}
    void operator()() {
        static const double CONTAINS_WEIGHT = 0.2;
        static const double INSERT_WEIGHT = 0.7;

        for (int i = 0; i < MAX_NUM_OPERATIONS; ++i) {
            std::random_device rd; // random device engine, usually based on /dev/random on UNIX-like systems
            // initialize Mersennes' twister using rd to generate the seed
            std::mt19937 rng(rd());

            if (sample_01_(rng) < CONTAINS_WEIGHT) {
                lista_.contains(random_(rng));
            }
            else {
                if (sample_01_(rng) < INSERT_WEIGHT)
                    lista_.insert(random_(rng));
                else
                    lista_.remove(random_(rng));
            }
        }
    }

private:
    ADE::Concurrent::ConcurrentSkipList<int>& lista_;
    int modifier_;

    static std::uniform_real_distribution <double>
        sample_01_;
    static std::uniform_int_distribution<unsigned int>
        random_;
};

int ListTester::MIN_VALS = 1;
int ListTester::MAX_VALS = 15000;

/*std::uniform_real_distribution<double>
    ListTester::sample_01_(
        static_cast<double>(std::mt19937(static_cast<unsigned long>(std::time(NULL)))),
        static_cast<double> std::uniform_real<double>());

std::uniform_real_distribution<std::mt19937, std::uniform_int<unsigned int> >
    ListTester::random_(
        std::mt19937(static_cast<unsigned long>(std::time(NULL))),
        std::uniform_int<unsigned int>(ListTester::MIN_VALS,
                                       ListTester::MAX_VALS));
*/
std::uniform_real_distribution<double>
ListTester::sample_01_(
    0.0,
    1.0);

std::uniform_int_distribution<unsigned int>
ListTester::random_(
    MIN_VALS,
    MAX_VALS);

void RunTest(bool print) {
    int num_threads = 8;

    ADE::Concurrent::ConcurrentSkipList<int> lista;

    std::vector<std::thread*> threads;

    for (int i = 0; i < num_threads; i++) {
        ListTester tester(lista, i * ListTester::MAX_NUM_OPERATIONS);
        std::thread* process = new std::thread(tester);
        threads.push_back(process);
    }
    for (int i = 0; i < num_threads; ++i) {
        threads[i]->join();
    }

}

int main() {
    //freopen("output.txt", "w", stdout);
    
    TestSequential();
    const int trials = 10;
    for (int i = 0; i < trials; ++i) {
        std::cout << "Running test case " << i << endl;
        RunTest(i == trials - 1);
    }

    return 0;
}
