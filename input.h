#ifndef __INPUT_H_
#define __INPUT_H_
#include<fstream>
using namespace std;
class Input
{
    char string[800];
    fstream file;
    public:
    Input(char *c)
    {
        file.open(c,fstream::in);
    }
    char *getline()
    {
        if(file.good())
        {
            file.getline(string,800);
            return string;
        }
        file.close();
        return NULL;
    }
};
#endif
