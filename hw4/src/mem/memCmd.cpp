/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include <sstream>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
    // TODO
    vector<string>   options;
    istringstream option_line(option);
    string temp;
    int _id_int;
    int _id2_int;
    size_t _id;
    size_t _id2;
    while(getline(option_line,temp,' ')){
      options.push_back(temp);
    }
    if(options.size()==0)
      return CmdExec::errorOption(CMD_OPT_MISSING,option);
    if(myStrNCmp("-array",options[0],2)==0){
        if(options.size()==3&&myStr2Int(options[1],_id_int)==true&&_id_int>0&&
        myStr2Int(options[2],_id2_int)==true&&_id2_int>0){
           _id=_id_int;
           _id2=_id2_int;
           mtest.newArrs(_id2,_id);
        }
        else
          return CmdExec::errorOption(CMD_OPT_MISSING,option);
    }
    else if(myStr2Int(options[0],_id_int)==true&&_id_int>0){
        _id=_id_int;
        if(options.size()==1)
           mtest.newObjs(_id);
        else if(options.size()==3&&myStrNCmp("-array",options[1],2)==0&&
          myStr2Int(options[2],_id2_int)==true&&_id2_int>0){
           _id2=_id2_int;
           mtest.newArrs(_id,_id2);//(多少個,size=?)
        }
        else
          return CmdExec::errorOption(CMD_OPT_MISSING,option);
    }
    else{
       return CmdExec::errorOption(CMD_OPT_MISSING,option);
    }
    /*
    mtest.newObjs(7);
    mtest.newArrs(5,8);
    mtest.deleteObj(3);
    mtest.deleteArr(2);
    mtest.print();*/
    return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   vector<string>     options;
   istringstream option_line(option);
   string temp;
   int _id_int;
   size_t _id;
   size_t _objlistsize=mtest.getObjListSize();
   size_t _arrlistsize=mtest.getArrListSize();
   while(getline(option_line,temp,' ')){
      options.push_back(temp);
   }
   if(options.size()==0)
      return CmdExec::errorOption(CMD_OPT_MISSING,option);
   if(myStrNCmp("-random",options[0],2)==0){
      if(options.size()==2&&myStr2Int(options[1],_id_int)==true&&_id_int>0){
          _id=_id_int;
          for(size_t i=0;i<_id;i++){
            if(_objlistsize==0){
              cout<<"Size of array list (0) is <= "<<_id<<"!!"<<endl;
              return CmdExec::errorOption(CMD_OPT_MISSING,option);
            }
            else
              mtest.deleteObj(rnGen(_objlistsize-1));
          }
      }
      else if(options.size()==3&&myStr2Int(options[1],_id_int)==true&&_id_int>0
        &&myStrNCmp("-array",options[2],2)==0){
          _id=_id_int;
          for(size_t i=0;i<_id;i++){
            if(_arrlistsize==0){
              cout<<"Size of array list (0) is <= "<<_id<<"!!"<<endl;
              return CmdExec::errorOption(CMD_OPT_MISSING,option);
            }
            else
              mtest.deleteArr(rnGen(_arrlistsize-1));
          }
      }
      else
        return CmdExec::errorOption(CMD_OPT_MISSING,option);
   }
   else if(myStrNCmp("-index",options[0],2)==0){
      if(options.size()==2&&myStr2Int(options[1],_id_int)==true&&_id_int>=0){
          _id=_id_int;
          if(_id<_objlistsize)
            mtest.deleteObj(_id);
          else{
            cout<<"Size of object list ("<<_objlistsize<<") is <= "<<_id<<"!!"<<endl;
            cout<<"Error: Illegal option!! ("<<_id<<")"<<endl;
          }
      }
      else if(options.size()==3&&myStr2Int(options[1],_id_int)==true&&_id_int>=0
        &&myStrNCmp("-array",options[2],2)==0){
          _id=_id_int;
          if(_id<_arrlistsize)
            mtest.deleteArr(_id);
          else{
            cout<<"Size of array list ("<<_arrlistsize<<")"<<endl;
            cout<<"Error: Illegal option!! ("<<_id<<")"<<endl;
          }
      }
      else
        return CmdExec::errorOption(CMD_OPT_MISSING,option);
   }
   else if(myStrNCmp("-array",options[0],2)==0&&options.size()==3
    &&myStr2Int(options[2],_id_int)==true&&_id_int>=0){
      _id=_id_int;
      if(myStrNCmp("-random",options[1],2)==0){
         for(size_t i=0;i<_id;i++){
            if(_arrlistsize==0){
              cout<<"Size of array list ("<<_arrlistsize<<") "<<endl;
              return CmdExec::errorOption(CMD_OPT_MISSING,option);
            }
            else
              mtest.deleteArr(rnGen(_arrlistsize-1));
         }
      }
      else if(myStrNCmp("-index",options[1],2)==0){
          if(_id<_arrlistsize)
            mtest.deleteArr(_id);
          else{
            cout<<"Size of array list ("<<_arrlistsize<<") is <= "<<_id<<"!!"<<endl;
            cout<<"Error: Illegal option!! ("<<_id<<")"<<endl;
          }
      }
      else
        return CmdExec::errorOption(CMD_OPT_MISSING,option);
   }
   else{
      return CmdExec::errorOption(CMD_OPT_MISSING,option);
   }
   return CMD_EXEC_DONE;
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}


