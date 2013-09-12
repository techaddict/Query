#ifndef __CLUSTERING_H_
#define __CLUSTERING_H_
 
#include<iostream>
#include<algorithm>
#include<vector>
#include<map>
#include<cstdio>
#include<cstring>
#include<cmath>
#include "input.h"
#include "suffix.h"
using namespace std;
 
//Initializations of variables
 
class Clusters
{
    int Tabs,Tfreq;
    char *file;
    double Trel,Tdiameter;
    map<string,int> querynode,urlnode,querycluster,clickcount;
    map<string,map<string,double> >clickgraph;
    map<int,vector<string> >clusters;
    map<int,map<string,double> >centroids;
    map<int,string>represent;
    map<string,vector<int> >dimension;
    vector<vector<int> >session;
    map<vector<int>,int> concept;
    public:
    Clusters(char *path)
    {
        Tabs=5;Tfreq=5;
        Trel=0.1;
        Tdiameter=1.0;
        file=new char[256];
        strcpy(file,path);
    }
    double find(vector<int> &v,int x);
    double getdistance(map<string,double> m,map<string,double> n);
    void prunegraph();
    map<string,double> updatecentroid(vector<string> v,map<string,double> n);
    int checkcluster(vector<string> &cluster);
    void makecluster();
    void makegraph();
    void printgraph();
    void printcentroid();
    void printdimension();
    void representcluster();
    void getsessions();
    void printsessions();
    void frequentconcepts();
    void pruneconcepts();
    void printquerycluster();
    void printconcepts();
    void buildsuffixtree();
    void suggestions();
};
 
void Clusters::makegraph()
{
    char *str,*part;
    int flag,count=0;
    string query,url;
    Input inp(file);
    str=new char[800];
    part=new char[100];
    while(1)
    {
        str=inp.getline();
        if(str==NULL)
            break;
        part=strtok(str,"\t");
        flag=1;
        while(part!=NULL && count )
        {
            part=strtok(NULL,"\t");
            if(flag==1)
            {
                query=part;
                querynode[query]+=1;
            }
            if(flag==4)
            {
                url=part;
                urlnode[url]+=1;
                clickgraph[query][url]+=1;
                clickcount[query]+=1;
            }
            flag++;
        }
        count++;
    }
}
 
double Clusters::find(vector<int> &v,int x)
{
    int size=v.size(),i;
    for(i=0;i<size;i++)
        if(v[i]==x)
            return 1;
    return 0;
}
double Clusters::getdistance(map<string,double> m,map<string,double> n)
{
    double dist=0.0;
    map<string,double> temp=m;
    map<string,double>::iterator iter;
    for(iter=n.begin();iter!=n.end();iter++)
        temp[(*iter).first]+=0.0;
    for(iter=temp.begin();iter!=temp.end();iter++)
        dist+=pow(((*iter).second-n[(*iter).first]),2); 
    return sqrt(dist);
}
 
map<string,double> Clusters::updatecentroid(vector<string> v,map<string,double> n)
{
    int i=0,size=v.size();
    double mod=0.0;
    string query;
    map<string,double> temp;
    map<string,double>::iterator iter;
    for(i=0;i<size;i++)
    {
        query=v[i];
        for(iter=clickgraph[query].begin();iter!=clickgraph[query].end();iter++)
            temp[(*iter).first]+=(*iter).second;
    }
    for(iter=n.begin();iter!=n.end();iter++)
        temp[(*iter).first]+=(*iter).second;
    for(iter=temp.begin();iter!=temp.end();iter++)
    {
        (*iter).second/=double(size+1.0);
        mod+=pow((*iter).second,2);
    }
    mod=sqrt(mod);
    for(iter=temp.begin();iter!=temp.end();iter++)
        (*iter).second/=mod;
    return temp;
}
 
int Clusters::checkcluster(vector<string> &cluster)
{
    double dist=0.0;
    int size=cluster.size(),i,j;
    for(i=0;i<size-1;i++)
        for(j=i+1;j<size;j++)
            dist+=2*pow(getdistance(clickgraph[cluster[i]],clickgraph[cluster[j]]),2);
    dist/=double(size*(size-1));
    dist=sqrt(dist);
    if(dist>Tdiameter)
        return 0;
    return 1;
}
 
void Clusters::prunegraph()
{
    string query;
    double total=0.0;
    vector<string>eraseurl,erasequery;
    vector<string>::iterator iter;
    map<string,map<string,double> >::iterator it;
    map<string,double>::iterator it1;
 
    for(it=clickgraph.begin();it!=clickgraph.end();it++)
    {
        query=(*it).first;
        total=0.0;
        eraseurl.clear();
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
        {
            if((*it1).second<Tabs)
                eraseurl.push_back((*it1).first);
            else
                total+=(*it1).second;
        }
        for(iter=eraseurl.begin();iter!=eraseurl.end();iter++)
            (*it).second.erase(*iter);
        eraseurl.clear();
        if(total==0.0)
        {
            erasequery.push_back((*it).first);
            continue;
        }
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
        {
            if(((*it1).second/total)<Trel)
                eraseurl.push_back((*it1).first);
        }
        for(iter=eraseurl.begin();iter!=eraseurl.end();iter++)
            (*it).second.erase(*iter);
        total=0.0;
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
            total+=pow((*it1).second,2);
        if(total==0.0)
        {
            erasequery.push_back((*it).first);
            continue;
        }
        total=sqrt(total);
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
            (*it1).second/=total;
    }
    for(iter=erasequery.begin();iter!=erasequery.end();iter++)
        clickgraph.erase(*iter);
}
void Clusters::makecluster()
{
    int t=0,i,size,clustercount=0;
    double min=0.0,temp=0.0;
    string query,url;
    vector<string>tmpcluster,tmp;
    map<int,int>candidates;
    map<int,int>::iterator iter;
    map<string,map<string,double> >::iterator it;
    map<string,double>::iterator it1;
 
    for(it=clickgraph.begin();it!=clickgraph.end();it++)
    {
        query=(*it).first;
        candidates.clear();
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
        {
            url=(*it1).first;
            size=dimension[url].size();
            for(i=0;i<size;i++)
                candidates[dimension[url][i]]=1;
        }
        min=99999;
        for(iter=candidates.begin();iter!=candidates.end();iter++)
        {
            temp=getdistance(centroids[(*iter).first],(*it).second);
            if(temp<min)
            {
                min=temp;
                t=(*iter).first;
            }
        }
        if(min<2)
        {
            tmpcluster=clusters[t];
            tmpcluster.push_back(query);
        }
        if(min<2 && checkcluster(tmpcluster))
        {
            centroids[t]=updatecentroid(clusters[t],(*it).second);
            clusters[t]=tmpcluster;
            for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
            {
                url=(*it1).first;
                if(!find(dimension[url],t))
                    dimension[url].push_back(t);
            }
            if(clickcount[represent[t]]<clickcount[query])
                represent[t]=query;
            querycluster[query]=t;
        }
        else
        {
            tmp.clear();
            tmp.push_back(query);
            clusters[clustercount]=tmp;
            centroids[clustercount]=(*it).second;
            for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
            {
                url=(*it1).first;
                dimension[url].push_back(clustercount);
            }
            querycluster[query]=clustercount;
            represent[clustercount]=query;
            clustercount++;
        }
    }
}
void Clusters::printgraph()
{
    map<string,map<string,double> >::iterator it;
    map<string,double>::iterator it1;
    for(it=clickgraph.begin();it!=clickgraph.end();it++)
    {
        cout << "query "<<(*it).first<<endl;
        for(it1=(*it).second.begin();it1!=(*it).second.end();it1++)
            cout << (*it1).first<<" : "<<(*it1).second<<endl;
    }
    exit(0);    
 
 
}
void Clusters::printcentroid()
{
    cout <<"Printing Centroid Details\n";
    cout <<"=========================\n";
    map<int,map<string,double> >::iterator cent;
    map<string,double>::iterator doubit;
    for(cent=centroids.begin();cent!=centroids.end();cent++)
    {
        cout <<"centroid "<<(*cent).first <<"\n";
        for(doubit=(*cent).second.begin();doubit!=(*cent).second.end();doubit++)
            cout << (*doubit).first<<":"<<(*doubit).second<<endl;
    }
}
void Clusters::printdimension()
{
    cout <<"Printing Dimension Details\n";
    cout <<"==========================\n";
    int i;
    map<string,vector<int> >::iterator dimn;
    for(dimn=dimension.begin();dimn!=dimension.end();dimn++)
    {
        cout<<(*dimn).first<<" :: ";
        for(i=0;i<(*dimn).second.size();i++)
            cout << (*dimn).second[i]<<" ";
        cout<<endl;
    }
}
//----------------------------------------------Frequent concept mining Part -----------------------------------------------------------------------//
void Clusters::getsessions()
{
    char *str,*part;
    char y[10],m[10],d[10],h[10],mi[10],s[10];
    int flag,count=0,click=0;
    int prevd=0,prevm=0,prevy=0,prevt=0,curd,curm,cury,curt,hh,mm,ss;
    vector<int> tempsession;
    string query,url,prevquery,prevu="",curu;
    Input inp(file);
    str=new char[800];
    part=new char[100];
    str=inp.getline();
    while(1)
    {
        str=inp.getline();
        if(str==NULL)
            break;
        part=strtok(str,"\t");
        if(part!=NULL)
            curu=part;
        flag=1;
        click=0;
        while(1)
        {
            part=strtok(NULL,"\t");
            if(part==NULL)
                break;
            if(flag==1)
                query=part;
            else if(flag==2)
            {
                sscanf(part,"%s-%s-%s %s:%s:%s",y,m,d,h,mi,s);
                cury=atoi(y);
                curm=atoi(m);
                curd=atoi(d);
                hh=atoi(h);
                mm=atoi(mi);
                ss=atoi(s);
                curt=hh*3600+mm*60+ss;
            }
            else if(flag==4)
                click=1;
            flag++;
        }
        //checking whether the query exists in the clickgraph or not
        if(clickgraph.find(query)==clickgraph.end())
            continue;
        if(curu!=prevu)
        {
            if(tempsession.size())
            {
                session.push_back(tempsession);
                cout <<"---------\n";
            }
            tempsession.clear();    
            prevd=prevm=prevy=prevt=0;
            prevu=curu;
            prevquery="";
        }
        if(!click) 
        {
            if(cury==prevy && curm==prevm && curd==prevd && curt-prevt<=1800)
            {
                if(prevquery!=query)
                {   
                    tempsession.push_back(querycluster[query]);
                    cout <<query<<endl;
                }
            }
            else
            {
                if(tempsession.size())
                {
                    session.push_back(tempsession);
                    cout <<"---------\n";
                }
                tempsession.clear();    
                tempsession.push_back(querycluster[query]);
                cout <<query<<endl;
            }
            prevd=curd;
            prevm=curm;
            prevy=cury;
            prevt=curt;
            prevu=curu;
            prevquery=query;
        }
    }
    if(tempsession.size())
        session.push_back(tempsession);
}
void Clusters::printquerycluster()
{
    cout <<"Printing queries and clusters"<<endl;
    map<string,int>::iterator it;
    for(it=querycluster.begin();it!=querycluster.end();it++)
        cout<<(*it).first<<" "<<(*it).second<<endl;
    cout<<"=======================\n";
}
void Clusters::representcluster()
{
    map<int,string>::iterator it;
    cout<<"Representative queries of the clusters\n";
    cout<<"======================================\n";
    for(it=represent.begin();it!=represent.end();it++)
        cout<<(*it).first<<" "<<(*it).second<<endl;
    cout<<"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n";
}
void Clusters::printsessions()
{
    vector<vector<int> >::iterator it;
    vector<int>::iterator it1;
//  exit(0);
    for(it=session.begin();it!=session.end();it++)
    {
        for(it1=(*it).begin();it1!=(*it).end();it1++)
            cout<<(*it1)<<" ";
        cout<<endl;
    }
}
void Clusters::frequentconcepts()
{
    int i=0,j=0,k=0,count,l,m,n,prev;
    vector<int>temp;
    m=session.size();
    for(i=0;i<m;i++)
    {
        n=session[i].size();
        for(j=0;j<n;j++)
        {
            temp.clear();
            temp.push_back(session[i][j]);
            count=1;
            prev=session[i][j];
            for(k=j+1;k<n &&count<4;k++)
            {
                if(session[i][k]!=prev)
                {
                    temp.push_back(session[i][k]);
                    count++;
                }
                prev=session[i][k];
                concept[temp]+=1;
            }
        }
    }
 
}
void Clusters::pruneconcepts()
{
    int i=0;
    map<vector<int>,int>::iterator it;
    vector<vector<int> >temp;
    for(it=concept.begin();it!=concept.end();it++)
        if((*it).second<Tfreq)
            temp.push_back((*it).first);
    for(i=0;i<temp.size();i++)
        concept.erase(temp[i]);
}
void Clusters::printconcepts()
{
    map<vector<int>,int>::iterator it;
    vector<int> v;
    int i,size;
    cout<<"Frequent Concepts\n";
    cout<<"=================\n";
    for(it=concept.begin();it!=concept.end();it++)
    {
        v=(*it).first;
        size=v.size();
        for(i=0;i<size;i++)
            cout<<v[i]<<" ";
        cout<<" : "<<(*it).second;
        cout <<endl;
    }
}
void Clusters::buildsuffixtree()
{
    int flag=1;
    map<int,int> ans;
    map<int,int>::iterator it;
    SuffixTree s=SuffixTree();
    s.insert(concept);
    char str[800];
    string query;
    vector<int> v;
     
     
    while(flag)
    {
        v.clear();
        cout << "-------------" << endl;
        cout << "Enter queries:" << endl;
        cout << "-------------" << endl;
        while(1)
        {
            cin.getline(str,800);
            if(cin.eof())
            {
                flag=0;
                break;
            }
            else if(!strcmp(str,""))
                break;
            query=str;
            if(querycluster.find(query)!=querycluster.end())
                v.push_back(querycluster[query]);
        }
        if(v.size()==0)
        {
            cout<<"No available Suggestions \n";
            continue;
        }
        ans=s.suggestions(v);
        if(ans.begin()==ans.end())
        {
            cout<<"No available Suggestions \n";
            continue;
        }
        cout << "Available Suggestions\n";
        cout << "---------------------" << endl;
        for(it=ans.begin();it!=ans.end();it++)
            cout<<represent[(*it).first]<<endl;
    }
}
#endif
