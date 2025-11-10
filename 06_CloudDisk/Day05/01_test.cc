#include "person.pb.h"

using namespace std;
using namespace test;

int main()
{
    Person p1;
    p1.set_id(100);
    p1.set_name("test");
    p1.set_email("77189904@qq.com");

    string output;
    p1.SerializeToString(&output);

    cout << "size: " << output.size() << endl;
    cout << "vlaue: " << output << endl;

    Person p2;
    cout << "{ id: " << p2.id()
         << ", name: " << p2.name()
         << ", email: " << p2.email() << " }" << endl;
    
    cout << "------------" << endl;

    // 反序列化
    p2.ParseFromString(output);
    cout << "{ id: " << p2.id()
         << ", name: " << p2.name()
         << ", email: " << p2.email() << " }" << endl;

    return 0;
}