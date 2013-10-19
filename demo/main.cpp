#include "main.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <curses.h>


#include "agent.hpp"
#include "environment.hpp"
#include "search.hpp"
#include "util.hpp"

// Streams for logging
std::ofstream verboseLog;        // A verbose human-readable log
std::ofstream compactLog; // A compact comma-separated value log

// The main agent/environment interaction loop
void mainLoop(Agent &ai, Environment &env, options_t &options) {

	// Determine exploration options
	bool explore = options.count("exploration") > 0;
	double explore_rate, explore_decay;
	if (explore) {
		strExtract(options["exploration"], explore_rate);
		strExtract(options["explore-decay"], explore_decay);
		assert(0.0 <= explore_rate && explore_rate <= 1.0);
		assert(0.0 <= explore_decay && explore_decay <= 1.0);
	}

	// Determine termination age
	bool terminate_check = options.count("terminate-age") > 0;
	age_t terminate_age;
	if (terminate_check) {
		strExtract(options["terminate-age"], terminate_age);
		assert(0 <= terminate_age);
	}

    // Determine mc-timelimit
    timelimit_t mc_timelimit;
    strExtract(options["mc-timelimit"], mc_timelimit);
    //if we assume that time_limit > agent.numActions() we can be sure 
    //that every action is selected at least once
    if(mc_timelimit < ai.numActions()){
        std::cerr << "WARNING: time_limit not large enough to sample all actions" << std::endl;
    }

    std::cout << "starting agent/environment interaction loop...\n"; 

    // Intialise simulation
    initscr();
    clear();

    // Screen variables
    int width, height;
    getmaxyx(stdscr, height, width);
    if (width < 60 || height < 20) {
        endwin();
        std::cout << "Please increase your terminal size and run again." << std::endl;
        return;
    }

	// Agent/environment interaction loop
	for (unsigned int cycle = 1; !env.isFinished(); cycle++) {
        refresh();

        // check for agent termination
        if (terminate_check && ai.age() > terminate_age) {
            addstr(" Terminate age reached. ");
            break;
        }

		// Get a percept from the environment
		percept_t observation = env.getObservation();
		percept_t reward = env.getReward();

		// Update agent's environment model with the new percept
		ai.modelUpdate(observation, reward);

		// Determine best exploitive action, or explore
		action_t action;
		if (explore && rand01() < explore_rate) {
			action = ai.genRandomAction();
		}
		else {
			action = search(ai, mc_timelimit); // TODO: implement in search.cpp
		}

		// Send an action to the environment
		env.performAction(action); 

		// Update agent's environment model with the chosen action
		ai.modelUpdate(action); 

		// Update exploration rate
		if (explore) explore_rate *= explore_decay;
	}
    addstr("Press any key to exit...");
    char input = getch();
    endwin();

	// Print summary to standard output
	std::cout << std::endl << std::endl << "SUMMARY" << std::endl;
	std::cout << "agent age: " << ai.age() << std::endl;
	std::cout << "average reward: " << ai.averageReward() << std::endl;

    // Write context tree file
    if(options["write-ct"] != ""){
    	// write a ct for the final cycle too.
		char cycle_string[256];
		sprintf(cycle_string, "%lld", ai.age());
		std::ofstream ct((options["write-ct"] + std::string(cycle_string) + ".ct").c_str());
		ai.writeCT(ct);
		ct.close();
    }
}


// Populate the 'options' map based on 'key=value' pairs from an input stream
void processOptions(std::ifstream &in, options_t &options) {
	std::string line;
	size_t pos;

	for (int lineno = 1; in.good(); lineno++) {
		std::getline(in, line);

		// Ignore # comments
		if ((pos = line.find('#')) != std::string::npos) {
			line = line.substr(0, pos);
		}

		// Remove whitespace
		while ((pos = line.find(" ")) != std::string::npos)
			line.erase(line.begin() + pos);
		while ((pos = line.find("\t")) != std::string::npos)
			line.erase(line.begin() + pos);


		// Split into key/value pair at the first '='
		pos = line.find('=');
		std::string key = line.substr(0, pos);
		std::string value = line.substr(pos + 1);

		// Check that we have parsed a valid key/value pair. Warn on failure or
		// set the appropriate option on success.
		if (pos == std::string::npos) {
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no '=')" << std::endl;
		}
		else if (key.size() == 0) {
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no key)" << std::endl;
		}
		else if (value.size() == 0) {
			std::cerr << "WARNING: processOptions skipping line " << lineno << " (no value)" << std::endl;
		}
		else {
			options[key] = value; // Success!
			//std::cout << "OPTION: '" << key << "' = '" << value << "'" << std::endl;
		}

	}
}

void parseCmdOptions(int argc, char *argv[], options_t &options){
    for(int i = 0; i < argc; ++i){
        std::string arg = argv[i];

        std::size_t pos = arg.find("=");
        if(pos != std::string::npos){
            std::string option = arg.substr(0, pos);
            //remove leading -- 
            if(option.find("--") == 0){
                option = option.substr(2);
            }
            std::string value = arg.substr(pos+1);
            
            //add to options dict
            if(option.length() > 0 && value.length() > 0){
                options[option] = value;
            }
        }
    }
}

void printOptions(options_t &options){
    std::cout << "Agent configuration:\n------------------------------\n";
    for(options_t::iterator it = options.begin(); it != options.end(); ++it){
		std::cout << "OPTION: '" << it->first << "' = '" << it->second << "'" << std::endl;
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		std::cerr << "USAGE: ./aixi agent.conf [--option1=value1 --option2=value2 ...] " << std::endl;
		std::cerr << "The first argument should indicate the location of the configuration file. Further arguments can either be specified in the config file or passed as command line option. Command line options are used over options specified in the file." << std::endl;
		return -1;
	}

	// Load configuration options
	options_t options;

	// Default configuration values
	options["ct-depth"] = "16";
	options["agent-horizon"] = "3";
	options["exploration"] = "0";     // do not explore
	options["explore-decay"] = "1.0"; // exploration rate does not decay
    options["mc-timelimit"] = "500"; //number of mc simulations per search
    options["terminate-age"] = "10000";
    options["log"]  = "log";
    options["load-ct"] = "";
    options["write-ct"] = "";

	// Read configuration options
	std::ifstream conf(argv[1]);
	if (!conf.is_open()) {
		std::cerr << "ERROR: Could not open file '" << argv[1] << "' now exiting" << std::endl;
		return -1;
	}
	processOptions(conf, options);
	conf.close();

    //parse command line options (overwrites values of config files)
    parseCmdOptions(argc, argv, options);

	// Set up the environment
	Environment *env;
	std::string environment_name = options["environment"];
    if (environment_name == "pacman") {
        env = new Pacman(options);
		options["agent-actions"] = "4";
		options["observation-bits"] = "16";
		options["reward-bits"] = "8";
	}
	else {
		std::cerr << "ERROR: unknown environment '" << environment_name << "'" << std::endl;
		return -1;
	}

    printOptions(options);

	// Set up the agent
	Agent ai(options);

    if(options["load-ct"] != ""){
        std::ifstream ct(options["load-ct"].c_str());

        if(ct.is_open()){
            ai.loadCT(ct);
        }
        else{
            std::cerr << "WARNING: specified context tree file could not be loaded.\n";
        }
        ct.close();
    }  

    // Run the main agent/environment interaction loop
	mainLoop(ai, *env, options);

	return 0;
}
