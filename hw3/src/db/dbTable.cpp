/****************************************************************************
  FileName     [ dbTable.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Table member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <string>
#include <cctype>
#include <cassert>
#include <sstream>
#include <set>
#include <algorithm>
#include "dbTable.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/
ostream& operator << (ostream& os, const DBRow& r)
{
   // TODO: to print out a row.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
   for(int i=0;i<r.size();i++){
      if(r[i]==INT_MAX)
        os<<setw(6)<<right<<".";
      else
        os<<setw(6)<<right<<r[i];
   }
   return os;
}

ostream& operator << (ostream& os, const DBTable& t)
{
   // TODO: to print out a table
   // - Data are seperated by setw(6) and aligned right.
   // - Null cells are printed as '.'
   for (int i = 0; i < t.nRows();i++){
      for(int j=0; j< t.nCols(); j++){
         if(t[i][j]==INT_MAX)
            os<<setw(6)<<right<<".";
         else
            os<<setw(6)<<right<<t[i][j];
      }
      os<<endl;
   }
   return os;
}
//helper funtion-------------------
void DBTable::rowsize_number_f(const string& csvFile){
  ifstream infile_test(csvFile.c_str());
  string line;
  max_rowsize_number=0;
  while(getline(infile_test, line,'\r')) 
  {
      istringstream templine(line); 
      string data;
      int rowsize_number=0;
      while (getline( templine, data,',')){
        rowsize_number++;
      }
      if(rowsize_number>max_rowsize_number)
        max_rowsize_number=rowsize_number;
  }
}
//helper funtion-------------------end
ifstream& operator >> (ifstream& ifs, DBTable& t)
{
   // TODO: to read in data from csv file and store them in a table
   // - You can assume all the data of the table are in a single line.
     string line;
     bool _adjust=0;
     while(getline(ifs,line,'\r')){
        istringstream templine(line);
        string data;
        DBRow new_row;
        //new_row.reset();
        while(getline( templine, data,',')){
          if(data.empty()){
            new_row.addData(INT_MAX);
          }
          else{
            int input_data=atoi(data.c_str());
            new_row.addData(input_data);
            _adjust=1;
          }
        }
        for(int i=new_row.size();i<t.max_rowsize_number;i++)
          new_row.addData(INT_MAX);
        //----------------
        if(_adjust==0)
          return ifs;
        else{
          _adjust=0;
        }
        //----------------
        t.addRow(new_row);
     }
   return ifs;
}

/*****************************************/
/*   Member Functions for class DBRow    */
/*****************************************/
void
DBRow::removeCell(size_t c)
{
   // TODO
   _data.erase(_data.begin()+c);

}

/*****************************************/
/*   Member Functions for struct DBSort  */
/*****************************************/
bool
DBSort::operator() (const DBRow& r1, const DBRow& r2) const
{
   // TODO: called as a functional object that compares the data in r1 and r2
   //       based on the order defined in _sortOrder
   for(size_t i=0;i<_sortOrder.size();i++){
      if(r1[_sortOrder[i]]<r2[_sortOrder[i]])
        return true;
      if(r1[_sortOrder[i]]>r2[_sortOrder[i]])
        return false;
   }
   return false;
}

/*****************************************/
/*   Member Functions for class DBTable  */
/*****************************************/
void
DBTable::reset()
{
   // TODO
   _table.clear();
}

void
DBTable::addCol(const vector<int>& d)
{
   // TODO: add a column to the right of the table. Data are in 'd'.
  for(size_t i=0;i<d.size();i++)
    _table[i].addData(d[i]);
}

void
DBTable::delRow(int c)
{
   // TODO: delete row #c. Note #0 is the first row.
   _table.erase(_table.begin()+c);
}

void
DBTable::delCol(int c)
{
   // delete col #c. Note #0 is the first row.
   for (size_t i = 0, n = _table.size(); i < n; ++i)
      _table[i].removeCell(c);
}

// For the following getXXX() functions...  (except for getCount())
// - Ignore null cells
// - If all the cells in column #c are null, return NAN
// - Return "float" because NAN is a float.
float
DBTable::getMax(size_t c) const
{
   // TODO: get the max data in column #c
  float max=0.0;
  int count=0;
  for(size_t i=0;i<_table.size();i++){
      if( _table[i][c]!=INT_MAX && _table[i][c]>max)
        max=_table[i][c];
      else
        count++;
  }
  if(count==_table.size())
    return NAN;
  else
    return max;
}

float
DBTable::getMin(size_t c) const
{
   // TODO: get the min data in column #c
  float min=_table[0][c];
  int count=0;
  if(_table[0][c]==INT_MAX)
    count++;
  for(size_t i=1;i<_table.size();i++){
      if( _table[i][c]!=INT_MAX && _table[i][c]<min)
        min=_table[i][c];
      else
        count++;
  }
  if(count==_table.size()){
    return NAN;
  }
  else{
    return min;
  }
}

float 
DBTable::getSum(size_t c) const
{
   // TODO: compute the sum of data in column #c
   float sum=0;
   int count=0;
   for(size_t i=0;i<_table.size();i++){
      if( _table[i][c]!=INT_MAX )
        sum=sum+_table[i][c];
      else
        count++;
   }
   if(count==_table.size())
     return NAN;
   else
     return sum;
}

int
DBTable::getCount(size_t c) const
{
   // TODO: compute the number of distinct data in column #c
   // - Ignore null cells
    vector<int>  temp_row;
    vector<int>  ::iterator bt;
    temp_row.clear();
    for(int i=0;i<_table.size();i++){
      int temp=_table[i][c];
      bt=find(temp_row.begin(),temp_row.end(),temp);
      if(_table[i][c]!=INT_MAX &&bt==temp_row.end()){
        temp_row.push_back(temp);
      }
    }
    return temp_row.size();
}

float
DBTable::getAve(size_t c) const
{
   // TODO: compute the average of data in column #c
  double average=0.0;
  int sum=0;
  int number=0;
  for(size_t i=0;i<_table.size();i++){
    if(_table[i][c]!=INT_MAX ){
      sum=sum+_table[i][c];
      number++;
    }
  }
  average=(double)sum/number;
  return average;
}

void
DBTable::sort(const struct DBSort& s)
{
   // TODO: sort the data according to the order of columns in 's'
    ::sort(_table.begin(),_table.end(),s);
}

void
DBTable::printCol(size_t c) const
{
   // TODO: to print out a column.
   // - Data are seperated by a space. No trailing space at the end.
   // - Null cells are printed as '.'
  for(size_t i=0;i<_table.size();i++){
      if(_table[i][c]==INT_MAX)
          cout<<setw(6)<<right<<".";
      else
          cout<<setw(6)<<right<<_table[i][c];
   }
}

void
DBTable::printSummary() const
{
   size_t nr = nRows(), nc = nCols(), nv = 0;
   for (size_t i = 0; i < nr; ++i)
      for (size_t j = 0; j < nc; ++j)
         if (_table[i][j] != INT_MAX) ++nv;
   cout << "(#rows, #cols, #data) = (" << nr << ", " << nc << ", "
        << nv << ")" << endl;
}

