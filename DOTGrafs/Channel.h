#ifndef CHANNEL_H
#define CHANNEL_H

#include <string>
using namespace std;


class Channel {

public:

    Channel();
    Channel(const string &portOut, const string &portIn, 
                const string &blockOut, const string &blockIn);
    ~Channel();

    void setPortOut(const string &portOut);
    void setPortIn(const string &portIn);
    void setBlockOut(const string &blockOut);
    void setBlockIn(const string &blockIn);

    string getPortOut();
    string getPortIn();
    string getBlockOut();
    string getBlockIn();

private:

    string portOut;
    string portIn;
    string blockOut;
    string blockIn;

};

#endif // CHANNEL_H