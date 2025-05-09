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
        ifstream file(TRANSACTIONS_FILE);
        string line;
        while (getline(file, line)) {
            vector<string> parts;
            size_t pos;
            for (int i = 0; i < 4; i++) {
                pos = line.find('|');
                parts.push_back(line.substr(0, pos));
                line = line.substr(pos + 1);
            }
            parts.push_back(line);
            
            if (parts.size() == 5) {
                Transaction t(
                    parts[0],
                    parts[1],
                    stod(parts[2]),
                    parts[3],
                    stod(parts[4])
                );
                transactions.push_back(t);
            }
        }
    }

    void saveTransactions() {
        ofstream file(TRANSACTIONS_FILE);
        for (const Transaction& t : transactions) {
            file << t.serialize() << endl;
        }
    }

public:
    Bank() {
        loadAccounts();
        loadTransactions();
    }

    ~Bank() {
        saveAccounts();
        saveTransactions();
    }

    bool createAccount(const string& num, const string& name) {
        if (num.empty() || name.empty()) return false;
        
        auto it = find_if(accounts.begin(), accounts.end(),
            [&](const Account& acc) { return acc.getNumber() == num; });
        
        if (it == accounts.end()) {
            accounts.emplace_back(num, name, 0.0);
            return true;
        }
        return false;
    }

    Account* findAccount(const string& num) {
        auto it = find_if(accounts.begin(), accounts.end(),
            [&](const Account& acc) { return acc.getNumber() == num; });
        return it != accounts.end() ? &(*it) : nullptr;
    }

    bool deposit(const string& num, double amount, const string& date) {
        Account* acc = findAccount(num);
        if (acc && amount > 0) {
            acc->deposit(amount);
            transactions.emplace_back(num, "DEPOSIT", amount, date, acc->getBalance());
            return true;
        }
        return false;
    }

    bool withdraw(const string& num, double amount, const string& date) {
        Account* acc = findAccount(num);
        if (acc && amount > 0 && acc->getBalance() >= amount) {
            acc->withdraw(amount);
            transactions.emplace_back(num, "WITHDRAW", amount, date, acc->getBalance());
            return true;
        }
        return false;
    }

    vector<Transaction> getTransactions(const string& num) const {
        vector<Transaction> result;
        for (const Transaction& t : transactions) {
            if (t.getAccountNumber() == num) {
                result.push_back(t);
            }
        }
        return result;
    }
};

void displayMainMenu() {
    cout << "\nBanking System\n"
         << "1. Create Account\n"
         << "2. Deposit\n"
         << "3. Withdraw\n"
         << "4. Check Balance\n"
         << "5. View Transactions\n"
         << "6. Show All Accounts\n"
         << "0. Exit\n"
         << "Enter choice: ";
}

void handleCreateAccount(Bank& bank) {
    string num, name;
    cout << "Enter account number: ";
    cin >> num;
    cout << "Enter account holder name: ";
    cin.ignore();
    getline(cin, name);

    if (bank.createAccount(num, name)) {
        cout << "Account created successfully!\n";
    } else {
        cout << "Account creation failed! (Account number might already exist)\n";
    }
}

void handleDeposit(Bank& bank) {
    string num, date;
    double amount;
    cout << "Enter account number: ";
    cin >> num;
    cout << "Enter amount to deposit: ";
    cin >> amount;
    cout << "Enter transaction date (DD-MM-YYYY): ";
    cin >> date;

    if (bank.deposit(num, amount, date)) {
        cout << "Deposit successful!\n";
    } else {
        cout << "Deposit failed! (Invalid account or amount)\n";
    }
}

int main() {
    Bank bank;
    int choice;
    string num;

    do {
        displayMainMenu();
        cin >> choice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        switch (choice) {
            case 1:
                handleCreateAccount(bank);
                break;
            case 2: {
                handleDeposit(bank);
                break;
            }
            case 3: {
                string date;
                double amount;
                cout << "Enter account number: ";
                cin >> num;
                cout << "Enter amount to withdraw: ";
                cin >> amount;
                cout << "Enter transaction date (DD-MM-YYYY): ";
                cin >> date;

                if (bank.withdraw(num, amount, date)) {
                    cout << "Withdrawal successful!\n";
                } else {
                    cout << "Withdrawal failed! (Invalid account, amount, or insufficient balance)\n";
                }
                break;
            }
            case 4: {
                cout << "Enter account number: ";
                cin >> num;
                Account* acc = bank.findAccount(num);
                if (acc) {
                    cout << "\nAccount Balance: ?" << fixed << setprecision(2) << acc->getBalance() << endl;
                } else {
                    cout << "Account not found!\n";
                }
                break;
            }
            case 5: {
                cout << "Enter account number: ";
                cin >> num;
                vector<Transaction> transactions = bank.getTransactions(num);
                if (!transactions.empty()) {
                    cout << "\nTransaction History:\n";
                    cout << left << setw(15) << "Date" << setw(12) << "Type" 
                         << setw(12) << "Amount" << "Balance" << endl;
                    for (const Transaction& t : transactions) {
                        t.display();
                    }
                } else {
                    cout << "No transactions found!\n";
                }
                break;
            }
            case 6: {
                cout << "\nAll Accounts:\n";
                cout << left << setw(15) << "Account#" << setw(25) << "Name" << "Balance" << endl;
                // Implementation for showing all accounts would require adding a method in Bank
                // This is left as an exercise for further development
                cout << "Feature not implemented yet!\n";
                break;
            }
            case 0:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice!\n";
        }
    } while (choice != 0);

    return 0;
}