#include<iostream>
#include<string>
#include<unistd.h>
#include<cstring>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h> 
#include<vector>
#include<dirent.h>
#include<map>

using namespace std;


vector<vector <string> > getTokens(char str[]);
vector <string>  getTok(char str[]);
void processarguments(vector<vector <string> > &tokens);
map<string,string> m;

void normalexec(vector<vector<string>> tokens)
{
	char *arr[1000];
	int p;
	bool flag=false;
	int k;
	int pip[2];
	for(k=0;k<tokens[0].size();k++)
	{
		if((tokens[0][k].compare(">")==0)||(tokens[0][k].compare(">>")==0))
		{
			flag=true;
			break;
		}
		arr[k]=(char*)tokens[0][k].c_str();
		//cout<<arr[k]<<endl;
	}
	arr[k]=NULL;

	p=fork();
	
	if(p==0)
	{
		cout<<"Child\n";
		int f1;
		if(flag)
		{
			pipe(pip);

			//cout<<"TOken is "<<tokens[0][k+1]<<endl;
			if(tokens[0][k].compare(">")==0)
				f1=open(tokens[0][k+1].c_str(),O_WRONLY|O_CREAT|O_TRUNC);
			else
				f1=open(tokens[0][k+1].c_str(),O_WRONLY|O_APPEND);
			chmod(tokens[0][k+1].c_str(),0666);
			dup2(f1,1);
			close(pip[0]);
			close(pip[1]);

		}	
		if(execvp(arr[0],arr)<0)
		{
			perror("Execution failed");
			exit(1);
		}
		close(f1);
		cout<<"Child\n";	
	}
	else
	{
		wait(NULL);
	}
}

void pipedexecution(vector<vector<string>> tokens)
{
	char *arr[100];
	int pip[2];
	int p;

	int u=0,f1;
	int k;
	bool flag=false;
	for(int i=0;i<tokens.size();i++)
	{
		pipe(pip);
		for(k=0;k<tokens[i].size();k++){

			cout<<tokens[i][k]<<" ";
			if((tokens[i][k].compare(">")==0)||(tokens[i][k].compare(">>")==0))
			{
				flag=true;
				break;
			}

			arr[k]=(char*)tokens[i][k].c_str();
			cout<<arr[k]<<endl;
		}
		
		arr[k]=NULL;
		
		p=fork();


		if(p==0)
		{
			cout<<"Child begin\n";
			//close(pip[1]);
			if(flag)
			{
				
				//cout<<"Token is "<<tokens[i][k+1]<<endl;
				if(tokens[i][k].compare(">")==0)
					f1=open(tokens[i][k+1].c_str(),O_WRONLY|O_CREAT|O_TRUNC);
				else
					f1=open(tokens[i][k+1].c_str(),O_WRONLY|O_APPEND);
				chmod(tokens[i][k+1].c_str(),0666);
				dup2(f1,1);
				dup2(u,0);
				close(pip[0]);
				close(pip[1]);

			}
			else
			{
				dup2(u,0);
				//close(pip[0]);
							
				if(i!=tokens.size()-1)
					dup2(pip[1],1);

				close(pip[0]);
				close(pip[1]);
			}
		
			if(execvp(arr[0],arr)<0)
			{
				perror("Execution failed");
				
			}
			close(f1);
			//exit(0);
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

	bool flag;
	while(1)
	{
		// cout<<s<<"\n";
		getline(cin,s);
		flag=true;
		strcpy(str,s.c_str());
		vector< vector<string>> tokens=getTokens(str);

		if(tokens[0][0].compare("exit")==0)
		{
			cout<<"bye...\n";
			exit(1);
		}
		
		if(tokens[0][0].compare("alias")==0)
		{
			if(tokens[0].size()<3)
			{
				cout<<"Provide proper arguments\n";
			}
			else
			{
				string t="";
				for(int i=2;i<tokens[0].size();i++)
					t=t+tokens[0][i]+" ";

				//cout<<"IN "<<t<<endl;
				m[tokens[0][1]]=t;
			}
			flag=false;
		}

		
		if(tokens.size()>1&&flag)
		{
			processarguments(tokens);
			cout<<"Piped execution\n";
			pipedexecution(tokens);
		}
		else if(flag)
		{
			processarguments(tokens);
			cout<<"Normal execution\n";
			normalexec(tokens);
		}
		cout<<"In main method\n$";
	}
}

void processarguments(vector<vector <string> > &tokens)
{

	int i,j;
	
	for(i=0;i<tokens.size();i++)
	{
		for(j=0;j<tokens[i].size();j++)
		{
			if(m.find(tokens[i][j])!=m.end())
			{
				cout<<tokens[i][j]<<" map "<<m[tokens[i][j]]<<endl;
				string t=m[tokens[i][j]];

				vector< string> res=getTok((char*)t.c_str());
				cout<<res[0];

				tokens[i][0]=res[0];
				for(int k=1;k<res[0].size();k++)
					tokens[i].insert((tokens[i].begin()+j+k),res[k]);

				//cout<<tokens[i][j];
			}
		}
	}
}


vector <string> getTok(char str[])
{
	vector< vector<string>> vt;
	vector<string> t;
	string temp="";
	bool flag=true;
	for(int i=0;i<strlen(str);i++)
	{

		if(str[i]==' '||str[i]=='\t'||str[i]=='='||str[i]=='\''||str[i]=='\"')
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
		}

		else if(flag)
		{
			temp=temp+str[i];
		}
	}
	cout<<"While pushing "<<temp<<"\n";
	if(temp!="")
		t.push_back(temp);

	return t;
}


vector<vector <string> > getTokens(char str[])
{
	vector< vector<string>> vt;
	vector<string> t;
	string temp="";
	bool flag=true;
	for(int i=0;i<strlen(str);i++)
	{
		flag=true;
		if(str[i]==' '||str[i]=='\t')
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
		}
		else if(str[i]=='>'&&str[i+1]=='>')
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
			t.push_back(">>");
			i++;
			flag=false;
			continue;
		}
		else if(str[i]=='>'&&flag)
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
			t.push_back(">");
		}

		else if(str[i]=='|' && flag)
		{
			cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
			vt.push_back(t);
			t.clear();
		}
		else if(flag)
		{
			temp=temp+str[i];
			// cout<<"temp in or part is "<<temp<<"\n";
		}
	}
	cout<<"While pushing "<<temp<<"\n";
	if(temp!="")
		t.push_back(temp);

	if(t.size()!=0)
		vt.push_back(t);

	return vt;
}
