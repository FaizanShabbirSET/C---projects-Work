#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <limits>
using namespace std;

class Transaction;

class Account {
private:
    string accountNumber;
    string name;
    double balance;
public:
    Account(string num = "", string n = "", double bal = 0.0)
        : accountNumber(num), name(n), balance(bal) {}

    string getNumber() const { return accountNumber; }
    string getName() const { return name; }
    double getBalance() const { return balance; }

    void deposit(double amount) { balance += amount; }
    bool withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            return true;
        }
        return false;
    }

    void display() const {
        cout << left << setw(15) << accountNumber 
             << setw(25) << name 
             << "?" << fixed << setprecision(2) << balance << endl;
    }
};

class Transaction {
private:
    string accountNumber;
    string type;
    double amount;
    string date;
    double balanceAfter;
public:
    Transaction(string acc = "", string t = "", double amt = 0.0, 
                string d = "", double bal = 0.0)
        : accountNumber(acc), type(t), amount(amt), 
          date(d), balanceAfter(bal) {}

    string serialize() const {
        ostringstream oss;
        oss << accountNumber << "|" << type << "|" 
            << fixed << setprecision(2) << amount << "|"
            << date << "|" << fixed << setprecision(2) << balanceAfter;
        return oss.str();
    }

    void display() const {
        cout << left << setw(15) << date
             << setw(12) << type
             << "?" << setw(10) << fixed << setprecision(2) << amount
             << "?" << fixed << setprecision(2) << balanceAfter << endl;
    }

    string getAccountNumber() const { return accountNumber; }
};

class Bank {
private:
    vector<Account> accounts;
    vector<Transaction> transactions;
    const string ACCOUNTS_FILE = "accounts.dat";
    const string TRANSACTIONS_FILE = "transactions.dat";

    void loadAccounts() {
        ifstream file(ACCOUNTS_FILE);
        string line;
        while (getline(file, line)) {
            size_t pos1 = line.find('|');
            size_t pos2 = line.find('|', pos1 + 1);
            if (pos1 == string::npos || pos2 == string::npos) continue;
            
            Account acc(
                line.substr(0, pos1),
                line.substr(pos1 + 1, pos2 - pos1 - 1),
                stod(line.substr(pos2 + 1))
            );
            accounts.push_back(acc);
        }
    }

    void saveAccounts() {
        ofstream file(ACCOUNTS_FILE);
        for (const Account& acc : accounts) {
            file << acc.getNumber() << "|" << acc.getName() << "|"
                 << fixed << setprecision(2) << acc.getBalance() << endl;
        }
    }

    void loadTransactions() {
        ifstream