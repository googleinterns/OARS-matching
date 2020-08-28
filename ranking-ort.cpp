#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <set>
#include <cmath>
#include <thread>
#include <mutex>
#include <string>

#include "ortools/linear_solver/linear_solver.h"

using namespace std;

const int MN=100;
const int noRun=400;

int maxn=10,maxm=20,maxd=10;

const double eps=1e-6;

double wc=2;

mutex wcl,outl;

int ranking(const int &n,const int &m,const vector<int> &d, const vector<int>* adj,bool resample=true)
{
	int available[MN];
	int rnk[MN];

	int res=0;
	for (int i=0;i<n;i++) available[i]=0,rnk[i]=rand();
	for (int i=0;i<m;i++)
	{
		if (resample) for (int j=0;j<n;j++) if ((i%d[j])==0) rnk[j]=rand();
		int bst=-1, rb;
		for (auto & x:adj[i]) if (available[x]<=i)
			if (bst==-1 || (rnk[x]<rb))
			{
				bst=x;
				rb=rnk[x];
			}
		if (bst==-1) continue;
		res++;
		available[bst]=i+d[bst];
	}
	return res;
}

int randommatching(const int &n,const int &m,const vector<int> &d, const vector<int>* adj,int start=0,int end=-1)
{
	if (end==-1) end=m;
	int available[MN];
	int res=0;
	for (int i=0;i<n;i++) available[i]=0;
	for (int i=start;i<end;i++)
	{
		vector<int> val;
		for (int j=adj[i].size()-1;j>=0;j--) if (available[adj[i][j]]<=i)
			val.push_back(adj[i][j]);
		if (val.size()==0) continue;
		int t=rand()%val.size();
		res++;
		available[val[t]]=i+d[val[t]];
	}
	return res;

}



double opt(const int &n,const int &m,const vector<int> &d, const vector<int>* adj,int start=0,int end=-1)
{
	if (end==-1) end=m;
	typedef operations_research::MPVariable* lpvars;
	typedef operations_research::MPConstraint* lpconst;
	typedef operations_research::MPObjective* lpobj;

	operations_research::MPSolver solver("reusable", operations_research::MPSolver::GLOP_LINEAR_PROGRAMMING);
	vector<vector<lpvars> > vars;
	vector<vector<pair<int,lpvars> > > var_other;
	vars.resize(m);
	var_other.resize(n);
	for (int i=start;i<end;i++)
		for (auto & x:adj[i])
		{
			lpvars temp=solver.MakeNumVar(0.0,1,"x_"+to_string(i)+","+to_string(x));
			vars[i].push_back(temp);
			var_other[x].push_back({i,temp});
		}

	for (int i=start;i<end;i++)
	{
		lpconst temp=solver.MakeRowConstraint(0,1,"beta_"+to_string(i));
		for (int j=0;j<adj[i].size();j++)
			temp->SetCoefficient(vars[i][j],1);
	}
	for (int i=0;i<n;i++)
		for (int t=start;t<end;t++)
		{
			lpconst temp=solver.MakeRowConstraint(0,1,"alpha_"+to_string(i)+","+to_string(t));
			for (auto & x:var_other[i]) if (x.first>=t && x.first<t+d[i])
				temp->SetCoefficient(x.second,1);
		}

	lpobj obj=solver.MutableObjective();
	for (int i=start;i<end;i++)
		for (int j=0;j<adj[i].size();j++)
			obj->SetCoefficient(vars[i][j],1);
	obj->SetMaximization();

	solver.Solve();
	return obj->Value();
}


void build(int &n,int &m, vector<int> &d, vector<int>* adj, bool different=true)
{
	n=rand()%maxn+1,m=rand()%maxm+1;
	
	d.clear();
	int _=rand()%maxd+1;
	for (int i=0;i<n;i++) d.push_back(different?rand()%maxd+1:_);

	for (int i=0;i<MN;i++) adj[i].clear();
	int e=rand()%(m*n)+1;
	set<pair<int,int> > edges;
	vector<int> online;
	for (int i=0;i<m;i++) online.push_back(i);
	random_shuffle(online.begin(),online.end());
	for (int i:online)
	{
		int t=rand()%min(e+1,n+1);
		e-=t;
		for (int j=0;j<t;j++)
		{
			int f=i,s=rand()%n;
			if (edges.find({f,s})!=edges.end())
			{
				j--;
				continue;
			}
			edges.insert({f,s});
			adj[f].push_back(s);
		}
	}
	for (int i=0;i<e;i++)
	{
		int f=rand()%m,s=rand()%n;
		if (edges.find({f,s})!=edges.end())
		{
			i--;
			continue;
		}
		edges.insert({f,s});
		adj[f].push_back(s);
	}
}

void build_service(int &n,int &m, vector<int> &d, vector<int> *adj,bool different=true)
{
	n=rand()%maxn+1,m=rand()%maxm+1;
	d.clear();
	int _=rand()%maxd+1;
	for (int i=0;i<n;i++) d.push_back(different?rand()%maxd+1:_);
	sort(d.begin(),d.end());
	for (int i=0;i<MN;i++) adj[i].clear();
	for (int i=0;i<m;i++)
	{
		int t=rand()%(n+1);
		for (int j=0;j<t;j++) 
			adj[i].push_back(j);
	}
}

void run()
{
	vector<int> adj[MN];
	int n,m;
	vector<int> d;

	while (true)
	{
		build(n,m,d,adj);
		double res=opt(n,m,d,adj);
		double r=0;

		for (int i=0;i<noRun;i++)
			r+=ranking(n,m,d,adj);
		r/=noRun;


		bool out=false;
		wcl.lock();
		if (wc>r/res)
			wc=r/res, out=true;
		wcl.unlock();
		if (out)
		{
			outl.lock();
			cerr<<"--------------------------"<<endl;
			cerr<<"n= "<<n<<", m= "<<m<<endl;
			cerr<<"d : ";
			for (auto & x:d)
				cerr<<x<<" ";
			cerr<<endl;
			cerr<<"Edges:"<<endl;
			for (int i=0;i<m;i++) if (adj[i].size()>0)
			{
				cerr<<i<<" : ";
				for (auto & x:adj[i])
					cerr<<x<<" ";
				cerr<<endl;
			}
			cerr<<"Alg= "<<r<<" , OPT= "<<res<<" , ratio = "<<r/res<<endl;
			outl.unlock();
		}

	}
}

int main()
{
	srand(time(0));
	vector<thread> threads;
	for (int i=0;i<32;i++)
		threads.push_back(thread(run));
	for (auto & x:threads)
		x.join();
	return 0;
}
