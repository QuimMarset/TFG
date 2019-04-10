#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
#include <fstream>
using namespace std;


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

    void printChannel(ostream &file) const;

    friend ostream &operator << (ostream &out, const Channel &channel);

private:

    string portOut;
    string portIn;
    string blockOut;
    string blockIn;

};

#endif // CHANNEL_H