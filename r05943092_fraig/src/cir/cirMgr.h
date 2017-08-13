/****************************************************************************
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  FileName     [ cirMgr.h ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <stack>
#include <fstream>
#include <iostream>

using namespace std;

// TODO: Feel free to define your own classes, variables, or functions.

#include "cirDef.h"

extern CirMgr *cirMgr;

class Feclist
{
public:
  Feclist(){}
  ~Feclist(){}
  void initial();
  void update();
  void print();
private:
  vector< vector<CirGate*> >   _Fecgroup;
};

class CirMgr
{
public:
   CirMgr() {}
   ~CirMgr() {} 
   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      if(_gate[gid]!=NULL) { return _gate[gid]; }
      else { return 0; }
   }
   // Member functions about circuit construction
   bool readCircuit(const string&);
   // Member functions about circuit optimization
   void sweep();
   void optimize();
   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }
   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs(){ _feclist.print();}
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

   //自行自訂
   void set_all_false() ;
   void set_all_false_const() const;
   
   void build_dfs() ;
   void build_dfs_list(CirGate *g);
   //sweep
   bool sweep_delete_gate(unsigned id);
   //simulation
   vector<CirGate*> get_dfsList(){ return _dfsList; }
   CirGate * get_const_gate(){ return _gate[0]; }  
   void simulation(vector<unsigned>& _pattern);
   void simLog(vector<string>& _temp_pattern);
   void print_all_gate_value();
   //fraig
   void merge_fraig(CirGate *s,CirGate *g);
private:
   //自行自訂
   unsigned          _max_var;
   unsigned          _pi_num;
   unsigned          _po_num;
   unsigned          _aig_num;
   unsigned          _latch;
   vector<CirGate*>  _gate;
   vector<CirGate*>  ::iterator at;
   vector<CirGate*>  _dfsList;
   vector<unsigned>  _temp_net;
   vector<unsigned>  ::iterator bt;
   //simulation
   Feclist           _feclist;
   //orgin
   ofstream          *_simLog;

};

#endif // CIR_MGR_H
