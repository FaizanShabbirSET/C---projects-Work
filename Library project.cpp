#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

// Forward declarations
class Member;
class Issue;

// Book Class
class Book {
private:
    string ISBN;
    string title;
    string author;
    bool available;
public:
    Book(string isbn = "", string t = "", string a = "", bool av = true)
        : ISBN(isbn), title(t), author(a), available(av) {}

    // Getters
    string getISBN() const { return ISBN; }
    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    bool isAvailable() const { return available; }

    // Setters
    void setAvailable(bool status) { available = status; }

    void display() const {
        cout << left << setw(15) << ISBN 
             << setw(25) << title 
             << setw(20) << author 
             << setw(10) << (available ? "Available" : "Issued") << endl;
    }
};

// Member Class
class Member {
private:
    int id;
    string name;
public:
    Member(int i = 0, string n = "") : id(i), name(n) {}

    // Getters
    int getID() const { return id; }
    string getName() const { return name; }

    void display() const {
        cout << left << setw(10) << id << setw(20) << name << endl;
    }
};

// Issue Class
class Issue {
private:
    int memberID;
    string ISBN;
    string issueDate;
public:
    Issue(int mid = 0, string isbn = "", string date = "")
        : memberID(mid), ISBN(isbn), issueDate(date) {}

    // Getters
    int getMemberID() const { return memberID; }
    string getISBN() const { return ISBN; }
    string getDate() const { return issueDate; }

    void display() const {
        cout << left << setw(10) << memberID 
             << setw(15) << ISBN 
             << setw(15) << issueDate << endl;
    }
};

// Library Management System Class
class LibrarySystem {
private:
    vector<Book> books;
    vector<Member> members;
    vector<Issue> issues;

    // File names
    const string BOOKS_FILE = "books.txt";
    const string MEMBERS_FILE = "members.txt";
    const string ISSUES_FILE = "issues.txt";

public:
    LibrarySystem() {
        loadBooks();
        loadMembers();
        loadIssues();
    }

    ~LibrarySystem() {
        saveBooks();
        saveMembers();
        saveIssues();
    }

    // Book management
    void addBook() {
        string isbn, title, author;
        cout << "Enter ISBN: ";
        cin >> isbn;
        cout << "Enter Title: ";
        cin.ignore();
        getline(cin, title);
        cout << "Enter Author: ";
        getline(cin, author);

        books.emplace_back(isbn, title, author);
    }

    void searchBook() {
        string query;
        cout << "Enter ISBN or Title to search: ";
        cin.ignore();
        getline(cin, query);

        bool found = false;
        for (const auto& book : books) {
            if (book.getISBN() == query || book.getTitle() == query) {
                book.display();
                found = true;
            }
        }
        if (!found) cout << "Book not found!" << endl;
    }

    void removeBook() {
        string isbn;
        cout << "Enter ISBN to remove: ";
        cin >> isbn;

        auto it = remove_if(books.begin(), books.end(),
            [&](const Book& b) { return b.getISBN() == isbn; });
        
        if (it != books.end()) {
            books.erase(it, books.end());
            cout << "Book removed successfully!" << endl;
        } else {
            cout << "Book not found!" << endl;
        }
    }

    // File handling
    void loadBooks() {
        ifstream file(BOOKS_FILE);
        string isbn, title, author, available;
        while (getline(file, isbn, '|') &&
               getline(file, title, '|') &&
               getline(file, author, '|') &&
               getline(file, available)) {
            books.emplace_back(isbn, title, author, available == "1");
        }
    }

    void saveBooks() {
        ofstream file(BOOKS_FILE);
        for (const auto& book : books) {
            file << book.getISBN() << "|"
                 << book.getTitle() << "|"
                 << book.getAuthor() << "|"
                 << (book.isAvailable() ? "1" : "0") << endl;
        }
    }

    // Member management
    void addMember() {
        int id;
        string name;
        cout << "Enter Member ID: ";
        cin >> id;
        cout << "Enter Name: ";
        cin.ignore();
        getline(cin, name);

        members.emplace_back(id, name);
    }

    void loadMembers() {
        ifstream file(MEMBERS_FILE);
        string id, name;
        while (getline(file, id, '|') &&
               getline(file, name)) {
            members.emplace_back(stoi(id), name);
        }
    }

    void saveMembers() {
        ofstream file(MEMBERS_FILE);
        for (const auto& member : members) {
            file << member.getID() << "|"
                 << member.getName() << endl;
        }
    }

    // Issue management
    void issueBook() {
        int memberID;
        string isbn, date;
        cout << "Enter Member ID: ";
        cin >> memberID;
        cout << "Enter ISBN: ";
        cin >> isbn;
        cout << "Enter Issue Date (DD-MM-YYYY): ";
        cin >> date;

        auto member = find_if(members.begin(), members.end(),
            [&](const Member& m) { return m.getID() == memberID; });
        
        auto book = find_if(books.begin(), books.end(),
            [&](const Book& b) { return b.getISBN() == isbn && b.isAvailable(); });

        if (member != members.end() && book != books.end()) {
            issues.emplace_back(memberID, isbn, date);
            book->setAvailable(false);
            cout << "Book issued successfully!" << endl;
        } else {
            cout << "Invalid member ID or book not available!" << endl;
        }
    }

    void loadIssues() {
        ifstream file(ISSUES_FILE);
        string mid, isbn, date;
        while (getline(file, mid, '|') &&
               getline(file, isbn, '|') &&
               getline(file, date)) {
            issues.emplace_back(stoi(mid), isbn, date);
        }
    }

    void saveIssues() {
        ofstream file(ISSUES_FILE);
        for (const auto& issue : issues) {
            file << issue.getMemberID() << "|"
                 << issue.getISBN() << "|"
                 << issue.getDate() << endl;
        }
    }

    // Display functions
    void showBooks() const {
        cout << "\nBooks List:\n";
        cout << left << setw(15) << "ISBN" << setw(25) << "Title" 
             << setw(20) << "Author" << setw(10) << "Status" << endl;
        for (const auto& book : books) book.display();
    }

    void showMembers() const {
        cout << "\nMembers List:\n";
        cout << left << setw(10) << "ID" << setw(20) << "Name" << endl;
        for (const auto& member : members) member.display();
    }

    void showIssues() const {
        cout << "\nIssued Books:\n";
        cout << left << setw(10) << "MemberID" << setw(15) << "ISBN" 
             << setw(15) << "Issue Date" << endl;
        for (const auto& issue : issues) issue.display();
    }
};

// Main menu
int main() {
    LibrarySystem lib;
    int choice;

    do {
        cout << "\nLibrary Management System\n";
        cout << "1. Add Book\n2. Search Book\n3. Remove Book\n4. Show All Books\n";
        cout << "5. Add Member\n6. Show Members\n7. Issue Book\n8. Show Issued Books\n";
        cout << "0. Exit\nEnter choice: ";
        cin >> choice;

        switch(choice) {
            case 1: lib.addBook(); break;
            case 2: lib.searchBook(); break;
            case 3: lib.removeBook(); break;
            case 4: lib.showBooks(); break;
            case 5: lib.addMember(); break;
            case 6: lib.showMembers(); break;
            case 7: lib.issueBook(); break;
            case 8: lib.showIssues(); break;
            case 0: cout << "Exiting..."; break;
            default: cout << "Invalid choice!" << endl;
        }
    } while(choice != 0);

    return 0;
}