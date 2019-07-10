#include <iostream>

using namespace std;

class Base {
public:
  virtual Base &operator=(const Base &rhs) {
    cout<<"Base"<<endl;
  }
};

class Derived :
  public Base {
public:
  virtual Derived &operator=(const Base &rhs) {
    cout<<"Derived"<<endl;
    Base::operator=(rhs);
  }
};

int main()
{
  Base *base1 = new Derived();
  Base *base2 = new Derived();

  *base2 = *base1;

  delete base1;
  delete base2;
  return 0;
}
