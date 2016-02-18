

 #include <chrono>

using namespace std;
using namespace std::chrono;

auto t1 = high_resolution_clock::now();

auto t2 = high_resolution_clock::now();

auto duration = duration_cast<microseconds>( t2 - t1 ).count();

cout << duration;
