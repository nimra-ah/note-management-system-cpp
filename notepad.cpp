#include <iostream>
#include <string>
#include <cstdio> 
#include <ctime>
#include <fstream> 
#include <vector>

using namespace std;

class Error
{
    
    public:
    string e;
    Error()
    {}
    Error(string s)
    {
        e=s;
    }
};

string getCurrentTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);

    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", ltm);
    return string(buf);
}

class BasicNote {
protected:
    static const int charLimit;
    string title;
    string creationTime;
  
public:

    BasicNote() {
        title = "";
        
        creationTime = getCurrentTime();
    }
    BasicNote(string title, string creationTime) {
        this->title = title;
        this->creationTime = creationTime;
    }

    virtual void display() = 0;
    virtual void write_content() = 0;
    virtual void edit() = 0;
    virtual void saveToFile() const = 0;
    virtual void loadFromFile()  = 0;
    

    virtual void rename(const string& newTitle) {
        title = newTitle;
    }
    
    void edit_content(string& content ){
    	int option;
    	cout << "\n--- Edit Menu ---\n";
	    cout << "1. Append new content\n";
	    cout << "2. Replace part of content\n";
	    cout << "3. Clear all content\n";
	    cout << "Enter your choice: ";
	    cin >> option;
	    cin.ignore();
	    
	    if(option==1){
	    	string newText;
	    	cout<<"Enter your new content: "<<endl;
	    	getline(cin,newText);
	    	content.append(" "+ newText);
	    	
	    	if (content.length() > charLimit) {
	    		content.resize(content.length() - (newText.length() - 1));
	    		cout << "\nLimit exceeded! You entered " << content.length() << "/" << charLimit << "\n";

			}
	    	else {
            cout << " Appended successfully.\n";
        }
		}
		else if(option==2){
			string to_replace, replace_with;
			cout<<"Enter the content you want to replace: ";
			getline(cin, to_replace);
			
			cout<<"Enter new content : ";
			getline(cin, replace_with);
			
			int pos=content.find(to_replace); 
			if(pos != -1){          
				string original= content;  
			    content.replace(pos, to_replace.length(),replace_with );
			    
			    if (content.length() > charLimit){
	    		content=original;
	    		cout << "\n Limit exceeded! You entered " << content.length() << "/" << charLimit << "\n" << ".!!! Replacement canceled. !!!\n";
			}
			else {
                cout << "  Replacement successful.\n";
            }
			}
			else {
            cout << " Text not found.\n";
            }
		}
		else if (option == 3) {
        content.clear();
        cout << " Content cleared.\n";
    	} else {
        cout << "Invalid choice.\n";
    }
	}
    virtual ~BasicNote() {}
};
const int BasicNote::charLimit = 1000 ;


class PlainText : public BasicNote {
    string content;

    
     
public:
    PlainText() {
        content = "";   
    }    
    void write_content() override {
    	
    	
        cout << "Enter content for PlainText note (word limit 1000):  ";
        getline(cin, content);
        
        if (content.length() > charLimit) {
        	 cout << "\nLimit exceeded! You entered " << content.length() << "/" << charLimit << "\n";
             content = "";
             cout << "\n Content was too long and has been cleared.\n";
		}
		else{
			
            cout << "\n Content saved successfully.\n";
		}

    }
	void edit() override {
    cout << "\n--- Editing PlainText Note ---\n";

    edit_content(content);
	}

    void display() override {
        cout << "Title: " << title << endl;
        cout << "Created: " << creationTime << endl;
        cout << "Content: " << content << endl;
        cout << "Characters entered: " << content.length() << "/" << charLimit << endl;
    }

    void saveToFile() const override
    {
    ofstream file(title + "_plain.txt", ios::binary);
    if (!file) {
        throw Error ("Error opening file for binary write.");
        return;
    }

    unsigned int titleLength = title.length();
    file.write(reinterpret_cast<const char*>(&titleLength), sizeof(titleLength));
    file.write(title.data(), titleLength);  

    unsigned int timeLength = creationTime.length();
    file.write(reinterpret_cast<const char*>(&timeLength), sizeof(timeLength));
    file.write(creationTime.data(), timeLength);

     unsigned int  contentLength = content.length();
    file.write(reinterpret_cast<const char*>(&contentLength), sizeof(contentLength));
    file.write(content.data(), contentLength); 
    file.close();
    cout << "\nNote saved successfully in binary format.\n";
}

     void loadFromFile() override
     {
    ifstream file(title + "_plain.txt", ios::binary);
    if (!file) {
       throw  Error ("Error opening file for binary write.");
        return;
    }

    unsigned int len;

 
    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    title.resize(len);
    file.read(&title[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    creationTime.resize(len);
    file.read(&creationTime[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    content.resize(len);
    file.read(&content[0], len);
     display();
    file.close();
    cout << "Note loaded successfully.\n";
}


    ~PlainText() {}
};

class Encrypted : public BasicNote {
    string encryptedContent;
    bool islocked;

    static string globalPassword;
    static bool passwordSet;

public:
    Encrypted() : encryptedContent(""), islocked(true) {}

    void write_content() override {
        cout << "\n--- Write Encrypted Content ---\n\n";
        cout << "Enter encrypted content: ";
        getline(cin, encryptedContent);
        islocked = true;

        
        if (encryptedContent.length() > charLimit) {
    		cout << "\nLimit exceeded! You entered " << encryptedContent.length() << "/" << charLimit << "\n";
    		encryptedContent = "";
    		cout << "Content was too long and has been cleared.\n";
		} else {
    		cout << "Content saved successfully.\n";
}


    }
    

    void display() override {
        cout << "\n--- Preview Encrypted Note ---\n\n";
        if (!verifyPassword()) {
            cout << "\nIncorrect password. Cannot display.\n\n";
            return;
        }
        cout << "Title: " << title << endl;
        cout << "Created: " << creationTime << endl;
        cout << "Encrypted Content: " << encryptedContent << endl;
        cout << "Characters entered: " << encryptedContent.length() << "/" << charLimit << endl;
        cout << "\n-------------------------------\n\n";
        islocked = true;
    }

    void edit() override {
        cout << "\n--- Edit Encrypted Content ---\n\n";
        if (!verifyPassword()) {
            cout << "\nIncorrect password. Cannot edit.\n\n";
            return;
        }

        cout << "Current encrypted content:\n" << encryptedContent << "\n\n";

        cout << "Do you want to edit the content? (y/n): ";
        string choice;
        getline(cin, choice);
        if (choice == "y" || choice == "Y"){
        	edit_content(encryptedContent);

		}

		else {
            cout << "\nContent unchanged.\n\n";
        }
        islocked = true;
    }


    static void ensurePasswordSet() {
        if (!passwordSet) {
            string pass1, pass2;
            do {
                cout << "\n--- Set Password for Encrypted Notes ---\n\n";
                cout << "Set global password for all encrypted notes: ";
                getline(cin, pass1);
                cout << "Confirm password: ";
                getline(cin, pass2);

                if (pass1 != pass2) {
                    cout << "\nPasswords do not match. Try again.\n\n";
                }
            } while (pass1 != pass2);

            globalPassword = pass1;
            passwordSet = true;
            cout << "\nPassword set successfully.\n\n";
        }
    }

void saveToFile() const override {
    ofstream file(title + "_encrypted.txt", ios::binary);
    if (!file) {
        throw Error ("Error opening file for binary write.");
        return;
    }

    unsigned int titleLength = title.length();
    file.write(reinterpret_cast<const char*>(&titleLength), sizeof(titleLength));
    file.write(title.data(), titleLength);

    unsigned int timeLength = creationTime.length();
    file.write(reinterpret_cast<const char*>(&timeLength), sizeof(timeLength));
    file.write(creationTime.data(), timeLength);

    unsigned int encLength = encryptedContent.length();
    file.write(reinterpret_cast<const char*>(&encLength), sizeof(encLength));
    file.write(encryptedContent.data(), encLength);

    file.close();
    cout << "Encrypted note saved successfully.\n";
}

void loadFromFile() override {
    ifstream file(title + "_encrypted.txt", ios::binary);
    if (!file) {
       throw Error ("Error opening file for binary write.");
        return;
    }

    unsigned int len;

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    title.resize(len);
    file.read(&title[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    creationTime.resize(len);
    file.read(&creationTime[0], len);

    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    encryptedContent.resize(len);
    file.read(&encryptedContent[0], len);

    display();
    file.close();
    cout << "Encrypted note loaded successfully.\n";
}


private:
    static bool verifyPassword() {
        if (!passwordSet) {
            cout << "\nPassword not set. Cannot proceed.\n\n";
            return false;
        }
        cout << "\nEnter password: ";
        string input;
        getline(cin, input);
        return input == globalPassword;
    }
};


string Encrypted::globalPassword = "";
bool Encrypted::passwordSet = false;


class CheckList : public BasicNote {
    vector<string> taskList;  
    vector<bool> isCompleted;  

public:
    CheckList() {}

    void write_content() override {
        taskList.clear();
        isCompleted.clear();

        int total;
        cout << "How many tasks do you want to add? ";
        cin >> total;
        cin.ignore(); 

        for (int i = 0; i < total; ++i) {
            string task;
            char done;

            cout << "Task " << i + 1 << ": ";
            getline(cin, task);

            cout << "Is this task completed? (y/n): ";
            cin >> done;
            cin.ignore();

            taskList.push_back(task);
            isCompleted.push_back(done == 'y' || done == 'Y');
        }
    }

    void display() override {
        cout << "Title: " << title << endl;
        cout << "Created: " << creationTime << endl;
        cout << "Checklist:\n";

        for (int i = 0; i < taskList.size(); ++i) {
            cout << (i + 1) << ". [" << (isCompleted[i] ? "Completed" : "Pending") << "] " << taskList[i] << endl;
        }
    }

    void edit() override {
        if (taskList.empty()) {
            cout << "No tasks to edit.\n";
            return;
        }

        int taskNumber;
        cout << "Which task number do you want to edit? ";
        cin >> taskNumber;
        cin.ignore();

        if (taskNumber <= 0 || taskNumber > taskList.size()) {
            cout << "Invalid task number.\n";
            return;
        }

        string newText;
        char newStatus;

        cout << "Enter new task text: ";
        getline(cin, newText);

        cout << "Is this task completed? (y/n): ";
        cin >> newStatus;
        cin.ignore();

        taskList[taskNumber - 1] = newText;
        isCompleted[taskNumber - 1] = (newStatus == 'y' || newStatus == 'Y');

        cout << "Task updated successfully.\n";
    }

void saveToFile() const override {
    ofstream file(title + "_checklist.txt", ios::binary);
    if (!file) {
       throw Error ("Error opening file for binary write..");
        return;
    }


    unsigned int titleLength = title.length();
    file.write(reinterpret_cast<const char*>(&titleLength), sizeof(titleLength));
    file.write(title.data(), titleLength);


    unsigned int timeLength = creationTime.length();
    file.write(reinterpret_cast<const char*>(&timeLength), sizeof(timeLength));
    file.write(creationTime.data(), timeLength);


    unsigned int taskCount = taskList.size();
    file.write(reinterpret_cast<const char*>(&taskCount), sizeof(taskCount));

    for (unsigned int i = 0; i < taskCount; ++i) {
        unsigned int taskLength = taskList[i].length();
        file.write(reinterpret_cast<const char*>(&taskLength), sizeof(taskLength));
        file.write(taskList[i].data(), taskLength);

       bool status = isCompleted[i];
       file.write(reinterpret_cast<const char*>(&status), sizeof(bool));

    }

    file.close();
    cout << "\nChecklist saved successfully.\n";
}
void loadFromFile() override {
    ifstream file(title + "_checklist.txt", ios::binary);
    if (!file) {
      throw Error ("Error opening file for binary write...");
        return;
    }

    unsigned int len;


    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    title.resize(len);
    file.read(&title[0], len);


    file.read(reinterpret_cast<char*>(&len), sizeof(len));
    creationTime.resize(len);
    file.read(&creationTime[0], len);

    unsigned int taskCount;
    file.read(reinterpret_cast<char*>(&taskCount), sizeof(taskCount));

    taskList.clear();
    isCompleted.clear();

 
    for (unsigned int i = 0; i < taskCount; ++i) {
        file.read(reinterpret_cast<char*>(&len), sizeof(len));
        string task(len, '\0');
        file.read(&task[0], len);

        bool done;
        file.read(reinterpret_cast<char*>(&done), sizeof(bool));

        taskList.push_back(task);
        isCompleted.push_back(done);
    }

    file.close();
    display();
    cout << "\nChecklist loaded successfully.\n";
}


    ~CheckList() {}
};


int main() {
    char option;
    cout << "===== Notepad App =====\n\n";
 do{
     
    int choice;
    cout << "Select note type:\n";
    cout << "1. PlainText\n";
    cout << "2. Encrypted\n";
    cout << "3. Checklist\n";
    cout << "4. Exit\n";
    cout << "Enter choice: ";
    cin >> choice;
    cin.ignore();

    BasicNote* note = nullptr;

    if (choice == 1) {
        note = new PlainText();
    } 
    else if (choice == 2) {
        Encrypted::ensurePasswordSet();
        note = new Encrypted();
    } 
    else if (choice == 3) {
        note = new CheckList();
    } 
    else if (choice == 4) {
        cout<<"\nExiting program\n";
        return 1;
    } 
    
    else {
        cout << "\nInvalid choice! \n";
        
    }

    string name;
    cout << "\nEnter note title: ";
    getline(cin, name);
    note->rename(name);

    bool exitMenu = false;
    while (!exitMenu) {
        cout << "\n--- Note Menu ---\n";
        cout << "1. Write (Create or overwrite note)\n";
        cout << "2. Read from file\n";
        cout << "3. Preview (View note content)\n";
        cout << "4. Edit note\n";
        cout << "5. Delete note file\n";
        cout << "6. Exit\n";
        cout << "Enter your choice: ";

        int action;
        cin >> action;
        cin.ignore();

        cout << endl;
     
        switch (action) {
            
            case 1:
            try{
                note->write_content();
                note->saveToFile();
            }
               catch( Error e1)
             {
                cout<<e1.e;
              }

                break;
            
            
            case 2:
            try{
                note-> loadFromFile();
            }
                     catch(Error e1)
             {
                cout<<e1.e;
              }

                break;

            case 3:
                note->display();
                break;

            case 4:
                note->edit();
                note->saveToFile();
                break;

            case 5: {
                string filename;
                if (choice == 1) filename = name + "_plain.txt";
                else if (choice == 2) filename = name + "_encrypted.txt";
                else if (choice == 3) filename = name + "_checklist.txt";

                if (remove(filename.c_str()) == 0) {
                    cout << "Note file deleted successfully!\n";
                     exitMenu = true;
                
                } else {
                    cout << "Error deleting the note file.\n";
                }
                break;
            
}
            case 6:
                cout << "Exiting note menu.\n";
                exitMenu = true;
                break;
  
            default:
                cout << "Invalid choice. Please try again.\n";
        }
        
     
    }
 
    cout<<"\nDo you want to continue y/n:";
    cin>>option;
     delete note;
 }while(option=='y');
 cout << "\nExiting Program. Goodbye!\n";
 
 
   
    return 0;
}
