#include<iostream>
#include<string>
#include<unistd.h>
#include<cstring>
#include<sys/wait.h> 
#include<vector>
#include<stdio.h> 
#include<stdlib.h> 
#include<fcntl.h> 
#include<errno.h> 

using namespace std;


vector<vector <string> > getTokens(char str[]);

void normalexec(vector<vector<string>> tokens)
{
	char **arr;
	int p;
	arr=(char **)malloc(1*sizeof(char**));

	for(int i=0;i<tokens[0].size();i++)
		arr[i]=(char*)malloc(100*sizeof(char));
	
	for(int i=0;i<tokens[0].size();i++)
		strcpy(arr[i],tokens[0][i].c_str());

	arr[tokens[0].size()]=NULL;
	cout<<arr[0]<<"\n"<<arr[1]<<"\n"<<arr[2]<<"\n";
	p=fork();
	
	if(p==0)
	{
		if(execvp(arr[0],arr)<0)
		{
			perror("Execution failed");
			exit(1);
		}
		exit(0);
	}
	else
	{
		wait(NULL);
		cout<<"$";
		return;
	}
}

void pipedexecution(vector<vector<string>> tokens)
{
	char **arr;
	int pip[2];
	int p;

	pipe(pip);
	arr=(char **)malloc(1*sizeof(char**));

	for(int i=0;i<tokens[0].size();i++)
		arr[i]=(char*)malloc(100*sizeof(char));
	
	for(int i=0;i<tokens[0].size();i++)
		strcpy(arr[i],tokens[0][i].c_str());

	char **arr1;
	arr1=(char **)malloc(1*sizeof(char**));

	for(int i=0;i<tokens[1].size();i++)
		arr1[i]=(char*)malloc(100*sizeof(char));
	
	for(int i=0;i<tokens[1].size();i++)
		strcpy(arr1[i],tokens[1][i].c_str());

	arr[tokens[0].size()]=NULL;

	arr1[tokens[0].size()]=NULL;
	cout<<arr1[0]<<" "<<arr1[1]<<"\n";
	p=fork();
	
	if(p==0)
	{
		//close(0);
		dup(pip[0]);
		close(pip[0]);
		if(execvp(arr1[0],arr1)<0)
		{
			perror("Execution failed in child");
			exit(1);
		}
	}
	else
	{
		
		close(1);
		dup(pip[1]);
		close(pip[0]);
		if(execvp(arr[0],arr)<0)
		{
			perror("Execution failed in parent");
			exit(1);
		}

		wait(NULL);
	}
}

int main()
{
	string s;
	char str[1000];
	int p;
	cout<<"\n";
	cout<<"$";
	while(getline(cin,s))
	{
		// cout<<s<<"\n";
		strcpy(str,s.c_str());
		vector< vector<string>> tokens=getTokens(str);

		if(tokens.size()>1)
		{
			cout<<"Piped execution\n";
			pipedexecution(tokens);
		}
		else
		{
			cout<<"Normal execution\n";
			normalexec(tokens);
		}
		cout<<"In main method\n$";
	}
}

vector<vector <string> > getTokens(char str[])
{
	vector< vector<string>> vt;
	vector<string> t;
	string temp="";
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]==' '||str[i]=='\t')
		{
			//cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
		}
		else if(str[i]=='|')
		{
			if(temp!="")
				t.push_back(temp);
			temp="";
			vt.push_back(t);
			t.clear();
		}
		else
			temp=temp+str[i];
	}

	if(temp!="")
		t.push_back(temp);

	if(t.size()!=0)
		vt.push_back(t);

	cout<<"size of token is "<<vt.size()<<"\n";
	return vt;
}



/*
int main()
{

	cout<<"String processing\n";
	// char str[]="ls -l|grep -i |cat t|toch -o -i p |ki| uou -o jk | kl -o |";
	string s;
	char str[1000];	
	getline(cin,s);

	strcpy(str,s.c_str());
	//char str[]="ls -l";
	vector< vector<string>> vt;
	vector<string> t;
	string temp="";
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]==' '||str[i]=='\t')
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
		}
		else if(str[i]=='|')
		{
			if(temp!="")
				t.push_back(temp);
			temp="";
			vt.push_back(t);
			t.clear();
		}
		else
		{
			temp=temp+str[i];
			cout<<"temp in or part is "<<temp<<"\n";
		}
	}

	if(temp!="")
		t.push_back(temp);

	if(t.size()!=0)
		vt.push_back(t);


	for(auto x:vt)
	{
		for(auto d:x)
			cout<<d<<" \" \" ";
		cout<<"\n";
	}

}

*/

