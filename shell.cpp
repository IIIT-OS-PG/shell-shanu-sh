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
#include<stdlib.h>

using namespace std;

vector<vector <string> > getTokens(char str[]);
void processarguments(vector<vector <string> > &tokens);
vector <string>  getTok(char str[]);
void init();
void setup();
map<string,string> m;
map<string,string> envi;
void handlealarm(int);
void exporting(string,string);

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
			if(flag)
			{
				//cout<<"Token is "<<tokens[i][k+1]<<endl;
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

int main()
{
	string s;
	char str[1000];
	int p;
	vector< vector<string>> tokens;
	bool flag;
	init();
	//setup();
	

	while(1)
	{
		cout<<envi["USER"]<<"@"<<envi["HOSTNAME"]<<":"<<envi["PS1"];
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
			cout<<"In export\n";
			exporting(tokens[0][1],tokens[0][2]);
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
			{
				cout<<"Provide proper arguments\n";
			}
			else
			{
				string t="";
				for(int i=2;i<tokens[0].size();i++)
					t=t+tokens[0][i]+" ";

				cout<<"IN "<<t<<endl;

				m[tokens[0][1]]=t;
			}
			flag=false;
		}

		
		// if(flag&&tokens.size()>1)
		// {
		// 	processarguments(tokens);
		// 	cout<<"Piped execution\n";
		// 	pipedexecution(tokens);
		// }
		else if(flag)
		{
			processarguments(tokens);
			cout<<"Piping execution\n";
			pipedexecution(tokens);
		}
		//setup();
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
		wait(NULL);
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
				cout<<tokens[i][j]<<" map "<<m[tokens[i][j]]<<endl;
				string t=m[tokens[i][j]];

				vector< string> res=getTok((char*)t.c_str());
				cout<<res[0];

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
			cout<<"While pushing in getTok"<<temp<<"\n";
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
	path=getenv("PATH");
	home=getenv("HOME");
	user=getenv("USER");
	int p;
	char buff[100];
	gethostname(buff,100);
	
	int f1;

	string t(home);
	t=t+"/.temp";
	f1=open(t.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
	chmod(t.c_str(),0666);
	if(f1<0)
	{
		perror("Failed to create file");
	}

	t="";
	t=t+path+"\n"+home+"\n"+user+"\n"+buff+"\n"+"$\n";
	write(f1,t.c_str(),t.length());
	close(f1);

	envi["PATH"]=string(path);
	envi["HOME"]=string(home);
	envi["USER"]=string(user);
	envi["HOSTNAME"]=string(buff);
	envi["PS1"]="$";
	setenv("HOSTNAME",buff,1);
 	setenv("PS1","$",1);
}

void exporting(string data1,string data2)
{
	int f1;
	char buff[1000];
	int n;
	string t,temp="";
	t=envi["HOME"]+"/.temp";
	vector<string> arr;
	cout<<t<<endl;
	f1=open(t.c_str(),O_WRONLY|O_CREAT|O_TRUNC);
	chmod(t.c_str(),0666);
	if(f1<0)
	{
		perror("Failed to create file");
	}

	envi[data1]=data2;
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
