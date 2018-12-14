#include <vector>
#include <random>
#include <set>
#include <deque>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <time.h>

using namespace std;

string current_best;
int population_score;


struct candidate {
	string str;
	int score;
};

vector<candidate> candidates;

char get_bit() {
	if (rand() >= RAND_MAX / 2)
		return '0';

	return '1';
}

struct scheme {
	string hp;
	int score;
};

void cross_string(string &a, string &b,  int length) {
	random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(0, a.length() - length);

	int i = distribution(gen);
	cout << i << endl;

	string sub = a.substr(i, length);

	a.replace(i, length, b.substr(i, length));
	b.replace(i, length, sub);
}

void mutate_string(string& str, double p_x) {
	for (int i = 0; i < str.length();++i) {
		int random = rand();
		//cout << random<< endl;
		if (random < RAND_MAX * p_x) {
			
			char current = str[i];
			string mutated = "1";
			if (current == '1')
				mutated = "0";

			str.replace(i, 1, mutated);
		}
	}
}

bool in_scheme(const string& cand, const string& plane) {
	for (int i = 0; i < plane.length(); ++i) {
		if (plane[i] != '*') {
			if (plane[i] != cand[i])
				return false;
		}
	}

	return true;
}

int score_string(const string& cand, const vector<scheme>& schemata) {
	int score = 0;
	for (scheme s : schemata) {
		if (in_scheme(cand, s.hp))
			score += s.score;
	}
	return score;
}

void score_generation(vector<candidate> &cands, const vector<scheme>& schemata) {
	for (candidate cand : cands) {
		score_string(cand.str, schemata);
	}
}

void select_by_fitness(vector<candidate> &cands, vector<string> &chosen, int num_chosen) {
	set<int> picked_indices;
	vector<int> sum_weight;

	sum_weight.push_back(cands[0].score);

	for (int i = 1; i < cands.size();++i) {
		sum_weight.push_back(sum_weight.back() + cands[i].score);
	}

	random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(0, sum_weight.back() - 1 );

	while (chosen.size() < num_chosen) {
		int number = distribution(gen);

		int picked = -1;
		for (int i = 0; i < cands.size() && picked < 0; ++i) {
			if (sum_weight[i] > number) 
				picked = i;
		}

		if (picked_indices.find(picked) == picked_indices.end()) {
			picked_indices.insert(picked);
			chosen.push_back(cands[picked].str);
		}
	}
}

void read_in_schematas(char * directory) {}

void init_strings(vector<candidate> &holder, int num_strings, int string_length) {

	for (int i = 0; i < num_strings; ++i) {
		string str = "";
		for (int j = 0; j < string_length; ++j)
		{
			str += get_bit();
		}

		candidate temp = { str, 0 };
		holder.push_back(temp);
	}
}

int main() {
	srand(time(NULL));
	vector<candidate> population;

	init_strings(population, 10, 32);
	/*
	for (candidate cand : population) {
		cout << cand.str << endl;
	}
	*/

	cout << population[0].str << endl;
	cout << population[1].str << endl;

	cross_string(population[0].str, population[1].str, 3);

	cout << population[0].str << endl;
	cout << population[1].str << endl;
}