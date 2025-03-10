#ifndef NODE_H
#define NODE_H
#include "node_class.h"
#include<cstdint>
#include<vector>
#include<map>
#include<iostream>
#define BITLENGTH 8
using namespace std;

void Node::join(Node* node){    
    Node *successor = NULL;

    if(node == NULL){
        std::cout<<"First node in network"<<std::endl;
        for(int i = 0; i < BITLENGTH; i++){
            fingerTable_.set(i, this, id_);
        }
    }

    else{
        successor = node->find_predecessor(id_);
        initialize_finger_table(successor);
        update_finger_table();
        move_keys(successor, this);                
        
    }

    cout<<"Join completed"<<endl<<endl<<endl;
    return;
}

Node* Node::find_predecessor(uint8_t key){

    //Only one node present in the table
    if(predecessor_id == id_)
        return this;
    
    //Normal range
    if((key > predecessor_id) && (key <= id_)){        
        return this;
    }

    //Circular range eg: 255, 2, 7
    if(predecessor_id > id_){
        if((key > predecessor_id) || (key <= id_)){
            return this;            
        }
    }

    for(int i = 0; i < BITLENGTH - 1; i++)
    {
        int succ0 = (id_ + (1 << i)) % 256;
        int succ1 = (id_ + (1 << (i + 1))) % 256;
        if(succ1 == 0)
            succ1 = 255;
        else
            succ1 -= 1;
        if(in_range(succ0, int(key), succ1)){
            return fingerTable_.get(i)->find_predecessor(key);            
        }
    }
    cout<<"LAST INDEX CHECK"<<endl;
    return fingerTable_.get(7)->find_predecessor(key);
}

void Node::initialize_finger_table(Node *node){    
    Node *tmp = node->find_predecessor(id_);
    
    cout<<static_cast<int>(id_)<<" returned successor "<<static_cast<int>(tmp->id_)<<endl;
    fingerTable_.set(0, tmp, tmp->id_);

    int normtempid = tmp->id_;
    if (tmp->id_ <= id_){
        normtempid += 255;
        cout<<" normalized succid "<<normtempid<<endl;        
    }

    for(int i = 1; i < 8; i++){
        Node *tmpnode1 = NULL;
        int normchordjump = id_ + (1 << i);
        if(normchordjump <= id_){
            normchordjump += 255;
            cout<<" normalized normchordjump "<<normchordjump<<endl;
        }

        if(normtempid >= normchordjump){            
            tmpnode1 = fingerTable_.get(i-1);
        }
        else{            
            tmpnode1 = node->find_predecessor(id_ + (1<<i));
        }
        fingerTable_.set(i, tmpnode1, tmpnode1->id_);
    }

    predecessor_id = tmp->predecessor_id;
    cout<<"Updated pred_id to "<<static_cast<int>(predecessor_id)<<endl;
    predecessor_ptr = tmp->predecessor_ptr;
    tmp->predecessor_id = id_;
    cout<<"Updated "<<unsigned(tmp->id_)<<" predid to "
        <<unsigned(id_)<<endl;
    tmp->predecessor_ptr = this;        
}

void Node::update_finger_table(void){
    cout<<"Updating finger tables"<<endl;
    for(int i = 0; i < BITLENGTH; i++)
    {
        Node* successor = find_predecessor(id_ - (1 << i));
        uint8_t tmp = id_ - (1 << i);
        cout<<"update_finger_table for "<<i<<" succ found is "<<static_cast<int>(successor->id_)<<endl;
        successor->predecessor_ptr->update_current_nodes_ft(this, id_, i);
    }
}

void Node::update_current_nodes_ft(Node *update, uint8_t updateid, uint8_t index){

    int normftindex = fingerTable_.get(index)->id_;
    Node *ftnode = predecessor_ptr;
    int normupdate = updateid;

    if(updateid <= id_){
        cout<<"using MAXINDEX boundary logic";
        normupdate += (255 - id_);
        cout<<" new normupdate "<<normupdate<<endl;
    }

    if(id_ >= normftindex){
        

        if(updateid <= id_)
            normftindex += 255 - id_;
        else
            normftindex += 255;
        
    }

    if((normupdate >= id_) && (normupdate < normftindex)) {        
        fingerTable_.set(index, update, updateid);        
        ftnode->update_current_nodes_ft(update, updateid, index);
    }
}

void Node::move_keys(Node *src, Node *dest){
    Node* pred = 0;
    map<uint8_t, uint8_t>::iterator it = src->localKeys_.begin();    
    cout<<"Keys at old node "<<static_cast<int>(src->id_)<<" before movement"<<endl;
    printKeys(src);
    cout<<"Keys at new node "<<static_cast<int>(dest->id_)<<" before movement"<<endl;
    printKeys(dest);        
    if(dest->predecessor_id >= src->id_){
        for(it = src->localKeys_.begin(); it != src->localKeys_.end() ; ++it){
            if(((it->first > dest->predecessor_id) && (it->first <= dest->id_)) || ((it->first <= dest->id_) && (it->first  > src->id_))){
                dest->localKeys_.insert(std::make_pair(it->first, it->second));
            }
        }
    }
    else{
        for(it = src->localKeys_.begin(); it != src->localKeys_.end(); ++it){            
            if((it->first <= dest->id_) && (it->first > dest->predecessor_id)){
                dest->localKeys_.insert(std::make_pair(it->first, it->second));
                it->second += 1;
            }
        }
    }

    for (it = src->localKeys_.begin(); it != src->localKeys_.end() ; ) {
        if (it->first != it->second)
            src->localKeys_.erase(it++);   
        else
            ++it;
    }

    cout<<"Keys at old node "<<static_cast<int>(src->id_)<<" after movement"<<endl;
    printKeys(src);
    cout<<"Keys at new node "<<static_cast<int>(dest->id_)<<" after movement"<<endl;
    printKeys(dest);    

}

void Node::printKeys(Node *node)
{
    map<uint8_t, uint8_t>::iterator it;
    cout<<"Keys at "<<static_cast<int>(node->id_)<<endl;
    cout<<"-------------------"<<endl;
    for(it = node->localKeys_.begin();
        it != node->localKeys_.end(); ++it) {
        std::cout<<"| (Key, value)="<<static_cast<int>(it->first)
                 << "," <<static_cast<int>(it->second)<< "| \n";
    }
    cout<<"-------------------"<<endl;
}

bool Node::find(uint8_t key){
    cout<<"Search request for key="<<static_cast<int>(key)<<endl;
    Node* found = find_predecessor(key);
    map<uint8_t, uint8_t>::iterator it = found->localKeys_.find(key);
    if(it != found->localKeys_.end()){
        cout<<"Key ("<<unsigned(found->localKeys_[key])<<") found at "<<static_cast<int>(found->id_)<<endl;
        return 1;
    }
    return 0;
}


void Node::insert(uint8_t key, uint8_t value){
    if(find(key)){
        cout<<"Key already exists and update is not supported";
        return;
    }

    Node *found = find_predecessor(key);
    found->localKeys_.insert(make_pair(key, value));
    cout<<"("<<static_cast<int>(key)<<", "<<static_cast<int>(value)<<" ) inserted at Node id "<<static_cast<int>(found->id_)<<endl<<endl<<endl<<endl;
    return;

}

void Node::insert(uint8_t key){
    if(find(key)){
        cout<<"Key already exists and update is not supported";
        return;
    }

    Node *found = find_predecessor(key);
    found->localKeys_.insert(make_pair(key, NULL));
    cout<<"("<<static_cast<int>(key)<<", NULL ) inserted at Node id "<<static_cast<int>(found->id_)<<endl<<endl<<endl<<endl;
    return;
}

void Node::remove(uint8_t key){
    if(!find(key)){
        cout<<"Key not found"<<endl;
        return;
    }    
    Node *found = find_predecessor(key);
    map<uint8_t, uint8_t>::iterator it = found->localKeys_.find(key);
    if(it != found->localKeys_.end()){
        cout<<"Element found "<<unsigned(it->first)<<", "<<unsigned(it->second)<<" at NodeID= "<<found->id_<<endl;
        found->localKeys_.erase(it);
        cout<<"Delete done"<<endl<<endl<<endl<<endl;
        return;
    }            
}
#endif