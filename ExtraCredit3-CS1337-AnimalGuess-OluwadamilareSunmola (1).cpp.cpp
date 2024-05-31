





#include <cctype>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

struct animalNode {
    string question;
    string guess;
    animalNode *yesAnswer;
    animalNode *noAnswer;
    // Constructor initializes question to empty, guess to "lizard", and both answers to nullptr
    animalNode() : question(""), guess("lizard"), yesAnswer(nullptr), noAnswer(nullptr) {}
};

//Converting each character in the string to lowercase leters
void lowerCaseLetter(string &str) {
    for (char &c : str) {
        c = tolower(c);
    }
}

void deleteTree(animalNode *node) {
    if (node) {
        deleteTree(node->yesAnswer); //deleting the yesanswer part of the tree
        deleteTree(node->noAnswer); //deleting the noanswer part of the tree
        delete node; //deleting the current node
    }
}

// Function to save the tree to a file in a format that can be loaded later
void saveTree(animalNode *node, ofstream &file) {
    if (node == nullptr) { 
        return; //if the above code is true, function to cease to continue
    }
    // Save the node as a question or a guess
     //cout << "Debug: Saving " << (!node->question.empty() ? "Question: " + node->question : "Guess: " + node->guess) << endl;
    if (!node->question.empty()) {
        file << "Q\n" << node->question << "\n";
    } else {
        file << "G\n" << node->guess << "\n";
    }
    
    //A recursive function to save the "yes" and "no" subtree
    saveTree(node->yesAnswer, file);
    saveTree(node->noAnswer, file);
}

// Function to load the tree from a file
void loadTree(animalNode *&node, ifstream &file) {
    string type;
    string line;

    while (file >> type) {
        getline(file, line);  // Skip to the end of the line after reading 'Q' or 'G'
        getline(file, line);  // Read the actual question or guess

        if (node == nullptr) {
            node = new animalNode();
        }
        
        // Depending on the type of current information, loads the data as a question or guess and recurse if necessary
        if (type == "Q") {
            node->question = line;
            node->yesAnswer = new animalNode();
            node->noAnswer = new animalNode();
            loadTree(node->yesAnswer, file);  // Recursively load the 'yes' subtree
            loadTree(node->noAnswer, file);   // Recursively load the 'no' subtree
        } else if (type == "G") {
            node->guess = line;
            return;
        }
        // cout << "Debug: Loaded " << (type == "Q" ? "Question: " + line : "Guess: " + line) << endl;
        
    }
}

// Saves the current state of the game tree to a file
void saveToFile(animalNode *root, const string &filename) {
    ofstream fileOut(filename);
    if (!fileOut) {
        cerr << "Error: File could not be opened for writing." << endl;
        return;
    }
    saveTree(root, fileOut);
    fileOut.close();
}

// Loads the game tree from a file or starts a new game if the file doesn't exist
void loadFromFile(animalNode *&root, const string &filename) {
    ifstream fileIn(filename);
    //cout << "Debug: Starting to load from file: " << filename << endl;
    if (!fileIn) {
        
        cerr << "No existing database, starting a new game." << endl;
        root = nullptr; // Clear the existing tree
    } else {
        loadTree(root, fileIn);
        //cout << "Debug: Finished loading from file." << endl;
        fileIn.close();
    }
}

void askingQuestion(animalNode *&root) {
    animalNode *currentNode = root;
    if (currentNode == nullptr) { // i am checking to see if the newly created root node is null, this can not be possible because we allocated memory in the main function
        root = currentNode = new animalNode(); // Start with a new game
    }

    cout << "\n\nLet's play the \"Guess the Animal\" game." << endl;
    cout << "Think of an animal and hit enter when you are ready" << endl;
    cin.get();

    // checking if the question node is not empty
    while (currentNode != nullptr && !currentNode->question.empty()) {
        cout << currentNode->question << " (y/n): ";
        string useranswer;
        cin >> useranswer;
        cin.ignore(100, '\n');
        lowerCaseLetter(useranswer);

        if (useranswer == "y" || useranswer == "yes") {
            if (currentNode->yesAnswer == nullptr) currentNode->yesAnswer = new animalNode();
            currentNode = currentNode->yesAnswer; //go to the yesanswer part of the tree
        } else {
            if (currentNode->noAnswer == nullptr) currentNode->noAnswer = new animalNode();
            currentNode = currentNode->noAnswer; //go to the noanswer part of the tree
        }
    }

    if (currentNode == nullptr) return;

    string user_input;
    cout << "Is it a(an) " << currentNode->guess << "? (y/n): "; //testing the current guess by asking the user if it is correct
    cin >> user_input;
    cin.ignore(100, '\n');
    lowerCaseLetter(user_input);

    if (user_input == "y" || user_input == "yes") {
        cout << "Great!!! You owe me lunch for guessing it right.\n";
    } 
    // if the guess is incorrect then we will get the new information regarding the new animal and allocate a node for it
    else {
        string new_animal, new_question, user_response;
        cout << "\nBummer! What animal were you thinking of? ";
        getline(cin, new_animal);
        cout << "What is a yes/no question that distinguishes a " << new_animal << " from a " << currentNode->guess << "? ";
        getline(cin, new_question);
        cout << "For a(an) " << new_animal << ", what is the answer to your question? (yes/no): ";
        getline(cin, user_response);
        lowerCaseLetter(user_response);

        currentNode->question = new_question;
        // Depending on the user's response, create new nodes for the 'yes' and 'no' answers
        if (user_response == "yes") {
            // Create a new node for the 'yes' answer and set its guess to the new animal
            currentNode->yesAnswer = new animalNode();
            currentNode->yesAnswer->guess = new_animal;
            currentNode->noAnswer = new animalNode();
            currentNode->noAnswer->guess = currentNode->guess;
        } else {
            // Create a new node for the 'yes' answer and set its guess to the current node's previous guess
            currentNode->yesAnswer = new animalNode();
            currentNode->yesAnswer->guess = currentNode->guess;
            currentNode->noAnswer = new animalNode();
            currentNode->noAnswer->guess = new_animal;
        }
        // Reset the current node's guess to empty as it now holds a question
        currentNode->guess = "";
    }
}

int main() {
    animalNode *rootNode = nullptr;
    string filename;
    
    //cout << "The current guess is " << rootNode->guess << endl;
    //cout << "The current value of yesanswer is " << rootNode->yesAnswer << endl;
    //cout << "The current value of noanswer is " << rootNode->noAnswer << endl;
    //cout << "The current value of question is " << rootNode->question << endl;
    //cout << "THe current value of rootNode is " << rootNode << endl;
    cout << "Enter database filename to load or hit ENTER to use default (animalTreeDB.txt): ";
    getline(cin, filename);
    if (filename.empty()) {
        filename = "animalTreeDB.txt";
    }

    loadFromFile(rootNode, filename);

    string playAnswer;
    do {
        askingQuestion(rootNode);
        cout << "Do you want to play again? (y/n): ";
        cin >> playAnswer;
        lowerCaseLetter(playAnswer);
        cin.ignore(200, '\n');
    } while (playAnswer == "yes" || playAnswer == "y");

    saveToFile(rootNode, filename);
    deleteTree(rootNode);
    cout << "\n\nThanks for playing!" << endl;

    return 0;
}
