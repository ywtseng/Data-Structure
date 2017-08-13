/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <vector>
#include <iostream>
#include "cirDef.h"

using namespace std;

class CirGate;
//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class Net
{
public:   
  Net(CirGate* g,bool n):_gate(g),_not(n){}
  CirGate* net_get_gate() const { return _gate; }
  bool net_get_not() const { return _not; }
private:
  CirGate* _gate;
  bool     _not;
};
class CirGate
{
public:
   CirGate(unsigned d,unsigned n):_id(d),_line_no(n){
      _fanin.clear();
      _fanout.clear();
   }
   virtual ~CirGate() {}
   // Basic access methods
   virtual string getTypeStr() const=0;
   // Printing functions
   virtual void printGate() const = 0;
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   //自行自訂----------------------------------
   void set_fanin(CirGate* b,bool n){ _fanin.push_back(Net(b,n));  }
   void set_fanout(CirGate* g,bool n){ _fanout.push_back(Net(g,n)); }

   void set_name(string n){  _name=n; }
   void set_symbol_name(string symbol){  _symbol_name=symbol;  }
   void set_trace_true()  { _trace=true;  }
   void set_trace_false() { _trace=false; }

   size_t get_fanin_num(){  return _fanin.size(); }
   size_t get_fanout_num(){  return _fanout.size(); }
   unsigned get_id() const{ return _id;}
   unsigned getLineNo() const { return  _line_no; }
   string get_symbol_name()const{  return _symbol_name;  }
   bool get_trace(){ return _trace; }
   void cout_fanout_num(){  
      for(int i=0;i<_fanout.size();i++)
        cout<<_fanout[i].net_get_gate()->get_id()<<"  ";
   }
   virtual bool change_undef_to_const() {}
   void dfs_fanin(CirGate *g,int level,int back) const;
   void dfs_fanout(CirGate *g,int level,int back) const;
   vector<Net>      _fanin;
   vector<Net>      _fanout;
protected:
  unsigned _line_no;
  unsigned _id;
  bool     _out_gate;
  string   _name;
  string   _symbol_name=" ";
  bool     _trace=false;
};

class PI:public CirGate
{
public:
  PI(unsigned d,unsigned n):CirGate(d,n){
    _name="PI";
  }
  ~PI() {};
  string getTypeStr() const{ return _name; }
  void printGate() const{}
private:
  string _name;
  
};

class PO:public CirGate
{
public:
  PO(unsigned d,unsigned n):CirGate(d,n){
    _name="PO";
  }
  ~PO() {};
  string getTypeStr() const{ return _name; }
  void printGate() const{}
  //void change_aig_to_undef(){}
private:
  string _name;
};

class AIG:public CirGate
{
public:
  AIG(unsigned d,unsigned n):CirGate(d,n){
    _name="AIG";
  }
  ~AIG() {};
  string getTypeStr() const{ return _name; }
  void printGate() const{}
private:
  string _name;
};

class UNDEF:public CirGate
{
public:
  UNDEF(unsigned d,unsigned n):CirGate(d,n){
    _name="UNDEF";
  }
  ~UNDEF() {};
  string getTypeStr() const{ return _name; }
  void printGate() const{}
  bool change_undef_to_const(){ 
    if(_id==0){ 
      _name="CONST"; 
      return true;
    }
    return false;
  }
private:
  string _name;
};

#endif // CIR_GATE_H
