/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;
class CirGate;


// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr() {}
   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const { 
      CirGate* target=_gate.find(gid)->second;
      if(target!=NULL) {return target; }
      else { return 0; }
   }
   // Member functions about circuit construction
   bool readCircuit(const string&);
   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printNetlist_line(CirGate *g) const;
   void writeAag(ostream&) const;
   void dfs_netlist(CirGate *g) const;
   void set_all_false() ;
   void set_all_false_const() const;
   bool white_space(string);
private:
   unsigned       _max_var;
   unsigned       _input;
   unsigned       _latch;
   unsigned       _output;
   unsigned       _aig;
   map<unsigned,CirGate*>             _gate;
   map<unsigned,CirGate*> ::iterator  at;
};
#endif // CIR_MGR_H
