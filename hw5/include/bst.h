/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>
#include <iomanip>
using namespace std;
template <class T> class BSTree;
// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
	friend class BSTree<T>;
	friend class BSTree<T>::iterator;
	BSTreeNode(const T &d,BSTreeNode<T>* pl=0,BSTreeNode<T>* ll=0,BSTreeNode<T>* rl=0):
	_data(d),_parent(pl),_llink(ll),_rlink(rl){}
	T            		   _data;
	BSTreeNode<T> *		_parent;
	BSTreeNode<T> *		_llink;
	BSTreeNode<T> *		_rlink;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
public:
	BSTree(){
      T x;
      _root = new BSTreeNode<T>(x,NULL,NULL,NULL);
      _root->_parent=_root;
	}
	~BSTree(){}
   class iterator {
      friend class BSTree;
   public:
      iterator(BSTreeNode<T>*n= 0):_node(n){}
      iterator(const iterator&i):_node(i._node){}
      ~iterator(){}
      const T& operator* ()const{ return (_node->_data); }
      T& operator* (){ return (_node->_data); }
      iterator& operator++(){
         //condition 1
         if(_node->_rlink==NULL&&_node->_llink==NULL){
            while(_node->_parent->_rlink==_node)
               _node=_node->_parent;
            _node=_node->_parent;
         }
         //condition 2
         else if(_node->_rlink==NULL){
            while(_node->_parent->_rlink==_node)
               _node=_node->_parent;
            _node=_node->_parent;
         }
         //condition 3
         else if(_node->_llink==NULL){
            _node=_node->_rlink;
            while(_node->_llink!=NULL)
               _node=_node->_llink;
         }
         //condition 4
         else{
            _node=_node->_rlink;
            while(_node->_llink!=NULL)
               _node=_node->_llink;
         }
         return *(this);
      }
      iterator operator++(int){
         iterator temp(*this);
         operator++();
         return temp;
      }
      iterator& operator--(){
         //condition 1
         if(_node->_llink==NULL&&_node->_rlink==NULL){
            while(_node->_parent->_llink==_node)
               _node=_node->_parent;
            _node=_node->_parent;
         }
         //condition 2
         else if(_node->_rlink==NULL){
            _node=_node->_llink;
            while(_node->_rlink!=NULL)
               _node=_node->_rlink;
         }
         //condition 3
         else if(_node->_llink==NULL){
            while(_node->_parent->_llink==_node)
               _node=_node->_parent;
            _node=_node->_parent;
         }
         //condition 4
         else{
            _node=_node->_llink;
            while(_node->_rlink!=NULL)
               _node=_node->_rlink;
         }
         return*(this);
      }
      iterator operator--(int){
         iterator temp(*this);
         operator--();
         return temp;
      }
      iterator& operator=(const iterator& i){
         _node=i._node;
         return *(this); 
      }
      bool operator !=(const iterator& i)const{
         if(_node!=i._node)
            return true;
         else
            return false;
      }
      bool operator ==(const iterator& i)const{
         if(_node==i._node)
            return true;
         else
            return false;
      }
   private:
      BSTreeNode<T>* _node;
   };
   void sort(){}
   void print(){}
   iterator begin()const{
      BSTreeNode<T>* _ptr=_root;
      while(_ptr->_llink!=NULL){
         _ptr=_ptr->_llink;
      }
      return _ptr;
   }
   iterator end()const{
      return _root;
   }
   bool empty() const{
      if(_root->_llink==NULL)
         return true;
      else
         return false;
   }
   size_t size(){
      iterator at=begin();
      size_t count=0;
      while(at!=end()){
         at++;
         count++;
      }
      return count;
   }
   void insert(const T& x){ 
	  BSTreeNode<T>* _ptr=new BSTreeNode<T>(x,NULL,NULL,NULL);
	  BSTreeNode<T>* _node;
	  BSTreeNode<T>* _prev;
	  if(_root->_llink==NULL){
       _root->_llink=_ptr;
       _ptr->_parent=_root;
	  }
	  else{
	  	 _node=_root->_llink;
	  	 while(_node!=NULL){
	  	 	_prev=_node;
	  	 	if(_ptr->_data < _node->_data)
	  	 		_node=_node->_llink;
	  	 	else
	  	 		_node=_node->_rlink;
	  	 }
	  	 if(_ptr->_data < _prev->_data)
	  	 	_prev->_llink=_ptr;
	  	 else
	  	 	_prev->_rlink=_ptr;
	  	 _ptr->_parent=_prev;
	  }
   }
   void pop_front(){
      iterator at=begin();
      erase(at);
   }
   void pop_back(){
      iterator at=end();
      at--;
      erase(at);
   }
   bool erase(iterator pos){
      BSTreeNode<T> *_del_node=pos._node;
      size_t _judge=judge_left_or_right(_del_node);
      if(_del_node->_llink==NULL&&_del_node->_rlink==NULL){
         if(_judge==1)
            _del_node->_parent->_llink=NULL;
         if(_judge==2)
            _del_node->_parent->_rlink=NULL;
         _del_node->_parent=NULL;
      }
      else if(_del_node->_rlink==NULL){
         if(_judge==1)
            _del_node->_parent->_llink=_del_node->_llink;
         if(_judge==2)
            _del_node->_parent->_rlink=_del_node->_llink;
         _del_node->_llink->_parent=_del_node->_parent;
      }
      else if(_del_node->_llink==NULL){
         if(_judge==1)
            _del_node->_parent->_llink=_del_node->_rlink;
         if(_judge==2)
            _del_node->_parent->_rlink=_del_node->_rlink;
         _del_node->_rlink->_parent=_del_node->_parent;
      }
      else{
         BSTreeNode<T>* _find_node=find_right_tree_min(_del_node);
         //special case-----------
         if(_find_node==_del_node->_rlink){
            if(_judge==1)
               _del_node->_parent->_llink=_find_node;
            if(_judge==2)
               _del_node->_parent->_rlink=_find_node;
            _find_node->_parent=_del_node->_parent;
            _find_node->_llink=_del_node->_llink;
            _find_node->_rlink=NULL;
            _del_node->_llink->_parent=_find_node;
         }
         else{
            size_t _judge_find_node=judge_left_or_right(_find_node);
            //-----------------------
            if(_judge_find_node==1)
               _find_node->_parent->_llink=NULL;
            if(_judge_find_node==2)
               _find_node->_parent->_rlink=NULL;
            //---------------------------------------
            _find_node->_parent=_del_node->_parent;
            if(_judge==1)
               _del_node->_parent->_llink=_find_node;
            if(_judge==2)
               _del_node->_parent->_rlink=_find_node;
            //---------------------------------------
            _find_node->_llink=_del_node->_llink;
            _find_node->_rlink=_del_node->_rlink;
            _find_node->_llink->_parent=_find_node;
            _find_node->_rlink->_parent=_find_node;
         }
      }
      delete _del_node;
      return true;
   }
   bool erase(const T& x){
      iterator at=begin();
      while(at!=end()){
         if(*at==x){
            erase(at);
            return true;
         }
         at++;
      }
      return false;
   }
   void clear(){
      _root->_llink = NULL;
   }
private:
	BSTreeNode<T>*	  _root;
   //helper function======================
   BSTreeNode<T> *search(T&x){
      _root=_root->_llink;
      BSTreeNode<T>* _search_node=_root;
      while(_search_node!=NULL){
         if(x==_search_node->_data)
            return _search_node;
         else if(x<_search_node->_data)
            _search_node=_search_node->_llink;
         else
            _search_node=_search_node->_rlink;    
      }
      return _search_node;
   }
   BSTreeNode<T>* find_right_tree_min(BSTreeNode<T>* _search_node){
      _search_node=_search_node->_rlink;
      while(_search_node->_llink!=NULL||_search_node->_rlink!=NULL){
         if(_search_node->_llink==NULL)
            _search_node=_search_node->_rlink;
         else 
            _search_node=_search_node->_llink;
      }
      return _search_node;
   }
   size_t judge_left_or_right(BSTreeNode<T>* _node){
      if(_node->_parent->_llink==_node)
         return 1;
      if(_node->_parent->_rlink==_node)
         return 2;
   }
};

#endif // BST_H
