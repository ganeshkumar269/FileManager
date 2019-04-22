#include <fstream>
#include <iostream>
#include <time.h>
#include <string.h>
#include <process.h>
#include "sqlite3.h"
#include <ctime>
using namespace std;
//command list
enum commandList{
    cleanUp,
    printAll,
    help,
    notCommand
};
commandList hashit(string const& inString){
    if(inString == "cleanUp") return cleanUp;
    if(inString == "printAll") return printAll;
    if(inString == "help") return help;
    return notCommand;
}
static int callback(void* data, int argc, char** argv, char** azColName) { 
    int i; 
    fprintf(stderr, "%s: ", (const char*)data); 
  
    for (i = 0; i < argc; i++) { 
        printf("%s : %s\n", azColName[i], argv[i] ? argv[i] : "NULL"); 
    } 
  
    printf("\n"); 
    return 0; 
}

void insert(sqlite3 *db,string fileName,char *Time){
    char* messageError;
    string query="insert into database values(\"" + fileName + "\"" + ",\"" + Time + "\");";
    int exit=sqlite3_exec(db,query.c_str(),NULL,0,&messageError);
    if(exit!=SQLITE_OK)
        cout<<"Error Bruv :"<<messageError<<endl;
    else
        cout<<"Success in inserting"<<endl;
}

void printAllFunc(sqlite3 *db){
    string query = "select * from database";
    char* messageError;
    sqlite3_exec(db,query.c_str(),callback,NULL,NULL);
}
bool searchDB(sqlite3* db,string fileName){
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
    printf("Not Found\n");
    sqlite3_finalize(stmt);
    return false;
}

void cleanUpFunc(sqlite3* db){
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
        
        file.seekg(0,ios::end);
        int n = file.tellg();
        file.close();
        cout<<"File Size is :"<<n<<endl;
        if(n == 59){
            string tempFileName((char*)fileName);
            string tempPath = "D:\\codeblocks\\"+tempFileName;
            if(remove(tempPath.c_str()) == 0){
                cout<<"Successfully removed "<<fileName<<endl;
                count++;
            }
            else
                cout<<"Removal of "<<fileName<<" unsuccessful"<<endl;
        }
    }
    if(count)
        cout<<"Freed "<<count*59<<" bytes of memory, you're welcome"<<endl;
    else
        cout<<"Nothing to be freed, today's not the day for me"<<endl;
    
}

bool isNew(sqlite3* db){
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

bool searchFile(char filename[100],char name[100]){
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

string toString(int num,string str){
    int i=0;
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
}
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
    srand(time(NULL));
    int a = rand();
    time_t now = time(0);
    char *Time = ctime(&now);
    string name;
    sqlite3* db;
    sqlite3_open("D:\\filesLog.db",&db); // Name of the DB-file
    bool random = false, exists = false;
    cout<<"Enter Name of the file (type 'rand' for a random name): ";
    cin>>name;
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
    switch(hashit(name)){
        case cleanUp:
            cout<<"CleanUp started"<<endl;
            cleanUpFunc(db);
            cout<<"CleanUp ended";
            break;
        case printAll:
            printAllFunc(db);
            break;
        case help:
            helpMenu();
            break;
        default:
        if( name == "rand"){
            name = toString(a,name);
            cout<<"Random name is selected"<<endl;
            random = true;
        }
        
        if(!random){
            if(!searchDB(db,name)){
                insert(db,name,Time);
                cout<<"Opening a new file "<<name<<".cpp"<<endl;
            }
            else{
                exists = true;
                cout<<"Opening existing file "<<name<<".cpp"<<endl;
            }
        }
        else{
            insert(db,name,Time);
            cout<<"Opening a random file "<<name<<".cpp"<<endl;
        }
        string format(".cpp");
        name.append(".cpp");
        cout<<"The name of the file after appending format :"<<name<<endl;
        if(!exists){
            string path = "D:\\codeblocks\\"+name;
            ofstream file(path.c_str());
            if(!file.is_open())
                cout<<"Opening "<<name<<" Failed"<<endl;
            else {
                file<<"#include<iostream>"<<endl;
                file<<"using namespace std;"<<endl;
                file<<"int main(){"<<endl;
                file<<"\t"<<endl;
                file<<"}";
                file.close();           
            }
        } 
        string command = "D:\\codeblocks\\"+name;
        system(command.c_str());
        system("exit");
        cout<<"Successfully Opened "<<command;
    }
        return 0;
}
