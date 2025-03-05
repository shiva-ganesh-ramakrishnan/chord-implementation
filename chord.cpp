#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include <map>
#include <vector>
#include <iostream>

#define BITLENGTH 8

class Node;

class FingerTable {
public:
    FingerTable(uint8_t nodeId);
    void set(size_t index, Node* successor);
    Node* get(size_t index);
    void prettyPrint();
private:
    uint8_t nodeId_;
    std::vector<Node*> fingerTable_;
};

class Node {
public:
    Node(uint8_t id);
    void join(Node* node);
    uint8_t find(uint8_t key);
    void insert(uint8_t key);
    void remove(uint8_t key);
    Node* get_finger_table_entry(uint8_t index);
    uint8_t get_id();
private:
    uint64_t id_;
    FingerTable fingerTable_;
    std::map<uint8_t, uint8_t> localKeys_;
    Node* predecessor_;

    Node* find_predecessor(uint8_t id);
    Node* find_successor(uint8_t id);
    bool in_range(uint8_t start, uint8_t end, uint8_t key);
    Node* find_closest_preceding_finger(uint8_t id);
};


FingerTable::FingerTable(uint8_t nodeId) : nodeId_(nodeId) {
    fingerTable_.resize(BITLENGTH + 1);
}

void FingerTable::set(size_t index, Node* successor) {
    fingerTable_[index] = successor;
}

Node* FingerTable::get(size_t index) {
    return fingerTable_[index];
}

void FingerTable::prettyPrint() {
    std::cout << "FingerTable for Node " << unsigned(nodeId_) << ":\n";
    for (size_t i = 1; i < fingerTable_.size(); i++) {
        if (fingerTable_[i]) {
            std::cout << "Index " << i << " -> Node " << unsigned(fingerTable_[i]->get_id()) << std::endl;
        } else {
            std::cout << "Index " << i << " -> NULL\n";
        }
    }
}


Node::Node(uint8_t id) : id_(id), fingerTable_(FingerTable(id)) {
    predecessor_ = nullptr;
}

uint8_t Node::get_id() {
    return id_;
}

Node* Node::get_finger_table_entry(uint8_t index) {
    return fingerTable_.get(index);
}

bool Node::in_range(uint8_t start, uint8_t end, uint8_t key) {
    if (start > end) {
        return (key >= start) || (key <= end);
    }
    return (key >= start) && (key <= end);
}

Node* Node::find_closest_preceding_finger(uint8_t id) {
    for (int i = BITLENGTH - 1; i >= 0; i--) {
        Node* fnode = fingerTable_.get(i + 1);
        if (fnode && in_range(id_ + 1, id - 1, fnode->get_id())) {
            return fnode;
        }
    }
    return this;
}

Node* Node::find_predecessor(uint8_t id) {
    Node* n = this;
    while (true) {
        uint8_t n_id = n->id_;
        Node* successor = n->get_finger_table_entry(0);
        if (!successor) return n;
        uint8_t s_id = successor->get_id();
        if (in_range(n_id + 1, s_id, id)) {
            return n;
        }
        n = n->find_closest_preceding_finger(id);
    }
}

Node* Node::find_successor(uint8_t id) {
    return find_predecessor(id)->get_finger_table_entry(0);
}

void Node::join(Node* node) {
    if (node) {
        predecessor_ = node->find_predecessor(id_);
        uint8_t shift = 1;
        for (int i = 0; i < BITLENGTH; i++) {
            uint8_t start_id = id_ + shift;
            fingerTable_.set(i + 1, find_successor(start_id));
            shift <<= 1;
        }
    } else {
        for (int i = 0; i < BITLENGTH; i++) {
            fingerTable_.set(i + 1, this);
        }
        predecessor_ = this;
    }
}

uint8_t Node::find(uint8_t key) {
    if (localKeys_.find(key) != localKeys_.end()) {
        return localKeys_[key];
    }
    return find_successor(key)->find(key);
}

void Node::insert(uint8_t key) {
    Node* targetNode = find_successor(key);
    targetNode->localKeys_[key] = key;
}

void Node::remove(uint8_t key) {
    Node* targetNode = find_successor(key);
    if (targetNode->localKeys_.find(key) != targetNode->localKeys_.end()) {
        targetNode->localKeys_.erase(key);
    }
}

#endif
