#include "AutoAssign.h"
AutoAssign::AutoAssign(IRs * irs) {
    this->irs = irs;
}
void AutoAssign::create() {
    this->data.push_back({});
}
void AutoAssign::add(int pos) {
    std::vector<int>& lastVec = this->data.back().first;
    lastVec.push_back(pos);
}
void AutoAssign::set(const Address& addr) {
    this->data.back().second = addr;
}
void AutoAssign::assign() {
    std::vector<int>& lastVec = this->data.back().first;
    const Address& addr = this->data.back().second;
    for (int i : lastVec) {
        this->irs->content[i].set_addr(addr);
    }
    this->data.pop_back();
}