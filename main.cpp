#include<iostream>
#include<queue>
#include<vector>
#include<algorithm>
#include<cstdio>
#include<cstring>
#include "frequent.h"
using namespace std;
main(int argc,char *argv[])
{
    if(argc<2)
    {
        printf("See proper usage\n");
        exit(0);
    }
    Clusters c(argv[1]);
    c.makegraph();
    c.prunegraph();
    c.makecluster();
    //c.printcentroid();
    //c.printquerycluster();
    c.getsessions();
//  c.printsessions();
    c.frequentconcepts();
    c.pruneconcepts();
    c.representcluster();
    c.printconcepts();
    c.buildsuffixtree();
    return 0;
}
