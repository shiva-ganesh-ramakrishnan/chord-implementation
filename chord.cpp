#ifndef NODE_H
#define NODE_H

#include <stdint.h>
#include<iostream>
#include <map>
#include <set>
#include <vector>

#define BITLENGTH 8

using namespace std;
//forward declaration
class Node;

//The following code is just for reference. You can define your own finger table class.
//Since the index uniquely determines the interval, only the successor needs to be maintained.  
class FingerTable{
public:
	/**
	 * @param nodeId: the id of node hosting the finger table.
	 */
	FingerTable() {}
	FingerTable(uint8_t nodeId);
	
	void set(size_t index, Node* successor){
		fingerTable_[index] = successor;
	}
	Node* get(size_t index) {
		return fingerTable_[index];
	}
	// TODO: complete print function
	void prettyPrint();
private:
	uint8_t nodeId_;
	std::vector<Node*> fingerTable_;
};

FingerTable::FingerTable(uint8_t nodeId): nodeId_(nodeId) {
	// According to Chord paper, the finger table starts from index=1
	fingerTable_.resize(BITLENGTH + 1);
}

class Node {
public:
	Node(uint8_t id): id_(id), fingerTable_(FingerTable(id)){};
	//TODO: implement node join function
	/**
	 * @param node: the first node to contact with to initialize join process. If this is the first node to join the Chord network, the parameter is NULL.
	 */
	void join(Node* node);
	//TODO: implement DHT lookup
	uint8_t find(uint8_t key);
	//TODO: implement DHT key insertion
	void insert(uint8_t key);
	//TODO: implement DHT key deletion
	void remove(uint8_t key);

	/**
	 * @param finger: the inder of the finger table entry to retrieve
	*/
	Node* get_finger_table_entry(uint8_t finger);

	void update_finger_table(Node* s, uint8_t finger_ind);

	std::map<uint8_t, uint8_t> transfer_keys_new_node(Node* new_node);

	uint8_t get_id();	
private:
	uint64_t id_;
	FingerTable fingerTable_;
	std::map<uint8_t, uint8_t> localKeys_;

	Node* predecessor_;

	Node* find_predecessor(uint8_t id);
	Node* find_successor(uint8_t id);

	/**
	 * Finds if key is in range [start, end] given module BIT_LENGTH
	*/
	bool in_range(uint8_t start, uint8_t end, uint8_t key);

	Node* find_closest_preceeding_finger(uint8_t id);
};

bool Node::in_range(uint8_t start, uint8_t end, uint8_t key) {
	if(start > end) {
		return (key >= start) or (key <= end);	
	} return (key >= start) and (key <= end);
}

uint8_t Node::get_id() {
	return id_;
}

Node* Node::get_finger_table_entry(uint8_t finger) {
	return fingerTable_.get(finger);
}

Node* Node::find_closest_preceeding_finger(uint8_t id) {
	for(int i=BITLENGTH-1;i>=0;i--) {
		Node* fnode = fingerTable_.get(i+1);
		uint8_t f_id = fnode->get_id();
		if(in_range(id_+1, id-1, f_id)) {
			return fnode;
		}
	}

	return this;
}

Node* Node::find_predecessor(uint8_t id) {
	Node* n = this;
	while(true) {
		uint8_t n_id = n->id_;
		uint8_t s_id = n->get_finger_table_entry(0)->get_id();

		if(in_range(n_id+1, s_id, id)) {
			return n;
		}

		n = n->find_closest_preceeding_finger(id);
	}
}

Node* Node::find_successor(uint8_t id) {
	return find_predecessor(id)->get_finger_table_entry(0);
}

void Node::update_finger_table(Node* s, uint8_t finger_ind) {
	uint8_t f_id = fingerTable_.get(finger_ind)->get_id();
	uint8_t s_id = s->get_id();
	if(in_range(id_, f_id, s_id)) {
		fingerTable_.set(finger_ind, s);
		predecessor_->update_finger_table(s, finger_ind);
	}
}

std::map<uint8_t, uint8_t> Node::transfer_keys_new_node(Node* new_node){
	uint8_t n_id = new_node->get_id();
	uint8_t s_id = id_;
	
	std::map<uint8_t, uint8_t> out;

	for(auto it = localKeys_.begin(); it != localKeys_.end();) {
		uint8_t key = it->first;
		uint8_t val = it->second;

		if(!in_range(n_id+1, s_id, key)) {
			it = localKeys_.erase(it);
			cout<<"Migrate key "<<key<<" from node "<<s_id<<" to node "<<n_id;
			out[key] = val;
		} else {
			++it;
		}
	}

	return out;
}

void Node::join(Node* node) {
	if(node != nullptr) {
		// set predecessor and successor for n and neighbours
		predecessor_ = find_predecessor(id_);
		
		// set finger table
		uint8_t shift = 1;
		for(int i=0;i<BITLENGTH;i++) {
			uint8_t start_id = id_ + shift;
			fingerTable_.set(i+1, find_successor(start_id));

			shift = shift << 1; 
		}

		// update others
		shift = 1;
		for(int i=0;i<BITLENGTH;i++) {
			Node* p = find_predecessor(id_ - shift);
			p->update_finger_table(this, i+1);

			shift = shift << 1;
		}

		// transfer keys from successor
		localKeys_ = fingerTable_.get(0)->transfer_keys_new_node(this);

	} else {
		for(int i=0;i<BITLENGTH;i++) {
			fingerTable_.set(i+1, this);
		} predecessor_ = this;
	}
}

uint8_t Node::find(uint8_t key) {
	if(localKeys_.find(key) != localKeys_.end()) {
		return localKeys_[key];
	}

	return find_successor(key)->find(key);
}

#endif
