/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"
#include "cirDef.h"
using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
class Key
{
public:
  Key(CirGate *g){
    _p1=size_t(g->_fanin[0].net_get_id());
    _p2=size_t(g->_fanin[1].net_get_id());
    if(g->_fanin[0].net_get_not()==true) _p1=~_p1;
    if(g->_fanin[1].net_get_not()==true) _p2=~_p2;
  }
  ~Key(){}
  bool operator ==(const Key& k){
    if(_p1==k._p1&&_p2==k._p2) return true;
    else if(_p2==k._p1&&_p1==k._p2) return true;
    else return false;
  }
  size_t operator()()const{ return (_p1+_p2); }
  size_t _p1;
  size_t _p2;
};
/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed

void
CirMgr::strash()
{
   bool undo=false;
   HashMap<Key,CirGate*> hashMap(_dfsList.size());
   for(int i=0;i<_dfsList.size();i++){
   	  if(_dfsList[i]->getTypeStr()=="AIG"){
     	  CirGate* d=_dfsList[i];
        CirGate* s;
     	  Key key(d);
     	  if( hashMap.check(key,s) ){
          for(size_t i=0;i<2;i++){
            CirGate *p=cirMgr->getGate(d->_fanin[i].net_get_id());
            p->delete_fanout(d->get_id());
          }
          for(size_t i=0;i<d->_fanout.size();i++){
            CirGate *g=cirMgr->getGate(d->_fanout[i].net_get_id());
            g->delete_fanin(d->get_id());
            g->re_set_fanin(s->get_id(),d->_fanout[i].net_get_not());
          }
          _gate[d->get_id()]=0;
          delete d;
          undo=true;
          cout <<"Strashing: "<<s->get_id()<< " merging "<<d->get_id()<<"..."<< endl;
          _aig_num--;
          
        }
     	  else{ hashMap.insert(key,d); }
      }
      else{}
   }
   if(undo==true){ _dfsList.clear();  build_dfs(); }
   
   
}
//------------------------------------------------------------
void
CirMgr::fraig()
{
   
   
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::merge_fraig(CirGate *s,CirGate *d)
{
  //sGate 保留 /dGate被刪除
  //cout<<"Test  "<<s->get_id()<<"  "<<d->get_id()<<endl;
  for(size_t i=0;i<2;i++){
    CirGate *p=cirMgr->getGate(d->_fanin[i].net_get_id());
    p->delete_fanout(d->get_id());
  }
  for(size_t i=0;i<d->_fanout.size();i++){
    CirGate *g=cirMgr->getGate(d->_fanout[i].net_get_id());
    g->delete_fanin(d->get_id());
    g->re_set_fanin(s->get_id(),d->_fanout[i].net_get_not());
    s->re_set_fanout(g->get_id(),d->_fanout[i].net_get_not());
  }
  for(size_t i=0;i<_dfsList.size();i++){
    cout<<_dfsList[i]->getTypeStr()<<"  "<<_dfsList[i]->get_id()<<endl;
  }
  _gate[d->get_id()]=0;
  delete d;
}
