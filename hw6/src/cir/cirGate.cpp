/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"


using namespace std;

extern CirMgr *cirMgr;
//vector<unsigned>  trace;
//vector<unsigned>  ::iterator ct;


// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
void
CirGate::reportGate() const 
{
	cout<<"=================================================="<<endl;
	cout<<"=  "<<getTypeStr()<<"("<<_id<<")";
  if(get_symbol_name()!=" ")
      cout<<get_symbol_name();
  cout<<", line "<<getLineNo()<<endl;
	cout<<"=================================================="<<endl;
}

void
CirGate::reportFanin(int level) const
{
    assert (level >= 0);
    int back=0;
    cirMgr->set_all_false();
    cout<<getTypeStr()<<setw(2)<<right<<get_id()<<endl;
    //cout<<"Test::  "<<_fanin[0].net_get_gate()->get_id()<<endl;
    for(size_t i=0;i<_fanin.size();i++){
      cout<<setw(6)<<right;
      if(_fanin[i].net_get_not()==true)
        cout<<"!";
      else
        cout<<" ";
      cout<<_fanin[i].net_get_gate()->getTypeStr();
      cout<<setw(3)<<right<<_fanin[i].net_get_gate()->get_id();
      if(_fanin[i].net_get_gate()->get_trace()==true)
          cout<<"(*)";
      cout<<endl;
      _fanin[i].net_get_gate()->set_trace_true();
      dfs_fanin(_fanin[i].net_get_gate(),level-1,back+4);
    }
}

void
CirGate::reportFanout(int level) const
{
   assert (level >= 0);
   int back=0;
   cirMgr->set_all_false();
   cout<<getTypeStr()<<setw(2)<<right<<get_id()<<endl;
   for(size_t i=0;i<_fanout.size();i++){
      cout<<setw(6)<<right;
      if(_fanout[i].net_get_not()==true)
        cout<<"!";
      else
        cout<<" ";
      cout<<_fanout[i].net_get_gate()->getTypeStr();
      cout<<setw(3)<<right<<_fanout[i].net_get_gate()->get_id();
      if(_fanout[i].net_get_gate()->get_trace()==true)
        cout<<"(*)";
      cout<<endl;
      _fanout[i].net_get_gate()->set_trace_true();
      dfs_fanout(_fanout[i].net_get_gate(),level-1,back+4);
    }
}
void 
CirGate::dfs_fanin(CirGate *g,int level,int back) const
{
   if(level>0){
     for(size_t i=0;i<g->_fanin.size();i++){
        cout<<setw(6+back)<<right;
        if(g->_fanin[i].net_get_not()==true)
          cout<<"!";
        else
          cout<<" ";
        cout<<g->_fanin[i].net_get_gate()->getTypeStr();
        cout<<setw(3)<<right<<g->_fanin[i].net_get_gate()->get_id();
        if(g->_fanin[i].net_get_gate()->get_trace()==true)
          cout<<"(*)";
        cout<<endl;
        g->_fanin[i].net_get_gate()->set_trace_true();
        dfs_fanin(g->_fanin[i].net_get_gate(),level-1,back+4);
     }
   }
}
void 
CirGate::dfs_fanout(CirGate *g,int level,int back) const
{
  if(level>0){
     for(size_t i=0;i<g->_fanout.size();i++){
        cout<<setw(6+back)<<right;
        if(g->_fanout[i].net_get_not()==true)
          cout<<"!";
        else
          cout<<" ";
        cout<<g->_fanout[i].net_get_gate()->getTypeStr();
        cout<<setw(3)<<right<<g->_fanout[i].net_get_gate()->get_id();
        if(g->_fanout[i].net_get_gate()->get_trace()==true)
          cout<<"(*)";
        cout<<endl;
        g->_fanout[i].net_get_gate()->set_trace_true();
        dfs_fanout(g->_fanout[i].net_get_gate(),level-1,back+4);
     }
   }
   
}


