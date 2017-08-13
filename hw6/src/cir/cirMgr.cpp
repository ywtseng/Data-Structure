  /****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include <stack> 
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;
stack<CirGate*> mystack;


enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;
static unsigned _max_var_stable;
static unsigned print_lineNo=0;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   ifstream  infile(fileName.c_str());
   if(!infile) return false;
   //set variable------------------------------
   string _temp,_temp_b,_temp_c;
   int    _id,_id_b,_id_c;
   unsigned _input_check=0;
   unsigned _ouput_check=0;
   unsigned _aig_check=0;
   bool   build_const=false;
   vector <unsigned>  _temp_net;
   vector <unsigned>  _temp_po;
   vector <unsigned>  _temp_pi;
   vector <unsigned>  _temp_po_lineNo;
   vector <unsigned>  ::iterator bt;
   _temp_net.clear();
   _temp_po.clear();
   _temp_pi.clear();
   _temp_po_lineNo.clear();

   lineNo=0;
   //-----------------------------------------

   //Read all_information
   infile>>_temp>>_max_var>>_input>>_latch>>_output>>_aig;
   if(_temp!="aag"){
      errMsg=_temp;
      return parseError(ILLEGAL_IDENTIFIER);
   }
   if( white_space(_temp) ){
      errMsg=_temp;
      return parseError(EXTRA_SPACE);
   }
   lineNo++;
   _max_var_stable=_max_var;
   //Read PI
   for(int i=0;i<_input;i++){
      infile>>_temp;
      lineNo++;
      //-------------------------------------------------
      if( white_space(_temp) ){
        errMsg=_temp;
        return parseError(EXTRA_SPACE);
      }
      if(!myStr2Int(_temp,_id)){
         errMsg=_temp;
         return parseError(ILLEGAL_NUM);
      }
      //-------------------------------------------------
      assert(_id%2==0);
      CirGate *pi=new PI(_id/2,lineNo);
      _gate.insert(pair<unsigned,CirGate*>(_id/2,pi));
      _temp_pi.push_back(_id/2);
   }
   //Read PO
   for(int i=0;i<_output;i++){
      infile>>_temp;
      if( white_space(_temp) ){
        errMsg=_temp;
        return parseError(EXTRA_SPACE);
      }
      if(!myStr2Int(_temp,_id)){
         errMsg=_temp;
         return parseError(ILLEGAL_NUM);
      }
      lineNo++;
      _temp_po.push_back(_id);
      _temp_po_lineNo.push_back(lineNo);
   }
   //Read AIG
   for(int i=0;i<_aig;i++){
      //parser-----------------------------
      infile>>_temp>>_temp_b>>_temp_c;
      lineNo++;
      if( white_space(_temp) ){
        errMsg=_temp;
        return parseError(EXTRA_SPACE);
      }
      if(!myStr2Int(_temp  ,_id)){
         errMsg=_temp;
         return parseError(ILLEGAL_NUM);
      }
      if( white_space(_temp_b) ){
        errMsg=_temp;
        return parseError(EXTRA_SPACE);
      }
      if(!myStr2Int(_temp_b,_id_b)){
         errMsg=_temp_b;
         return parseError(ILLEGAL_NUM);
      }
      if( white_space(_temp_c) ){
        errMsg=_temp;
        return parseError(EXTRA_SPACE);
      }
      if(!myStr2Int(_temp_c,_id_c)){
         errMsg=_temp_c;
         return parseError(ILLEGAL_NUM);
      }
      //--------------------------------------
      assert(_id%2==0);
      CirGate *aig=new AIG(_id/2,lineNo);
      _gate.insert(pair<unsigned,CirGate*>(_id/2,aig));
      _temp_net.push_back(_id);
      _temp_net.push_back(_id_b);
      _temp_net.push_back(_id_c);
   }
   if(build_const==true){}
   else{
      CirGate *undef=new UNDEF(0,0);
      undef->change_undef_to_const();
      _gate.insert(pair<unsigned,CirGate*>(0,undef));
   }
   //Build net
   for(bt=_temp_net.begin();bt!=_temp_net.end();bt++){
      unsigned aig_id=*bt;
      bt++;
      unsigned in_left_id=*bt;
      bt++;
      unsigned in_right_id=*bt;
      CirGate *aig=_gate.find(aig_id/2)->second;
      //Build undef in_left
      at=_gate.find(in_left_id/2);
      if(at==_gate.end()){     
        CirGate *undef;
        undef=new UNDEF(in_left_id/2,0);
        if(in_left_id==0||in_left_id==1)
        build_const=undef->change_undef_to_const();
        _gate.insert(pair<unsigned,CirGate*>(in_left_id/2,undef));
        
      }
      CirGate* in_left=_gate.find(in_left_id/2)->second;
      //build-net-----------
      if(in_left_id%2==1){
          //加入not gate
          aig->set_fanin(in_left,true);
          in_left->set_fanout(aig,true);
      }
      else{
          aig->set_fanin(in_left,false);
          in_left->set_fanout(aig,false);
      }
      //--------------------
      //Build undef in_right      
      at=_gate.find(in_right_id/2);
      if(at==_gate.end()){
        CirGate *undef;
        undef=new UNDEF(in_right_id/2,0);
        if(in_right_id==0||in_right_id==1)
          undef->change_undef_to_const();
        _gate.insert(pair<unsigned,CirGate*>(in_right_id/2,undef));
        
      }
      //build-net-----------
      CirGate* in_right=_gate.find(in_right_id/2)->second;
      if(in_right_id%2==1){
          //加入not gate          
          aig->set_fanin(in_right,true);
          in_right->set_fanout(aig,true);
      }
      else{         
          aig->set_fanin(in_right,false);
          in_right->set_fanout(aig,false);
      }
      //--------------------
   }
   //build PO
   int i=0;
   for(bt=_temp_po.begin(),i=0;bt!=_temp_po.end();bt++,i++){
      int id=*bt;      
      CirGate *aig=_gate.find(id/2)->second;
      CirGate *po=new PO(_max_var+1,_temp_po_lineNo[i]);
      _gate.insert(pair<unsigned,CirGate*>(_max_var+1,po));

      if(id%2==1){
        aig->set_fanout(po,true);
        po->set_fanin(aig,true);
      }
      else{
        aig->set_fanout(po,false);
        po->set_fanin(aig,false);
      }
      _max_var++;
   }
    //read symbol
    getline(infile,_temp);
    string tok;
    int num;
    int num_pi=0;
    int num_po=0;
    while(!infile.eof()){
        getline(infile,_temp);
        if(_temp=="c"||_temp.size()==0)
          break;
        colNo=myStrGetTok(_temp,tok,1);
        colNo=myStrGetTok(_temp,tok,colNo);
        if(_temp[0]=='i'){
          CirGate *pi=_gate.find(_temp_pi[num_pi])->second;
          pi->set_symbol_name(tok);
          num_pi++;
        }
        if(_temp[0]=='o'){
          CirGate *po=_gate.find(_max_var_stable+num_po+1)->second;
          po->set_symbol_name(tok);
          num_po++;
        }
    }
    infile.close();
    return true;
}
void
CirMgr::set_all_false() {
  for(at=_gate.begin();at!=_gate.end();at++)
    at->second->set_trace_false(); 
}
void
CirMgr::set_all_false_const() const{
  map<unsigned,CirGate*> ::const_iterator  bt;
  for(bt=_gate.begin();bt!=_gate.end();bt++)
    bt->second->set_trace_false(); 
}
/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
  cout<<endl;
  cout<<"Circuit Statistics"<<endl;
  cout<<"=================="<<endl;
  cout<<"  "<<setw(3)<<left<<"PI"<<setw(10)<<right<<_input<<endl;
  cout<<"  "<<setw(3)<<left<<"PO"<<setw(10)<<right<<_output<<endl;
  cout<<"  "<<setw(3)<<left<<"AIG"<<setw(10)<<right<<_aig<<endl;
  cout<<"------------------"<<endl;
  cout<<"  "<<setw(3)<<left<<"Total"<<setw(8)<<right<<_input+_output+_aig<<endl;
}

void
CirMgr::printNetlist() const
{
    set_all_false_const();
    print_lineNo=0;
    while(!mystack.empty()){
      mystack.pop();
    }
    map<unsigned,CirGate*> ::const_iterator  bt;
    //-----------------------------
    for(bt=_gate.begin();bt!=_gate.end();bt++){
        if(bt->second->getTypeStr()=="PO"){
           mystack.push(bt->second);
           bt->second->set_trace_true();  
           dfs_netlist(bt->second);
        }
    }
}
bool 
CirMgr::white_space(string _temp){
  for(int i=0;i<_temp.size();i++){
    if(_temp[i]==' ')
      return true;
  }
  return false;
}
void
CirMgr::dfs_netlist(CirGate *g) const
{   
    if(mystack.size()==0)
      return;
    if(g->_fanin.size()!=0){
        bool _check=false;
        int i=0;
        while(i<g->_fanin.size()){
          if(g->_fanin[i].net_get_gate()->get_trace()==false&&g->_fanin[i].net_get_gate()->getTypeStr()!="UNDEF"){
            g->_fanin[i].net_get_gate()->set_trace_true();
            if(g->_fanin[i].net_get_gate()!=NULL)
              mystack.push( g->_fanin[i].net_get_gate() );
            _check=true;
            break;
          }
          i=i+1;
        }
        if(_check==false){
            if(mystack.size()!=0){
              CirGate *p=mystack.top();
              printNetlist_line(p);
              mystack.pop();  
            }
            else 
              return;
        }
        if(!mystack.empty()){
          dfs_netlist(mystack.top());
        }
        else 
          return;
    }
    else{
      while(mystack.size()!=0){
        //check fanin 為0
        if(mystack.top()->_fanin.size()==0){
          printNetlist_line(mystack.top());
          mystack.pop();
        }
        else{
          bool _check=false;
          CirGate *p=mystack.top();
          int i=0;
          while(i<p->_fanin.size()){
            if(p->_fanin[i].net_get_gate()->get_trace()==false&&p->_fanin[i].net_get_gate()->getTypeStr()!="UNDEF"){
              p->_fanin[i].net_get_gate()->set_trace_true();
              mystack.push( p->_fanin[i].net_get_gate() );
              _check=true;
              break;
            }
            i=i+1;
          }
          if(_check==false){
            printNetlist_line(mystack.top());
            mystack.pop();
          }
          if(_check==true){
            dfs_netlist(mystack.top());
          }
        }
      }
    }
}
void
CirMgr::printNetlist_line(CirGate *g) const
{
  if(g->getTypeStr()=="PI"){
    cout<<"["<<print_lineNo<<"]"<<setw(4)<<left<<g->getTypeStr();
    cout<<" "<<left<<g->get_id();
    if(g->get_symbol_name()!=" ")
      cout<<"("<<g->get_symbol_name()<<")";
  }
  else if(g->getTypeStr()=="PO"){
    cout<<"["<<print_lineNo<<"]"<<setw(4)<<left<<g->getTypeStr();
    cout<<" "<<left<<g->get_id();
    cout<<" ";
    if(g->_fanin[0].net_get_not()==true)
      cout<<"!";
    cout<<g->_fanin[0].net_get_gate()->get_id();
    if(g->get_symbol_name()!=" ")
      cout<<"("<<g->get_symbol_name()<<")  ";
  }
  else if(g->getTypeStr()=="AIG"){
    cout<<"["<<print_lineNo<<"]"<<setw(4)<<left<<g->getTypeStr();
    cout<<" "<<g->get_id();
    cout<<" ";
    //-----------------------------
    if(g->_fanin[0].net_get_gate()->getTypeStr()=="UNDEF")
      cout<<"*";
    if(g->_fanin[0].net_get_not()==true)
      cout<<"!";
    cout<<g->_fanin[0].net_get_gate()->get_id();
    cout<<" ";
    //-----------------------------
    if(g->_fanin[1].net_get_gate()->getTypeStr()=="UNDEF")
      cout<<"*";
    if(g->_fanin[1].net_get_not()==true)
      cout<<"!";
    cout<<g->_fanin[1].net_get_gate()->get_id();
  }
  else if(g->getTypeStr()=="CONST"){
    cout<<"["<<print_lineNo<<"]"<<setw(4)<<left<<g->getTypeStr();
    cout<<" "<<g->get_id();
  }
  else{
    return;
  }
  print_lineNo++;
  cout<<endl;
}
void
CirMgr::printPIs() const
{
   cout<<"PIs of the circuit:  ";
   map<unsigned,CirGate*> ::const_iterator  bt;
   for(bt=_gate.begin();bt!=_gate.end();bt++){
      if(bt->second->getTypeStr()=="PI")
        cout<<bt->second->get_id()<<"  ";
   }
}

void
CirMgr::printPOs() const
{
   cout<<"POs of the circuit:  ";
   map<unsigned,CirGate*> ::const_iterator  bt;
   for(bt=_gate.begin();bt!=_gate.end();bt++){
      if(bt->second->getTypeStr()=="PO")
        cout<<bt->second->get_id()<<"  ";
   }
}

void
CirMgr::printFloatGates() const
{
   map<unsigned,CirGate*> ::const_iterator  bt;
   vector<CirGate *> _temp_undef;
   vector<unsigned> _temp_dbniu;
   _temp_undef.clear();
   _temp_dbniu.clear();

   for(bt=_gate.begin();bt!=_gate.end();bt++){
       if(bt->second->getTypeStr()=="UNDEF"){
          _temp_undef.push_back(bt->second);
       }
       if(bt->second->get_fanout_num()==0&&bt->second->getTypeStr()!="PO"&&bt->second->get_id()!=0){
          _temp_dbniu.push_back(bt->second->get_id());
       }
   }
   if(_temp_undef.size()!=0){
      cout<<"Gates with floating fanin(s):  ";
      for(int i=0;i<_temp_undef.size();i++){
        _temp_undef[i]->cout_fanout_num();
      }
      cout<<endl;
   }
   if(_temp_dbniu.size()!=0){
      cout<<"Gates defined but not used  :  ";  
      for(int i=0;i<_temp_dbniu.size();i++){
        cout<<_temp_dbniu[i]<<"  ";
      }
      cout<<endl;
   }
          


}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile<<"aag  "<<_max_var_stable<<" "<<_input<<" "<<_latch<<" "<<_output<<" "<<_aig<<endl;
   map<unsigned,CirGate*> ::const_iterator  bt;
   int i;
   for(bt=_gate.begin(),i=0;i<_input;bt++){
      if(bt->second->getTypeStr()=="PI"){
        outfile<<bt->second->get_id()*2<<endl;
        i++;
      }
   }
   bt=_gate.end();
   bt--;
   for(bt,i=0;i<_output;bt--){
      if(bt->second->getTypeStr()=="PO"){
        if(bt->second->_fanin[0].net_get_not()==true)
          outfile<<bt->second->_fanin[0].net_get_gate()->get_id()*2+1<<endl;
        else
          outfile<<bt->second->_fanin[0].net_get_gate()->get_id()*2<<endl;
        i++;
      }
   }
   for(bt=_gate.begin(),i=0;i<_aig;bt++){
      if(bt->second->getTypeStr()=="AIG"){
        outfile<<bt->second->get_id()*2<<" ";
        if(bt->second->_fanin[0].net_get_not()==true)
          outfile<<bt->second->_fanin[0].net_get_gate()->get_id()*2+1<<" ";
        else
          outfile<<bt->second->_fanin[0].net_get_gate()->get_id()*2<<" ";
        if(bt->second->_fanin[1].net_get_not()==true)
          outfile<<bt->second->_fanin[1].net_get_gate()->get_id()*2+1;
        else
          outfile<<bt->second->_fanin[1].net_get_gate()->get_id()*2;
        outfile<<endl;
        i++;
      }
   }
    for(bt=_gate.begin(),i=0;i<_input;bt++){
      if(bt->second->getTypeStr()=="PI"){
        if(bt->second->get_symbol_name()!=" ")
          outfile<<"i"<<i<<" "<<bt->second->get_symbol_name()<<endl;
        i++;
      }
   }
   for(bt=_gate.begin(),i=0;i<_output;bt++){
      if(bt->second->getTypeStr()=="PO"){
        if(bt->second->get_symbol_name()!=" ")
          outfile<<"o"<<i<<" "<<bt->second->get_symbol_name()<<endl;
        i++;
      }
   }
   outfile<<"c"<<endl;
   outfile<<"AAG output by Yu-Wei (Nady) Tseng"<<endl;
}
