/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
extern CirMgr *cirMgr;
// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
void
CirMgr::sweep()
{ 
  vector<CirGate* > _output;
  _output.clear();
  _dfsList.clear();
  build_dfs();
  for(int i=(_gate.size()-1);i>=0;i--){
    if(_gate[i]!=NULL&&_gate[i]->get_trace()==false){
      if(_gate[i]->getTypeStr()=="AIG"){
        _output.push_back(_gate[i]);
        sweep_delete_gate(i);
        _aig_num--;
      }
      else if(_gate[i]->getTypeStr()=="UNDEF"){
        CirGate *p=_gate[i];
        bool undo=false;
        for(int i=0;i<p->_fanout.size();i++){
          CirGate *q=getGate(p->_fanout[i].net_get_id());
          if(q->get_trace()==true) undo=true;
        }
        if(undo==false){
          _output.push_back(_gate[i]);
          sweep_delete_gate(i);
        }
        else{}
      }
    }
  }
  if(_output.size()!=0){
    for(int i=(_output.size()-1);i>=0;i--)
       cout<<"Sweeping: "<<_output[i]->getTypeStr()<<"("<<_output[i]->get_id()<<") removed..."<<endl;
  }
}
bool 
CirMgr::sweep_delete_gate(unsigned id)  {
   CirGate* g=getGate(id);
   if(g->getTypeStr()=="AIG"){
     CirGate* p1=getGate(g->_fanin[0].net_get_id());
     CirGate* p2=getGate(g->_fanin[1].net_get_id()); 
     if(p1!=NULL){
       for(int i=0;i<p1->_fanout.size();i++){
          if(p1->_fanout[i].net_get_id()==id)
            p1->_fanout.erase(p1->_fanout.begin()+i);
       }
     }
     if(p2!=NULL){
       for(int i=0;i<p2->_fanout.size();i++){
          if(p2->_fanout[i].net_get_id()==id)
            p2->_fanout.erase(p2->_fanout.begin()+i);
       }
     }
   }
   if(g->getTypeStr()=="UNDEF"){
      for(int i=0;i<g->_fanout.size();i++){
        CirGate* p=getGate(g->_fanin[i].net_get_id());
        p->delete_fanin(g->get_id());
      }
   }
   _gate.erase(_gate.begin()+id);
}
void
CirMgr::optimize()
{
  bool undo=false;
  for(size_t i=0;i<_dfsList.size();i++){
     //g為準備被刪除的gate
     CirGate* g=_dfsList[i];
     if(g->getTypeStr()=="AIG"){
       unsigned delete_id=g->get_id();
       unsigned n=g->check_fanin();
       //cout<<n<<endl;
       if(n==5){}
       else{
          CirGate* p2=NULL;
          unsigned in;
          bool inv=true;
          bool test=false;
          if(n==1){
            _gate[0]->delete_fanout(g->get_id());
            if(g->_fanin[0].net_get_id()==0){ p2=getGate(g->_fanin[1].net_get_id()); in=0; }
            if(g->_fanin[1].net_get_id()==0){ p2=getGate(g->_fanin[0].net_get_id()); in=1; }
            (p2->search_fanout(g->get_id())==true)?(test=true):(test=false);
            p2->delete_fanout(g->get_id());
            for(int i=0;i<g->_fanout.size();i++){
               CirGate *p=getGate(g->_fanout[i].net_get_id());
               p->delete_fanin(g->get_id());
               // 4 case true/false
               if(g->_fanout[i].net_get_not()==false&&test==false) inv=false;
               if(g->_fanout[i].net_get_not()==true &&test==true ) inv=false;
               p->set_fanin(p2->get_id(),inv);
               p2->set_fanout(p->get_id(),inv);
            } 
          }
          if(n==2){
            _gate[0]->delete_fanout(g->get_id());
            if(g->_fanin[0].net_get_id()==0){ p2=getGate(g->_fanin[0].net_get_id()); in=0; }
            if(g->_fanin[1].net_get_id()==0){ p2=getGate(g->_fanin[1].net_get_id()); in=1; }
             p2->delete_fanout(g->get_id());
             for(int i=0;i<g->_fanout.size();i++){
               CirGate *p=getGate(g->_fanout[i].net_get_id());
               p->delete_fanin(g->get_id());
               // 4 case true/false
               if(g->_fanout[i].net_get_not()==false&&g->_fanin[in].net_get_not()==false) inv=false;
               if(g->_fanout[i].net_get_not()==true &&g->_fanin[in].net_get_not()==true ) inv=false;
               p->set_fanin(0,inv);
               p2->set_fanout(p->get_id(),false);
             }
          }
          if(n==3){
            p2=getGate(g->_fanin[0].net_get_id());
            p2->delete_fanout(g->get_id());
            (g->_fanin[0].net_get_not()==true)?(test=true):(test=false);
            for(int i=0;i<g->_fanout.size();i++){
               CirGate *p=getGate(g->_fanout[i].net_get_id());
               p->delete_fanin(g->get_id());
               if(g->_fanout[i].net_get_not()==false&&g->_fanin[0].net_get_not()==false) inv=false;
               if(g->_fanout[i].net_get_not()==true &&g->_fanin[0].net_get_not()==true ) inv=false;
               p->set_fanin(p2->get_id(),inv);
               p2->set_fanout(p->get_id(),inv);
            }
          }
          if(n==4){
            p2=getGate(g->_fanin[0].net_get_id());
            p2->delete_fanout(g->get_id());
            for(int i=0;i<g->_fanout.size();i++){
               CirGate *p=getGate(g->_fanout[i].net_get_id());
               p->delete_fanin(g->get_id());
               inv=false;
               if(g->_fanout[i].net_get_not()==true) inv=true;
               p->set_fanin(0,inv);
               _gate[0]->set_fanout(p->get_id(),inv);
            }
            //只是為了控制輸出
            p2=_gate[0];
          }
          _aig_num--;
          undo=true;
          cout<<"Simplifying: "<<p2->get_id()<<" merging ";
          cout<<((test==true)?"!":" ");
          cout<<g->get_id() <<"..."<< endl;
          _gate[g->get_id()]=0;
          delete g;
       }
     }
  }
  if(undo==true){  _dfsList.clear();  build_dfs();  }  
}