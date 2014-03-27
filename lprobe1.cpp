/*  Scott Hale
    CS 302
    Assignment 5
    Part 2
    Probing
*/

#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <string>

using namespace std;

struct listItem{
    int value;
    bool vacant;
};


class listType{
    public:
        listType();
        void initList(listItem* , int primeLength);
        int hashValue(int x, int primeLength);
        bool insertValue(listItem *hashTable, int value, int &primeLength);
        bool findValue(listItem *hashTable, int value, int primeLength);
        int linear(int index, int primeLength);
        int quadratic(int index, int primeLength, int i);
        listItem* reHashTable(listItem* hashTable, int &primeLength);
        void newPrimeLength(int& primeLength);
        void setProbeAlgorithm(char key);
        void printInsertStats();
        void printFindStats();
        void printTable(listItem* hashTable, int primeLength);
        void initStats();

    private:
        bool isQuadratic;           // bool to determine linear or quadratic implementation

        int findAttempts;           // number of attempts to find a value
        int findCollisions;         // number of collisions in finding all values
        int findFailures;           // number of find fails

        int insertAttempts;         // number of attempts to insert a value
        int insertCollisions;       // number of collisions inserting all data
        int insertFailures;         // number of failures in finding values

        int reHashes;               // total number of times the table was rehashed
};

listType::listType(){
}

void listType::initList(listItem* hashTable, int primeLength){
/*Function initializes a new table so that all the vacant values are true*/
    for(int i = 0; i<primeLength; i++){
        hashTable[i].vacant = true;
    }
    return;
}

int listType::hashValue(int x, int primeLength){
/*hash function returns an index to store the value. index is the value % tablesize*/
    return (x%primeLength);
}

bool listType::insertValue(listItem *hashTable, int value, int &primeLength){
/*insert function: inserts a value into the table if the index returned by the
hashValue function is vacant. If the index is not vacant the insertValue function calls
one of the probing functions until a vacant index can be found or the failure condition
has been met (attempted inserts == table size). if the value is succesfully placed in
the table, the function returns trueto the main, otherwise it returns false to the main. */

    insertAttempts = 1;                         // counts the total number of insert attempts to determine 
                                                // failure condition.
    int i = 1;                                  // counter used for quadratic probing function
    int index = hashValue(value,primeLength);   // index is initialized to the hashValue
    int newIndex;                               // index used to increment the offset calculated by linear 
                                                // and quadratic functions.
    int originalIndex;                          // index used to store the base index for the quadratic 
                                                // function.
    originalIndex = index;

    if(hashTable[index].vacant){                // if initial hashValue index is vacant,  place the value, 
                                                // return success.
        hashTable[index].value = value;
        hashTable[index].vacant = false;
        return true;
    }
    else{
        while(!hashTable[index].vacant){        // loop while current table index is not vacant
            insertAttempts++;
            if(insertAttempts==primeLength){    // if the number of attempts to insert == table size, 
                                                // return failure.
                insertFailures++;
                return false;
            }else{                              // otherwise...
                insertCollisions++;             // increment collisions and call either the linear or 
                                                // quadratic probing function.
                if(isQuadratic){
                    newIndex = quadratic(originalIndex, primeLength, i);
                    i++;
                }else{
                    newIndex = linear(index, primeLength);
                }
                index = newIndex;
            }
        }                                       // end while loop when vacant index is found
        hashTable[index].value = value;         // write value to vacant index
        hashTable[index].vacant = false;        // set vacant bool to false
        return true;                            // return success
    }
}

bool listType::findValue(listItem *hashTable, int value, int primeLength){
    /*The find Value function probes the table for values. if a value is found the function
    returns true to the main, if the value is not found a value of false is retuened to the main.
    this function also keeps track of: the number of collisions that occur searching for a value,
    the number of attempts to find a value, and any failures to find a value*/

    findAttempts = 1;                           // number of attempts to find a value
    int i = 1;                                  // counter used for quadratic probing function
    int index = hashValue(value,primeLength);   // index is initialized to the hashValue
    int newIndex;                               // index used to increment the offset calculated by linear and 
                                                // quadratic functions
    int originalIndex;                          // index used to store the base index for the quadratic function
    originalIndex = index;

        while(!hashTable[index].vacant){            // while the current index is not vacant
            if(hashTable[index].value == value){    // if the index value == value we are looking for, 
                                                    // then return true.
                return true;
            }else{                                  // otherwise...
                if(findAttempts==primeLength){      // if number of attempts == table size, fail: return false
                    findFailures++;
                    return false;
                }else{                              // otherwise, use selected probing algorthm to search 
                                                    // through indicies
                    if(isQuadratic){
                        newIndex = quadratic(originalIndex, primeLength, i);
                        i++;
                    }else{
                        newIndex = linear(index, primeLength);
                    }
                    index = newIndex;
                    findAttempts++;
                    findCollisions++;
                }
            }
        }
        findFailures++;
        return false;                     // if a vacant index is encountered and value hasnt been found, fail
}

int listType::linear(int index, int primeLength){
    /*linear is a probing algorithm that increments an index by one and returns the value
    to the calling function. if the index value exceeds the table size it returns the
    index % table size*/

    index = index + 1;
    if (index >= primeLength){
        return (index%primeLength);
    }
    else{
        return index;
    }
}

int listType::quadratic(int index, int primeLength, int i){
    /*quadratic is a probing function that increments a base index by the square of value i. if
    the base index + i*i is greater than the table size qudaatic returns ((base index + i*i) % table size) */

    int nextIndex;  // used to store the value to be returned
    nextIndex = index + (i*i);
    if(nextIndex>=primeLength){
        return nextIndex%primeLength;
    }else{
        return nextIndex;
    }
}

listItem* listType::reHashTable(listItem* hashTable, int &primeLength){
    /*Ths reHashTable function takes an exiting table and its length as arguments. this function creates a new
    table of prime size that is larger than twice the size of the existing table. It then inserts all the 
    values from the existing table into the new table. Once all the values from the old table have been copied
    the old table id deleted and a pointer to the new table is returned to the main*/

    reHashes++;                             // counter for the number of rehashes
    int oldPrimeLength = primeLength;       // stores old table length for the purpose of copying values 
                                            // from the old table.
    newPrimeLength(primeLength);            // call funtion that calculated new tablesize
    listItem *newHashTable = new listItem[primeLength];
    initList(newHashTable, primeLength);    // initializes new table
    for(int i=0; i< oldPrimeLength; i++){   // copy old table into new table
        if(!hashTable[i].vacant){
            insertValue(newHashTable,hashTable[i].value,primeLength);
        }
    }
    delete hashTable;                       // delete old table
    return newHashTable;

}

void listType::newPrimeLength(int &primeLength){
    /*this function takes in a prime number doubles it and then increments it until it is
    prime. */

    int upperBound;
    bool isPrime = false;
    primeLength = primeLength*2+1;
    upperBound = sqrt(static_cast<double>(primeLength));
    while(!isPrime){
        isPrime = true;
        for(int i = 2; i<= upperBound; i++){
            if (primeLength%i == 0){
                isPrime = false;
            }
        }
        if(isPrime){
            return;
        }
        else{
            primeLength = primeLength+1;
        }
    }
}

void listType::setProbeAlgorithm(char key){
    /*This function sets a boolean flag that selects which probing algoritm to use*/

    if(key == '1'){
        isQuadratic = true;
    }else if(key == '0'){
        isQuadratic = false;
    }
}

void listType::initStats(){
    /*This function initializes class variable counters to 0*/

    insertAttempts = 0;
    insertCollisions = 0;
    insertFailures = 0;

    findAttempts = 0;
    findCollisions = 0;
    findFailures = 0;

    reHashes = 0;
}

void listType::printInsertStats(){
    if(isQuadratic){
        cout<<"Probing Algorithm:\tQuadratic"<<endl;
    }else{
        cout<<"Probing Algorithm:\tLinear"<<endl;
    }
    cout<<"Total Collisions:\t"<<insertCollisions<<endl;
    cout<<"Total Re-Hashes:\t"<<reHashes<<endl;
    cout<<"Total Failures:\t\t"<<insertFailures<<endl;
}

void listType::printFindStats(){
    cout<<"Total Find Collisions:\t"<<findCollisions<<endl;
    cout<<"Total Find Failures:\t"<<findFailures<<endl;
}

void listType::printTable(listItem* hashTable, int primeLength){
    for(int i = 0; i<primeLength; i++){
        if(!hashTable[i].vacant){
            cout<<"Index "<<i+1<<": "<<hashTable[i].value<<endl;
        }
    }cout<<endl;
}


int main(int argc, char** argv){

    if(argc!=4){
        cout<<"\nUsage:\n\targuments: <input_file_name> <probing_option> <load_factor>\n\n";
        cout<<"Input file:\t\tA file of arbitrary integers separated by LF character.\n\n";
        cout<<"Probing option: \t0 for Linear Probing\n";
        cout<<"\t\t\t1 for Quadratic Probing\n\n";
        cout<<"Load Factor:\t\tis a floating point value x, (where 0 < x < 1) that represents the threshold\n\t\t\tat which the table should rehash\n\n";
        return 0;
    }

    string fileName = argv[1];              // command line option variables
    char key = (*argv[2]);
    char *loadFactorString = argv[3];



    int value;                              // used for integer input
    int numberOfEntries = 0;                // number of entires counter
    int numberOfSearches = 0;               // number of searches counter
    int primeLength = 53;                   // initial table size
    double loadFactor = 0;                  // load factor
    double loadFactorUpperBound = 0;        // upper bound for rehashing condition
    bool success;                           // used to flag insert/find failure condition

    if(loadFactorString){                   // set upper bound only if argv[3] is present
        loadFactorUpperBound = atof(loadFactorString);
    }

    listType myList;                        // instaciate new list object

    listItem *hashTable;                    // create pointer to list item array
    hashTable = new listItem[primeLength];  // instanciate new list item array
    myList.initList(hashTable, primeLength);// initialize array
    myList.initStats();                     // initialize list stats to zero
    myList.setProbeAlgorithm(key);          // set probing algorithm based on command line input

    ifstream infile;
    infile.open(fileName.c_str());


/*---------------------------------------INPUT FROM FILE SECTION-------------------------------------------*/


    if(loadFactorString){                           // if argv[3] is present use this algorthm to rehash when 
                                                    // loadfactor upperbound is reached
        while(infile>>value){
            loadFactor = static_cast<double>(numberOfEntries)/primeLength;  // calulate current load factor
            if(loadFactor<loadFactorUpperBound){    // if upperbound not met insert value
                success = myList.insertValue(hashTable,value,primeLength);
                if(success){                        // if insert succeeds increment entries
                    numberOfEntries++;
                }else{                              // otherwise rehash table, then insert value
                    hashTable = myList.reHashTable(hashTable, primeLength);
                    myList.insertValue(hashTable,value,primeLength);
                    numberOfEntries++;
                }
            }
            else{                                   // if upperbound met rehash table then insert value
                hashTable = myList.reHashTable(hashTable, primeLength);
                success = myList.insertValue(hashTable,value,primeLength);
                numberOfEntries++;
            }
        }
    }else{                                          // if argv[3] is not present use this algorithm to rehash 
                                                    // only when a failure occurs
        while(infile>>value){
                success = myList.insertValue(hashTable,value,primeLength); 
                                                    // if insert succeeds increment entries
                if(success){
                    numberOfEntries++;
                }else{                              // otherwise rehash the table, then 
                                                    // insert and increment entries
                    hashTable = myList.reHashTable(hashTable, primeLength);
                    myList.insertValue(hashTable,value,primeLength);
                    numberOfEntries++;
                }
            }
    }
    infile.close();

/*------------------------------------------PRINT INSERTION STATS------------------------------------------*/

    cout<<"---------CURRENT TABLE---------\n"<<endl;

    myList.printTable(hashTable, primeLength);      // print current table 1 based indexing
    loadFactor = static_cast<double>(numberOfEntries)/primeLength;
    cout<<"--------INSERTION STATS---------\n"<<endl;
    cout<<"Total Entries:\t\t"<<numberOfEntries<<endl;
     cout<<"Upper Bound for Load:\t"<<loadFactorUpperBound<<endl;
    myList.printInsertStats();                      // print class stats
    cout<<"\nFinal Table Size:\t"<<primeLength<<endl;
    cout<<"Final Load Factor:\t"<<loadFactor<<endl;

   

/*----------------------------------------------SEARCH TABLE FOR VALUES-------------------------------------*/
   infile.open(fileName.c_str());

    while(infile>>value){
        numberOfSearches++;
        success = myList.findValue(hashTable,value, primeLength);
        if(!success){
            cout<<value<<"\tnot found"<<endl;
        }
    }
    infile.close();
/*-------------------------------------------------PRINT SEARCH STATS---------------------------------------*/
    cout<<"\n----------SEARCH STATS----------\n"<<endl;
    cout<<"Number of searches:\t"<<numberOfSearches<<endl;
    myList.printFindStats();                        //print class stats

    return 0;
}
