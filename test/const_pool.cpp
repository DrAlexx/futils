#include <util/const_pool.h>

#define BOOST_TEST_MODULE Const_pool
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_SUITE(Const_pool)


BOOST_AUTO_TEST_CASE(Const_pool)
{
    constexpr std::size_t N = 100;
    ConstPool<int, N> pool;

    BOOST_REQUIRE_EQUAL(pool.GetSize(), N);
    BOOST_REQUIRE_EQUAL(pool.GetElementSize(), sizeof(int));

    auto a = pool.Alloc();
    BOOST_REQUIRE_EQUAL(a.first, 0);
    pool.Free(a.first);
    a = pool.Alloc();
    BOOST_REQUIRE_EQUAL(a.first, 0);

    pool.Reset();
    for(auto i = 0u; i < N; ++i) {
        BOOST_REQUIRE_EQUAL(pool.Alloc().first, i);
    }
    BOOST_CHECK_THROW(pool.Alloc(), std::bad_alloc);
}

BOOST_AUTO_TEST_SUITE_END()
