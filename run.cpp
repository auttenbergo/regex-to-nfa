#include<iostream>
#include<vector>
#include<map>
#include<algorithm>
#include<string>
#include<queue>
#include<fstream>



using namespace std;

const string TEST_INPUT_PATH = "Public tests/Public tests/P2/In (public)/";
const string TEST_OUTPUT_PATH = "Public tests/Public tests/P2/Out (public)/";


struct State {
	bool isRecepient;
	map<char, vector<State*>> transitions;
};

struct NFA {
	vector<State*> states;
};


State* createSimpleState() {
	State* state = new State;
	state->isRecepient = false;
	state->transitions = map<char, vector<State*>>();
	return state;
}


NFA buildNFA() {
	NFA nfa;

	string strStateCount; cin >> strStateCount;
	string strRecepientCount; cin >> strRecepientCount;
	string strTransitionCount; cin >> strTransitionCount;

	int stateCount = stoi(strStateCount);
	int recepientCount = stoi(strRecepientCount);
	int transitionCount = stoi(strTransitionCount);


	for (int i = 0; i < stateCount; i++) {
		State* state = createSimpleState();
		nfa.states.push_back(state);
	}

	for (int i = 0; i < recepientCount; i++) {
		string strIndex; cin >> strIndex;
		int index = stoi(strIndex);
		nfa.states[index]->isRecepient = true;
	}
	for (int i = 0; i < stateCount; i++) {
		State* current = nfa.states[i];
		string strCurrentTransitionCount; cin >> strCurrentTransitionCount;
		int currentTransitionCount = stoi(strCurrentTransitionCount);
		for (int j = 0; j < currentTransitionCount; j++) {
			string strSymbol; cin >> strSymbol;
			char symbol = strSymbol[0];
			string strIndex; cin >> strIndex;
			int index = stoi(strIndex);
			current->transitions[symbol].push_back(nfa.states[index]);
		}
	}
	return nfa;
}
int countRecepients(NFA nfa) {
	int rv = 0;
	for (int i = 0; i < nfa.states.size(); i++) {
		if (nfa.states[i]->isRecepient)
			rv++;
	}
	return rv;
}

int countTransitions(NFA nfa) {
	int rv = 0;
	for (int i = 0; i < nfa.states.size(); i++) {
		auto transitions = nfa.states[i]->transitions;
		for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
			rv += itr->second.size();
		}
	}
	return rv;
}

int findIndexOfStateInNFA(NFA& nfa, State* state) {
	for (int i = 0; i < nfa.states.size(); i++) {
		if (state == nfa.states[i])
			return i;
	}
	return -1;
}


void printNFA(NFA nfa) {
	/* States, Recepients, Transitions */
	cout << nfa.states.size() << " " << countRecepients(nfa) << " " << countTransitions(nfa) << endl;

	/* Recepient indexes */
	for (int i = 0; i < nfa.states.size(); i++) {
		if (nfa.states[i]->isRecepient) {
			cout << i << " ";
		}
	}
	cout << endl;

	/* For each state */
	for (int i = 0; i < nfa.states.size(); i++) {
		State* current = nfa.states[i];
		auto transitions = current->transitions;
		/* Print transition count */
		cout << transitions.size() << " ";

		/* For each transition */
		for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> transitionStates = itr->second;
			/* For each transitionState print transitions */
			for (int j = 0; j < transitionStates.size(); j++) {
				State* toState = transitionStates[j];
				cout << symbol << " " << findIndexOfStateInNFA(nfa, toState) << " ";
			}
		}
		cout << endl;
	}
	cout << endl << endl;
}

string simulateNFA(NFA& nfa, string word) {

	queue<State*> bfs;
	bfs.push(nfa.states[0]);
	string result = "";
	int currentIndex = 0;

	while (!bfs.empty() && currentIndex < word.size()) {
		vector<State*> states;
		while (!bfs.empty()) {
			states.push_back(bfs.front());
			bfs.pop();
		}

		bool wasInRecepient = false;
		char symbol = word[currentIndex];
		currentIndex++;
		for (int i = 0; i < states.size(); i++) {
			State* current = states[i];

			vector<State*> transitions = current->transitions[symbol];
			for (int j = 0; j < transitions.size(); j++) {
				bfs.push(transitions[j]);
				if (transitions[j]->isRecepient)
					wasInRecepient = true;
			}
		}
		if (wasInRecepient)
			result += "Y";
		else
			result += "N";
	}
	for (; currentIndex < word.size(); currentIndex++) {
		result += "N";
	}
	return result;
}



NFA parseFile(ifstream& stream) {
	NFA nfa;

	string strStateCount; stream >> strStateCount;
	string strRecepientCount; stream >> strRecepientCount;
	string strTransitionCount; stream >> strTransitionCount;

	int stateCount = stoi(strStateCount);
	int recepientCount = stoi(strRecepientCount);
	int transitionCount = stoi(strTransitionCount);


	for (int i = 0; i < stateCount; i++) {
		State* state = createSimpleState();
		nfa.states.push_back(state);
	}

	for (int i = 0; i < recepientCount; i++) {
		string strIndex; stream >> strIndex;
		int index = stoi(strIndex);
		nfa.states[index]->isRecepient = true;
	}
	for (int i = 0; i < stateCount; i++) {
		State* current = nfa.states[i];
		string strCurrentTransitionCount; stream >> strCurrentTransitionCount;
		int currentTransitionCount = stoi(strCurrentTransitionCount);
		for (int j = 0; j < currentTransitionCount; j++) {
			string strSymbol; stream >> strSymbol;
			char symbol = strSymbol[0];
			string strIndex; stream >> strIndex;
			int index = stoi(strIndex);
			current->transitions[symbol].push_back(nfa.states[index]);
		}
	}
	return nfa;
}

void checkLecturerTests() {
	int passed = 0;
	int failed = 0;
	for (int test = 0; test < 20; test++) {
		string inputFile = "in";
		inputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		inputFile += ".txt";
		string outputFile = "out";
		outputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		outputFile += ".txt";


		ifstream inputStream;
		ifstream outputStream;

		inputStream.open(TEST_INPUT_PATH+inputFile);
		outputStream.open(TEST_OUTPUT_PATH + outputFile);

		string word; inputStream >> word;
		string expected; outputStream >> expected;

		NFA nfa = parseFile(inputStream);
		string answer = simulateNFA(nfa, word);
		if (answer == expected)
			passed++;
		else
			failed++;
		inputStream.close();
	}
	cout << "Passed: " << passed << endl;
	cout << "Failed: " << failed << endl;
}

int main() {
	//checkLecturerTests();
	string word; 
	cin >> word;
	NFA nfa = buildNFA();
	string answer = simulateNFA(nfa, word);
	cout << answer << endl;

	return 0;
}