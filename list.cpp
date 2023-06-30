/*
  list.cpp
    
  Author: Paul Hamilton (paul@visualops.com)
  Date: 5-May-2023
  
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#include "list.hpp"

#ifndef ARDUINO
#include <iostream>

using namespace std;
#endif

ListPool::ListPool() {

  // mark all the nodes as free.
  short len = sizeof(_nodes) / sizeof(ListNode);
  for (int i=0; i<len; i++) {
    _nodes[i].data1 = 0;
    _nodes[i].data2 = 0;
    _nodes[i].next = NODE_FREE;
  }
}
  
void ListPool::add(tNodeType *next, ListNodeVal val) {

  // reserve this node.
  val.copyto(&_nodes[*next]);
  _nodes[*next].next = 0;
  
  // get the next free and set the next to that.
  int f = free();
  _nodes[*next].next = f;
  *next = f;
  
}

bool ListPool::next(tNodeType *node) const {

  if (_nodes[*node].next != NODE_FREE) {
    *node = _nodes[*node].next;
    return true;
  }
  return false;
}

void ListPool::getdata(tNodeType node, ListNodeVal *val) const {
  val->copy(_nodes[node]);
}

int ListPool::free() const {

  // loop through all nodes and find the next free one.
  short len = sizeof(_nodes) / sizeof(ListNode);
  for (int i=0; i<len; i++) {
    if (_nodes[i].next == NODE_FREE) {
      return i;
    }
  }
  return -1;
}

void ListPool::pop(tNodeType *node, ListNodeVal *val) {

  tNodeType n = *node;
  
  // get this node and move the head along.
  val->copy(_nodes[n]);
  *node = _nodes[n].next;
  
  // free up this node.
  _nodes[n].data1 = 0;
  _nodes[n].data2 = 0;
  _nodes[n].next = NODE_FREE;

}

#ifndef ARDUINO
void ListNodeVal::dump(const char *msg) const {
  cout << (short)_type << " (" << _data1 << ", " << _data2 << ")" << endl;
}

void ListPool::dump(const char *msg, bool all) const {
  
  cout << msg << endl;
  short len = sizeof(_nodes) / sizeof(ListNode);
  for (int i=0; i<len; i++) {
    if (_nodes[i].next == NODE_FREE) {
      if (all) {
        cout << "(free)" << endl;
      }
    }
    else {
      cout << "(" << _nodes[i].data1 << ", " << _nodes[i].data2 << ") -> " << (short)_nodes[i].next << endl;
    }
  }
  
}
#endif

List::List(ListPool *pool) : _pool(pool) {

  _head = _pool->free();
  _tail = _head;
  
}

List::List(ListPool *pool, tNodeType head, tNodeType tail) : _pool(pool) {

  _head = head;
  _tail = tail;

}

void List::first(ListNodeVal *val) const {

  if (_tail != _head) {
    _pool->getdata(_head, val);
    return;
  }
  val->init();
}

void List::pop(ListNodeVal *val) {
  if (_tail != _head) {
    _pool->pop(&_head, val);
    return;
  }
  val->init();
}

void List::push(ListNodeVal val) {
  _pool->add(&_tail, val);
}

int List::length() const {

  int len = 0;
  tNodeType node = _head;
  while (_pool->next(&node)) {
    len++;
  }
  return len;
  
}

bool List::iter(tNodeType *node, ListNodeVal *val) {

  if (*node == _tail) {
    val->init();
    return false;
  }
  
  _pool->getdata(*node, val);
  return _pool->next(node);
}

void ListNodeVal::init() {
  _type = 0;
  _data1 = 0;
  _data2 = 0;
}

void ListNodeVal::copy(const ListNode &node) {
  _type = node.type;
  _data1 = node.data1;
  _data2 = node.data2;
}

void ListNodeVal::copyto(ListNode *node) const {
  node->type = _type;
  node->data1 = _data1;
  node->data2 = _data2;
}

ListNodeVal ListNodeVal::newintval(short n) {
  ListNodeVal val;
  val._type = LTYPE_INT;
  val._data1 = n;
  val._data2 = 0;
  return val;
}

bool ListNodeVal::isint() const {
  return _type == LTYPE_INT;
}

short ListNodeVal::intval() const {
  if (isint()) {
    return _data1;
  }
  return 0;
}

