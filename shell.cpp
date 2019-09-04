#include<iostream>
#include<string>
#include<unistd.h>
#include<cstring>
#include<sys/wait.h> 
#include<vector>

using namespace std;

vector<vector <string> > getTokens(char str[]);


int main()
{
	string s;
	char str[1000];
	char temp[1000]="/bin/";
	int p;
	cout<<"\n";
	cout<<"$";
	while(getline(cin,s))
	{
		// cout<<s<<"\n";
		strcpy(str,s.c_str());
		vector< vector<string>> tokens=getTokens(str);

		

		char **arr;
		cout<<tokens[0].size()<<"\n";
		arr=(char **)malloc(tokens[0].size()+1*sizeof(char**));

		for(int i=0;i<tokens[0].size();i++)
			arr[i]=(char*)malloc(2*sizeof(char));
		
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
		}
		else
		{
			wait(NULL);
			cout<<"$";
		}

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
			temp=temp+str[i];
	}

	if(temp!="")
		t.push_back(temp);

	if(t.size()!=0)
		vt.push_back(t);

	return vt;
}



/*
int main()
{

	cout<<"String processing\n";
	// char str[]="ls -l|grep -i |cat t|toch -o -i p |ki| uou -o jk | kl -o |";
	char str[]="ls -l";
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
			cout<<d<<" ";
		cout<<"\n";
	}

}

*/

// int main()
// {

// 	char *str[100];

// 	str[0][0]='l';
// 	str[0][1]='s';
// 	str[1][0]='-';
// 	str[1][1]='l';
// 	str[2][0]='-';
// 	str[2][1]='a';

// 	if(execvp(str[0],str)<0)
// 		perror();
// }