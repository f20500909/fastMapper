#ifndef BITMAP_HPP
#define BITMAP_HPP

#include <iostream>
#include <cassert>
#include <memory>
#include <ostream>
#include <memory.h>
#include <bitset>

class BitMap {
public:
    BitMap() {
        data = nullptr;
    }

    ~BitMap() {
//        delete data;
    }

    BitMap(int _size) : charSize((_size / 8) + 1),_size(_size) { // contractor, init the data
        data = new uint8_t[charSize];
        assert(data);
        memset(data, 0x0, charSize * sizeof(uint8_t));
        this->_markSize = 0;
    }

    void set(int index, bool status) {
        if (status) {
            setTrue(index);
        } else {
            setFalse(index);
        }
    }

    bool get(int index) const {
        int addr = index / 8;
        int addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert(addr <= charSize);
        return (data[addr] & temp) > 0 ? 1 : 0;
    }

    inline int size() const{
        return this->_size;
    }
    inline int markSize() const{
        return this->_markSize;
    }

private:

    void setTrue(int index) {
        if (get(index)) {
            return;
        }
        int addr = index / 8;
        int addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert (addr <= charSize + 1);
        data[addr] |= temp;
        _markSize++;
    }

    void set(int charId, int bitId, bool status) {
        int index = charId * 8 + bitId;
        set(index, status);
    }

    bool get(int charId, int bitId) {
        int index = charId * 8 + bitId;
        return get(index);
    }



    void setFalse(int index) {
        if (!get(index)) {
            return;
        }
        int addr = index / 8;
        int addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert(addr <= charSize);
        data[addr] ^= temp;
        assert(_markSize>=0);
        _markSize--;
    }

    //把一个字符的8位设置为值
    void setNumber(int index, uint8_t number) {
        data[index] = number;
        return;
    }

    friend std::ostream &operator<<(std::ostream &os, const BitMap &map) {
        for (int i = 0; i < map.charSize; i++) {
            os << std::bitset<8>(map.data[i]) << "";
        }
        os << std::endl;
        return os;
    }

private:
    uint8_t *data;
    int charSize;
    int _size;
    int _markSize;
};


#endif // BITMAP_HPP