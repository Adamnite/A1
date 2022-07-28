/**
 *
 * Copyright (c)2022 The Adamnite C++ Authors.
 *
 * This code is open-sourced under the MIT license.
 */

#include<bits/stdc++.h>

using namespace std;


void printTokens(vector<string> &tokens){
    
    for(int i=0;i<tokens.size();i++)
    {
        cout<<tokens[i]<<" ";
    }
}

int validateDouble(string str){
    try {
        stod(str);
        return 1;
    }
    catch (...) {
        cout << "Invalid input \'"<<str<<"\' Please try again!\n";
        return 0;
    }
}

int executeLine(vector<string> &tokens){
    
    vector<string> acceptedCommands = {"add","mul","sub","div","exit"};
    
    int cmdInt = 999;
    
    for(int i=0;i<acceptedCommands.size();i++){
        if(acceptedCommands[i].compare(tokens[0])==0){
            cmdInt = i;
            break;
        }
    }
    
    double ans = 0;
    
    switch(cmdInt){
        case 0 :
            if(tokens.size()<3){
                cout<<"\'add\' needs atleast 2 arguments"<<endl;
                return 1;
            }
            ans = 0;
            for(int i=1;i<tokens.size();i++){
                if(validateDouble(tokens[i])){
                    ans += stod(tokens[i]);
                }
                else
                    return 1;
            }
            break;
        case 1 :
            if(tokens.size()<3){
                cout<<"\'mul\' needs atleast 2 arguments"<<endl;
                return 1;
            }
            ans = 1;
            for(int i=1;i<tokens.size();i++){
                if(validateDouble(tokens[i])){
                    ans *= stod(tokens[i]);
                }
                else
                    return 1;
            }
            break;
        case 2 :
            if(tokens.size()!=3){
                cout<<"\'sub\' needs 2 arguments"<<endl;
                return 1;
            }
            if(validateDouble(tokens[1]) && validateDouble(tokens[2])){
                ans = stod(tokens[1]) - stod(tokens[2]);
            }
            else
                return 1;
            break;
        case 3 :
            if(tokens.size()!=3){
                cout<<"\'div\' needs 2 arguments"<<endl;
                return 1;
            }
            if(validateDouble(tokens[1]) && validateDouble(tokens[2])){
                ans = stod(tokens[1]) / stod(tokens[2]);
            }
            else
                return 1;
            break;
        case 4 :
            cout<<"bye! bye!";
            return 0;
        default :
            cout<<"\'"<<tokens[0]<<"\' is not a valid command"<<endl;
            return 1;
    }
    
    cout<<ans<<endl;
    return 1;
}



int processLine(string &line){
    vector<string> tokens;
    
    istringstream ss(line);
    
    string token;
    
    while(ss >> token){
        tokens.push_back(token);
    }
    
    
    return executeLine(tokens);
    
        
}

int main(){
    string curr_line;
    while(1){
        cout<<">>";
        getline( cin, curr_line );
        if(processLine(curr_line) == 0)
            break;
    }
    return 0;
}