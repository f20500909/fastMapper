#ifndef SRC_DIRECTION_HPP
#define SRC_DIRECTION_HPP

class Options;

template<class Item>

class Iterator {
public:
    Iterator() {};

    virtual ~Iterator() {};

    virtual void first() = 0;

    virtual void next() = 0;

    virtual Item *curItem() = 0;

    virtual bool isDone() = 0;
};


template<class Item>
class Aggregate {
public:
    Aggregate() {};

    virtual ~Aggregate() {};

    virtual void pushData(Item item) = 0;

    virtual Iterator<Item> *createIterator() = 0;

    virtual Item &operator[](int index) = 0;

    virtual int getSize() = 0;
};


template<class Item>
class ConcreteIterator : public Iterator<Item> {
public:
    ConcreteIterator(Aggregate<Item> *a) : aggr(a), cur(0) {};

    virtual ~ConcreteIterator() {};

    virtual void first(){
        cur = 0;
    };

    virtual void next(){
        if (cur < aggr->getSize())
            cur++;
    };

    virtual Item *curItem();

    virtual bool isDone();

private:
    Aggregate<Item> *aggr;
    int cur;
};

template<class Item>
class ConcreteAggregate : public Aggregate<Item> {
public:
    ConcreteAggregate() {};

    virtual ~ConcreteAggregate() {};

    virtual void pushData(Item item){
        data.push_back(item);

    };

    virtual Iterator<Item> *createIterator(){
        return new ConcreteIterator<Item>(this);
    };

    virtual Item &operator[](int index){
        return data[index];
    };

    virtual int getSize(){
        return data.size();
    };

private:
    vector<Item> data;
};



void test_temp(){
    Aggregate<int> * aggr = new ConcreteAggregate<int>();
    aggr->pushData(3);
    aggr->pushData(2);
    aggr->pushData(1);
    Iterator<int> * it = aggr->createIterator();

    for (it->first(); !it->isDone(); it->next())
    {
        std::cout << *it->curItem() << std::endl;
    }
    delete it;
    delete aggr;
}




class Direction {
public:

    Direction(const Options &op) : option(op) {

    }

    Direction(std::string type) {
        if (type == "x") {
//            directions_x

        }
    }

    Options option;
    const int dim = 2;

    int operator+(Direction &d) {

    }

    std::vector<int> directions_x = {0, -1, 1, 0};
    std::vector<int> directions_y = {-1, 0, 0, 1};

};


#endif
