#include<iostream>
#include<string>
#include<unistd.h>
#include<cstring>
#include<sys/wait.h> 
#include<vector>

using namespace std;


vector<vector <string> > getTokens(char str[]);

void normalexec(vector<vector<string>> tokens)
{
	char *arr[1000];
	int p;
	
	for(int k=0;k<tokens[0].size();k++)
		arr[k]=(char*)tokens[0][k].c_str();
	arr[tokens[0].size()]=NULL;
	p=fork();
	
	if(p==0)
	{
		cout<<"Child\n";	
		if(execvp(arr[0],arr)<0)
		{
			perror("Execution failed");
			exit(1);
		}
		cout<<"Child\n";	
	}
	else
	{
		wait(NULL);
		cout<<"oipipi$";
		
	}
}

void pipedexecution(vector<vector<string>> tokens)
{
	char *arr[100];
	int pip[2];
	int p;

	pipe(pip);

	int fd=0;

	int u=0;
	for(int i=0;i<tokens.size();i++)
	{
		
		for(int k=0;k<tokens[i].size();k++){
			arr[k]=(char*)tokens[i][k].c_str();
			cout<<tokens[i][k]<<endl;
		}
		
		arr[tokens[i].size()]=NULL;
		
		p=fork();


		if(p==0)
		{
			cout<<"Child begin\n";
			
				dup2(u,0);
			
			if(i==tokens.size()-1)
				dup2(1,1);
			else
				dup2(pip[1],1);

			close(pip[0]);
			close(pip[1]);

		
			if(execvp(arr[0],arr)<0)
			{
				perror("Execution failed");
				
			}
			exit(0);
			cout<<"Child exit\n";	
		}
		else
		{
			wait(NULL);

			close(pip[1]);
			u=pip[0];
		}
		
	}
	
}

int main()
{
	string s;
	char str[1000];
	int p;
	cout<<"\n";
	cout<<"$";
	while(1)
	{
		// cout<<s<<"\n";
		getline(cin,s);
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
