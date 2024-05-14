#include <functional>
#include <vector>
#include <thread>

struct Foo {
    Foo() = default;
    std::vector<std::thread> a;

    ~Foo(){
        for(auto& f : a){
            f.join();
        }
    }
};



int main () {
    Foo bar;

}