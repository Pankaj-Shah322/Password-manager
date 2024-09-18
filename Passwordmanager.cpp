#include <iostream>
#include <string>
#include <unordered_map>
#include <fstream>
#include <ctime>
#include <random>
#include <stdexcept> // For exception handling
#include <iomanip>   // For formatting

using namespace std;

// PasswordManager class definition
class PasswordManager
{
private:
    unordered_map<string, string> passwords; // Stores website and password pairs
    string masterPassword;
    const string filename = "passwords.txt";

    // Function to generate a random password using better randomness
    string generatePassword(int length)
    {
        const char charset[] = "0123456789"
                               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                               "abcdefghijklmnopqrstuvwxyz"
                               "!@#$%^&*()";
        string password;
        random_device rd;                                                // Obtain a random number from hardware
        mt19937 generator(rd());                                         // Seed the generator
        uniform_int_distribution<> distribution(0, sizeof(charset) - 2); // Define the range
        for (int i = 0; i < length; ++i)
        {
            password += charset[distribution(generator)];
        }
        return password;
    }

    // Function to save passwords to a file with error checking
    void saveToFile()
    {
        ofstream file(filename);
        if (!file)
        {
            throw runtime_error("Unable to open file for writing.");
        }
        for (const auto &pair : passwords)
        {
            file << pair.first << ":" << pair.second << endl;
        }
    }

    // Function to load passwords from a file with error checking
    void loadFromFile()
    {
        ifstream file(filename);
        if (!file)
        {
            cerr << "Unable to open file for reading. Starting fresh." << endl;
            return;
        }
        string line;
        while (getline(file, line))
        {
            size_t pos = line.find(':');
            if (pos != string::npos)
            {
                string website = line.substr(0, pos);
                string password = line.substr(pos + 1);
                passwords[website] = password;
            }
        }
    }

public:
    PasswordManager(const string &masterPwd) : masterPassword(masterPwd)
    {
        loadFromFile(); // Load passwords from file at startup
    }

    // Function to add a new password
    void addPassword(const string &website, const string &password)
    {
        if (password.length() < 8)
        {
            throw runtime_error("Password must be at least 8 characters long.");
        }
        passwords[website] = password;
        saveToFile();
    }

    // Function to retrieve a password
    string getPassword(const string &website)
    {
        if (passwords.find(website) == passwords.end())
        {
            throw runtime_error("Website not found.");
        }
        return passwords[website];
    }

    // Function to generate and add a new password
    void generateAndAddPassword(const string &website, int length)
    {
        if (length < 8)
        {
            throw runtime_error("Password length must be at least 8 characters.");
        }
        string newPassword = generatePassword(length);
        addPassword(website, newPassword);
        cout << "Generated password for " << website << ": " << newPassword << endl;
    }

    // Function to authenticate the user
    bool authenticate(const string &password)
    {
        return password == masterPassword;
    }
};

// Main function to interact with the password manager
int main()
{
    string masterPwd;
    cout << "Set your master password: ";
    cin >> masterPwd;

    PasswordManager manager(masterPwd);

    string inputPwd;
    cout << "Enter master password to access the manager: ";
    cin >> inputPwd;

    if (manager.authenticate(inputPwd))
    {
        cout << "Authentication successful.\n";

        while (true)
        {
            cout << "\nMenu:\n";
            cout << "1. Add Password\n";
            cout << "2. Get Password\n";
            cout << "3. Generate and Add Password\n";
            cout << "4. Exit\n";
            cout << "Enter choice: ";

            int choice;
            cin >> choice;

            if (choice == 4)
            {
                cout << "Exiting...\n";
                break;
            }

            string website;
            string password;
            int length;

            try
            {
                switch (choice)
                {
                case 1:
                    cout << "Enter website: ";
                    cin >> website;
                    cout << "Enter password: ";
                    cin >> password;
                    manager.addPassword(website, password);
                    cout << "Password added.\n";
                    break;
                case 2:
                    cout << "Enter website: ";
                    cin >> website;
                    cout << "Password for " << website << ": " << manager.getPassword(website) << endl;
                    break;
                case 3:
                    cout << "Enter website: ";
                    cin >> website;
                    cout << "Enter password length (min 8): ";
                    cin >> length;
                    manager.generateAndAddPassword(website, length);
                    break;
                default:
                    cout << "Invalid choice.\n";
                    break;
                }
            }
            catch (const runtime_error &e)
            {
                cerr << "Error: " << e.what() << endl;
            }
        }
    }
    else
    {
        cout << "Authentication failed.\n";
    }

    return 0;
}
