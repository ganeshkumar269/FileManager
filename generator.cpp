#include <fstream>
#include <iostream>
#include <time.h>
#include <string.h>
#include <process.h>
#include "sqlite3.h"
#include <ctime>
using namespace std;

// Functions used

void insert(sqlite3*,string,char *); //inserts the name of the file and timestamp
void printAllFunc(sqlite3 *); //prints all the records in the database
bool searchDB(sqlite3* ,string );//searches for the filename in the db if found returns true
void deleteRecord(sqlite3* ,string );//deletes the record containing the filename
void cleanUpFunc(sqlite3* );//deletes the file whose size is 59 bytes ie, unedited after creation 
bool isNew(sqlite3* ); //returns true if the database has been created for the first time
void getRecent(sqlite3*);

//string toString(int);  //converts integer to string and returns the string - deprecated
void helpMenu();

//command list
enum commandList{   //if u want to add additional commands include it here.
    cleanUp,
    printAll,
    help,
    quit,
    recent,
    notCommand
};
commandList hashit(string const& inString){  
    if(inString == "cleanUp") return cleanUp;
    if(inString == "printAll") return printAll;
    if(inString == "help") return help;
    if(inString == "quit") return quit;
    if(inString == "recent") return recent;
    return notCommand;
}
static int callback(void* data, int argc, char** argv, char** azColName) { //called by sqlite3_exec
                                //argc-no.of columns, argv- column values , azColName - column name
    int i; 
    fprintf(stderr, "%s: ", (const char*)data); 
  
    for (i = 0; i < argc; i++) { 
        printf("%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); 
    } 
  
    printf("\n"); 
    return 0; 
}

void insert(sqlite3 *db,string fileName,char *Time){ //inserts the name of the file and timestamp
    char* messageError;
    string query="insert into database values(\"" + fileName + "\"" + ",\"" + Time + "\");";
    int exit=sqlite3_exec(db,query.c_str(),NULL,0,&messageError);
    if(exit!=SQLITE_OK)
        cout<<"Error in inserting:"<<fileName<<" and " <<Time<<messageError<<endl;
    else
        cout<<"Success in inserting"<<endl;
}

void printAllFunc(sqlite3 *db){  //prints all the records in the database
    string query = "select * from database";
    char* messageError;
    sqlite3_exec(db,query.c_str(),callback,NULL,NULL);
}
bool searchDB(sqlite3* db,string fileName){ //searches for the filename in the db if found returns true
    string query = "select * from database where fileName=\""+fileName+"\";";
    char *messageError;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,query.c_str(),query.length(),&stmt,NULL);
    int code;
    if((code = sqlite3_step(stmt)) == SQLITE_ROW){
        printf("fileName:%s TimeStamp:%s \n",sqlite3_column_text(stmt,0),sqlite3_column_text(stmt,1));
        sqlite3_finalize(stmt);
        return true;
    }
    printf("Not Found inside database\n");
    sqlite3_finalize(stmt);
    return false;
}

void deleteRecord(sqlite3* db,string fileName){ //deletes the record containing the filename
    fileName = fileName.substr(0,fileName.find('.'));
    string query = "delete from database where fileName=\""+fileName+"\";";
    char *messageError;
    sqlite3_stmt *stmt;
    sqlite3_prepare_v2(db,query.c_str(),query.length(),&stmt,NULL);
    int code = sqlite3_step(stmt);
    if(code == SQLITE_DONE)
        cout<<"Successsfully deleted "<<fileName<<" from the database."<<endl;
    else
    {
        cout<<"Error in deletion of "<<fileName<<endl;
    }
    
}

void cleanUpFunc(sqlite3* db){  //deletes the file whose size is 59 bytes ie, unedited after creation 
    string query = "SELECT fileName FROM database; ";
    sqlite3_stmt* stmt;
    int code;
    int count = 0;
    sqlite3_prepare_v2(db,query.c_str(),query.length(),&stmt,NULL);
    while((code = sqlite3_step(stmt)) == SQLITE_ROW){
        auto fileName = sqlite3_column_text(stmt,0);
        char ext[10] = ".cpp";
        strcat((char*)fileName,ext);
        string path = "D:\\codeblocks\\";
        path.append((char*)fileName);
        cout<<"Path :"<<path<<endl;
        ifstream file(path);
        cout<<"Processing file "<<fileName<<endl;
        if(!file.is_open())
            cout<<"Operation to open the file failed"<<endl;
        else{
            file.seekg(0,ios::end);
            int n = file.tellg();
            file.close();
            cout<<"File Size is :"<<n<<endl;
            if(n == 59){
                string tempFileName((char*)fileName);
                string tempPath = "D:\\codeblocks\\"+tempFileName;
                if(remove(tempPath.c_str()) == 0){
                    cout<<"Successfully removed "<<fileName<<endl;
                    deleteRecord(db,tempFileName);
                    count++;
                }
                else
                    cout<<"Removal of "<<fileName<<" unsuccessful"<<endl;
            }
        }
    }
    if(count)
        cout<<"Freed "<<count*59<<" bytes of memory, you're welcome"<<endl;
    else
        cout<<"Nothing to be freed, today's not the day for me"<<endl;
    
}

bool isNew(sqlite3* db){ //returns true if the database has been created for the first time
    string query = "SELECT * FROM sqlite_master WHERE type='table' AND name='{database}'; ";
    sqlite3_stmt* stmt;
    sqlite3_prepare_v2(db,query.c_str(),query.length(),&stmt,NULL);
    int code ;
    if((code = sqlite3_step(stmt)) == SQLITE_ROW) {
       cout<<"Database is New"<<endl;
       return true;
    }
    else{
        cout<<"Database is not New"<<endl;
        return false;
    }
}

void helpMenu(){
    cout<<"These are commands and must not be used as names for files."<<endl;
    cout<<"commands:"<<endl;
    cout<<"cleanUp  ---- helps remove unused/unwanted files in the directory."<<endl;
    cout<<"printAll ---- prints all the files recorded by the database."<<endl;
    cout<<"help     ---- you probably know what this does.";
    cout<<"Thats all"<<endl;
}

bool searchFile(char filename[100],char name[100]){//using c++ files ---deprecated
     ifstream file(filename,ios::binary);
     while(1){
        char str[100];
        file.read(reinterpret_cast<char*>(str),100);
        if(!strcmp(name,str)){
            file.close();
            return true;
        }
        if(file.eof())
            return false;
    }
    return false;
}

void getRecent(sqlite3* db){
    string query = "select * from database order by ID DESC LIMIT 1";
    sqlite3_exec(db,query.c_str(),callback,NULL,NULL);
}

/*string toString(int num){  //converts integer to string and returns the string
    int i=0;
    string str;
    while(num!=0){
        int temp=num%10;
        str[i]=char(temp+48);
        num/=10;
        i++;
    }
    str[i]='\0';
    for(int j=0;j<i/2;j++){
        char temp;
        temp=str[j];
        str[j]=str[i-j-1];
        str[i-j-1]=temp;
    }
    return str;
}*/
/*
int main(){
    srand(time(NULL));
    int a=rand();
    time_t now = time(0);
    char *Time = ctime(&now);
    char name[100];
    char nameWithoutFormat[100];
    cout<<"Enter Name of the file (type 'rand' for a random name):";
    cin>>name;
    if(!strcmp(name,"rand"))
        tostring(a,name);
    strcpy(nameWithoutFormat,name);
    char fileName[100]="log.txt";
    sqlite3 *db;
    if(!searchFile(fileName,nameWithoutFormat)){
        char format[5]=".cpp";
        strcat(name,format);
        ofstream file(name);
        ofstream log("log.txt",ios::binary|ios::app);

        if(!file.is_open())
            cout<<"Operation Failed";
        else {
            file<<"#include<iostream>"<<endl;
            file<<"using namespace std;"<<endl;
            file<<"int main(){"<<endl;
            file<<"\t"<<endl;
            file<<"}";
            log.write(reinterpret_cast<char*>(nameWithoutFormat),100);
            file.close();
            log.close();
        }
    }
    else{
        char format[5]=".cpp";
        strcat(name,format);
        cout<<"\nOpening Existing file "<<name<<endl;
    }
    char str[100]="D:\\codeblocks\\";
    strcat(str,name);
    system(str);
    cout<<"Enjoy Coding!";
    exit(NULL);
}
*/

int main(){
    srand(time(NULL));//setting the random seed corresponding to the current time value
    string name;
    sqlite3* db;
    sqlite3_open("D:\\filesLog.db",&db); // Name of the DB-file
    if(isNew(db)){
        string query = "create table database(fileName text, TimeStamp text);";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db,query.c_str(),query.length(),&stmt,NULL);
        int code = sqlite3_step(stmt);
        if(code == SQLITE_OK){
            cout<<"Created Database"<<endl;
        }
        else{
            cout<<"Error in creating database"<<endl;
        }
    }
    do {
        cout<<"Enter Name of the file ('rand' for a random name, 'help' for helpMenu): ";
        cin>>name;
        bool random = false, exists = false;
        switch(hashit(name)){
            case cleanUp:
                cout<<"CleanUp started"<<endl;
                cleanUpFunc(db);
                cout<<"CleanUp ended"<<endl;
                break;
            case printAll:
                printAllFunc(db);
                break;
            case help:
                helpMenu();
                break;
            case quit:
                break;
            case recent:
                getRecent(db);
                break;
            default:
                if( name == "rand"){
                    int a = rand(); //picking random value
                    cout<<"The random name selected is:"<<a<<endl;
                    name = to_string(a);
                    random = true;
                }
                
                if(!random){
                    if(!searchDB(db,name)){
                        time_t now = time(0);
                        char *Time = ctime(&now);  //timeStamp
                        insert(db,name,Time);
                        cout<<"Opening a new file "<<name<<".cpp"<<endl;
                    }
                    else{
                        exists = true;
                        cout<<"Opening existing file "<<name<<".cpp"<<endl;
                    }
                }
                else{
                    time_t now = time(0);
                    char *Time = ctime(&now);  //timeStamp
                    insert(db,name,Time);
                    cout<<"Opening a random file "<<name<<".cpp"<<endl;
                }
                string format(".cpp");
                name.append(".cpp");
                cout<<"The name of the file after appending format :"<<name<<endl;
                if(!exists){
                    string path = "D:\\codeblocks\\"+name;
                    ofstream file(path.c_str());
                    ifstream fin ("boilerplate.cpp");
                    char temp[200];
                    if(!file.is_open())
                        cout<<"Opening "<<name<<" Failed"<<endl;
                    while(!fin.eof()){
                        fin.getline(temp,200);
                        file<<temp<<endl;
                    }
                } 
                string pathOfDestination = "D:\\codeblocks\\"+name;
                system(pathOfDestination.c_str());
                cout<<"Successfully Opened "<<pathOfDestination<<endl;
        }
    }while(name != "quit");
    system("exit");
    return 0;
}
