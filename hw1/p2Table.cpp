#include "p2Table.h"
#include <algorithm>
#include <cstdlib>

using namespace std;
// Implement member functions of class Row and Table here
Row::Row(int rowsize){
	size=(rowsize>0?rowsize:5 );
	_data=new int[size];
	/*for(int i=0;i<size;i++)
		_data[i]=0;*/
}
Row::Row(const Row&rowToCopy):size(rowToCopy.size){
	_data=new int[size];
	for(int i=0;i<size;i++)
		_data[i]=rowToCopy._data[i];
}
Row::~Row(){
	delete []_data;
}
const int Row::operator[](size_t i)const{
	if(i<0||i>=size){
		cerr<<"Error "<< i <<"out of range"<<endl;
		exit(1);
	}
	return _data[i];
}
int& Row::operator[](size_t i){
	if(i<0||i>=size){
		cerr<<"Error "<< i <<"out of range"<<endl;
		exit(1);
	}
	return _data[i];
}
int Row::getsize() const{
	return size;
}
//---------------------------------------------------
Table::Table(){}

const Row& Table::operator[] (size_t i) const{
	return _rows.at(i);
}
Row& Table::operator[](size_t i){
	return _rows.at(i);
}
void Table::rowsize_number_f(const string& csvFile){
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

bool Table::read(const string& csvFile)
{
	ifstream infile(csvFile.c_str());
	string line;
  	while (getline(infile, line,'\r')) 
	{
	    istringstream templine(line); 
	    string data;
	    int rowsize_number=0;
	    //initalize
	    Row new_row(max_rowsize_number);
    	for(int i = 0; i < new_row.getsize(); ++i)
    		new_row[i] =  INT_MAX;
    	//store value
	    while (getline( templine, data,',')){
	    	if(data.empty()){
	    		new_row[rowsize_number]=INT_MAX;
	    	}
	    	else{
	    		new_row[rowsize_number]=atoi(data.c_str());
	    	}
	    	rowsize_number++;
	  	}
        _rows.push_back(Row(new_row));
	}
	//for(at=_rows.begin();at!=_rows.end();at++)
	  	// cout<<(*at).getsize()<<endl;
	//print();
	return 1; // TODO
}
int Table::get_rows_size(void) {
	return _rows.size();
}
void Table::sum(string s_column){
	int column=atoi(s_column.c_str());
	int sum=0;
	for(int i=0;i<get_rows_size();i++){
		if(_rows[i][column]!=INT_MAX)
			sum=sum+_rows[i][column];
	}
	cout<<"The summation of data in column #"<<column<<"  is "<<sum<<endl;
}
void Table::max(string s_column){
	int column=atoi(s_column.c_str());
	int max=0;
	for(int i=0;i<get_rows_size();i++){
		if(_rows[i][column]!=INT_MAX && _rows[i][column]>max)
			max=_rows[i][column];
	}
	cout<<"The maxmum of data in column #"<<column<<"  is "<<max<<endl;
}
void Table::min(string s_column){
	int column=atoi(s_column.c_str());
	int min=_rows[0][column];
	for(int i=1;i<get_rows_size();i++){
		if(_rows[i][column]!=INT_MAX &&_rows[i][column]<min)
			min=_rows[i][column];
	}
	cout<<"The minimum of data in column #"<<column<<"  is "<<min<<endl;
}
void Table::count(string s_column){
	int column=atoi(s_column.c_str());
	temp_row.clear();
	for(int i=0;i<get_rows_size();i++){
		int temp=_rows[i][column];
		bt=find(temp_row.begin(),temp_row.end(),temp);
		if(_rows[i][column]!=INT_MAX &&bt==temp_row.end()){
			temp_row.push_back(temp);
		}
	}
   cout<<"The distinct count of data in column #"<<column<<"  is "<<temp_row.size()<<endl;
}
void Table::average(string s_column){
	int column=atoi(s_column.c_str());
	double average=0.0;
	int sum=0;
	int number=0;
	for(int i=0;i<get_rows_size();i++){
		if(_rows[i][column]!=INT_MAX ){
			sum=sum+_rows[i][column];
			number++;
		}
	}
	average=(double)sum/number;
	cout<<"The average of data in column #"<<column<<"  is "<<average<<endl;
}
void Table::add_row(string line){
	Row new_row(max_rowsize_number);
	for(int i = 0; i < new_row.getsize(); ++i)
		new_row[i]=INT_MAX;
	//----------------------------------
	istringstream in_test(line);
	int count=0;
    string temp_a;
	in_test>>temp_a;
    for(int i=0;i<max_rowsize_number;i++){
        string temp_b;
    	in_test>>temp_b;
    	if(temp_b.empty()){}
    	else
    		count++;
    }
	//----------------------------------
    istringstream in(line);
    string temp;
    in>>temp;
    for(int i=0;i<count;i++){
        string temp;
        in>>temp;
		if(temp=="-"){
			new_row[i]=INT_MAX;
		}
		else{
			new_row[i]=atoi(temp.c_str());
		}
    }
    _rows.push_back(Row(new_row));
    //print();
}
void Table::print(void){
	for(int i=0;i<get_rows_size();i++){
		for(int j=0;j<_rows[i].getsize();j++){
			if(_rows[i][j]!=INT_MAX )
				cout<<setw(4)<<_rows[i][j];
			else
				cout<<setw(4)<<" ";
		}
		cout<<endl;
	}
	cout<<"Print all the data"<<endl;
}

