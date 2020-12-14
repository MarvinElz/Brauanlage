#include "Rezept.h"

#include <mosquitto.h>
#include "../Eventbus.h"
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

using namespace std;

bool verbose = false;
int  instance_number = 0;
string recipe_file_path = "";


std::string get_id_string( ){
	std::string id_string = "Rezept" + std::to_string(instance_number);
	return id_string;
}

bool init_mqtt( struct mosquitto *mosq ){
	mosquitto_lib_init();
	mosq = mosquitto_new(get_id_string().c_str(), CLEANSESSION, NULL);
	if(!mosq){
		cout << "Error: Out of memory." << endl;
		return false;
	}
  
	if(mosquitto_connect(mosq, HOST, PORT, KEEPALIVE)){
		fprintf(stderr, "Unable to connect.\n");
		return false;
	}
	
	// Subscriptions
	
	int loop = mosquitto_loop_start(mosq);
	if(loop != MOSQ_ERR_SUCCESS){
		cout << "Fehler bei mosquitto_loop_start: " << loop << endl;
		return false;
	}
	return true;
}

void print_help(){
	cout << "Rezept" << endl;
	cout << "      optional:" << endl;
	cout << "      [-h | --help]               gibt diese Nachricht aus." << endl;
	cout << "      [-i | --instance] NUMMER    sollen mehrere Rezepte gleichzeitig ausgeführt werden," << endl;
	cout << "                                  kann hier eine Nummer übergeben werden." << endl;
	cout << "      [-v | --verbose]            aktiviert die ausführliche Ausgabe." << endl;
	cout << "      [-r | --recipe] REZEPTDATEI direktes Übergeben des Rezeptes." << endl;
}

bool parse_input_parameter(int argc, char *argv[]){
	for( int i = 1; i < argc; i++ ){
		if( strcmp( "-h", argv[i] ) == 0 || strcmp( "--help", argv[i] ) == 0 ){
			print_help();
			return false;
		}
		if( strcmp( "-i", argv[i] ) == 0 || strcmp( "--instance", argv[i]) == 0 ){
			if( i+1 == argc ){
				cout << "Bitte Instanznummer angeben." << endl;
				print_help();
				return false;
			}
			try{
				instance_number = atoi( argv[i+1] );
			}catch(std::exception& e){
				cout << "Konnte Instanznummer nicht lesen." << endl;
				print_help();
				return false;
			}
			i++;
			continue;
		}
		if( strcmp( "-v", argv[i] ) == 0 || strcmp( "--verbose", argv[i]) == 0 ){
			verbose = true;
			continue;
		}
		if( strcmp( "-r", argv[i] ) == 0 || strcmp( "--recipe", argv[i]) == 0 ){
			if( i+1 == argc ){
				cout << "Bitte Rezeptpfad angeben." << endl;
				print_help();
				return false;
			}
			recipe_file_path = argv[i+1];
			i++;
			continue;
		}
		cout << "Konnte Parameter '" << argv[i] << "' nicht interpretieren." << endl;
		print_help();
		return false; 
	}
	return true;
}

int main(int argc, char *argv[]){
	if( !parse_input_parameter(argc, argv)){
		return 1;
	}
	
	if(verbose){
		cout << "Instanznummer:" << instance_number << endl;
		if( recipe_file_path.length() != 0 ){
			cout << "Rezept-Datei:" << recipe_file_path << endl;
		}
	}
	
	struct mosquitto *mosq = NULL;
	if( !init_mqtt(mosq ) ){
		return 1;
	}
	
	while(1){
		usleep(5*1000);
	}
}
