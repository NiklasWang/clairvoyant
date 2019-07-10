#include <iostream>

using namespace std;

template <typename T>
struct Type2ID;
template <int id>
struct ID2Type;

#define REGISTER_TYPE(type, n) \
    template <> \
    struct Type2ID<type> { char id[n]; }; \
    template <> \
    struct ID2Type<n>    { typedef type type_t; };

template <typename T>
Type2ID<T> encode(const T&);
template <typename T>
Type2ID<T> encode(T&);

#define type_of(...) \
    ID2Type<sizeof(encode((__VA_ARGS__)))>::type_t

class A {
public:
  virtual void exe() {
    cout<<"I'm A"<<endl;
  }
};

REGISTER_TYPE(A, 1);

class B : public A {
public:
  virtual void exe() {
    cout<<"I'm B"<<endl;
  }
};

REGISTER_TYPE(B, 2);

int main()
{
  A a;
  B b;
  B &b1 = b;
  B *b2 = &b;
  A &b3 = b;
  A *b4 = &b;

  type_of(a)   c;   //I'm A
  type_of(b)   d;   //I'm B
  type_of(b1)  d1;  //I'm B
  type_of(*b2) d2;  //I'm B
  type_of(b3)  d3;  //I'm A
  type_of(*b4) d4;  //I'm A

  c.exe();
  d.exe();
  d1.exe();
  d2.exe();
  d3.exe();
  d4.exe();

  return 0;
}
