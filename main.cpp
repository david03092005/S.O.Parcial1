#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>

#include "process.h "

using namespace std;

vector<string> split(string str, char pattern);
void executeProcess();
bool empty(map<vector<string>, vector<process>> queues);
string toUpper(string str);
bool confirm (int time);
int MLQ(string nameFile, int op);
void executeProcessF();

map<int, vector<string>> priority;
map<vector<string>, vector<process>> queues;
map<string, int> waitingTime;
map<string, int> turnAroundTime;


int main() {
	int option;
	string nameFile; //Archivo con los datos
	cout << "1. MLQ" << endl;
	cout << "2. MLFQ" << endl;
	cout << "0. Exit" << endl;
	cout << "Insert the option that you want run: ";
	cin >> option;
	while (option != 0) {
		cout << "Put the name of the program (file) that you want to execute (without the extension .txt) 0 to exit: ";
		cin >> nameFile;
		nameFile = nameFile + ".txt";
		
		int i = MLQ(nameFile, option);
		
		float waitingTimeAv = 0;
		float turnAroundTimeAv = 0;
		for (const auto& key_value : waitingTime) {
			cout << "waiting time " << key_value.first << ": " << key_value.second << endl;
			waitingTimeAv += key_value.second;
		}
		for (const auto& key_value : turnAroundTime) {
			cout << "turnaround time " << key_value.first << ": "  << key_value.second << endl;
			turnAroundTimeAv += key_value.second;
		}
		
		cout << "average waiting time ----> "<< waitingTimeAv / i << endl;
		cout << "average turnaround time ----> "<< turnAroundTimeAv / i << endl;
		
		cout << endl;
		cout << "1. MLQ" << endl;
		cout << "2. MLFQ" << endl;
		cout << "0. Exit" << endl;
		cout << "Insert the option that you want run: ";
		cin >> option;
	}
	
	return 0;
}


int MLQ(string nameFile, int op) {
	ifstream file(nameFile.c_str());
	if (!file.is_open()) {
		std::cerr << "Error: No se pudo abrir el archivo " << nameFile << std::endl;
	    return 404;  // Salir del programa con un código de error
	}
	else {
		string line;
		getline(file, line);
		string word;
		istringstream stream(line);
		int prio = 0;
		while (stream >> word) {
			vector<string> word2 = split(word, '(');
	        priority[prio] = word2;
	        word2[1].pop_back();
	        queues[word2];
        	prio++;
    	}
			
		int i = 0;
		while (getline(file, line)) {
			istringstream stream(line);
			string idP;
			int atP;
		    int btP;
		    int q;
		    stream >> idP;
		    stream >> atP;
		    stream >> btP;
		    stream >> q;
		    
		    cout << idP << " " << atP << " " << btP << " " << q << endl;
		    
		    process p = process(idP, atP, btP);
		    if (op == 1) {
		    	q -= 1;
			}
			else if (op == 2) {
				q = 0;
			}
		    queues[priority[q]].push_back(p);
		    waitingTime[idP] = btP + atP;
		    turnAroundTime[idP] = atP;
		    i++;
		}
		if (op == 1) {
		    executeProcess();
		}
		else if (op == 2) {
			executeProcessF();
		}
		return i;
	}
}


void executeProcess() {
	int time = 0;
	int gotoRR = 999999;
	
	bool result = empty(queues);
	
	while(not result) {
		int i = 0;
		for (int j = 0; j < queues[priority[i]].size(); j++){
			int atJ = queues[priority[i]][j].getAT(); 
			int btJ = queues[priority[i]][j].getBT();
			string id = queues[priority[i]][j].getID();
			
			if (atJ <= time){
				cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
				int quantus = stoi(priority[i][1]);
				if (btJ > quantus) {
					time = time + quantus;
					queues[priority[i]][j].setBT(btJ - quantus);
					cout << " hasta el tiempo " << time << endl;
				}
				else if (btJ <= quantus) {
					time = time + btJ;
					queues[priority[i]].erase(queues[priority[i]].begin() + j);
					waitingTime[id] = time - waitingTime[id];
					turnAroundTime[id] = time - atJ;
					j--;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
			else {
				if (atJ < gotoRR) {
					gotoRR = atJ;
				}
			}
		}
		
		if (not confirm(time)) {
			i++;
			int j = 0;
			while (j < queues[priority[i]].size() and time < gotoRR) {
				int atJ = queues[priority[i]][j].getAT(); 
				int btJ = queues[priority[i]][j].getBT();
				string id = queues[priority[i]][j].getID();
				if (atJ <= time){
					cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
					int ej = 0;
					while (btJ > ej){
						ej++;
						time++;
					}
					waitingTime[id] = time - waitingTime[id];
					turnAroundTime[id] = time - atJ;
					queues[priority[i]].erase(queues[priority[i]].begin() + j);
					j--;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
		}
		result = empty(queues);
	}
}


bool confirm(int time){
	bool result = false;
	for (int i = 0; i < queues[priority[0]].size(); i++) {
		if (queues[priority[0]][i].getAT() <= time){
			result = true;
		}
	}
	return result;
}


bool empty(map<vector<string>, vector<process>> queues){
	bool ans = all_of(queues.begin(), queues.end(), [](const pair<const vector<string>, vector<process>>& pair) {
    	return pair.second.empty();
	});
	
	return ans;
}


vector<string> split(string str, char pattern) {
    
    int posInit = 0;
    int posFound = 0;
    string splitted;
    vector<string> results;
    
    while(posFound >= 0){
        posFound = str.find(pattern, posInit);
        splitted = str.substr(posInit, posFound - posInit);
        posInit = posFound + 1;
        results.push_back(splitted);
    }
    
    return results;
}


string toUpper(string str) {
    string upperStr = str;  // Copia de la cadena original
    transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}


void executeProcessF() {
	int time = 0;
	int gotoRR = 999999;
	
	bool result = empty(queues);
	
	while(not result) {
		int i = 0;
		for (int j = 0; j < queues[priority[i]].size(); j++){
			int atJ = queues[priority[i]][j].getAT(); 
			int btJ = queues[priority[i]][j].getBT();
			string id = queues[priority[i]][j].getID();
			if (atJ <= time){
				cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
				int quantus = stoi(priority[i][1]);
				if (btJ > quantus) {
					time = time + quantus;
					queues[priority[i]][j].setBT(btJ - quantus);
					cout << " hasta el tiempo " << time << endl;
					queues[priority[i+1]].push_back(queues[priority[i]][j]);
				}
				else if (btJ <= quantus) {
					time = time + btJ;
					waitingTime[id] = time - waitingTime[id];
					turnAroundTime[id] = time - atJ;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
				queues[priority[i]].erase(queues[priority[i]].begin() + j);
				j--;
				gotoRR = 999999;
			}
			else {
				if (atJ < gotoRR) {
					gotoRR = atJ;
				}
			}
		}
		if (not confirm(time)) {
			i++;
			int j = 0;
			while (j < queues[priority[i]].size() and time < gotoRR) {
				int atJ = queues[priority[i]][j].getAT(); 
				int btJ = queues[priority[i]][j].getBT();
				string id = queues[priority[i]][j].getID();
				if (atJ <= time){
					cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
					int ej = 0;
					while (btJ > ej){
						ej++;
						time++;
					}
					waitingTime[id] = time - waitingTime[id];
					turnAroundTime[id] = time - atJ;
					queues[priority[i]].erase(queues[priority[i]].begin() + j);
					j--;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
		}
		result = empty(queues);
	}

}