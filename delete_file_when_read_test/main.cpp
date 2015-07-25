#include <iostream>
#include <fstream>
#include <string>
using namespace std;

int main()
{

    fstream  f("data.txt");

    string s;
    while(f >> s){
        cout << s << endl;
        sleep(1);
    }
    return 0;
}
