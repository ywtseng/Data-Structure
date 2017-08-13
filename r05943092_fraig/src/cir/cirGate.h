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
#include "sat.h"

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

class CirGate;
class CirSort;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
class Net
{
public:   
  Net(unsigned i,bool n):_id(i),_not(n){}
  unsigned net_get_id() const { return _id; }
  bool net_get_not() const { return _not; }
private:
  unsigned   _id;
  bool       _not;
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
   bool isAig() const { 
      if(getTypeStr()=="AIG") return true; 
      else return false;
   }

   // Printing functions
   virtual void printGate() const {}
   void reportGate() const;
   void reportFanin(int level) const;
   void reportFanout(int level) const;
   //自行自訂
   void set_fanin(unsigned b,bool n){ _fanin.push_back(Net(b,n));  }
   void set_fanout(unsigned b,bool n){ _fanout.push_back(Net(b,n)); }
   void set_symbol_name(string symbol){  _symbol_name=symbol;  }
   void set_trace_true()  { _trace=true;  }
   void set_trace_false() { _trace=false; }

   size_t get_fanin_num(){  return _fanin.size(); }
   size_t get_fanout_num(){  return _fanout.size(); }
   unsigned get_id() const { return _id; }
   unsigned getLineNo() const { return _line_no; }
   string get_symbol_name()const{  return _symbol_name;  }
   bool get_trace(){ return _trace; }

   void cout_fanout_num(){  
      for(int i=0;i<_fanout.size();i++)
        cout<<_fanout[i].net_get_id()<<"  ";
   }
   void dfs_fanin(unsigned id,int level,int back) const;
   void dfs_fanout(unsigned id,int level,int back) const;
   void printGate();
   //只有特定function有的
   virtual bool change_undef_to_const() {}
   virtual unsigned get_pre_id(){}
   //Re_set fanin&fanout
   void delete_fanin(unsigned id);
   void delete_fanout(unsigned id);
   void clear_all_fanin(){ _fanin.clear(); }
   void clear_all_fanout(){ _fanout.clear(); }
   void re_set_fanin(unsigned b,bool n);
   void re_set_fanout(unsigned b,bool n);
   //print fanin
   void print_fanin();
   void print_fanout();
   //search fanin&fanout
   bool search_fanin(unsigned id);
   bool search_fanout(unsigned id);
   //data member-----------
   vector<Net>  _fanin;
   vector<Net>  _fanout;
   //optimize
   unsigned check_fanin();
   //simulation function
   void set_value(unsigned p) { _value = p; }
   unsigned get_value() { return _value; }
   void update_aig_value();
   void update_po_value();
   void set_fec_true(){ _fec=true; }
   void set_fec_false(){ _fec=false; }
   bool get_fec(){ return _fec; }
   void clear_fec_group(){ _fecgroup.clear();}
   //fraig
   void set_var(const Var& v) { _var = v; }
   Var get_var() const { return _var; }
   //vector<CirGate*>& getFecGroup() { return _fecgroup; }
   vector<CirGate*>  _fecgroup;
   //writegate
   void dfs_fanin_gate();
   void set_write_value(){ _write_value=true; }
   void set_write_value_false(){ _write_value=false; }
   bool get_write_value(){ return _write_value; }
protected:
  unsigned  _line_no;
  unsigned  _id;
  string    _name;
  string    _symbol_name=" ";
  bool      _trace=false;
  unsigned  _value;
  //writegate
  bool      _write_value=false;
  //simulation
  
  bool      _fec=false;
  //fraig
  Var       _var;
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
  PO(unsigned d,unsigned n,unsigned p):CirGate(d,n),_pre_id(p){
    _name="PO";
  }
  ~PO() {};
  string getTypeStr() const{ return _name; }
  void printGate() const{}
  unsigned get_pre_id(){ return _pre_id; }

private:
  string _name;
  unsigned _pre_id;
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
class CirSort
{
   public:
   bool operator() (CirGate* a, CirGate* b)
   { return ((a->get_id())< (b->get_id())); }
};

#endif // CIR_GATE_H
