/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include <math.h>
#include "cirMgr.h"

#include "util.h"
#include "cirDef.h"
#include "cirGate.h"
using namespace std;

// TODO: Keep "CirMgr::randimSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/
extern CirMgr *cirMgr;

class HashKey
{
public:
  HashKey(CirGate *g){ _value =g->get_value(); }
  HashKey(const HashKey& h){ _value=h._value; }
  size_t operator() () const { return (size_t)_value; }
  bool operator == (const HashKey& k) { return _value==k._value; }
  HashKey operator~(){
     _value=~_value;
     HashKey ret(*this);
     _value=~_value;
     return ret;
  }
  unsigned   _value;
};
void
Feclist::initial(){
  _Fecgroup.clear();
  vector<CirGate*> fec;
  vector<CirGate*> _dfsList=cirMgr->get_dfsList();
  for(int i=0;i<_dfsList.size();i++){
      if(_dfsList[i]->getTypeStr()=="AIG")
        fec.push_back(_dfsList[i]); 
  }
  fec.push_back(cirMgr->get_const_gate());
  _Fecgroup.push_back(fec);
}
void 
Feclist::update(){
  for(int i=0;i<_Fecgroup.size();i++){
    vector<CirGate*>& fec=_Fecgroup[0];
    HashMap<HashKey,vector<CirGate*>*> hashmap(fec.size());
    HashMap<HashKey,vector<CirGate*>*>::iterator gt=hashmap.begin();
    for(int i=0;i<fec.size();i++){
      HashKey key(fec[i]);
      vector<CirGate*>* list;
      if(hashmap.check(key,list)){
         fec[i]->set_fec_false();
         list->push_back(fec[i]); 
      }
      else if(hashmap.check(~key,list)){
        fec[i]->set_fec_true(); 
        list->push_back(fec[i]); 
      }
      else{
         vector<CirGate*>* list_temp=new vector<CirGate*>;
         fec[i]->set_fec_false();
         list_temp->push_back(fec[i]);
         hashmap.insert(key,list_temp);
      }
    }
    _Fecgroup.erase(_Fecgroup.begin());
    for(gt=hashmap.begin();gt!=hashmap.end();gt++){
         if(((*gt).second)->size()>1) 
          _Fecgroup.push_back(*((*gt).second));
    }
  }
}
void 
Feclist::print(){
  vector<CirGate*>  _fec_bool;
  for(int i=0;i<_Fecgroup.size();i++){
    vector<CirGate*>& fec=_Fecgroup[i];
    for(int j=0;j<fec.size();j++){
      for(int k=0;k<fec.size();k++){
          if(fec[j]->get_id()<fec[k]->get_id()){
              CirGate *p=fec[j];
              fec[j]=fec[k];
              fec[k]=p;
          }
      }
    }
  }
  for(int i=0;i<_Fecgroup.size();i++){
    vector<CirGate*>& fec=_Fecgroup[i];
    bool _fec_first_bool=fec[0]->get_fec();
    cout<<"["<<i<<"]";
    for(int j=0;j<fec.size();j++){
      CirGate *g=fec[j];
      if(g->get_fec()!=_fec_first_bool) cout<<"!";
      cout<<g->get_id()<<" ";
    }
    cout<<endl;
  }
}
/**************************************/
/*   Static varaibles and functions   */
/**************************************/
	
/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
    int _max_fails=6*sqrt(_pi_num)+10;
    cout<<"MAX_FAILS = "<<_max_fails<<endl;
    _feclist.initial();
    for(int i=0;i<_max_fails;i++){
      vector<unsigned>   _pattern;
      _pattern.clear();
      for(int j=0;j<_pi_num;j++){
          unsigned pattern = 0;
          for(size_t k=0;k<4;++k)
             pattern |= (rnGen(256) << (8*k));
          _pattern.push_back(pattern);
      }
      simulation(_pattern);
    }
    for(size_t i=0;i<_dfsList.size();++i)
      _dfsList[i]->clear_fec_group();
    cout<<_pi_num*32<<" patterns simulated."<<endl;
}
void
CirMgr::fileSim(ifstream& patternFile)
{
  //declare
  vector<unsigned> _pattern;  
  vector<string>   _temp_pattern;
  string           _temp;
  //read pattern-------------------------
  for(int i=0;i<32;i++){
    //以32單位為一組
    patternFile>>_temp;
    if (patternFile.eof()){
       if (i==0) return;
       else break;
    }
    if(_temp.length()!=_pi_num){
       cerr<<"Error: Pattern("<<_temp<<") length("<< _temp.size()<< ") does not match the number of inputs("<<_pi_num<< ") in a circuit!!" << endl;
       return;
    }
    for(int j=0;j<_pi_num;j++){
       if(!(_temp[j]=='0'||_temp[j]=='1')){
         cerr<<"Error: Pattern("<< _temp[j] << ") contains a non-0/1 character("
             <<_temp[j]<<" )" << endl;
          return;
       }
    }
    _temp_pattern.push_back(_temp);
  }
  _pattern.clear();
  _pattern.resize(_pi_num);
  for(int i=0;i<_temp_pattern.size();i++){
    for(int j=0;j<_pi_num;j++)
      _pattern[j]|=((unsigned)(_temp_pattern[i][j]-'0') << i);
  }
  //---------------------------------
  _feclist.initial();
  simulation(_pattern);
  
  for(size_t i=0;i<_dfsList.size();++i)
    _dfsList[i]->clear_fec_group();
  simLog(_temp_pattern);
  cout<<_temp_pattern.size()<<" patterns simulated."<<endl;
}
void
CirMgr::simulation(vector<unsigned>& _pattern)
{
  for(int i=0;i<_pi_num;i++){
    CirGate *g=_gate[i+1];
    g->set_value(_pattern[i]);
  }
  for(int i=0;i<_dfsList.size();i++){
    if(_dfsList[i]->getTypeStr()=="PI"||
      _dfsList[i]->getTypeStr()=="CONST"){}
    else if(_dfsList[i]->getTypeStr()=="PO"){
       CirGate* g = _dfsList[i];
       g->update_po_value();
    }
    else if(_dfsList[i]->getTypeStr()=="AIG"){
       CirGate* g=_dfsList[i];
       g->update_aig_value();
    }
    else{}
  }
  _feclist.update();
}
void
CirMgr::simLog(vector<string>& _temp_pattern)                  
{
   if(_simLog==false) return;
   vector<unsigned>   _output;
   for (int i=0;i<_po_num;i++){
      CirGate* g=_gate[_max_var+i+1];
      _output.push_back(g->get_value());
   }
   for (int i=0;i<8*sizeof(unsigned);i++){
      *_simLog<<_temp_pattern[i];
      *_simLog<<" ";
      for(int j=0;j<_output.size();j++){
         *_simLog<<((_output[j]&(1<<i))>>i);
      }
      *_simLog<< endl;
   }
}
/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/
//檢查用的
void 
CirMgr::print_all_gate_value()
{
  for(size_t i=0;i<_dfsList.size();i++){
      if(_dfsList[i]->getTypeStr()=="PI"||_dfsList[i]->getTypeStr()=="CONST") 
        continue;
      if(_dfsList[i]->getTypeStr()=="PO"){
         cout<<"PO"<<_dfsList[i]->get_id()<<" "<<_dfsList[i]->get_value()<<endl;
      }
      if(_dfsList[i]->getTypeStr()=="AIG"){
         cout<<"AIG"<<_dfsList[i]->get_id()<<" "<<_dfsList[i]->get_value()<<endl;
      }
   }
}