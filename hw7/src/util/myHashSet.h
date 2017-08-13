/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;
   public:
      iterator(vector<Data>* b,size_t n,size_t l,size_t p):
      _buckets(b),_numBuckets(n),_line(l),_pos(p){
         if(_numBuckets==_line){}
         else{ _data=&_buckets[_line][_pos]; }
      }
      ~iterator(){}
      const Data& operator*()const { return *this; }
      Data& operator* (){ return *_data; }
      iterator& operator++(){
         if(_numBuckets==_line){}
         else{
            if(_pos<buckets_size(_line)-1){  _pos=_pos+1; }
            else{
               while(_line<_numBuckets){
                  _line=_line+1;
                  if(buckets_size(_line)!=0)
                     break;
               }
               _pos=0;
            }
            _data=&_buckets[_line][_pos];
         }
         return (*this);
      }
      iterator operator++(int){
         iterator temp(*this);
         operator++();
         return temp;
      }
      iterator& operator--(){
         if(_numBuckets==_line){}
         else{
            if(_pos<buckets_size(_line)-1){  _pos=_pos-1; }
            else{
               while(_line<_numBuckets){
                  _line=_line-1;
                  if(buckets_size(_line)!=0)
                     break;
               }
               _pos=buckets_size(_line)-1;
            }
            _data=&_buckets[_line][_pos];
         }
         return (*this);
      }
      iterator operator--(int){
         iterator temp(*this);
         operator--();
         return temp;
      }
      iterator& operator=(const iterator& i){
         iterator temp(*this);
         _buckets=i._buckets;
         _numBuckets=i._numBuckets;
         _pos=i._pos;
         _line=i._line;
         _data=i._data;
         return temp;
      }
      bool operator !=(const iterator& i)const{
         if(_line==i._line&&_pos==i._pos)
            return false;
         else
            return true;
      }
      bool operator ==(const iterator& i)const{
         if(_line==i._line&&_pos==i._pos)
            return true;
         else
            return false;
      }
   private:
      size_t buckets_size(size_t i){  return _buckets[i].size(); }
      vector<Data>* _buckets;
      size_t        _numBuckets;
      size_t        _pos;
      size_t        _line;
      Data*         _data;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const {
      size_t i=0;
      while(i<_numBuckets){
         if(_buckets[i].size()!=NULL)
            return iterator(_buckets,_numBuckets,i,0);
         i++;
      }
      return iterator(_buckets,_numBuckets,_numBuckets,0); 
   }
   // Pass the end
   iterator end() const { 
      return iterator(_buckets,_numBuckets,_numBuckets,0); 
   }
   // return true if no valid data
   bool empty() const { 
      if(begin()==end())
         return true; 
      else
         return false;
   }
   // number of valid data
   size_t size() const { 
      iterator at=begin();
      size_t s = 0;
      while(at!=end()){
         at++;
         s++;
      }
      return s; 
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const { 
      size_t num=bucketNum(d);
      size_t i=0;
      while(i<_buckets[num].size()){
         if(_buckets[num][i]==d){
            return true;
         }
         i++;
      }
      return false; 
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const {
      size_t num=bucketNum(d);
      size_t i=0;
      while(i<_buckets[num].size()){
         if(_buckets[num][i]==d){
            d=_buckets[num][i];
            return true;
         }
         i++;
      } 
      return false; 
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d) { 
      size_t num=bucketNum(d);
      size_t i=0;
      while(i<_buckets[num].size()){
         if(_buckets[num][i]==d){
            _buckets[num][i]=d;
            return true;
         }
         i++;
      }
      _buckets[num].push_back(d);
      return false; 
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d) {
      size_t num=bucketNum(d);
      size_t i=0;
      while(i<_buckets[num].size()){
         if(_buckets[num][i]==d){
            return false;
         }
         i++;
      }
      _buckets[num].push_back(d);
      return true; 
   }
   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d) { 
      size_t num=bucketNum(d);
      size_t i=0;
      while(i<_buckets[num].size()){
         if(_buckets[num][i]==d){
            _buckets[num][i]=_buckets[num].back();
            _buckets[num].pop_back();
            //_buckets[num].erase(_buckets[num].begin()+i);
            return true;
         }
         i++;
      }
      return false; 
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
