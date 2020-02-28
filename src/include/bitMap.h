//
//  Header.h
//  BloomFilter
//
//  Created by Alps on 15/3/19.
//  Copyright (c) 2015年 chen. All rights reserved.
//
//这个是 BitMap.h文件。

class BitMap{
public:
    BitMap(){
        bitmap = NULL;
        size = 0;
    }
    BitMap(int size){ // contractor, init the bitmap
        bitmap = NULL;
        bitmap = new char[size];
        if (bitmap == NULL) {
            printf("ErroR In BitMap Constractor!\n");
        }else{
            memset(bitmap, 0x0, size * sizeof(char));
            this->size = size;
        }
    }


    /*
     * set the index bit to 1;
     */
    int bitmapSet(int index){
        int addr = index/8;
        int addroffset = index%8;
        unsigned char temp = 0x1 << addroffset;
        if (addr > (size+1)) {
            return 0;
        }else{
            bitmap[addr] |= temp;
            return 1;
        }
    }

    /*
     * return if the index in bitmap is 1;
     */
    int bitmapGet(int index){
        int addr = index/8;
        int addroffset = index%8;
        unsigned char temp = 0x1 << addroffset;
        if (addr > (size + 1)) {
            return 0;
        }else{
            return (bitmap[addr] & temp) > 0 ? 1 : 0;
        }
    }

    /*
     * del the index from 1 to 0
     */
    int bitmapDel(int index){
        if (bitmapGet(index) == 0) {
            return 0;
        }
        int addr = index/8;
        int addroffset = index%8;
        unsigned char temp = 0x1 << addroffset;
        if (addr > (size + 1)) {
            return 0;
        }else{
            bitmap[addr] ^= temp;
            return 1;
        }
    }

private:
    char *bitmap;
    int size;
};
