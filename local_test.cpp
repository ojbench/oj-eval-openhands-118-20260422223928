#include "src.hpp"
#include <iostream>
int main(){
    fraction<int> f; std::cout<<f<<"\n"; // 0/1
    f = fraction<int>(2,-6);
    std::cout<<f<<"\n"; // -1/3
    auto g = f ^ -1ll; // reciprocal -> -3/1
    std::cout<<g<<"\n";
    fraction<int> a=1,b=2; std::cout<<(a<b)<<" "<<(a==b)<<"\n";
    std::cout<<static_cast<double>(fraction<int>(2,6))<<"\n"; // 0.333333...
    try{
        auto z = fraction<int>() ^ -2; (void)z;
    }catch(const std::exception &e){ std::cout<<e.what()<<"\n"; }
    return 0;
}
