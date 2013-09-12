#ifndef __SUFFIX_H_
#define __SUFFIX_H_
#include<iostream>
#include<map>
#include<vector>
using namespace std;
class SuffixTree
{
    int Tk;
    vector<int> concept;
    map<int,int>Candlist;
    map<int,SuffixTree*> child;
    public:
    SuffixTree(){Tk=10;}
    SuffixTree(vector<int> v)
    {
        Tk=10;
        concept=v;
    }
    void insert(map<vector<int>,int >v);
    SuffixTree *findnode(vector<int> v,SuffixTree *t);
    map<int,int> suggestions(vector<int> v);
    void printTree();
};
void SuffixTree::insert(map<vector<int>,int > v)
{
    int i,size,min,count;
    map<vector<int>,int>::iterator iter;
    map<int,int>::iterator it,t;
    vector<int> temp,temp1;
    SuffixTree *cn;
    for(iter=v.begin();iter!=v.end();iter++)
    {
        temp=(*iter).first;
        temp1=temp;
        temp1.pop_back();
        cn=findnode(temp1,this);
        count=0;
        min=99999999;
        for(it=cn->Candlist.begin();it!=cn->Candlist.end();it++)
        {
            if((*it).second<min)
            {
                min=(*it).second;
                t=it;
            }
            count++;
        }
        size=temp.size();
        if((*iter).second>min  || count<Tk)
            cn->Candlist[temp[size-1]]=(*iter).second;
        if(count>=Tk)
            cn->Candlist.erase(t);
    }
}
SuffixTree *SuffixTree::findnode(vector<int> v,SuffixTree *t)
{
    if(!v.size())
        return t;
    vector<int> temp=v;
    temp.erase(temp.begin());
    SuffixTree *tree,*cn=NULL;
    tree=findnode(temp,t);
    if(tree!=NULL and tree->child.find(v[0])!=tree->child.end())
        cn=tree->child[v[0]];
    if(cn==NULL)
    {
        cn=new SuffixTree(v);
        tree->child[v[0]]=cn;
    }
    return cn;  
}
map<int,int> SuffixTree::suggestions(vector<int> v)
{
    SuffixTree *temp;
    temp=this;
    vector<int>::iterator it;
    for(it=v.end()-1;it>=v.begin();it--)
    {
        if(temp->child.find(*it)==temp->child.end())
            break;
        temp=temp->child[*it];
    }
    return temp->Candlist;
}
void SuffixTree::printTree()
{
    if(this==NULL)
        return;
    vector<int>::iterator it;
    map<int,int>::iterator iter;
    map<int,SuffixTree*>::iterator it1;
    cout<<"Concepts"<<endl;
    cout<<"===================="<<endl;
    for(it=this->concept.begin();it!=this->concept.end();it++)
        cout<<(*it)<<" ";
    cout<<endl;
    cout<<"Candidates Concepts"<<endl;
    cout<<"-------------------"<<endl;
    for(iter=this->Candlist.begin();iter!=this->Candlist.end();iter++)
        cout<<(*iter).first<<endl;
    for(it1=this->child.begin();it1!=this->child.end();it1++)
        (*it1).second->printTree();
}
#endif
