#include <iostream>
#include <string>
#include "p2Table.h"

using namespace std;

int main(int argc, char **argv)
{
   Table table;
   // TODO: read in the csv file
   string csvFile;
   cout << "Please enter the file name: ";
   cin >> csvFile;
   //csvFile="test_hw1.csv";
   table.rowsize_number_f(csvFile);
   if (table.read(csvFile))
     cout << "File \"" << csvFile << "\" was read in successfully." << endl;
   else 
      exit(0); // csvFile does not exist.

   //cout<<"Test"<<table[4][0]<<endl;
   // TODO read and execute commands
   while (true) {
      string action,action_a,action_b;
      string string_a,string_b;
      //----------------------------------
      string line;
      getline(cin,line);
      istringstream in(line);
      in>>action;
      in>>action_a;
      string_a=action_a.c_str();
      //----------------------------------
      if(action=="PRINT")
         table.print();
      if(action=="EXIT")
         exit(0);
      if(action=="SUM")
         table.sum(string_a);
      if(action=="MAX")
         table.max(string_a);
      if(action=="MIN")
         table.min(string_a);
      if(action=="COUNT")
         table.count(string_a);
      if(action=="AVE")
         table.average(string_a);
      if(action=="ADD")
         table.add_row(line);
   }
}
