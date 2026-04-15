#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <limits>
using namespace std;

// ===================== Book Class =====================
class Book {
private:
    string title;
    string author;
    string isbn;
    string category;
    bool available;
    string issuedTo;
    string issueDate;

public:
    Book(string t, string a, string i, string cat)
        : title(t), author(a), isbn(i), category(cat),
          available(true), issuedTo(""), issueDate("") {}

    string getTitle() { return title; }
    string getAuthor() { return author; }
    string getISBN() { return isbn; }
    string getCategory() { return category; }
    bool isAvailable() { return available; }
    string getIssuedTo() { return issuedTo; }
    string getIssueDate() { return issueDate; }

    void issueBook(string studentId) {
        available = false;
        issuedTo = studentId;
        time_t now = time(0);
        tm* t = localtime(&now);
        char buf[20];
        strftime(buf, 20, "%d-%m-%Y", t);
        issueDate = string(buf);
    }

    void returnBook() {
        available = true;
        issuedTo = "";
        issueDate = "";
    }

    // check if query matches title, author or category
    bool matches(string query) {
        string t = title, a = author, c = category, q = query;
        transform(t.begin(), t.end(), t.begin(), ::tolower);
        transform(a.begin(), a.end(), a.begin(), ::tolower);
        transform(c.begin(), c.end(), c.begin(), ::tolower);
        transform(q.begin(), q.end(), q.begin(), ::tolower);
        return (t.find(q) != string::npos ||
                a.find(q) != string::npos ||
                c.find(q) != string::npos);
    }
};

// ===================== Student Class =====================
class Student {
private:
    string name;
    string rollNo;
    string department;
    vector<string> borrowedISBNs;

public:
    Student(string n, string r, string dept)
        : name(n), rollNo(r), department(dept) {}

    string getName() { return name; }
    string getRollNo() { return rollNo; }
    string getDepartment() { return department; }
    int getBorrowedCount() { return borrowedISBNs.size(); }
    vector<string>& getBorrowedBooks() { return borrowedISBNs; }

    bool canBorrow() { return borrowedISBNs.size() < 3; }

    void borrowBook(string isbn) {
        borrowedISBNs.push_back(isbn);
    }

    void returnBook(string isbn) {
        auto it = find(borrowedISBNs.begin(), borrowedISBNs.end(), isbn);
        if (it != borrowedISBNs.end())
            borrowedISBNs.erase(it);
    }
};

// ===================== Library Class =====================
class Library {
private:
    string name;
    vector<Book> books;
    vector<Student> students;

    // helper to draw a line
    void drawLine(int len = 80) {
        cout << string(len, '-') << endl;
    }

    void clearScreen() {
        cout << "\033[2J\033[H";  // ANSI clear screen
    }

    void pause() {
        cout << "\n  Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

public:
    Library(string n) : name(n) {
        // preload some sample books
        books.push_back(Book("Data Structures", "Mark Allen Weiss", "978-0132847377", "Computer Science"));
        books.push_back(Book("Introduction to Algorithms", "Thomas Cormen", "978-0262033848", "Computer Science"));
        books.push_back(Book("Database Systems", "Ramez Elmasri", "978-0133970777", "Computer Science"));
        books.push_back(Book("Operating System Concepts", "Silberschatz", "978-1119800361", "Computer Science"));
        books.push_back(Book("Computer Networks", "Andrew Tanenbaum", "978-0132126953", "Computer Science"));
        books.push_back(Book("Calculus", "James Stewart", "978-1285740621", "Mathematics"));
        books.push_back(Book("Linear Algebra", "Gilbert Strang", "978-0980232714", "Mathematics"));
        books.push_back(Book("Digital Logic Design", "Morris Mano", "978-0132774208", "Computer Science"));
        books.push_back(Book("Software Engineering", "Ian Sommerville", "978-0133943030", "Computer Science"));
        books.push_back(Book("Physics for Scientists", "Serway & Jewett", "978-1133947271", "Physics"));

        // preload some students
        students.push_back(Student("Usman Muzafar Shah", "22P-9412", "CS"));
        students.push_back(Student("Ali Hassan", "22P-9401", "SE"));
        students.push_back(Student("Sara Khan", "22P-9415", "AI"));
    }

    Book* findBookByISBN(string isbn) {
        for (auto& b : books)
            if (b.getISBN() == isbn) return &b;
        return nullptr;
    }

    Student* findStudentByRoll(string roll) {
        for (auto& s : students)
            if (s.getRollNo() == roll) return &s;
        return nullptr;
    }

    // ---- Display Header ----
    void showHeader() {
        clearScreen();
        cout << endl;
        drawLine();
        cout << "       FAST UNIVERSITY - LIBRARY MANAGEMENT SYSTEM" << endl;
        drawLine();
    }

    // ---- Main Menu ----
    void showMenu() {
        cout << "\n  [1]  View All Books" << endl;
        cout << "  [2]  Search Books" << endl;
        cout << "  [3]  Add New Book" << endl;
        cout << "  [4]  Issue Book" << endl;
        cout << "  [5]  Return Book" << endl;
        cout << "  [6]  View Students" << endl;
        cout << "  [7]  Register Student" << endl;
        cout << "  [8]  Dashboard / Stats" << endl;
        cout << "  [0]  Exit" << endl;
        drawLine();
        cout << "  Enter your choice: ";
    }

    // ---- 1. View All Books ----
    void viewAllBooks() {
        showHeader();
        cout << "  BOOK CATALOG (" << books.size() << " books)" << endl;
        drawLine();

        cout << left
             << "  " << setw(4)  << "No."
             << setw(30) << "Title"
             << setw(20) << "Author"
             << setw(18) << "ISBN"
             << setw(10) << "Status" << endl;
        drawLine();

        for (int i = 0; i < books.size(); i++) {
            string status = books[i].isAvailable() ? "Available" : "Issued";
            cout << "  " << setw(4)  << (i + 1)
                 << setw(30) << books[i].getTitle().substr(0, 28)
                 << setw(20) << books[i].getAuthor().substr(0, 18)
                 << setw(18) << books[i].getISBN()
                 << setw(10) << status << endl;
        }
        drawLine();
        pause();
    }

    // ---- 2. Search Books ----
    void searchBooks() {
        showHeader();
        cout << "  SEARCH BOOKS" << endl;
        drawLine();

        string query;
        cout << "  Enter search (title/author/category): ";
        cin.ignore();
        getline(cin, query);

        vector<Book*> results;
        for (auto& b : books) {
            if (b.matches(query))
                results.push_back(&b);
        }

        if (results.empty()) {
            cout << "\n  No books found matching \"" << query << "\"" << endl;
        } else {
            cout << "\n  Found " << results.size() << " result(s):\n" << endl;
            cout << left
                 << "  " << setw(4)  << "No."
                 << setw(30) << "Title"
                 << setw(20) << "Author"
                 << setw(15) << "Category"
                 << setw(10) << "Status" << endl;
            drawLine();

            for (int i = 0; i < results.size(); i++) {
                string status = results[i]->isAvailable() ? "Available" : "Issued";
                cout << "  " << setw(4)  << (i + 1)
                     << setw(30) << results[i]->getTitle().substr(0, 28)
                     << setw(20) << results[i]->getAuthor().substr(0, 18)
                     << setw(15) << results[i]->getCategory().substr(0, 13)
                     << setw(10) << status << endl;
            }
        }
        drawLine();
        cout << endl;
        // no pause needed here, cin.ignore already consumed
        cout << "  Press Enter to continue...";
        cin.get();
    }

    // ---- 3. Add New Book ----
    void addBook() {
        showHeader();
        cout << "  ADD NEW BOOK" << endl;
        drawLine();

        string title, author, isbn, category;

        cout << "  Title    : ";
        cin.ignore();
        getline(cin, title);
        cout << "  Author   : ";
        getline(cin, author);
        cout << "  ISBN     : ";
        getline(cin, isbn);
        cout << "  Category : ";
        getline(cin, category);

        if (title.empty() || author.empty() || isbn.empty()) {
            cout << "\n  [ERROR] Title, Author and ISBN are required!" << endl;
        } else {
            if (category.empty()) category = "General";
            books.push_back(Book(title, author, isbn, category));
            cout << "\n  [SUCCESS] Book \"" << title << "\" added successfully!" << endl;
        }
        cout << "  Press Enter to continue...";
        cin.get();
    }

    // ---- 4. Issue Book ----
    void issueBook() {
        showHeader();
        cout << "  ISSUE BOOK" << endl;
        drawLine();

        string isbn, roll;
        cout << "  Enter Book ISBN     : ";
        cin.ignore();
        getline(cin, isbn);
        cout << "  Enter Student Roll  : ";
        getline(cin, roll);

        Book* book = findBookByISBN(isbn);
        Student* student = findStudentByRoll(roll);

        if (!book) {
            cout << "\n  [ERROR] Book with ISBN \"" << isbn << "\" not found!" << endl;
        } else if (!student) {
            cout << "\n  [ERROR] Student with Roll No \"" << roll << "\" not found!" << endl;
        } else if (!book->isAvailable()) {
            cout << "\n  [ERROR] This book is already issued to " << book->getIssuedTo() << endl;
        } else if (!student->canBorrow()) {
            cout << "\n  [ERROR] Student already has 3 books. Return one first." << endl;
        } else {
            book->issueBook(roll);
            student->borrowBook(isbn);
            cout << "\n  [SUCCESS] \"" << book->getTitle() << "\" issued to "
                 << student->getName() << " (" << roll << ")" << endl;
            cout << "  Issue Date: " << book->getIssueDate() << endl;
        }
        cout << "\n  Press Enter to continue...";
        cin.get();
    }

    // ---- 5. Return Book ----
    void returnBook() {
        showHeader();
        cout << "  RETURN BOOK" << endl;
        drawLine();

        string isbn, roll;
        cout << "  Enter Book ISBN     : ";
        cin.ignore();
        getline(cin, isbn);
        cout << "  Enter Student Roll  : ";
        getline(cin, roll);

        Book* book = findBookByISBN(isbn);
        Student* student = findStudentByRoll(roll);

        if (!book) {
            cout << "\n  [ERROR] Book not found!" << endl;
        } else if (!student) {
            cout << "\n  [ERROR] Student not found!" << endl;
        } else if (book->isAvailable()) {
            cout << "\n  [ERROR] This book is not currently issued." << endl;
        } else if (book->getIssuedTo() != roll) {
            cout << "\n  [ERROR] This book was not issued to " << roll << endl;
        } else {
            book->returnBook();
            student->returnBook(isbn);
            cout << "\n  [SUCCESS] \"" << book->getTitle() << "\" returned by "
                 << student->getName() << endl;
        }
        cout << "\n  Press Enter to continue...";
        cin.get();
    }

    // ---- 6. View Students ----
    void viewStudents() {
        showHeader();
        cout << "  REGISTERED STUDENTS (" << students.size() << " students)" << endl;
        drawLine();

        cout << left
             << "  " << setw(4)  << "No."
             << setw(25) << "Name"
             << setw(15) << "Roll No"
             << setw(12) << "Dept"
             << setw(15) << "Books Borrowed" << endl;
        drawLine();

        for (int i = 0; i < students.size(); i++) {
            cout << "  " << setw(4)  << (i + 1)
                 << setw(25) << students[i].getName()
                 << setw(15) << students[i].getRollNo()
                 << setw(12) << students[i].getDepartment()
                 << setw(15) << students[i].getBorrowedCount() << endl;
        }
        drawLine();
        pause();
    }

    // ---- 7. Register Student ----
    void registerStudent() {
        showHeader();
        cout << "  REGISTER NEW STUDENT" << endl;
        drawLine();

        string name, roll, dept;
        cout << "  Full Name   : ";
        cin.ignore();
        getline(cin, name);
        cout << "  Roll Number : ";
        getline(cin, roll);
        cout << "  Department  : ";
        getline(cin, dept);

        if (name.empty() || roll.empty()) {
            cout << "\n  [ERROR] Name and Roll Number are required!" << endl;
        } else {
            if (dept.empty()) dept = "CS";
            students.push_back(Student(name, roll, dept));
            cout << "\n  [SUCCESS] Student \"" << name << "\" registered successfully!" << endl;
        }
        cout << "\n  Press Enter to continue...";
        cin.get();
    }

    // ---- 8. Dashboard ----
    void showDashboard() {
        showHeader();
        cout << "  DASHBOARD" << endl;
        drawLine();

        int total = books.size();
        int avail = 0, issued = 0;
        for (auto& b : books) {
            if (b.isAvailable()) avail++;
            else issued++;
        }

        cout << "\n  Total Books      : " << total << endl;
        cout << "  Available Books  : " << avail << endl;
        cout << "  Issued Books     : " << issued << endl;
        cout << "  Total Students   : " << students.size() << endl;

        if (issued > 0) {
            cout << "\n  Currently Issued Books:" << endl;
            drawLine(50);
            cout << left
                 << "  " << setw(30) << "Book Title"
                 << setw(15) << "Issued To" << endl;
            drawLine(50);
            for (auto& b : books) {
                if (!b.isAvailable()) {
                    cout << "  " << setw(30) << b.getTitle().substr(0, 28)
                         << setw(15) << b.getIssuedTo() << endl;
                }
            }
        }
        drawLine();
        pause();
    }

    // ---- Main Loop ----
    void run() {
        int choice;
        do {
            showHeader();
            showMenu();
            cin >> choice;

            switch (choice) {
                case 1: viewAllBooks();     break;
                case 2: searchBooks();      break;
                case 3: addBook();          break;
                case 4: issueBook();        break;
                case 5: returnBook();       break;
                case 6: viewStudents();     break;
                case 7: registerStudent();  break;
                case 8: showDashboard();    break;
                case 0:
                    showHeader();
                    cout << "\n  Thank you for using FAST University Library System!" << endl;
                    cout << "  Goodbye!\n" << endl;
                    break;
                default:
                    cout << "\n  Invalid choice! Try again." << endl;
                    pause();
            }
        } while (choice != 0);
    }
};

// ===================== Main =====================
int main() {
    Library library("FAST University Library");
    library.run();
    return 0;
}
