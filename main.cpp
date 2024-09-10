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

map<int, vector<string>> priority; // Mapa que almacena las prioridades de las colas
map<vector<string>, vector<process>> queues; // Mapa que almacena los procesos por cada cola
map<string, int> waitingTime; // Mapa que almacena los tiempos de espera de los procesos
map<string, int> turnAroundTime; // Mapa que almacena los tiempos turnaround de los procesos


int main() {
	int option;
	string nameFile; // Archivo que contiene los datos de los procesos
	cout << "1. MLQ" << endl;
	cout << "2. MLFQ" << endl;
	cout << "0. Exit" << endl;
	cout << "Insert the option that you want run: ";
	cin >> option;
	while (option != 0) {
		cout << "Put the name of the program (file) that you want to execute (without the extension .txt) 0 to exit: ";
		cin >> nameFile;
		nameFile = nameFile + ".txt";
		
		int i = MLQ(nameFile, option); // Se llama a la función MLQ para procesar los datos del archivo y ejecutar los procesos
		
		// Cálculo y muestra de los tiempos de espera y turnaround promedio
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
		
		// Limpiar los mapas para permitir ejecutar otros archivos sin conflicto
		priority.clear();
		queues.clear();
		waitingTime.clear();
		turnAroundTime.clear();
	}	
	return 0;
}


// Función para procesar el archivo y asignar procesos a colas de prioridad
int MLQ(string nameFile, int op) {
	ifstream file(nameFile.c_str());  // Se abre el archivo con los datos de los procesos
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
		
		// Separar y asignar prioridades a cada cola
		while (stream >> word) {
			vector<string> word2 = split(word, '('); // Utiliza la función split para dividir la palabra
	        priority[prio] = word2; // Asigna la palabra a la prioridad correspondiente
	        word2[1].pop_back(); // Eliminar el carácter ')' final
	        queues[word2];
        	prio++;
    	}
			
		// Leer cada uno de los procesos en el archivo y separar sus atributos
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
		    process p = process(idP, atP, btP); // inicializar el objeto proceso
		    
			// Si selecciona MLQ se le asigana la prioridad -1, de lo contrario se le asigna 0 como prioridad.
			if (op == 1) {
		    	q -= 1;
			}
			else if (op == 2) {
				q = 0;
			}
		    queues[priority[q]].push_back(p); // Se agrega a la cola según la prioridad del proceso
		    waitingTime[idP] = btP + atP; // Se define el waiting time como (burst time - arrival time) para luego operarlo con el complete time
		    turnAroundTime[idP] = atP; // Se define el turnaround time como el arrival time, para luego operarlo con el complete time
		    i++;
		}
		//Si selecciona MLQ se ejecuta el primero, y MLFQ el segundo
		if (op == 1) {
		    executeProcess();
		}
		else if (op == 2) {
			executeProcessF();
		}
		return i; // Retorna la cantidad de procesos que se ejecutaron.
	}
}


// Ejecuta los procesos con el algoritmo MLQ
void executeProcess() {
	int time = 0;
	
	bool result = empty(queues); // Verificar si las colas están vacías
	
	// Bucle que simula el proceso mientras haya procesos pendientes
	while(not result) {
		int gotoRR = 999999; // Variable auxiliar para controlar en que momento el algoritmo debe dejar de ejecutar FCFS y volver a RR
		int i = 0; // Indice de la cola
		for (int j = 0; j < queues[priority[i]].size(); j++){
			int atJ = queues[priority[i]][j].getAT(); 
			int btJ = queues[priority[i]][j].getBT();
			string id = queues[priority[i]][j].getID();
			
			// Se verifica si el proceso ya ha llegado para ese momento, segun su arrival time
			if (atJ <= time){
				cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
				int quantus = stoi(priority[i][1]); // Se obtiene el quantum
				if (btJ > quantus) { // Si el proceso necesita mas tiempo que el asignado por el quantum
					time = time + quantus;
					queues[priority[i]][j].setBT(btJ - quantus);
					cout << " hasta el tiempo " << time << endl;
				}
				else if (btJ <= quantus) { // Si el proceso necesita menos tiempo que el asignado por el quantum
					time = time + btJ;
					queues[priority[i]].erase(queues[priority[i]].begin() + j); // Se elimina ese proceso de la cola de RR
					waitingTime[id] = time - waitingTime[id]; // Su waiting time va a ser el (complete time - burst time - arrival time)
					turnAroundTime[id] = time - atJ; // Su turnaround time va a ser el (complete time - arrival time)
					j--; // Se resta el j para pasar con el proceso que sigue
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
			else { // Si el proceso no ha llegado se activa una bandera con el arrival time del proceso
				if (atJ < gotoRR) { // La bandera debe estar en el menor arrival time que no se haya procesado
					gotoRR = atJ;
				}
			}
		}
		
		if (not confirm(time)) {
			i++; // Cambiar a la siguiente cola
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
					queues[priority[i]].erase(queues[priority[i]].begin() + j); // Elimina el proceso una vez ejecutado
					j--;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
		}
		result = empty(queues); // Nos dice si las colas ya estan vacias y se ejecutaron todos los procesos
	}
}


// Función que verifica si algún proceso puede ejecutarse en el tiempo actual
bool confirm(int time){
	bool result = false;
	for (int i = 0; i < queues[priority[0]].size(); i++) {
		if (queues[priority[0]][i].getAT() <= time){ // Verifica si el proceso puede ejecutarse
			result = true;
		}
	}
	return result;
}


// Función que verifica si las colas de procesos están vacías
bool empty(map<vector<string>, vector<process>> queues){
	bool ans = all_of(queues.begin(), queues.end(), [](const pair<const vector<string>, vector<process>>& pair) {
    	return pair.second.empty();
	});
	
	return ans;
}


// Función que divide una cadena basada en un patrón
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


// Función que convierte una cadena a mayúsculas
string toUpper(string str) {
    string upperStr = str;  // Copia de la cadena original
    transform(upperStr.begin(), upperStr.end(), upperStr.begin(), ::toupper);
    return upperStr;
}


// Función que simula la ejecución de procesos para el algoritmo MLFQ 
void executeProcessF() {
	int time = 0;
	
	bool result = empty(queues);  // Verifica si las colas están vacías
	
	// Bucle que simula el proceso mientras haya procesos pendientes
	while(not result) {
		int gotoRR = 999999; // Variable auxiliar para controlar en que momento el algoritmo debe dejar de ejecutar FCFS y volver a RR
		int i = 0;
		for (int j = 0; j < queues[priority[i]].size(); j++){
			int atJ = queues[priority[i]][j].getAT(); 
			int btJ = queues[priority[i]][j].getBT();
			string id = queues[priority[i]][j].getID();
			// Se verifica si el proceso ya ha llegado para ese momento, segun su arrival time
			if (atJ <= time){
				cout << "Se ejecuto el proceso " << id << " desde el tiempo " << time; 
				int quantus = stoi(priority[i][1]); // Se obtiene el quantum
				if (btJ > quantus) { // Si el proceso necesita mas tiempo que el asignado por el quantum
					time = time + quantus;
					queues[priority[i]][j].setBT(btJ - quantus);
					cout << " hasta el tiempo " << time << endl;
					queues[priority[i+1]].push_back(queues[priority[i]][j]); // Se agrega el proceso a la siguiente cola
				}
				else if (btJ <= quantus) { // Si el proceso necesita menos tiempo que el asignado por el quantum
					time = time + btJ;
					waitingTime[id] = time - waitingTime[id]; // Su waiting time va a ser el (complete time - burst time - arrival time)
					turnAroundTime[id] = time - atJ; // Su turnaround time va a ser el (complete time - arrival time)
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
				queues[priority[i]].erase(queues[priority[i]].begin() + j); // Se elimina el proceso de la cola, ya sea que haya terminado o no
				j--; // Se resta 1 para seguir con el proximo
				gotoRR = 999999;
			}
			else { // Si el proceso no ha llegado se activa una bandera con el arrival time del proceso
				if (atJ < gotoRR) { // La bandera debe estar en el menor arrival time que no se haya procesado
					gotoRR = atJ;
				}
			}
		}
		
		if (not confirm(time)) {
			i++; // Cambiar a la siguiente cola
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
					queues[priority[i]].erase(queues[priority[i]].begin() + j); // Elimina el proceso una vez ejecutado
					j--;
					cout << " hasta el tiempo " << time << endl;
					cout << "El proceso " << id << " termino su ejecucion" << endl;
				}
			}
		}
		result = empty(queues); // Nos dice si las colas ya estan vacias y se ejecutaron todos los procesos
	}

}