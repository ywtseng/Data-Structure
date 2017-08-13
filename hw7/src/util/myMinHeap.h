/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d) { 
      _data.push_back(d);
      size_t t=size()-1;
      while(t>0&&t<size()){
        int p=(t-1)/2;
        if(_data[p].getLoad()<_data[t].getLoad())
          break;
        else{
          Data temp=_data[p];
          _data[p]=_data[t];
          _data[t]=temp;
        }
        t=p;
      }
   }
   void delMin() { delData(0);}
   void delData(size_t i) {
      if(i>=size()){}
      else{
        _data[i]=_data.back();
        _data.pop_back();
        size_t l=2*i+1;
        size_t r=2*i+2;
        while(l<_data.size()){
          if(!(_data[i]<_data[l])||!(_data[i]<_data[r])){
            if(_data[l]<_data[r]){
              Data temp=_data[l];
              _data[l]=_data[i];
              _data[i]=temp;
              i=l;
              set_left_right(i,r,l);
            }
            else{
              Data temp=_data[r];
              _data[r]=_data[i];
              _data[i]=temp;
              i=r;
              set_left_right(i,r,l);
            }
          }
          else{  break; }
        }
      }
      return;
   }
   void set_left_right(size_t &i,size_t& r,size_t& l){
       l=2*i+1;
       r=2*i+2;
   }
private:
   // DO NOT add or change data members
   vector<Data>   _data;
};

#endif // MY_MIN_HEAP_H
