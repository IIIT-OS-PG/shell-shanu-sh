#include<iostream>
#include<string>
#include<unistd.h>
#include<cstring>
#include<sys/wait.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h> 
#include<vector>
#include<map>
#include<stdio.h>
#include<signal.h>
#include<pwd.h>
#include<stdlib.h>

using namespace std;

map<string,string> m;
map <string,string> envi;


vector<vector <string> > getTokens(char str[]);
void processarguments(vector<vector <string> > &tokens);
vector <string>  getTok(char str[]);
void init();
void setup();
void handlealarm(int);
void exporting(string,string);
void pipedexecution(vector<vector<string>>);

int main()
{
	string s;
	char str[1000];
	int p;
	vector< vector<string>> tokens;
	bool flag;
	init();
	cout<<"*************************** Welcome to shell ********************************\n\n";	

	while(1)
	{
		cout<<envi["USER"]<<envi["PS1"];
		flag=true;
		getline(cin,s);
		strcpy(str,s.c_str());
		tokens=getTokens(str);

		if(tokens.size()==0)
			continue;
		if(flag&&tokens[0][0].compare("exit")==0)
		{
			cout<<"bye...\n";
			exit(1);
		}

		if(flag&&tokens[0][0].compare("cd")==0)
		{
			if(tokens[0].size()>2)
				cout<<"Too many arguments\n";
			else if(tokens[0].size()==1)
				flag=false;
			else
			{
				if(tokens[0][1].compare("~")==0)
					chdir(envi["HOME"].c_str());
			 	else if(chdir(tokens[0][1].c_str())<0)
					cout<<"Error \n";
				flag=false;
			}
		}

		if(flag&&tokens[0][0].compare("export")==0)
		{
			string te="";
			for(int i=2;i<tokens[0].size();i++)
			{
				te=te+tokens[0][i];
				if(i!=tokens[0].size()-1)
					te=te+" ";
			}
			exporting(tokens[0][1],te);
			flag=false;
		}

		if(flag&&tokens[0][0].compare("alarm")==0)
		{
			int val=stoi(tokens[0][1]);
			handlealarm(val);
			flag=false;
		}
		
		if(flag&&tokens[0][0].compare("alias")==0)
		{
			if(tokens[0].size()<3)
				cout<<"Provide proper arguments\n";
			else
			{
				string t="";
				for(int i=2;i<tokens[0].size();i++)
					t=t+tokens[0][i]+" ";

				m[tokens[0][1]]=t;
			}
			flag=false;
		}
		else if(flag)
		{
			processarguments(tokens);
			pipedexecution(tokens);
		}
	}
}

static void sig_alarm(int signo)
{
	cout<<"Alarm done\n";
}

void handlealarm(int val)
{
	int p=fork();
	if(p>0)
	{
		
	}
	else
	{
		signal(SIGALRM,sig_alarm);
		alarm(val);
		pause();

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
				string t=m[tokens[i][j]];
				vector< string> res=getTok((char*)t.c_str());
				
				tokens[i][j]=res[0];
				for(int k=1;k<res.size();k++)
					tokens[i].insert((tokens[i].begin()+j+k),res[k]);
			}
		}
	}
}


vector <string>  getTok(char str[])
{
	vector<string> t;
	string temp="";
	for(int i=0;i<strlen(str);i++)
	{
		if(str[i]==' '||str[i]=='\t'||str[i]=='='||str[i]=='\''||str[i]=='\"')
		{
			if(temp!="")
				t.push_back(temp);
			temp="";
		}
		else
			temp=temp+str[i];
	}

	if(temp!="")
		t.push_back(temp);

	return t;
}

void init()
{
	char *path,*home,*user;
	struct passwd *pw;
	pw=getpwuid(getuid());
	path=getenv("PATH");
	home=pw->pw_dir;
	user=pw->pw_name;
	int p;
	char buff[100];
	gethostname(buff,100);
	
	int f1;

	envi["PATH"]=string(path);
	envi["HOME"]=string(home);
	envi["USER"]=string(user);
	envi["HOSTNAME"]=string(buff);
	envi["PS1"]="$";
	setenv("HOSTNAME",buff,1);
 	setenv("PS1","$",1);

	string t(home);
	t=t+"/.temp";
	f1=open(t.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
	chmod(t.c_str(),0666);
	if(f1<0)
		perror("Failed to create file");

	for(auto it=envi.begin();it!=envi.end();it++)
		t=t+it->first+"="+it->second+"\n";
	write(f1,t.c_str(),t.length());
	close(f1);

 	vector<vector<string> > tp;
 	vector<string> t1;
 	t1.push_back("clear");
 	tp.push_back(t1);
 	pipedexecution(tp);
}

void exporting(string data1,string data2)
{
	int f1;
	char buff[1000];
	int n;
	string t="",temp="";
	t=envi["HOME"]+"/.temp";
	vector<string> arr;

	if(f1<0)
		perror("Failed to create file");

	envi[data1]=data2;
	f1=open(t.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
	chmod(t.c_str(),0666);
	t="";
	for(auto it=envi.begin();it!=envi.end();it++)
		t=t+it->first+"="+it->second+"\n";

	write(f1,t.c_str(),t.length());
	close(f1);
	setenv(data1.c_str(),data2.c_str(),1);
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
		if(str[i]==' '||str[i]=='\t'||str[i]=='=')
		{
			//cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
		}
		else if(str[i]=='>'&&str[i+1]=='>')
		{
			//cout<<"While pushing "<<temp<<"\n";
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
			//cout<<"While pushing "<<temp<<"\n";
			if(temp!="")
				t.push_back(temp);
			temp="";
			t.push_back(">");
		}

		else if(str[i]=='|' && flag)
		{
			//cout<<"While pushing "<<temp<<"\n";
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
	//cout<<"While pushing "<<temp<<"\n";
	if(temp!="")
		t.push_back(temp);

	if(t.size()!=0)
		vt.push_back(t);
	return vt;
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
		for(k=0;k<tokens[i].size();k++)
		{
			//cout<<tokens[i][k]<<" ";
			if((tokens[i][k].compare(">")==0)||(tokens[i][k].compare(">>")==0))
			{
				flag=true;
				break;
			}
			arr[k]=(char*)tokens[i][k].c_str();
			//cout<<arr[k]<<endl;
		}
		arr[k]=NULL;
		p=fork();
		if(p==0)
		{
			if(flag)
			{
				if(tokens[i][k].compare(">")==0)
					f1=open(tokens[i][k+1].c_str(),O_WRONLY|O_CREAT|O_TRUNC);
				else
					f1=open(tokens[i][k+1].c_str(),O_WRONLY);
					if(f1<0)
						f1=open(tokens[i][k+1].c_str(),O_WRONLY|O_CREAT|O_APPEND);
				chmod(tokens[i][k+1].c_str(),0666);
				dup2(f1,1);
				dup2(u,0);
				close(pip[0]);
				close(pip[1]);

			}
			else
			{
				if(i!=0)
					dup2(u,0);
				//close(pip[0]);
							
				if(i!=tokens.size()-1)
					dup2(pip[1],1);

				close(pip[0]);
				close(pip[1]);
			}
		
			if(execvp(arr[0],arr)<0)
				perror("Execution failed");

			close(f1);	
		}
		else
		{
			wait(NULL);
			close(pip[1]);
			u=pip[0];
		}	
	}
}

