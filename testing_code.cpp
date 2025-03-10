#include "shiva_chord.h"
#include <iostream>
using namespace std;

int main() {    
    Node *n0 = new Node(0);
    Node *n1 = new Node(30);
    Node *n2 = new Node(65);
    Node *n3 = new Node(110);
    Node *n4 = new Node(160);
    Node *n5 = new Node(230);    
    
    n0->join(NULL);
    n1->join(n0);
    n2->join(n1);

    n0->insert(3, 3);
    n1->insert(200);
    n2->insert(123);
    
    n3->join(n2);
    n4->join(n3);
    n5->join(n4);

    n0->get_finger_table().prettyPrint();
    n1->get_finger_table().prettyPrint();
    n2->get_finger_table().prettyPrint();
    n3->get_finger_table().prettyPrint();
    n4->get_finger_table().prettyPrint();
    n5->get_finger_table().prettyPrint();
    for(int i = 0; i < 10; i++)
        cout<<"-----"<<endl;

    n3->insert(45,3);
    n4->insert(99);
    n2->insert(60,10);
    n0->insert(50,8);
    n3->insert(100,5);
    n3->insert(101,4);
    n3->insert(102,6);
    n5->insert(240,8);
    n5->insert(250,10);
    return 0;
}
