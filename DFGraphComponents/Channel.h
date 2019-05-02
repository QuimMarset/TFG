#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <fstream>
#include <assert.h>
using namespace std;


namespace DFGraphComp
{


class Channel {

public:

    Channel();
    Channel(const string &blockOut, const string &blockIn, 
        const string &portOut, const string &portIn);
    ~Channel();

    void setPortOut(const string &portOut);
    void setPortIn(const string &portIn);
    void setBlockOut(const string &blockOut);
    void setBlockIn(const string &blockIn);

    string getPortOut();
    string getPortIn();
    string getBlockOut();
    string getBlockIn();

    void printChannel(ostream &file);

    // friend ostream &operator << (ostream &out, const Channel &channel);

private:

    string portOut;
    string portIn;
    string blockOut;
    string blockIn;

};


} // Close namespace

#endif // CHANNEL_H