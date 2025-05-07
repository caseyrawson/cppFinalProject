// main.cpp
#include "timedSharedPtr.h"
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

struct Node {
    Node(int v) : val{v} {}
    int val;
    TimedSharedPtr<Node> next;
};

int main() {
    using Clock = chrono::high_resolution_clock;
    using milliSeconds = chrono::milliseconds;

    // create a Node that expires in 100 ms
    TimedSharedPtr<Node> myNode(new Node(7), 100);
    TimedSharedPtr<Node> myOtherNode = myNode;

    // after 50 ms
    this_thread::sleep_until(Clock::now() + milliSeconds(50));
    cout << "myNode.get() address: <" << myNode.get() << ">" << endl;

    cout << "myNode.use_count(): " << myNode.use_count() << endl;
    cout << "myOtherNode.use_count(): " << myOtherNode.use_count() << endl;

    // after 25 ms more (total 75 ms)
    this_thread::sleep_until(Clock::now() + milliSeconds(25));
    cout << "myNode.get() address: <" << myNode.get() << ">" << endl;

    // after 75 ms more (total 150 ms)
    this_thread::sleep_until(Clock::now() + milliSeconds(75));
    cout << "The ptr should have expired: " << endl;
    cout << "myNode.get() address: <" << myNode.get() << ">" << endl;

    cout << "-----------" << endl;

    // now test with an int and default timeout (1000 ms)
    TimedSharedPtr<int> p(new int(42));
    cout << p.get() << endl;
    cout << "p.use_count(): " << p.use_count() << endl;

    TimedSharedPtr<int> q = p;
    cout << "p.use_count(): " << p.use_count() << endl;
    cout << "q.use_count(): " << q.use_count() << endl;

    return 0;
}
