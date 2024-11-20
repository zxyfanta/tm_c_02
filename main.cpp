#include <iostream>
#include "Parameter/Parameter.hpp"
#include "Utils/DataManagerUtil.hpp"
#include "Processor/Processor.hpp"
#include "chrono"


using namespace std;
int main(int argc, char const *argv[])
{
    /* code */
    auto start =chrono::high_resolution_clock::now();
    Processor processor;
    processor.run();
    auto end=chrono::high_resolution_clock::now();
    chrono::duration<double> diff=end-start;
    cout <<"Time taken:"<<diff.count()<<"s"<<endl;
    return 0;
}
