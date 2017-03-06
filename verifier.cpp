/*
 * the program takes a 9x9 solved sudoku grid and determines 
 * if there are any errors. it also showcases the usage
 * of threads in C++.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <list>
#include <pthread.h>

using namespace std;

list<int> rows;	// list of errors in a pair of index and value
list<int> columns; 	// list of errors in a pair of index and value
list<int> subgrids;	// list of errors in a pair of index and value more efficient than vectors
int *grid[9];

int missingno(const int a[],int n);	// find index of number in array missing
int same(const int a[], int n); // check if all numbers in an array are equal
void *checkrows(void *param); // passed as parameter to thread creation to check rows
void *checkcolumns(void *param); // check columns for errors
void *checksubgrids(void *param); // check sub grids for errors 
void fillGrid(const string& filename); // read grid from file 
void easyFixes(); // for when there is only 1 error in a single row/column
void hardFixes(); // for when there are multiple errors in the same row or column
void checkResults(); // once all threads return, check for errors

int main(int argc, char **argv)
{
	if(argc < 2)
	{
		cout << "No file provided!" << endl;
	}
	else
	{
		string filename = argv[1];
		fillGrid(filename);
		pthread_t subthreads[3];	// create 3 threads 
		for (int i = 0;i<3;i++)
		{
			switch(i)
			{	// create all threads, one each for checking rows, columns, and subgrids
				case 0: pthread_create(&subthreads[0], NULL, checkrows, NULL); break;
				case 1: pthread_create(&subthreads[1], NULL, checkcolumns, NULL); break;
				case 2: pthread_create(&subthreads[2], NULL, checksubgrids, NULL); break;
			}
		}

		for (int i = 0;i<3;i++)	// wait for threads to finish work
			pthread_join(subthreads[i],NULL);

		checkResults(); // check results from threads
	}
	
	return 0;
}

int missingno(const int a[],int n)
{
	for(int i = 0;i < n;i++)
	{
		if (a[i] == 0) // here 0 means the the number was not in a row/column/subgrid
		return i; // index specifies value-1 i.e. index 0 = value 1, index 4 = value 5 etc.
	}
	return -1;
}

int same(const int a[], int n) // checking if all array elements are same, easier than checking against value
{   
    for(int i = 0; i < n - 1; i++)      
    {         
        if(a[i] != a[i + 1])
            return 0;
    }
    return 1;
}

void checkResults()
{
	// no errors found
	if(rows.size() == 0 && columns.size() == 0 & subgrids.size() == 0)
	{
		cout << "Solved puzzle has no errors." << endl;
	}
	else
	{
		// only 1 error, or multiple errors with a simple replacement fix
		if(rows.size() == columns.size())
		{
			easyFixes();
		}
		else	// more than 1 errors in a single row or column, dispution solved by subgrid
		{
 			hardFixes();
		}
	}
}

void hardFixes()
{
	// find list that has more errors and keep popping out errors until either
 	// rows or columns list is empty.
 	int finalIndex = 0;
 	int state = -1;
 	list<int> temp;
 	list<int> other;
 	if(rows.size() > columns.size()) // multiple errors in a column
 	{
 		state = 0;
 		temp = columns;
 		other = rows;
 		finalIndex = columns.front(); // index to be used for odd error
 	}
 	if(rows.size() < columns.size()) // multiple errors in a row
 	{
 		state = 1;
 		temp = rows;
 		other = columns;
 		finalIndex = rows.front();
 	}
 			
 	while(temp.size()!=0) // fix all errors until smaller list is empty
 	{					  // this means that only 1 error will remain
 		int rowIndex =  temp.front(); // first value is always index
	    temp.pop_front();
		int colIndex =  other.front();
	    other.pop_front();
	    int rowVal =  temp.front(); // second value is always missing number
	    temp.pop_front();
		int colVal =  other.front();
		other.pop_front();

		if(rowVal == colVal)
			cout << "There is an error at (" << rowIndex << "," << colIndex << ") expected " << rowVal << endl;
 	}

 	// last remaining one is definitely solved using the grid.
 	int otherIndex = other.front();
 	other.pop_front();
 	int otherVal = other.front();
 	int gridIndex = subgrids.front();
 	subgrids.pop_front();
 	int gridVal = subgrids.front();
 	subgrids.pop_front();
 	if(otherVal == gridVal) // check against value stored in grid (fix for file 5)
 	{
 		if(state == 0)
 			cout << "There is an error at (" << otherIndex << "," << finalIndex << ") expected " << otherVal << endl;
 		else
 			cout << "There is an error at (" << finalIndex << "," << otherIndex << ") expected " << otherVal << endl;
 	}
}

void easyFixes() // there is only 1 error per column or row
{
	int index = rows.size()/2; // gives number of errors
	for(int i = 0;i<index;i++) // run for all such errors
	{
		int rowIndex =  rows.front(); // first popped value is always index
	    rows.pop_front();
		int colIndex =  columns.front();
	    columns.pop_front();

	    int rowVal =  rows.front(); // second popped value is always missing number
	    rows.pop_front();
		int colVal =  columns.front();
	    columns.pop_front();

	    if(rowVal == colVal) // is always true in case of 1 error per row/column
	    	cout << "There is an error at (" << rowIndex << "," << colIndex << ") expected " << rowVal << endl;
	}
}

void fillGrid(const string& filename) // read file and insert numbers into 2d array grid
{
	for (int i = 0; i < 9; ++i) 
	{
	  grid[i] = new int[9];
	}
	string line;
	int row = 0;
	int prow = 0;
	int pcol = 0;
	int column = 0;
	ifstream tf(filename.c_str());
	if(tf.is_open())
	{
		while(getline(tf,line))
		{
			// remove commas and read as char array
			line.erase(std::remove(line.begin(), line.end(), ','), line.end());
			for (int i =0;i< line.size();i++)
			{
				grid[row][column] = line[i] - '0'; // this converts char to number
				column++;
			}
			row++;
			column = 0;
		}
		tf.close();
	}
}

void *checksubgrids(void *param)
{
	// this is the combination used to define indexes for subgrids
	// in a 9x9 grid
	int rowind[9] = {1,1,1,4,4,4,7,7,7};
	int colind[9] = {1,4,7,1,4,7,1,4,7};

	for(int i = 0;i<9;i++)
	{
		int rno = rowind[i];
		int cno = colind[i];

		int gridtester[9] = {0,0,0,0,0,0,0,0,0}; // increment index value if number found
		// this uses the combination array defined above to create an array
		// of values part of a single sub grid for all 9 sub grids created using the for loop
		int tempgrid[9] = {grid[rno-1][cno-1],grid[rno-1][cno],grid[rno-1][cno+1],
		grid[rno][cno-1],grid[rno][cno],grid[rno][cno+1],
		grid[rno+1][cno-1],grid[rno+1][cno],grid[rno+1][cno+1]};

		for(int j = 0;j<9;j++)
		{
			switch(tempgrid[j])
			{
				// each value corresponds to index-1 in the array
				case 1: gridtester[0]++; break;
				case 2: gridtester[1]++; break;
				case 3: gridtester[2]++; break;
				case 4: gridtester[3]++; break;
				case 5: gridtester[4]++; break;
				case 6: gridtester[5]++; break;
				case 7: gridtester[6]++; break;
				case 8: gridtester[7]++; break;
				case 9: gridtester[8]++; break;
			}
		}	
		if (same(gridtester,9) == 0) // if there are unequals i.e. all != 1
		{
			for(int m = 0;m<9;m++) // find the index at which error was found
			{
				if(gridtester[m] == 0)
				{
					//cout << "found problem in grid " << i+1 << endl;
					//cout << "missing number:" << missingno(gridtester,9)+1 << endl;
					gridtester[m] = -1; // value no longer needed
					subgrids.push_back(i+1); // push index to back of list
					subgrids.push_back(missingno(gridtester,9)+1); // push missing number to back of list
				}
			}
		}
	}
	pthread_exit(0);
}

void *checkcolumns(void *param)
{
	//int coltester[9] = {0,0,0,0,0,0,0,0,0}; 

	for(int i = 0;i<9;i++)
	{
		int coltester[9] = {0,0,0,0,0,0,0,0,0}; // increment array if number found
		for(int j = 0;j<9;j++)
		{
			switch(grid[j][i])
			{
				case 1: coltester[0]++; break;
				case 2: coltester[1]++; break;
				case 3: coltester[2]++; break;
				case 4: coltester[3]++; break;
				case 5: coltester[4]++; break;
				case 6: coltester[5]++; break;
				case 7: coltester[6]++; break;
				case 8: coltester[7]++; break;
				case 9: coltester[8]++; break;
			}
		}	
		if (same(coltester,9) == 0) // if something missing
		{
			//cout << "found problem in column " << i+1 << endl;
			//cout << "missing number:" << missingno(coltester,9)+1 << endl;
			columns.push_back(i+1);	// column number pushed to list
			columns.push_back(missingno(coltester,9)+1); // missing number pushed to list
		}
		//coltester[0]=coltester[1]=coltester[2]=coltester[3]=coltester[4]=coltester[5]=coltester[6]=coltester[7]=coltester[8]=coltester[9]=0;
	}
	pthread_exit(0);
}

void *checkrows(void *param)
{
	//int rowtester[9] = {0,0,0,0,0,0,0,0,0}; 
	for(int i = 0;i<9;i++)
	{
		int rowtester[9] = {0,0,0,0,0,0,0,0,0};  // increment array if number found
		for(int j = 0;j<9;j++)
		{
			switch(grid[i][j])
			{
				case 1: rowtester[0]++; break;
				case 2: rowtester[1]++; break;
				case 3: rowtester[2]++; break;
				case 4: rowtester[3]++; break;
				case 5: rowtester[4]++; break;
				case 6: rowtester[5]++; break;
				case 7: rowtester[6]++; break;
				case 8: rowtester[7]++; break;
				case 9: rowtester[8]++; break;
			}
		}	
		if (same(rowtester,9) == 0) // if something missing
		{
			//cout << "found problem in row " << i+1 << endl;
			//cout << "missing number:" << missingno(rowtester,9)+1 << endl;
			rows.push_back(i+1); // push index to back of list
			rows.push_back(missingno(rowtester,9)+1); // push missing value to list
		}
		//rowtester[0]=rowtester[1]=rowtester[2]=rowtester[3]=rowtester[4]=rowtester[5]=rowtester[6]=rowtester[7]=rowtester[8]=rowtester[9]=0;
	}
	pthread_exit(0);
}
