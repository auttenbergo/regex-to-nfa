#include<iostream>
#include<string>
#include<vector>
#include<map>
#include<algorithm>
#include<stack>
#include<fstream>
#include<queue>

using namespace std;

const char EPSILON = '-';
const string TEST_INPUT_PATH = "Public tests/Public tests/P1/In (public)/";
const string TEST_OUTPUT_PATH = "Public tests/Public tests/P1/Out (public)/";

template <typename Map>
bool map_compare(Map const& lhs, Map const& rhs) {
	// No predicate needed because there is operator== for pairs already.
	return lhs.size() == rhs.size()
		&& std::equal(lhs.begin(), lhs.end(),
			rhs.begin());
}

struct State {
	bool isRecepient;
	bool isLast;
	map<char, vector<State*> > transitions;
	bool operator==(const State&) const;
};

bool State::operator==(const State& s) const {
	if (isRecepient != s.isRecepient || isLast != s.isLast) return false;
	return map_compare(transitions, s.transitions);
}

struct NFA {
	bool isStarNFA;
	vector<State*> states;
};

bool isSimpleEpsilonNFA(NFA& nfa) {
	if (nfa.states.size() == 2 && nfa.states[0]->transitions.size() == 1 
		&& nfa.states[0]->transitions[EPSILON].size() == 1 && nfa.states[0]->transitions[EPSILON][0] == nfa.states[1] 
		&& nfa.states[1]->transitions.size() == 0)
		return true;
	return false;
}

bool isStarState(State* s) {
	auto transitions = s->transitions;
	/* For each transition of given state */
	for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
		vector<State*> transitionStates = itr->second;
		/* If one of transition states is given state itself return true */
		for (int j = 0; j < transitionStates.size(); j++) {
			if (transitionStates[j] == s)
				return true;
		}
	}
	return false;
}

State* createSimpleState(bool isRecepient, bool isLast) {
	State* state = new State;
	state->isRecepient = isRecepient;
	state->isLast = isLast;
	state->transitions = map<char, vector<State*>>();
	return state;
}

vector<State*> getRecepientStates(NFA nfa) {
	vector<State*> rv;
	for (int i = 0; i < nfa.states.size(); i++) {
		State* current = nfa.states[i];
		if (current->isRecepient)
			rv.push_back(current);
	}
	return rv;
}


bool vectorContainsState(vector<State*>& v, State* state) {
	return count(v.begin(), v.end(), state);
}

vector<pair<State*, char>> findTransitionsToState(NFA nfa, State* state) {
	vector<pair<State*, char>> rv;
	/* For each NFA state */
	for (int i = 0; i < nfa.states.size(); i++) {
		State* current = nfa.states[i];
		auto transitions = current->transitions;
		/* For each transition of current state */
		for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> transitionStates = itr->second;
			/* For each transition state transitions */
			for (int j = 0; j < transitionStates.size(); j++) {
				/*If transition state has transition to state, push in rv*/
				if (transitionStates[j] == state)
					rv.push_back(pair<State*, char>(current, symbol));
			}
		}
	}
	return rv;
}

NFA createSimpleNFA(char symbol) {
	State* end = createSimpleState(true, true);
	State* start = createSimpleState(false, false);
	start->transitions[symbol].push_back(end);

	if (symbol == EPSILON)
		start->isRecepient = true;

	NFA rv;
	rv.states.push_back(start);
	rv.states.push_back(end);

	rv.isStarNFA = false;
	return rv;
}

int countRecepients(NFA nfa) {
	int rv = 0;
	for (int i = 0; i < nfa.states.size(); i++) {
		if (nfa.states[i]->isRecepient)
			rv++;
	}
	return rv;
}

int countStateTransitions(State* state) {
	int rv = 0;
	auto transitions = state->transitions;
	for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
		rv += state->transitions[itr->first].size();
	}
	return rv;
}

int countTransitions(NFA nfa) {
	int rv = 0;
	for (int i = 0; i < nfa.states.size(); i++) {
		rv += countStateTransitions(nfa.states[i]);
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
		cout << countStateTransitions(current) << " ";

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
		if(i < nfa.states.size()-1)
			cout << endl;
	}
}



/*
	Algorithm:
		Find recepient states of A
		Find transitions of first state of B
		For each recepient state of A add the transitions of the first state of B.

		Delete the first state B if it is not a start state
			Because if it is a start state, recepient states connect to it directly
			Otherwise, None of the recepient states connect to it
*/
NFA concatNFA(NFA a, NFA b) {
	if (isSimpleEpsilonNFA(a)) {
		NFA rv;
		for (int i = 0; i < b.states.size(); i++)
			rv.states.push_back(b.states[i]);
		return rv;
	}
	else if (isSimpleEpsilonNFA(b)) {
		NFA rv;
		for (int i = 0; i < a.states.size(); i++)
			rv.states.push_back(a.states[i]);
		return rv;
	}

	vector<State*> recepientStatesA = getRecepientStates(a);

	State* firstStateB = b.states[0];

	map<char, vector<State*>> firstStateTransitionsB = firstStateB->transitions;

	/*For each recepient state of A */
	for (int i = 0; i < recepientStatesA.size(); i++) {
		State* currentRecepientStateA = recepientStatesA[i];

		bool wasConnectionMadeToB = false;

		/* Connect to all the states that firstStateB connects to */
		for (auto itr = firstStateTransitionsB.begin(); itr != firstStateTransitionsB.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> transitionStates = itr->second;
			for (int j = 0; j < transitionStates.size(); j++) {
				State* transitionState = transitionStates[j];
				if (!vectorContainsState(currentRecepientStateA->transitions[symbol], transitionState)) {
					wasConnectionMadeToB = true;
					currentRecepientStateA->transitions[symbol].push_back(transitionState);
				}
			}
		}
		

		/* Make it non recepient */
		currentRecepientStateA->isRecepient = false;

		/* make it recepient if first state of B is recepient */
		if (firstStateB->isRecepient)
			currentRecepientStateA->isRecepient = true;

		if (currentRecepientStateA->isLast && wasConnectionMadeToB)
			currentRecepientStateA->isLast = false;
	}

	NFA rv; rv.isStarNFA = false;
	/* Push all states of A in new NFA */
	for (int i = 0; i < a.states.size(); i++) {
		rv.states.push_back(a.states[i]);
	}
	if (!isStarState(firstStateB)) {
		b.states.erase(b.states.begin());
		delete firstStateB;
	}
	/* Push all states of B in new NFA */
	for (int i = 0; i < b.states.size(); i++) {
		rv.states.push_back(b.states[i]);
	}
	
	return rv;
}

bool canStarResultBeSimplified(NFA nfa) {
	if (nfa.states.size() != 2) return false;
	if (isStarState(nfa.states[0]) && isStarState(nfa.states[1])) return true;
	if (!isStarState(nfa.states[0]) && !isStarState(nfa.states[1])) return true;
	return false;
}

/*
	Algorithm:
		Get the transitions of the start state
		Add these transitions to all the recepient states in given NFA

		If after given operations the NFA state size is 2, then "combine" them in one state
		Otherwise, if first state of NFA is not star state, make it recepient, otherwise add new start state with "old start state transitions"
*/
void starNFA(NFA& nfa) {
	if (nfa.isStarNFA && isSimpleEpsilonNFA(nfa))
		return;

	nfa.isStarNFA = true;

	bool canBeSimplified = canStarResultBeSimplified(nfa);

	/*Get the recepient states */
	vector<State*> recepientStates = getRecepientStates(nfa);

	/* Get the second states with its transitions */
	vector< pair<State*, char> > secondStates;

	for (auto itr = nfa.states[0]->transitions.begin(); itr != nfa.states[0]->transitions.end(); ++itr) {
		char symbol = itr->first;
		vector<State*> transitionStates = itr->second;
		for (int j = 0; j < transitionStates.size(); j++) {
			secondStates.push_back(pair<State*, char>(transitionStates[j], symbol));
		}
	}

	/*For each recepient state */
	for (int i = 0; i < recepientStates.size(); i++) {
		State* currentRecepientState = recepientStates[i];
		/* Connect the last state to all the transitions made from start state */
		for (int j = 0; j < secondStates.size(); j++) {
			if (!vectorContainsState(currentRecepientState->transitions[secondStates[j].second], secondStates[j].first))
				currentRecepientState->transitions[secondStates[j].second].push_back(secondStates[j].first);
		}
	}


	/* If there were two states in the NFA, is expresion like both states are stars or none of them  */
	if (canBeSimplified) {
		State* startState = nfa.states[0];
		State* endState = nfa.states[1];

		/*Every star symbol is going to be star symbol of end state*/
		for (auto itr = startState->transitions.begin(); itr != startState->transitions.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> states = itr->second;
			for (int i = 0; i < states.size(); i++) {
				if (states[i] == startState) {
					if (!vectorContainsState(endState->transitions[symbol], endState))
						endState->transitions[symbol].push_back(endState);
				}
			}
		}
		/*Remove transitions to start state*/
		for (auto itr = endState->transitions.begin(); itr != endState->transitions.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> states = itr->second;
			for (int i = 0; i < states.size(); i++) {
				if (states[i] == startState)
					endState->transitions[symbol].erase(endState->transitions[symbol].begin() + i);
			}
		}

		nfa.states.erase(nfa.states.begin());
		delete startState;
	}
	else {
		/* Make start state recepient if it is not star state */
		if (!isStarState(nfa.states[0]))
			nfa.states[0]->isRecepient = true;
		else {
			State* newStartState = createSimpleState(true, false);
			for (int i = 0; i < secondStates.size(); i++) {
				newStartState->transitions[secondStates[i].second].push_back(secondStates[i].first);
			}

			vector<State*> currentNFAstates = nfa.states;
			vector<State*> newNFAstates; newNFAstates.push_back(newStartState);
			for (int i = 0; i < currentNFAstates.size(); i++) {
				newNFAstates.push_back(currentNFAstates[i]);
			}
			nfa.states = newNFAstates;
		}
	}
}

/*
	Algorithm:
		Create new start state and connect to each nfa start state transitions
		remove start state transitions if they were not star states
		
*/
NFA unionNFA(NFA a, NFA b) {
	if (isSimpleEpsilonNFA(a)) {
		NFA rv;
		for (int i = 0; i < b.states.size(); i++)
			rv.states.push_back(b.states[i]);
		rv.states[0]->isRecepient = true;
		return rv;
	}
	else if (isSimpleEpsilonNFA(b)) {
		NFA rv;
		for (int i = 0; i < a.states.size(); i++)
			rv.states.push_back(a.states[i]);
		rv.states[0]->isRecepient = true;
		return rv;
	}

	State* firstStateA = a.states[0];
	State* firstStateB = b.states[0];

	/* Get the second states of A and B */
	vector<pair<State*, char>> secondStatesA;
	vector<pair<State*, char>> secondStatesB;
	for (auto itr = firstStateA->transitions.begin(); itr != firstStateA->transitions.end(); ++itr) {
		char symbol = itr->first;
		vector<State*> transitionStates = itr->second;
		for (int j = 0; j < transitionStates.size(); j++) {
			secondStatesA.push_back(pair<State*, char>(transitionStates[j], symbol));
		}
	}
	for (auto itr = firstStateB->transitions.begin(); itr != firstStateB->transitions.end(); ++itr) {
		char symbol = itr->first;
		vector<State*> transitionStates = itr->second;
		for (int j = 0; j < transitionStates.size(); j++) {
			secondStatesB.push_back(pair<State*, char>(transitionStates[j], symbol));
		}
	}

	/* Create a new start state */
	State* newStartState = createSimpleState(firstStateA->isRecepient || firstStateB->isRecepient, false);

	/* Connect it to the second states of A and B */
	for (int i = 0; i < secondStatesA.size(); i++) {
		pair<State*, char> currentPair = secondStatesA[i];
		if (!vectorContainsState(newStartState->transitions[currentPair.second], currentPair.first));
			newStartState->transitions[currentPair.second].push_back(currentPair.first);
	}
	for (int i = 0; i < secondStatesB.size(); i++) {
		pair<State*, char> currentPair = secondStatesB[i];
		if (!vectorContainsState(newStartState->transitions[currentPair.second], currentPair.first));
			newStartState->transitions[currentPair.second].push_back(currentPair.first);
	}
	
	NFA rv;
	rv.isStarNFA = false;

	rv.states.push_back(newStartState);

	/* Delete the first state of A if it is not star */
	if (!isStarState(firstStateA)) {
		a.states.erase(a.states.begin());
		delete firstStateA;
	}
	/* Push all the states of A in new NFA */
	for (int i = 0; i < a.states.size(); i++) {
		rv.states.push_back(a.states[i]);
	}
	/* Delete the first state of B if it is not star */
	if (!isStarState(firstStateB)) {
		b.states.erase(b.states.begin());
		delete firstStateB;
	}
	/* Push all the states of B in new NFA */
	for (int i = 0; i < b.states.size(); i++) {
		rv.states.push_back(b.states[i]);
	}


	return rv;
}
map<int, int> getBracketPairs(string regex) {
	map<int, int> rv;
	stack<int> openBrackets;
	for (int i = 0; i < regex.size(); i++) {
		char current = regex[i];
		if (current == '(') {
			openBrackets.push(i);
		}
		else if (current == ')') {
			int openBracketIndex = openBrackets.top();
			openBrackets.pop();
			rv[openBracketIndex] = i;

		}
	}
	return rv;
}

NFA convertRegexToNFA(string regex, int startIndex, int endIndex, map<int, int> bracketPairs) {
	NFA rv;
	int currentIndex = startIndex;
	while (currentIndex < endIndex) {
		char currentChar = regex[currentIndex];
		if (currentChar == '(') {
			int closingBracketIndex = bracketPairs[currentIndex];
			NFA secondNFA;
			if (closingBracketIndex - currentIndex == 1) {
				secondNFA = createSimpleNFA(EPSILON);
			}
			else
				secondNFA = convertRegexToNFA(regex, currentIndex + 1, closingBracketIndex, bracketPairs);

			int nextIndex = closingBracketIndex + 1;
			bool isStar = (closingBracketIndex < endIndex - 1) ? regex[nextIndex] == '*' : false;
			if (isStar)
				starNFA(secondNFA);
			if (rv.states.size() != 0)
				rv = concatNFA(rv, secondNFA);
			else
				rv = secondNFA;

			currentIndex = (isStar) ? closingBracketIndex + 2 : closingBracketIndex + 1;
		}
		else if (currentChar == '|') {
			NFA secondNFA = convertRegexToNFA(regex, currentIndex + 1, endIndex, bracketPairs);
			if (rv.states.size() != 0)
				rv = unionNFA(rv, secondNFA);
			else
				rv = secondNFA;
			break;
		}
		else {
			int nextIndex = currentIndex + 1;
			bool isStar = (currentIndex < endIndex - 1) ? regex[nextIndex] == '*' : false;
			NFA secondNFA = createSimpleNFA(currentChar);
			if (isStar)
				starNFA(secondNFA);
			if (rv.states.size() != 0)
				rv = concatNFA(rv, secondNFA);
			else
				rv = secondNFA;
			currentIndex += (isStar) ? 2 : 1;
		}
	}
	return rv;
}


vector<string> getAnswerNFA(NFA& nfa) {
	vector<string> rv;
	/* General Info */
	rv.push_back(to_string(nfa.states.size()) + " " + to_string(countRecepients(nfa)) + " " + to_string(countTransitions(nfa)));

	/* Recepient Indexes */
	string recepientIndexes = "";
	for (int i = 0; i < nfa.states.size(); i++) {
		if (nfa.states[i]->isRecepient)
			recepientIndexes += (to_string(i) + " ");
	}
	rv.push_back(recepientIndexes.substr(0, recepientIndexes.size() - 1));

	/*For each state transitions */
	for (int i = 0; i < nfa.states.size(); i++) {
		State* current = nfa.states[i];
		auto transitions = current->transitions;
		string stateTransitions = "";
		stateTransitions += to_string(countStateTransitions(current)) + " ";
		/* For each transition */
		for (auto itr = transitions.begin(); itr != transitions.end(); ++itr) {
			char symbol = itr->first;
			vector<State*> transitionStates = itr->second;
			/* For each transitionState print transitions */
			for (int j = 0; j < transitionStates.size(); j++) {
				State* toState = transitionStates[j];
				string symbolString; symbolString.assign(1, symbol);
				stateTransitions += symbolString + " " + to_string(findIndexOfStateInNFA(nfa, toState)) + " ";
			}
		}
		rv.push_back(stateTransitions.substr(0, stateTransitions.size() - 1));
	}


	return rv;
}

State* createEmptyState() {
	State* state = new State;
	state->isRecepient = false;
	state->transitions = map<char, vector<State*>>();
	return state;
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
		State* state = createEmptyState();
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
	cout << "Test 1 (BEGIN):" << endl;

	int passed = 0;
	int failed = 0;
	for (int test = 0; test < 20; test++) {
		string inputFile = "in";
		inputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		inputFile += ".txt";
		string outputFile = "out";
		outputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		outputFile += ".txt";


		ifstream outputStream;

		outputStream.open(TEST_OUTPUT_PATH + outputFile);
		NFA expectedNFA = parseFile(outputStream);
		outputStream.close();


		ifstream inputStream;

		inputStream.open(TEST_INPUT_PATH + inputFile);
		string regex; inputStream >> regex;
		inputStream.close();

		NFA answerNFA = convertRegexToNFA(regex,0,regex.size(),getBracketPairs(regex));
		
		vector<string> answerStringNFA = getAnswerNFA(answerNFA);
		vector<string> expectedStringNFA = getAnswerNFA(expectedNFA);

		for (int i = 2; i < answerStringNFA.size(); i++) {
			if(answerStringNFA[i].size() > 2)
				sort(answerStringNFA[i].begin()+2, answerStringNFA[i].end());
		}
		for (int i = 2; i < expectedStringNFA.size(); i++) {
			if (expectedStringNFA[i].size() > 2)
				sort(expectedStringNFA[i].begin()+2, expectedStringNFA[i].end());
		}

		if (expectedStringNFA == answerStringNFA)
			passed++;
		else {
			failed++;
		}

	}
	cout << "Passed: " << passed << endl;
	cout << "Failed: " << failed << endl;
	cout << "Test 1 (END):" << endl;

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
void checkLecturerTests2() {
	cout << "Test 2 (BEGIN):" << endl;
	const string TEST_INPUT_PATH_2 = "Public tests/P2/In (public)/";
	const string TEST_OUTPUT_PATH_2 = "Public tests/P2/Out (public)/";
	int passed = 0;
	int failed = 0;
	for (int test = 0; test < 20; test++) {
		string inputFile = "in";
		inputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		inputFile += ".txt";
		string outputFile = "out";
		outputFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		outputFile += ".txt";
		string regexFile = "reg";
		regexFile += (test / 10 == 0) ? "0" + to_string(test) : to_string(test);
		regexFile += ".txt";


		ifstream outputStream;

		outputStream.open(TEST_OUTPUT_PATH_2 + outputFile);
		string expected; outputStream >> expected;
		outputStream.close();


		ifstream inputStream;

		inputStream.open(TEST_INPUT_PATH_2 + inputFile);
		string word; inputStream >> word;
		inputStream.close();

		ifstream regexStream;
		regexStream.open(TEST_INPUT_PATH_2 + regexFile);
		string regex; regexStream >> regex;
		regexStream.close();

		NFA answerNFA = convertRegexToNFA(regex, 0, regex.size(), getBracketPairs(regex));
		string answer = simulateNFA(answerNFA, word);
		if (answer != expected)
			failed++;
		else
			passed++;

	}
	cout << "Passed: " << passed << endl;
	cout << "Failed: " << failed << endl;
	cout << "Test 2 (END):" << endl;

}

int main() {
	//checkLecturerTests();
	//checkLecturerTests2();
	string regex; cin >> regex;
	NFA nfa = convertRegexToNFA(regex, 0, regex.size(), getBracketPairs(regex));
	printNFA(nfa);

	return 0;
}