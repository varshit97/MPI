#include <iostream>
#include <fstream>
#include <pcre++.h>
#include <map>
#include "mpi.h"
using namespace std;

int main(int argc, char * argv[]){
	int world_rank, world_size;
	MPI_Status status;
	map<string, long> words;
	map<string, long>::iterator it;
	char line[1024];
	string sline;
	int source = 0;
	int p = 0, i, rc, pos;
	
	if(argc != 3 ){
		cout << "argument error" <<endl;
		return 1;
	}

	vector<string> allLines;

	string expression = "(" + string(argv[1]) + ")";
	pcrepp::Pcre reg(expression, "g");
	ifstream ifs(argv[2]);

	while(ifs.getline(line, 100024)){
		allLines.push_back(line);
	}

	int count = 0;
	int total = 1;
	vector<string> matchedLines;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int size = allLines.size()/(world_size - 1);
	int all = allLines.size()%(world_size - 1);
	if(world_rank == 0)
	{
		for(int i=1;i<world_size;i++)
		{
			for(int j=count;j<count + size;j++)
			{
				int len = allLines[j].length();
				MPI_Send(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(allLines[j].c_str(), allLines[j].length(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
				// cout << allLines[j] << " bsend" << endl;
			}
			count+=size;
		}
		for(int i=1;i<world_size;i++)
		{
			int l;
			MPI_Recv(&l, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(l != -1)
			{
				char *buf = new char[l];
				MPI_Recv(buf, l, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				string final = buf;
				matchedLines.push_back(final);
				total += 1;
			}
		}
		if(allLines.size()%world_size != 0)
		{
			for(int i=1;i<=all;i++)
			{
				int len = allLines[count].length();
				MPI_Send(&len, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
				MPI_Send(allLines[count].c_str(), allLines[count].length(), MPI_CHAR, i, 0, MPI_COMM_WORLD);
				count += 1;
			}
		}
		for(int i=1;i<=all;i++)
		{
			int l;
			MPI_Recv(&l, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if(l != -1)
			{
				char *buf = new char[l];
				MPI_Recv(buf, l, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
				string final = buf;
				string st = final.substr(0, final.size()-2);
				matchedLines.push_back(st);
				total += 1;
			}
		}
	}
	else
	{
		for(int num=0;num<size;num++)
		{
			int l, flag = 0;
			MPI_Recv(&l, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			char *buf = new char[l];
			MPI_Recv(buf, l, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			string newBuf = buf;
			if(reg.search(newBuf, p) == true){
				int len = newBuf.length();	
				MPI_Send(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(newBuf.c_str(), len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			}
			else
			{
				flag = -1;
				MPI_Send(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
		if(world_rank <= all)
		{
			int l, flag = 0;
			MPI_Recv(&l, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			char *buf = new char[l];
			MPI_Recv(buf, l, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			string newBuf = buf;
			if(reg.search(newBuf, p) == true){
				int len = newBuf.length();	
				MPI_Send(&len, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Send(newBuf.c_str(), len, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
			}
			else
			{
				flag = -1;
				MPI_Send(&flag, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
			}
		}
	}
	MPI_Finalize();
	for(int i=0;i<matchedLines.size();i++)
	{
		cout << matchedLines[i] << endl;		
	}
	cout << "Total " << matchedLines.size() << endl;
}