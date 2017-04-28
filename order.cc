#include <iostream>
#include <vector>
#include <string>
#include <list>
#include <exception>

using namespace std;

vector<string> split(const string &text, char delim) {
    vector<string> tokens;
    size_t start = 0;
    size_t end = 0;
    while ((end = text.find(delim, start)) != string::npos) {
        tokens.push_back(text.substr(start, end - start));
        start = end + 1;
    }
    tokens.push_back(text.substr(start));
    return tokens;
}

class Order {
private:
    string _id;
    string _type;
    int _price;
    int _quantity;
public:
    Order() {}
    Order(string id, string type, int price, int quantity):
    _id{id},_type{type},_price{price},_quantity{quantity} {}
    string getID() { return _id; }
    string getType() { return _type; }
    int getPrice() { return _price; }
    int getQuantity() { return _quantity; }
    void setQuantity(int newQuantity) { _quantity = newQuantity; }
};

list<Order> buyList;
list<Order> sellList;

class MatchEngine {
private:
    static MatchEngine* instance;
    MatchEngine();
public:
    static MatchEngine* getInstance();
    void buy(Order order);
    void sell(Order order);
    void cancel(string orderID);
    void modify(string orderID, string tradeType, int price, int quantity);
    void print();
};

MatchEngine* MatchEngine::instance = NULL;
MatchEngine::MatchEngine() {}
MatchEngine* MatchEngine::getInstance() {
    if (instance == NULL) instance = new MatchEngine(); 
    return instance;
}

void MatchEngine::buy(Order order) {
    for (list<Order>::iterator iter = sellList.begin(); iter != sellList.end(); ++iter) {
        if (iter->getPrice() > order.getPrice()) continue; //sell price too high, can't buy
        //Trade happened
        int tradeQuantity = min(iter->getQuantity(), order.getQuantity());
        cout << "TRADE" << " " 
        << iter->getID() << " " << iter->getPrice() << " " << tradeQuantity << " " 
        << order.getID() << " " << order.getPrice() << " " << tradeQuantity << endl;
        if (order.getQuantity() >= iter->getQuantity()) {
            int newQuantity = order.getQuantity() - iter->getQuantity();
            order.setQuantity(newQuantity);
            iter = sellList.erase(iter); //all bought, keep on searching in the list
        } else {
            int newQuantity = iter->getQuantity() - order.getQuantity();
            iter->setQuantity(newQuantity);
            order.setQuantity(0);
            break;
        }
    }
    //if buy order still not satisfied, add it to the end of the list
    if (order.getQuantity() > 0 && order.getType() != "IOC") buyList.push_back(order);
}

void MatchEngine::sell(Order order) {
    for (list<Order>::iterator iter = buyList.begin(); iter != buyList.end(); ++iter) {
        if (iter->getPrice() < order.getPrice()) continue; //offer too low, don't want to sell
        //Trade happened
        int tradeQuantity = min(iter->getQuantity(), order.getQuantity());
        cout << "TRADE" << " " 
        << iter->getID() << " " << iter->getPrice() << " " << tradeQuantity << " " 
        << order.getID() << " " << order.getPrice() << " " << tradeQuantity << endl;
        if (order.getQuantity() >= iter->getQuantity()) {
            int newQuantity = order.getQuantity() - iter->getQuantity();
            order.setQuantity(newQuantity);
            iter = buyList.erase(iter); //all bought, keep on searching in the list
        } else {
            int newQuantity = iter->getQuantity() - order.getQuantity();
            iter->setQuantity(newQuantity);
            order.setQuantity(0);
            break;
        }
    }
    //if sell order still not satisfied, add it to the end of the list
    if (order.getQuantity() > 0 && order.getType() != "IOC") sellList.push_back(order);
}

void MatchEngine::cancel(string orderID) {
    for (list<Order>::iterator iter = buyList.begin(); iter != buyList.end(); ++iter) {
        if (iter->getID() == orderID) iter = buyList.erase(iter);
    }
    for (list<Order>::iterator iter = sellList.begin(); iter != sellList.end(); ++iter) {
        if (iter->getID() == orderID) iter = sellList.erase(iter);
    }
}

void MatchEngine::modify(string orderID, string tradeType, int price, int quantity) {
    //check sell list
    for (list<Order>::iterator iter = sellList.begin(); iter != sellList.end(); ++iter) {
        if (iter->getID() != orderID) continue;
        if (tradeType == "SELL" && iter->getPrice() == price && iter->getQuantity() == quantity) {
            return; //same as before, do nothing and return
        }
        Order order(orderID, tradeType,price, quantity);
        iter = sellList.erase(iter);
        if (tradeType == "SELL") sell(order);
        else buy(order);
        return;
    }
    //check buy list
    for (list<Order>::iterator iter = buyList.begin(); iter != buyList.end(); ++iter) {
        if (iter->getID() != orderID) continue;
        if (tradeType == "BUY" && iter->getPrice() == price && iter->getQuantity() == quantity) {
            return; //same as before, do nothing and return
        }
        Order order(orderID, tradeType,price, quantity);
        iter = buyList.erase(iter);
        if (tradeType == "SELL") sell(order);
        else buy(order);
        return;
    }
}

void MatchEngine::print() {
    list<Order> copySellList(sellList);
    list<Order> copyBuyList(buyList);
    copySellList.sort([](Order o1, Order o2) { return o1.getPrice() >= o2.getPrice(); });
    copyBuyList.sort([](Order o1, Order o2) { return o1.getPrice() >= o2.getPrice(); });
    //merge sell List if price is duplicated
    list<Order>::iterator pre = copySellList.begin();
    for (list<Order>::iterator cur = copySellList.begin(); cur != copySellList.end(); ++cur) {
        if (cur == copySellList.begin()) {
            pre = cur;
            continue;
        }
        if (pre->getPrice() != cur->getPrice()) {
            pre = cur;
            continue;
        }
        cur->setQuantity(pre->getQuantity() + cur->getQuantity());
        copySellList.erase(pre);
        pre = cur;
    }
    //merge buy List if price is duplicated
    for (list<Order>::iterator cur = copyBuyList.begin(); cur != copyBuyList.end(); ++cur) {
        if (cur == copyBuyList.begin()) {
            pre = cur;
            continue;
        }
        if (pre->getPrice() != cur->getPrice()) {
            pre = cur;
            continue;
        }
        cur->setQuantity(pre->getQuantity() + cur->getQuantity());
        copyBuyList.erase(pre);
        pre = cur;
    }
    //now print the list: sell first then buy
    cout << "SELL:" << endl;
    for (list<Order>::iterator iter = copySellList.begin(); iter != copySellList.end(); ++iter) {
        cout << iter->getPrice() << " " << iter->getQuantity() << endl;
    }
    cout << "BUY:" << endl;
    for (list<Order>::iterator iter = copyBuyList.begin(); iter != copyBuyList.end(); ++iter) {
        cout << iter->getPrice() << " " << iter->getQuantity() << endl;
    }
}

int main(int argc, char** argv) {
    string inputStr;
    while(getline(cin, inputStr)) {
        vector<string> vs = split(inputStr, ' ');
        if (vs[0] == "BUY") {
            try {
                string type = vs[1];
                int price = stoi(vs[2]);
                int quantity = stoi(vs[3]);
                string id = vs[4];
                Order order(id, type, price, quantity);
                MatchEngine::getInstance()->buy(order);                
            } catch (exception &e) {
                cout << e.what() << endl;
            }
        } else if (vs[0] == "SELL") {
            try {
                string type = vs[1];
                int price = stoi(vs[2]);
                int quantity = stoi(vs[3]);
                string id = vs[4];
                Order order(id, type, price, quantity);
                MatchEngine::getInstance()->sell(order);
            } catch (exception &e) {
                cout << e.what() << endl;
            }
        } else if (vs[0] == "CANCEL") {
            string id = vs[1];
            MatchEngine::getInstance()->cancel(id);
        } else if (vs[0] == "MODIFY") {
            try {
                string id = vs[1];
                string type = vs[2];
                if (type == "IOC") break;
                int price = stoi(vs[3]);
                int quantity = stoi(vs[4]);
                MatchEngine::getInstance()->modify(id, type, price, quantity);
            } catch (exception &e) {
                cout << e.what() << endl;
            }
        } else if (vs[0] == "PRINT") {
            MatchEngine::getInstance()->print();
        } else {
            //ignore
        }
    }
    return 0;
}
