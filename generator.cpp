#include<fstream>
#include<iostream>
#include<time.h>
#include<string.h>
#include<process.h>
using namespace std;

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

char tostring(int num,char str[10]){
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

}

int main(){
    srand(time(NULL));
    int a=rand();
    char name[100];
    char nameWithoutFormat[100];
    cout<<"Enter Name of the file (type 'rand' for a random name):";
    cin>>name;
    if(!strcmp(name,"rand"))
        tostring(a,name);
    strcpy(nameWithoutFormat,name);
    char fileName[100]="log.txt";
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
