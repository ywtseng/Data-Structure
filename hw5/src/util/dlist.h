/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const {
         return (_node->_data);
      }
      T& operator * () { 
         return (_node->_data); 
      }
      iterator& operator ++ () {
         _node=_node->_next;
         return *(this); 
      }
      iterator operator ++ (int) {
         iterator temp(*this);
         operator++();
         return temp; 
      }
      iterator& operator -- () {
         _node=_node->_prev;
         return *(this);  
      }
      iterator operator -- (int) { 
         iterator temp(*this);
         operator--();
         return temp;  
      }
      iterator& operator = (const iterator& i) {
         *(this)=i._node;
         return *(this); 
      }
      bool operator != (const iterator& i) const {
         if(_node!=i._node)
            return true;
         else
            return false;
      } 
      bool operator == (const iterator& i) const { 
         if(_node==i._node)
            return true;
         else
            return false;
      }
   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return (_head);  }
   iterator end() const { return (_head->_prev); }
   bool empty() const {
      if(_head->_next==_head&&_head->_prev==_head)
         return true;
      else
         return false; 
   }
   size_t size() const {
      DListNode<T>* t=_head;
      size_t count=0;
      while(t->_next!=_head){
         count++;
         t=t->_next;
      }
      return count; 
   }
   void push_back(const T& x) { 
      if (empty()) {
         DListNode<T>* t = new DListNode<T>(x, _head, _head);
         _head->_prev = t; 
         _head->_next = t;
         _head = t;
         return;
      }
      DListNode<T>* t= new DListNode<T>(x, _head->_prev->_prev, _head->_prev);
      t->_prev->_next = t; 
      t->_next->_prev = t;
   }
   void pop_front() {
      DListNode<T>* t=_head;
      t->_next->_prev=t->_prev;
      t->_prev->_next=t->_next;
      _head=t->_next;
   }
   void pop_back() { 
      DListNode<T>* t=_head->_prev;
      t->_next->_prev=t->_prev;
      t->_prev->_next=t->_next;
   }

   // return false if nothing to erase
   bool erase(iterator pos) {
      DListNode<T>*  _ptr=pos._node;
      if(_ptr==_head){
         _head->_next->_prev=_head->_prev;
         _head->_prev->_next=_head->_next;
         _head=_head->_next;
         return true;
      }
      else{
         _ptr->_next->_prev=_ptr->_prev;
         _ptr->_prev->_next=_ptr->_next;
         return true;
      }
      return false;
   }
   bool erase(const T& x) {
      iterator t=begin();
      if(*t==x){
         t._node->_next->_prev=_head->_prev;
         t._node->_prev->_next=_head->_next;
         _head=_head->_next;
         return true;
      }
      t++;
      while(t!=end()){
         if(*t==x){
            t._node->_next->_prev=t._node->_prev;
            t._node->_prev->_next=t._node->_next;
            return true;
         }
         t++;
      }
      return false;
   }

   void clear() {
      _head->_prev=_head;
      _head->_next=_head;
   }  // delete all nodes except for the dummy node

   void sort() const {
      DListNode<T> *_temp=_head;
      DListNode<T> *_ptr;
      for(_temp=_head;_temp!=_head->_prev;_temp=_temp->_next){
         _ptr=_temp;
         while( _ptr->_prev!=_head->_prev&&(_ptr->_data)<(_ptr->_prev->_data) ){
            T a=_ptr->_data;
            T b=_ptr->_prev->_data;
            _ptr->_prev->_data=a;
            _ptr->_data=b;
            _ptr=_ptr->_prev;
         }
      }
   }
private:
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

};

#endif // DLIST_H
