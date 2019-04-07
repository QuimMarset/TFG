
#include "Channel.h"


Channel::Channel() {

    portOut = "";
    portIn = "";
    blockOut = "";
    blockIn = "";
}
Channel::Channel(const string &portOut, const string &portIn, 
                    const string &blockOut, const string &blockIn) {
    this->portOut = portOut;
    this->portIn = portIn;
    this->blockOut = blockOut;
    this->blockIn = blockIn;
}

Channel::~Channel() {}

void Channel::setPortOut(const string &portOut) {
    this->portOut = portOut;
}

void Channel::setPortIn(const string &portIn) {
    this->portIn = portIn;
}

void Channel::setBlockOut(const string &blockOut) {
    this->blockOut = blockOut;
}

void Channel::setBlockIn(const string &blockIn) {
    this->blockIn = blockIn;
}

string Channel::getPortOut() {
    return portOut;
}

string Channel::getPortIn() {
    return portIn;
}

string Channel::getBlockOut() {
    return blockOut;
}

string Channel::getBlockIn() {
    return blockIn;
}