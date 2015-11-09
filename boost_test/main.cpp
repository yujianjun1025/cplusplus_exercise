#include <boost/lexical_cast.hpp>
#include <iostream>
int main()
{
        using boost::lexical_cast;
        int a = lexical_cast<int> ("123");
        std::cout << a << std::endl;
        return 0;
}
