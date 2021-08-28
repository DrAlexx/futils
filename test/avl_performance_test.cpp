#include <util/profiler.h>
#include <binary_tree/binary_tree.h>

#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

#include <set>
#include <iostream>

profiler::point_set mgr_time;
constexpr int repeat = 100;

binary_tree::tree<int>  avl_tree;
std::set<int>  std_set;

using  ordered_set = __gnu_pbds::tree<
int,
__gnu_pbds::null_type,
std::less<int>,
__gnu_pbds::rb_tree_tag,
__gnu_pbds::tree_order_statistics_node_update>;
ordered_set pbds_set;

void avl_test(int n) {
    static const char point_name[] = "AVL Tree";
    profiler::point<mgr_time, point_name>   test_point;

    for(int i = 0; i < repeat; ++i) {
        for(int v = 0; v < n; ++v) {
            avl_tree.insert(v);
        }
        avl_tree.clear();
    }
}

void std_set_test(int n) {
    static const char point_name[] = "STD SET";
    profiler::point<mgr_time, point_name>   test_point;

    for(int i = 0; i < repeat; ++i) {
        for(int v = 0; v < n; ++v) {
            std_set.insert(v);
        }
        std_set.clear();
    }
}

//void pbds_test(int n) {
//    static const char point_name[] = "PBDS";
//    profiler::point<mgr_time, point_name>   test_point;

//    for(int i = 0; i < repeat; ++i) {
//        for(int v = 0; v < n; ++v) {
//            pbds_set.insert(v);
//        }
//        pbds_set.clear();
//    }
//}

int main(int /*argc*/, char** /*argv*/) {
    for (int i = 100; i < 1000000; i*=10) {
        avl_test(i);
        std_set_test(i);
//        pbds_test(i);

        profiler::point_set::get_manager<mgr_time>().for_each_point([i](const std::string_view name, uint64_t call_count, uint64_t cumulative_time_us){
            std::cout << name << "   Size: " << i << "\tTime: " << cumulative_time_us/call_count << "us" << "\t call count=" << call_count << std::endl;
        });
        profiler::point_set::get_manager<mgr_time>().reset();
        std::cout << std::endl;
    }
    return 0;
}
