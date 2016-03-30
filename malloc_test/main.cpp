#include <iostream>
#include <map>

using namespace std;


class aa {
    public:
        map<int, int> m_map;

        void f() {
            cout << "A::f()" << endl;
        }

        virtual void vf() {
            cout << "A::vf()" << endl;
        }

};


class b : public aa {

    void f() {
        cout << "B::f()" << endl;
    }

    virtual void vf() {
        cout << "B::vf()" << endl;
    }


};


int malloc_test() {
    int size = 1;
    try {

        aa *p1 = (aa *) malloc(sizeof(aa) * size);
        for (int i = 0; i < 200; i++) {
            p1 = (aa *) realloc(p1, size * 2);
            size *= 2;
            (p1 + size)->m_map.insert(make_pair(1, 3));
            cout<<"insert size "<< size << endl;
        }

    }catch (exception& e){
        cout << "Standard exception: " << e.what() << endl;

    }

    cout << "end" << endl;
    return 1;
}

int main() {

    /*aa *p = new b();
      p->f();
      p->vf();
      cout << "Hello, World!" << endl;*/

    malloc_test();
    return 0;
}

