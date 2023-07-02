/*
  list.hpp
  
  Author: Paul Hamilton (paul@visualops.com)
  Date: 28-Jun-2023
  
  Tiny list for a tiny logo interpreter.
  
  The lists are all pooled together in a static data structure.
      
  This work is licensed under the Creative Commons Attribution 4.0 International License. 
  To view a copy of this license, visit http://creativecommons.org/licenses/by/4.0/ or 
  send a letter to Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.

  https://github.com/visualopsholdings/tinylogo
*/

#ifndef H_list
#define H_list

#define LIST_SIZE   40

#if LIST_SIZE <= 256
#define NODE_FREE   255
typedef unsigned char tNodeType;
#else
#define NODE_FREE   -1
typedef short tNodeType;
#endif

typedef unsigned char tListDataType;

// these match the optypes in logo.hpp
#define LTYPE_STRING         6 // data1 = index of string (-fixedcount), data2 = length of string
#define LTYPE_INT            7 // data1 = literal integer
#define LTYPE_DOUBLE         8 // data1 = literal integer part of double, data2 is fractional

class ListNode {

private:
  friend class ListPool;
  friend class ListNodeVal;
  
  short data1;
  short data2;
  tListDataType type;
  tNodeType next;

};

class ListNodeVal {

public:
  ListNodeVal(tListDataType type, short data1, short data2) : 
    _type(type), _data1(data1), _data2(data2) {}
  ListNodeVal() : 
    _type(0), _data1(0), _data2(0) {}
    
  void init();
  void copy(const ListNode &node);
  void copyto(ListNode *node) const;
  
  // direct access for logo
  tListDataType type() const { return _type; }
  short data1() const { return _data1; }
  short data2() const { return _data2; }
  
  // convenience for testing.
  bool isint() const;
  short intval() const;  
  static ListNodeVal newintval(short val);

#ifndef ARDUINO
  void dump(const char *msg) const;
#endif

private:  
  tListDataType _type;
  short _data1;
  short _data2;
};

class ListPool {

public:
  ListPool();
  
#ifndef ARDUINO
  void dump(const char *msg, bool all) const;
#endif

private:
  friend class List;
  
  ListNode _nodes[LIST_SIZE];

  int free() const;
  void add(tNodeType *next, ListNodeVal val);
  bool next(tNodeType *node) const;
  void getdata(tNodeType node, ListNodeVal *val) const;
  void pop(tNodeType *node, ListNodeVal *val);
  
};

class List {
  
public:
  List(ListPool *pool);
  List(ListPool *pool, tNodeType head, tNodeType tail);
  
  tNodeType head() const { return _head; }
  tNodeType tail() const { return _tail; }
  
  void push(ListNodeVal val);
  void first(ListNodeVal *val) const;
  void pop(ListNodeVal *val);
  bool iter(tNodeType *node, ListNodeVal *val);  
  int length() const;

private:
  ListPool *_pool;
  tNodeType _head;
  tNodeType _tail;
  
};

#endif // H_list
