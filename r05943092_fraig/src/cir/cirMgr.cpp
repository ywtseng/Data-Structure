/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <algorithm>
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
//自行自訂
stack<unsigned> mystack;
int _pi,_po,_aig;
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
//自行自訂
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
              << ": Cannot redefine const (" << errInt << ")!!" << endl;
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
    ifstream  infile(fileName);
    if(!infile){
      cout<<fileName<<"is not exist!"<<endl;
      return false;
    }
    string    _temp,_temp_b,_temp_c;
    int       _id,_id_left,_id_right;
    bool      build_const=false;
    _temp_net.clear();
    //read header
    infile>>_temp>>_max_var>>_pi_num>>_latch>>_po_num>>_aig_num;
    _gate.resize(_max_var+_po_num+1);
    lineNo++;
    //read PI
    for(int i=0;i<_pi_num;++i,++lineNo){
      infile>>_temp;
      myStr2Int(_temp,_id);
      assert(_id%2==0);
      _gate[_id/2]=new PI(_id/2,lineNo);
    }
    //read PO
    for(int i=0;i<_po_num;++i,++lineNo){
      infile>>_temp;
      myStr2Int(_temp,_id);
      _gate[_max_var+i+1]=new PO(_max_var+i+1,lineNo,_id);
    }
    //read AIG
    for(int i=0;i<_aig_num;++i,++lineNo){
      infile>>_temp>>_temp_b>>_temp_c;
      myStr2Int(_temp,_id);
      myStr2Int(_temp_b,_id_left);
      myStr2Int(_temp_c,_id_right);
      assert(_id%2==0);
      _gate[_id/2]=new AIG(_id/2,lineNo);
      _temp_net.push_back(_id);
      _temp_net.push_back(_id_left);
      _temp_net.push_back(_id_right);
    }
    //build net
    for(bt=_temp_net.begin();bt!=_temp_net.end();++bt){
      _id=*bt;       bt++;
      _id_left=*bt;  bt++;
      _id_right=*bt;
      CirGate *aig=_gate[_id/2];
      //Build undef in_left
      if(_gate[_id_left/2]==NULL){
        CirGate *undef=new UNDEF(_id_left/2,0);
        if(_id_left==0||_id_left==1)
        build_const=undef->change_undef_to_const();
        _gate[_id_left/2]=undef;
      }
      CirGate* in_left=_gate[_id_left/2];
      //build-net-----------
      if(_id_left%2==1){
          //加入not gate
          aig->set_fanin(_id_left/2,true);
          in_left->set_fanout(_id/2,true);
      }
      else{
          aig->set_fanin(_id_left/2,false);
          in_left->set_fanout(_id/2,false);
      }
      //Build undef in_right
      if(_gate[_id_right/2]==NULL){
        CirGate *undef=new UNDEF(_id_right/2,0);
        if(_id_right==0||_id_right==1)
        build_const=undef->change_undef_to_const();
        _gate[_id_right/2]=undef;
      }
      CirGate* in_right=_gate[_id_right/2];
      //build-net-----------
      if(_id_right%2==1){
          //加入not gate
          aig->set_fanin(_id_right/2,true);
          in_right->set_fanout(_id/2,true);
      }
      else{
          aig->set_fanin(_id_right/2,false);
          in_right->set_fanout(_id/2,false);
      }
    }
    if(build_const==true){}
    else{
       CirGate *undef=new UNDEF(0,0);
       undef->change_undef_to_const();
       _gate[0]=undef;
    }
    for(int i=0;i<_po_num;i++){
      CirGate *po=_gate[_max_var+i+1];
      CirGate *aig=_gate[po->get_pre_id()/2];
      if(po->get_pre_id()%2==1){
        aig->set_fanout(_max_var+i+1,true);
        po->set_fanin(po->get_pre_id()/2,true);
      }
      else{
        aig->set_fanout(_max_var+i+1,false);
        po->set_fanin(po->get_pre_id()/2,false);
      }
    }
    //read symbol
    getline(infile,_temp);
    int num=0;
    int num_po=0;
    string tok;
    while(!infile.eof()){
        getline(infile,_temp);
        if(_temp=="c"||_temp.size()==0)break;
        colNo=myStrGetTok(_temp,tok,1);
        myStr2Int(tok,num);
        colNo=myStrGetTok(_temp,tok,colNo);
        if(_temp[0]=='i')
          dynamic_cast<PI*>(_gate[num+1])->set_symbol_name(tok);
        if(_temp[0]=='o'){
          dynamic_cast<PO*>(_gate[_max_var+num_po+1])->set_symbol_name(tok);
          num_po++;
        }
    }
    build_dfs();
    
    /*for(int i=0;i<_gate.size();i++){
      if(_gate[i]!=NULL)
        cout<<"T: "<<_gate[i]->getTypeStr()<<_gate[i]->get_id()<<endl;
    }*/

    return true;
}
void
CirMgr::set_all_false() {
  for(int i=0;i<_gate.size();i++){
    if(_gate[i]!=NULL)
      _gate[i]->set_trace_false();
  }
}
void
CirMgr::set_all_false_const() const{
  for(int i=0;i<_gate.size();i++){
    if(_gate[i]!=NULL)
      _gate[i]->set_trace_false();
  }
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
  cout<<"  "<<setw(3)<<left<<"PI"<<setw(10)<<right<<_pi_num<<endl;
  cout<<"  "<<setw(3)<<left<<"PO"<<setw(10)<<right<<_po_num<<endl;
  cout<<"  "<<setw(3)<<left<<"AIG"<<setw(10)<<right<<_aig_num<<endl;
  cout<<"------------------"<<endl;
  cout<<"  "<<setw(3)<<left<<"Total"<<setw(8)<<right<<_pi_num+_po_num+_aig_num<<endl;
}

void
CirMgr::printNetlist() const
{
   for (unsigned i = 0, n = _dfsList.size(); i < n; ++i) {
      cout << "[" << i << "] ";
      _dfsList[i]->printGate();
      cout<<endl;
   }
}
void 
CirMgr::build_dfs() 
{
    set_all_false_const();
    while(!mystack.empty()){
      mystack.pop();
    }
    for(int i=0;i<_po_num;i++){
      CirGate* po=_gate[_max_var+i+1];
      mystack.push(_max_var+i+1);
      po->set_trace_true();
      build_dfs_list(po);
    }
}
void 
CirMgr::build_dfs_list(CirGate *g)
{
    if(mystack.size()==0) return;
    if(g->_fanin.size()!=0){
        bool _check=false;
        int i=0;
        while(i<g->_fanin.size()){
          CirGate *p=getGate(g->_fanin[i].net_get_id());
          if(p->get_trace()==false&&p->getTypeStr()!="UNDEF"){
            p->set_trace_true();
            if(p!=NULL) mystack.push( p->get_id() );
            _check=true;
            break;
          }
          i=i+1;
        }
        if(_check==false){
            if(mystack.size()!=0){
              CirGate *p=getGate(mystack.top());
              _dfsList.push_back(p);
              mystack.pop();  
            }
            else return;
        }
        if(!mystack.empty()) build_dfs_list( getGate(mystack.top()) );
        else return;
    }
    else{
      while(mystack.size()!=0){
        //check fanin 為0
        CirGate *p=getGate(mystack.top());
        if(p->_fanin.size()==0){
          _dfsList.push_back(p);
          mystack.pop();
        }
        else{
          bool _check=false;
          int i=0;
          while(i<p->_fanin.size()){
            CirGate *g=getGate(p->_fanin[i].net_get_id());
            if(g->get_trace()==false&&g->getTypeStr()!="UNDEF"){
              g->set_trace_true();
              mystack.push(g->get_id());
              _check=true;
              break;
            }
            i=i+1;
          }
          if(_check==false){
            _dfsList.push_back(getGate(mystack.top()));
            mystack.pop();
          }
          if(_check==true) build_dfs_list(getGate(mystack.top()));
        }
      }
    }
}
void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit: ";
   for(int i=0;i<_pi_num;i++){
     cout<<_gate[i+1]->get_id()<<" ";
   }
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit: ";
   for(int i=0;i<_po_num;i++){
     cout<<_gate[_max_var+i+1]->get_id()<<" ";
   }
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
  vector<CirGate*> nofanin;
  vector<unsigned> nofanout;
  for(int i=0;i<_gate.size();i++){
    CirGate* g=getGate(i);
    if(g==NULL){}
    else{
      if(g->getTypeStr()=="UNDEF")
        nofanin.push_back(g);
      if(g->get_fanout_num()==0&&g->getTypeStr()!="PO"&&g->get_id()!=0)
        nofanout.push_back(g->get_id());
    }
  }
  if(nofanin.size()!=0){
    cout<<"Gates with floating fanin(s):  ";
    for(int i=0;i<nofanin.size();i++){
        nofanin[i]->cout_fanout_num();
    }
    cout<<endl;
  }
  if(nofanout.size()!=0){
     cout<<"Gates defined but not used  :  ";  
      for(int i=0;i<nofanout.size();i++){
        cout<<nofanout[i]<<"  ";
      }
      cout<<endl;
  }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   outfile<<"aag  "<<_max_var<<" "<<_pi_num<<" "<<_latch<<" "<<_po_num<<" "<<_aig<<endl;
   for(int i=1;i<=_pi_num;i++){
      if(_gate[i]!=0)
        outfile<<_gate[i]->get_id()*2<<endl;
   }
   for(int i=(_max_var+1);i<(_max_var+_po_num+1);i++){
      if(_gate[i]!=0){
        if(_gate[i]->_fanin[0].net_get_not()==true)
          outfile<<_gate[i]->_fanin[0].net_get_id()*2+1<<endl;
        else
          outfile<<_gate[i]->_fanin[0].net_get_id()*2<<endl;
      }
   }
   for(int i=_pi_num;i<_max_var;i++){
      if(_gate[i]!=0&&_gate[i]->getTypeStr()=="AIG"){
        outfile<<_gate[i]->get_id()*2<<" ";
        if(_gate[i]->_fanin[0].net_get_not()==true) outfile<<(_gate[i]->_fanin[0].net_get_id()*2+1);
        else  outfile<<(_gate[i]->_fanin[0].net_get_id()*2);
        outfile<<" ";
        if(_gate[i]->_fanin[1].net_get_not()==true) outfile<<(_gate[i]->_fanin[1].net_get_id()*2+1);
        else  outfile<<(_gate[i]->_fanin[1].net_get_id()*2);
        outfile<<" ";
        outfile<<endl;
      }
   }
   for(int i=0;i<=_pi_num;i++){
      if(_gate[i]!=0&&_gate[i]->get_symbol_name()!=" ")
        outfile<<"i"<<i<<_gate[i]->get_symbol_name()<<endl;
   }
   for(int i=(_max_var+1);i<(_max_var+_po_num+1);i++){
      if(_gate[i]!=0&&_gate[i]->get_symbol_name()!=" ")
        outfile<<"o"<<i<<_gate[i]->get_symbol_name()<<endl;
   }
  
   outfile<<"c"<<endl;
   outfile<<"AAG output by Chung-Yang (Ric) Huang"<<endl;

}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
   //一開始要設定全部為false
   _pi=0;
   _po=0;
   _aig=0;
   for(int i=0;i<_gate.size();i++){
      if(_gate[i]!=NULL){
         _gate[i]->set_write_value_false();
      }
   }
   g->set_write_value();
   _aig++;
   for(int i=0;i<g->_fanin.size();i++){
      CirGate *p=getGate(g->_fanin[i].net_get_id());
      if(p!=0){
        if(p->getTypeStr()=="PI") _pi++;
        else if(p->getTypeStr()=="PO") _po++;
        else if(p->getTypeStr()=="AIG") _aig++;
        p->set_write_value();
        p->dfs_fanin_gate();
      }
   }
   //write--------
   outfile<<"aag  "<<g->get_id()<<" "<<_pi<<" "<<_latch<<" 1 "<<_aig<<endl;
   for(int i=0;i<=_pi_num;i++){
      if(_gate[i]->get_write_value()==true&&_gate[i]!=0){
        outfile<<_gate[i]->get_id()*2<<endl;
      }
   }
   outfile<<g->get_id()*2<<endl;
   for(int i=_pi_num;i<_max_var;i++){
      if(_gate[i]->get_write_value()==true&&_gate[i]!=0&&_gate[i]->getTypeStr()=="AIG"){
        outfile<<_gate[i]->get_id()*2<<" ";
        if(_gate[i]->_fanin[0].net_get_not()==true) outfile<<(_gate[i]->_fanin[0].net_get_id()*2+1);
        else  outfile<<(_gate[i]->_fanin[0].net_get_id()*2);
        outfile<<" ";
        if(_gate[i]->_fanin[1].net_get_not()==true) outfile<<(_gate[i]->_fanin[1].net_get_id()*2+1);
        else  outfile<<(_gate[i]->_fanin[1].net_get_id()*2);
        outfile<<" ";
        outfile<<endl;
      }
   }
   for(int i=0;i<=_pi_num;i++){
      if(_gate[i]->get_write_value()==true&&_gate[i]!=0&&_gate[i]->get_symbol_name()!=" "){
        outfile<<"i"<<i<<" "<<_gate[i]->get_symbol_name()<<endl;
      }
   }
   outfile<<"o0"<<g->get_id()<<endl;
   outfile<<"c"<<endl;
   outfile<<"Write gate ("<<g->get_id()<<") by Chung-Yang (Ric) Huang"<<endl;
}
void
CirGate::dfs_fanin_gate()
{
  if(_fanin.size()!=0){
    for(int i=0;i<2;i++){
      CirGate *p=cirMgr->getGate(_fanin[i].net_get_id());
      if(p!=0){
        if(p->getTypeStr()=="PI") _pi++;
        else if(p->getTypeStr()=="PO") _po++;
        else if(p->getTypeStr()=="AIG") _aig++;
        p->set_write_value();
        p->dfs_fanin_gate();
      }
    }
  }
  else return;
}
