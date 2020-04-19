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
    BitMap() = delete ;

    BitMap(const BitMap& src): charSize(src.charSize),_size(src.size()),_markSize(src.markSize()) {
        this->data = new uint8_t[charSize];
        memcpy(this->data,src.data,charSize);
    }


    ~BitMap() {
        delete data;
        data = nullptr;
    }

    BitMap(unsigned _size) : charSize((_size / 8) + 1),_size(_size) { // contractor, init the data
        data = new uint8_t[charSize];
        assert(data);
        memset(data, 0x0, charSize * sizeof(uint8_t));
        this->_markSize = 0;
    }

    void set(unsigned index, bool status) {
        if (status) {
            setTrue(index);
        } else {
            setFalse(index);
        }
    }

    template <class T>
    bool get(T index) const {
        unsigned addr = index / 8;
        unsigned addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert(addr <= charSize);
        return (data[addr] & temp) > 0 ? 1 : 0;
    }

    inline unsigned size() const{
        return this->_size;
    }
    inline unsigned markSize() const{
        return this->_markSize;
    }

private:

    void setTrue(unsigned index) {
        if (get(index)) {
            return;
        }
        unsigned addr = index / 8;
        unsigned addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert (addr <= charSize + 1);
        data[addr] |= temp;
        _markSize++;
    }

    void set(unsigned charId, unsigned bitId, bool status) {
        unsigned index = charId * 8 + bitId;
        set(index, status);
    }

    bool get(unsigned charId, unsigned bitId) {
        unsigned index = charId * 8 + bitId;
        return get(index);
    }


    void setFalse(unsigned index) {
        if (!get(index)) {
            return;
        }
        unsigned addr = index / 8;
        unsigned addroffset = index % 8;
        uint8_t temp = 0x1 << (7 - addroffset);
        assert(addr <= charSize);
        data[addr] ^= temp;
        assert(_markSize>=0);
        _markSize--;
    }

    //把一个字符的8位设置为值
    void setNumber(unsigned index, uint8_t number) {
        data[index] = number;
        return;
    }

    friend std::ostream &operator<<(std::ostream &os, const BitMap &map) {
        for (unsigned i = 0; i < map.charSize; i++) {
            os << std::bitset<8>(map.data[i]) << "  ";
        }
        os << std::endl;
        return os;
    }

private:
    uint8_t *data;
    unsigned charSize;
    unsigned _size;
    unsigned _markSize;
};


#endif // BITMAP_HPP