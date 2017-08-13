/****************************************************************************
  FileName     [ array.h ]
  PackageName  [ util ]
  Synopsis     [ Define dynamic array package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef ARRAY_H
#define ARRAY_H

#include <cassert>
#include <algorithm>

using namespace std;

// NO need to implement class ArrayNode
//
template <class T>
class Array
{
public:
   // TODO: decide the initial value for _isSorted
   Array() : _data(0), _size(0), _capacity(0),_isSorted(false) {}
   ~Array() { delete []_data; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class Array;

   public:
      iterator(T* n= 0): _node(n) {}
      iterator(const iterator& i): _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return (*this); }
      T& operator * () { return (*_node); }

      iterator& operator ++ () {
         _node=_node+1; 
         return (*this); 
      }
      iterator operator ++ (int) {
         iterator temp(*this);
         operator++();
         return temp; 
      }
      iterator& operator -- () {
         _node=_node-1;  
         return (*this); 
      }
      iterator operator -- (int){
         iterator temp(*this);
         operator--();
         return temp;
      }
      iterator operator + (int i) const {
         iterator temp(*this);
         temp+=i;
         return (temp);   
      }
      iterator& operator += (int i) {
         //_node+=i; 
         //return (*this); 
         iterator temp(*this);
         _node+=i; 
         return (temp);  
      }
      iterator& operator = (const iterator& i) {
         //_node=i._node; 
         //return (*this); 
         iterator temp(*this);
         _node=i._node;
         return (temp); 
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
      T*    _node;
   };
   // TODO: implement these functions
   iterator begin() const { 
      if(empty())
         return 0;
      /*T* _temp=&(_data[0]);
      return (_temp);*/
      return iterator(_data);
   }
   iterator end() const {
      if(empty())
         return 0;
      /*if(_size>1){
         T* _temp=&(_data[_size]);
         return (_temp);
      }*/
      return iterator(_data+_size);
   }
   bool empty() const {
      if(_capacity==0)
         return true;
      else
         return false; 
   }
   size_t size() const { return (_size); }

   T& operator [] (size_t i) { return _data[i]; }
   const T& operator [] (size_t i) const { return _data[i]; }

   void push_back(const T& x) {
      if(_capacity==0){
         _capacity=1;
         _data=new T[_capacity];
      }
      else if(_size==_capacity){
         _capacity=_capacity*2;
         T* _past=_data;
         _data=new T[_capacity];
         for(size_t i=0;i<_size;i++)
            _data[i]=_past[i];
         delete[] _past;
      }
      _data[_size]=x;
      _size++;
   }
   void pop_front(){
      _data[0]=_data[_size-1];
      _size--;
   }
   void pop_back() {
      _size--;
   }

   bool erase(iterator pos) {
      T *d=pos._node;
      *d=_data[_size-1];
      --_size;
   }
   bool erase(const T& x) {
      iterator t=begin();
      size_t idx = 0;
      while(t!=end()){
         if(x==*(t._node)){
            _data[idx]=_data[_size-1];
            _size--;
            return true;
         }
         idx++;
         t++;
      } 
      return false;
   }
   void clear(){
      _data=NULL;
      _size=0;
      _capacity=0;
   }

   // This is done. DO NOT change this one.
   void sort() const { if (!empty()) ::sort(_data, _data+_size); }

   // Nice to have, but not required in this homework...
   // void reserve(size_t n) { ... }
   // void resize(size_t n) { ... }

private:
   T*            _data;
   size_t        _size;       // number of valid elements
   size_t        _capacity;   // max number of elements
   mutable bool  _isSorted;   // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] Helper functions; called by public member functions
};

#endif // ARRAY_H
