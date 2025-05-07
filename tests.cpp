#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "timedSharedPtr.h"
#include "include/doctest.h"
#include <thread>

TEST_CASE("use_count works correctly") {
    TimedSharedPtr<int> p(new int(42));
    CHECK(p.use_count() == 1);

    {
        TimedSharedPtr<int> q = p;
        CHECK(p.use_count() == 2);
        CHECK(q.use_count() == 2);
    }

    // q out of scope, use count should decrement
    CHECK(p.use_count() == 1);
}

TEST_CASE("copy assignment works correctly") {
    TimedSharedPtr<int> a(new int(57));
    TimedSharedPtr<int> b;
    b = a;  // copy‐assignment
    CHECK(a.use_count() == 2);
    CHECK(b.use_count() == 2);
}

TEST_CASE("default expiration doesn't expire immediately") {
    TimedSharedPtr<int> p(new int(7));
    REQUIRE(p.get() != nullptr);
}

TEST_CASE("custom expiration works") {
    TimedSharedPtr<int> p(new int(7), 50);  // expires after 50 ms
    CHECK(p.get() != nullptr);

    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    CHECK(p.get() == nullptr);
}

TEST_CASE("get() returns nullptr when called on empty pointer") {
    TimedSharedPtr<int> empty;
    CHECK(empty.get() == nullptr);
    CHECK(empty.use_count() == 0);
}
