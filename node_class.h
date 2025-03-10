#ifndef NODE_CLASS_H
#define NODE_CLASS_H
#include<cstdint>
#include<iostream>
#include<vector>
#include<map>
using namespace std;
#define BITLENGTH 8

class Node;

class FingerTable {
public:
    FingerTable () {}
    FingerTable(uint8_t nodeId);

    void set(size_t index, Node* successor, uint8_t succ_id) {
        fingerTable_[index] = successor;
        successor_ids[index] = succ_id;
    }

    Node* get(size_t index) {
        return fingerTable_[index];
    }

    void prettyPrint();  // TODO: Implement this function

private:
    uint8_t nodeId_;
    std::vector<Node*> fingerTable_;
    std::vector<uint8_t> successor_ids;
};

class Node {
public:    
    Node(uint8_t id) : id_(id){
        fingerTable_ = FingerTable(id);
        predecessor_id = id;
        predecessor_ptr = this;        
    }

    bool in_range(int left, int key, int right){
        if(left > right)
            return key <= right && key >= left;
        else
            return (key >= left) || (key <= right);
    }

    FingerTable get_finger_table(){
        return this->fingerTable_;
    }

    void join(Node* node);  // TODO: Implement node join function
    bool find(uint8_t key);  // TODO: Implement DHT lookup
    void insert(uint8_t key);  // TODO: Implement DHT key insertion
    void remove(uint8_t key);  // TODO: Implement DHT key deletion
    Node* find_predecessor(uint8_t key);
    void initialize_finger_table(Node* successor);
    void update_finger_table();
    void move_keys(Node *src, Node *dest);
    void update_current_nodes_ft(Node *update, uint8_t updateid, uint8_t index);
    void printKeys(Node *node);
    void insert(uint8_t key, uint8_t value);
    

private:
    uint8_t id_;
    FingerTable fingerTable_;
    Node* predecessor_ptr;
    uint8_t predecessor_id;
    std::map<uint8_t, uint8_t> localKeys_;
    
};

FingerTable::FingerTable(uint8_t nodeId)
{
    nodeId_ = nodeId;
    Node *dummy = NULL;    
    for(int i=0; i<BITLENGTH ; i++) {
        fingerTable_.push_back(dummy);
        successor_ids.push_back(0);        
    }
}

void FingerTable::prettyPrint(){
    cout<<"Finger table for "<<static_cast<int>(nodeId_)<<endl;
    uint8_t startt = nodeId_;
    for(int i = 0; i < BITLENGTH; i++){
        cout<<"[ "<<static_cast<int>(startt)<<" , "<<static_cast<int>(uint8_t(startt + (1 << (i))))<<")";
        cout<<" Successor value:"<<static_cast<int>(successor_ids[i])<<endl;
        startt += (1 << (i));
    }
}
#endif