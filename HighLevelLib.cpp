#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
using namespace std;

/**
 * The clearScreen function clears the console screen.
 */
void clearScreen() {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
}

/* The Savable class is an abstract base class that provides methods for converting an object to a
string representation and saving it to a file. */
class Savable {
protected:
    virtual string to_string() const = 0;
    virtual void save(const string& filename, ios_base::openmode mode) const = 0;
};

/* The above class is a base class called "Person" that represents a person with a name and password,
and provides methods to get and set the name and password, as well as a virtual method to display
information about the person. */
class Person {
protected:
    string name_;
    string password_;

public:
    Person(string name = "", string password = "") : name_(name), password_(password) {}
    Person(const Person& other) : name_(other.name_), password_(other.password_) {}

    string get_name() const { return name_; }
    string get_pass() const { return password_; }

    void set_name(string name) {  name_ = name; }
    void set_pass(string pass) {  password_ = pass; }

    virtual void assign(const Person& other) {
        name_ = other.name_;
        password_ = other.password_;
    }
    virtual void display_info() const = 0;

    virtual ~Person() {}
};

/* The Client class represents a client with a unique ID, phone number, and a list of rented books. */
class Client : public Person, public Savable {
private:
    string id_;
    string phone_number_;
    vector<string> rented_books_; 

public:
    Client(string name = "", string password = "", string id = "", string phone_number = "", vector<string> rented_books = {}) 
        : Person(name, password), id_(id), phone_number_(phone_number), rented_books_(rented_books) {}
    Client(const Client& other) 
        : Person(other), id_(other.id_), phone_number_(other.phone_number_), rented_books_(other.rented_books_) {}

    string get_id() const { return id_; }
    string get_phone_number() const { return phone_number_; }
    vector<string> get_rented_books() const { return rented_books_; }

    void set_id(string id) { id_ = id; }
    void set_phone_number(string phone_number) { phone_number_ = phone_number; }
    void set_rented_books(vector<string>& rented_books) { rented_books_ = rented_books; }

    virtual void assign(const Client& other) {
        Person::assign(other);
        id_ = other.id_;
        phone_number_ = other.phone_number_;
        rented_books_ = other.rented_books_;
    }
    virtual void display_info() const override {
        cout << "ID: " << id_ << endl;
        cout << "Name: " << name_ << endl;
        cout << "Phone number: " << phone_number_ << endl;
        cout << "Rented books: " << endl;
        for (auto book : rented_books_) 
            cout << "- " << book << endl; 
    }

    /**
     * The function converts the object's data into a JSON string format.
     * 
     * @return a string representation of an object in JSON format.
     */
    virtual string to_string() const override {
        stringstream ss;
        ss << "{" << endl;
        ss << "  \"name\": \"" << name_ << "\"," << endl;
        ss << "  \"password\": \"" << password_ << "\"," << endl;
        ss << "  \"id\": \"" << id_ << "\"," << endl;
        ss << "  \"phone_number\": \"" << phone_number_ << "\"," << endl;
        ss << "  \"rented_books\": [" << endl;
        int bnum = rented_books_.size();
        for (int i = 0; i < bnum; i++) {
            ss << "    \"" << rented_books_.at(i) << "\"";
            (i == bnum - 1)? ss << endl : ss << "," << endl;   
        }
        ss << "  ]" << endl;
        ss << "}";
        return ss.str();
    }
    /**
     * The function saves the object's data to a file with the specified filename and open mode.
     * 
     * @param filename The filename parameter is a string that specifies the name of the file to be
     * saved.
     * @param mode The `mode` parameter is an optional parameter that specifies the mode in which the
     * file should be opened. It is of type `ios_base::openmode`, which is an enumeration type that
     * represents the different modes in which a file can be opened.
     */
    virtual void save(const string& filename, ios_base::openmode mode = ios_base::app) const override {
        ofstream file;
        file.open(filename, mode);
        if (!file.is_open()) 
            throw runtime_error("Failed to open file {" + filename + "} for saving.");
        file << this->to_string() << endl;
        file.close();
    }

    /* The ClientLoader class is responsible for loading client information from a file and storing it
    in a vector of Client objects. */
    class ClientLoader {
    public:
        static void load(const string& filename) {
            ifstream file(filename);
            if (!file.is_open()) 
                throw runtime_error("Failed to open file {" + filename + "} for loading.");
            string line;
            while (getline(file, line)) {
                if (line == "{") {
                    Client cl;
                    cl.name_ = extract_client_info(file);
                    cl.password_ = extract_client_info(file);
                    cl.id_ = extract_client_info(file);
                    cl.phone_number_ = extract_client_info(file);
                    cl.rented_books_ = extract_rented_books(file);
                    clients.push_back(cl);
                }
            }
        }

    private:
        ClientLoader() {}
        static string extract_client_info(ifstream& file) {
            string line;
            getline(file, line);
            return line.substr(line.find('\"', line.find(':')) + 1, line.rfind('\"') - line.find('\"', line.find(':')) - 1);
        }
        static vector<string> extract_rented_books(ifstream& file) {
            vector<string> rented_books;
            string line;
            while (getline(file, line)) {
                if (line.find(':') != string::npos) 
                    continue;
                if (line.find(']') != string::npos) 
                    break;
                string bookName = line.substr(line.find('\"')+1, line.rfind('\"')-line.find('\"')-1);
                rented_books.push_back(bookName);
            }
            return rented_books;
        }
    };

    static vector<Client> clients;
};
vector<Client> Client::clients = {};


/* The MenuItem class represents a menu item with a title and an associated action. */
class MenuItem {
private:
    using Action = void (*)(); 
    string title_;
    void (*action_)();

public:
    MenuItem(const string& title, void (*action)()) : title_(title), action_(action) {}

    string get_title() const { return title_; }
    void execute() { action_(); }
};

/* The Menu class is an abstract class that represents a menu with a title and a list of menu items,
and provides methods for adding items, showing the menu, running the menu, and cleaning up the
memory. */
class Menu {
private:
    string title_;
    vector<MenuItem*> items_;

protected:
    virtual string call_back() = 0;
    virtual void add_item(MenuItem* item) {
        items_.push_back(item);
    }
    virtual void show() const {
        cout << title_ << endl;
        for (int i = 0; i < items_.size(); ++i) 
            cout << i + 1 << ". " << items_[i]->get_title() << endl;
    }

public:
    Menu(string title) : title_(title) {}

    virtual void run() {
        int choice;
        while (true) {
            show();
            cout << "0. " << call_back() << endl;
            cout << ">>> ";
            cin >> choice; cin.ignore();
            if (choice == 0)
                break;
            else if (choice > 0 && choice <= items_.size())
                items_[choice - 1]->execute();
            else cout << "Not valid input!" << endl;
        }
    }

    virtual ~Menu() {
        while (!items_.empty()) {
            MenuItem* item_ptr = items_.back();
            if (item_ptr) 
                delete item_ptr;
            items_.pop_back();
        }
        items_.clear();
    }
};

/**
 * The login function takes user input for ID and password, checks if there is a matching client in the
 * list of clients, and returns a pointer to the client if found, otherwise returns nullptr.
 * 
 * @return a pointer to a Client object.
 */
Client* login() {
    string id, password;
    cout << "Enter your ID:" << endl;
    cout << ">>> ";
    getline(cin, id);

    cout << "Enter your password:" << endl;
    cout << ">>> ";
    getline(cin, password);

    for (auto& client : Client::clients) 
        if (client.get_id() == id && client.get_pass() == password)
            return &client;
    return nullptr;
}

/**
 * The function generates a random UUID (Universally Unique Identifier) string with hyphens inserted at
 * the appropriate positions.
 * 
 * @return The function `generateUUID()` returns a string that represents a randomly generated UUID
 * (Universally Unique Identifier).
 */
string generateUUID() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 15);

    ostringstream oss;
    for (int i = 0; i < 32; ++i) {
        int randomValue = dis(gen);

        // Insert hyphens at the appropriate positions
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            oss << '-';
        }

        oss << hex << randomValue;
    }

    return oss.str();
}

/**
 * The signup function allows a user to enter their name, password, and phone number, generates a
 * random ID, creates a new Client object with the entered information and generated ID, saves the new
 * client information to a file, and displays the generated ID to the user.
 */
void signup() {
    string name, password, id, phone_number;

    cout << "Enter your name:" << endl;
    cout << ">>> ";
    getline(cin, name);

    cout << "Enter your password:" << endl;
    cout << ">>> ";
    getline(cin, password);

    id = generateUUID();

    cout << "Enter your phone number:" << endl;
    cout << ">>> ";
    getline(cin, phone_number);

    // create a new Client object with the entered info and generated ID
    Client new_client(name, password, id, phone_number, {});
    Client::clients.push_back(new_client);

    // save the new client info to a file (e.g., clients.txt)
    new_client.save("clients.txt");

    cout << "Successfully signed up! Your ID is: " << id << endl;
}

/* The ClientMenu class is a subclass of the Menu class and provides a menu for clients with an option
to edit their information. */
class ClientMenu : public Menu {
private:
    string call_back() override { return "Exit"; }
public:
    ClientMenu(Client* cl) : Menu("**** Client Menu ****") {
        add_item(new MenuItem("Edit Info", []() {
            cout << "Are you sure?(y/n)" << endl;
            cout << ">>> ";
            char ch; cin >> ch; cin.ignore();
            if (ch == 'y' || ch == 'Y') {
                //TODO
            }
        }));
    }
};

/* The LoginMenu class is a subclass of the Menu class and provides options for admin and client login
functionality. */
class LoginMenu : public Menu {
private:
    string call_back() override { return "Back"; }
public:
    LoginMenu() : Menu("**** Login Menu ****") {
        add_item(new MenuItem("Admin", []() {
            //TODO: Admin login functionality
        }));
        add_item(new MenuItem("Client", []() {
            Client* cl = login();
            if (!cl) {
                clearScreen();
                ClientMenu client_menu(cl);
                client_menu.run();
            }
            else
                cout << "Invalid ID or Password combination. Please try again." << endl;

        }));
    }
};

/* The MainMenu class represents the main menu of a program and contains options for login, signup, and
testing data loading. */
class MainMenu : public Menu {
private:
    string call_back() override { return "Exit"; }
public:
    MainMenu() : Menu("**** Main Menu ****") {
        add_item(new MenuItem("Login", []() {
            clearScreen();
            LoginMenu login_menu;
            login_menu.run();
        }));
        add_item(new MenuItem("Signup", []() {
            clearScreen();
            signup();
        }));
        // add_item(new MenuItem("Test Load", []() {
        //     for (auto &client : Client::clients) {
        //         client.display_info();
        //         cout << "===========================================" << endl;
        //     }
        // }));
    }
};

int main(int argc, char** argv) {
    // Client::ClientLoader::load("clients.txt");
    MainMenu main_menu;
    main_menu.run();
    return 0;
}
