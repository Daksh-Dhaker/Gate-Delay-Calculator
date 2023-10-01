#include <iostream>
#include<fstream>
#include<unordered_map>
#include<map>
#include<vector>
#include<climits>
#include <string>
using namespace std;

class Node{
public:
    double delay_val;
    vector<Node*> inputs;
    vector<Node*> outputs;

    Node(double value){
        delay_val = value;
    }

};

double actual_delay(Node* &output_node,unordered_map<Node*,double> &answer){
    if((output_node->inputs).size()==0){
        answer[output_node] =0;
        return 0;
    }if((output_node->inputs).size()!=0){
        if(answer[output_node]!=0){
            return answer[output_node];
        }else{
            double ad;
            ad = actual_delay(output_node->inputs[0], answer);
            for(int i=0;i<(output_node->inputs).size();i++){
                ad = max(ad,actual_delay(output_node->inputs[i], answer));
            }
            ad = ad+(output_node->delay_val);
            answer[output_node]=ad;
            return ad;
        }
    }return 0;
}

double required_delay(Node* &input_node,unordered_map<Node*,double> &answer2,unordered_map<Node*,string> str,unordered_map<string,double> required_delays,unordered_map<string,int> s){
    if((input_node->outputs).size()==0){
        if(s[str[input_node]]==1){
            answer2[input_node]=INT_MAX;
            return answer2[input_node];
        }
        answer2[input_node]= required_delays[str[input_node]];
        return answer2[input_node];
    }
    if((input_node->outputs).size()!=0){
        if(answer2[input_node]!=0){
            return answer2[input_node];
        }else{
            double rd;
            rd = required_delay(input_node->outputs[0],answer2, str, required_delays,s)-(input_node->outputs[0])->delay_val;
            for(int i=0;i<(input_node->outputs).size();i++){
                rd = min(rd,required_delay(input_node->outputs[i],answer2, str , required_delays, s)-(input_node->outputs[i])->delay_val);
            }
            answer2[input_node]=rd;
            return rd;
        }
    }return 0;
}
int main(int args,char * arg[])
{
    fstream Circuit_file;
    fstream gate_delay_file;
    fstream required_delays_file;
    fstream output_delay_file;
    fstream input_delay_file;
    vector<string> primary_outputs;
    vector<string> primary_inputs;
    unordered_map<string,int> internal_sig; 
    unordered_map<string,double> delays;
    unordered_map<Node*,double> answer;
    unordered_map<Node*,double> answer2;
    unordered_map<string,Node*> node;
    unordered_map<Node*,string> str;
    unordered_map<string,double> required_delays;
    
    Circuit_file.open(arg[2],ios::in);
    gate_delay_file.open(arg[3],ios::in);
    required_delays_file.open(arg[4],ios::in);
    
    if(gate_delay_file.is_open()){
        string line;
        while(getline(gate_delay_file,line)){
            if(line[0]=='/'){
                ;
            }else if(line.size()==0){
                ;
            }else{
                string s;
                int i=0;
                while(line[i]!=' ' && i < line.size()){
                    s = s+line[i];
                    i++;
                }
                if(i+1<line.size()){
                delays[s] = stod(line.substr(i+1,line.size()-i)); }   
            }
        }
        gate_delay_file.close();
    }
    if(Circuit_file.is_open()){
        string line;
        while(getline(Circuit_file,line)){
            if(line[0]=='/'){
                ;
            }else if(line.size()==0){
                ;
            }else{
                string s;
                int i=0;
                while(line[i]!=' '){
                    s = s+line[i];
                    i++;
                }
                if(s == "PRIMARY_INPUTS" || s=="PRIMARY_OUTPUTS" || s=="INTERNAL_SIGNALS"){
                    string s1;
                    i=i+1;
                    while(i<line.size()){
                        if(line[i]==' '){
                            Node* n = new Node(0);
                            node[s1]=n;
                            str[node[s1]]=s1;
                            if(s=="PRIMARY_OUTPUTS"){
                                primary_outputs.push_back(s1);
                            }if(s=="PRIMARY_INPUTS"){
                                primary_inputs.push_back(s1);
                            }
                            if(s=="INTERNAL_SIGNALS"){
                                internal_sig[s1]=1;
                            }
                            s1 ="";
                        }else{
                            s1 = s1+line[i];
                        }
                        i++;
                    }Node* n = new Node(0);
                    node[s1] = n;
                    str[node[s1]]=s1;
                    if(s=="PRIMARY_OUTPUTS"){
                        primary_outputs.push_back(s1);
                    }if(s=="PRIMARY_INPUTS"){
                        primary_inputs.push_back(s1);
                    }if(s=="INTERNAL_SIGNALS"){
                        internal_sig[s1]=1;
                    }
                }else{
                    int j = line.size()-1;
                    string output;
                    while(line[j]!=' '){
                        output = line[j]+output;
                        j--;
                    }node[output]->delay_val = delays[s];
                    string input;
                    j--;
                    while(j>=i){
                        if(line[j]==' '){
                            (node[input]->outputs).push_back(node[output]);
                            (node[output]->inputs).push_back(node[input]);
                            input ="";
                        }else{
                        input = line[j]+input;}
                        j--;
                    }
                }
            }
        }
        Circuit_file.close();
    }
    if(required_delays_file.is_open()){
        string line;
        while(getline(required_delays_file,line)){
            if(line[0]=='/'){
                ;
            }else if(line.size()==0){
                ;
            }else{
                string s;
                int i=0;
                while(line[i]!=' ' && i < line.size()){
                    s=s+line[i];
                    i++;
                }if(i+1<line.size()){
                required_delays[s]=stod(line.substr(i+1,line.size()-i));}
            }
        }
        required_delays_file.close();
    }
    // Part-A is this
    for(auto it : internal_sig){
        if(node[it.first]->outputs.size()!=0){
            it.second = 0;
        }
    }
    string A=arg[1];
    if(A=="A"){
        output_delay_file.open("output_delays.txt",ios::out);
        if(output_delay_file.is_open()){
        for(int i=0;i<primary_outputs.size();i++){
        output_delay_file<<primary_outputs[i]<<" "<<actual_delay(node[primary_outputs[i]], answer)<<endl;
        }
        output_delay_file.close();
    }
    }
    else{
    //Part-B Starts here
    input_delay_file.open("input_delays.txt",ios::out);
    if(input_delay_file.is_open()){
        for(int i=0;i<primary_inputs.size();i++){
        input_delay_file<<primary_inputs[i]<<" "<<required_delay(node[primary_inputs[i]], answer2 ,str,required_delays,internal_sig)<<endl;
        }
        input_delay_file.close();
    }
    }
}
