#ifndef P2_TABLE_H
#define P2_TABLE_H

#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <iomanip>
using namespace std;
#define INT_MAX 200 

class Row
{
public:
    Row(int=5);
    Row(const Row&);
    ~Row();
   	const int operator[] (size_t i) const;// TODO
   	int& operator[] (size_t i); // TODO
   	int getsize() const;
private:
    int *_data;
    int size;
};

class Table
{
public:
	  Table();
    const Row& operator[] (size_t i) const;	
    Row& operator[] (size_t i);
    void rowsize_number_f(const string&);
    int max_rowsize_number;
    bool read(const string&);
  	int get_rows_size(void);
  	void sum(string);
  	void max(string);
  	void min(string);
  	void count(string);
  	void average(string);
  	void add_row(string);
  	void print(void);
private:
   vector<Row>  _rows;
   vector<Row>  ::iterator at;
   vector<int>  temp_row;
   vector<int>  ::iterator bt;
};

#endif // P2_TABLE_H
