#include <vector>
#include <random>
#include <set>
#include <deque>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>

#if defined(WIN32) || defined(_WIN32) 
#define PATH_SEPARATOR '\\' 
#else 
#define PATH_SEPARATOR '/' 
#endif 


using namespace std;

string base_file = "base.csv";

vector<string> elementary;
vector<string> inter_blocks;
string highest_level;
size_t length;

bool fifty_fifty() {
	return rand() >= RAND_MAX / 2;
}

bool cycle_to_unused(deque<int> &digits, set<int> &used) {
	int first = digits.front();
	while (used.find(digits.front()) != used.end()) {
		digits.pop_front();
		if (digits.empty())
			return false;
	}
	return true;
}



vector<string> build_elem(int order, int num_blocks) {

	random_device rd;
	mt19937 g(rd());
	string elem_block = "";
	vector<string> building_blocks;

	vector<int> indices;

	for (size_t i = 0; i < length; ++i) {
		indices.push_back(i);
	}

	for (int index : indices) {
		elem_block += "*";
	}

	vector<int> shuffled_indices = vector<int>(indices);

	shuffle(shuffled_indices.begin(), shuffled_indices.end(), g);

	for (int i = 0; i < order; ++i) {
		if (fifty_fifty())
			elem_block.replace(shuffled_indices[i], 1, "1");
		else
			elem_block.replace(shuffled_indices[i], 1, "0");
	}

	deque<int> ones;
	deque<int> zeroes;
	string master = "";

	for (size_t i = 0; i < length; ++i) {
		master += elem_block[i];
		if (master[i] == '1')
			ones.push_back(i);
		else if (master[i] == '0')
			zeroes.push_back(i);
		else {
			ones.push_back(i);
			zeroes.push_back(i);
		}
	}

	building_blocks.push_back(elem_block);

	for (int i = 0; i < num_blocks - 1; ++i) {
		string block = "";

		while (block.length() < length) {
			block += "*";
		}
		shuffle(ones.begin(), ones.end(), g);
		shuffle(zeroes.begin(), zeroes.end(), g);
	

		set<int> used_ind;

		for (int j = 0; j < order; ++j) {
			if (fifty_fifty()) {
				cycle_to_unused(ones, used_ind);
				block.replace(ones.front(), 1, "1");
				used_ind.insert(ones.front());
				ones.pop_front();
			}
			else {
				cycle_to_unused(zeroes, used_ind);
				block.replace(zeroes.front(), 1, "0");
				used_ind.insert(zeroes.front());
				zeroes.pop_front();
			}
		}

		ones.clear();
		zeroes.clear();

		for(size_t j = 0; j < length; ++j){
			if (master[j] == '*') 
				master.replace(j, 1, block.substr(j, 1));
			
			if (master[j] == '1')
				ones.push_back(j);
			else if (master[j] == '0')
				zeroes.push_back(j);
			else {
				ones.push_back(j);
				zeroes.push_back(j);
			}
		}

		building_blocks.push_back(block);
	}

	highest_level = master;

	return building_blocks;
}

void build_inter(vector<string>& current_level) {
	random_device rd;
	mt19937 g(rd());
	
	if (current_level.size() <= 1)
		return;

	shuffle(current_level.begin(), current_level.end(), g);
	vector<string> next_level;

	for (size_t i = 0; i < current_level.size(); i += 2) {
		string block = "";

		string a = current_level[i];
		string b = current_level[i + 1];

		for (size_t j = 0; j < length; ++j) {
			if (a[j] != '*')
				block += a[j];
			else if (b[j] != '*')
				block += b[j];
			else
				block += "*";
		}

		next_level.push_back(block);
		inter_blocks.push_back(block);
	}

	build_inter(next_level);
}

vector<string> build_potholes(vector<string>& elem) {
	vector<string> potholes;
	random_device rd;
	mt19937 g(rd());

	shuffle(elem.begin(), elem.end(), g);

	for (int i = 0; i < elem.size(); ++i) {
		string base = elem[i];
		string neighbor = elem[(i + 1) % elem.size()];

		string hole = base;

		for (size_t j = 0; j < length; ++j) {
			if (neighbor[j] != '*') {
				if (fifty_fifty())
					hole.replace(j, 1, neighbor.substr(j, 1));
			}
		}
		
		potholes.push_back(hole);
	}

	return potholes;
}

void print(string block, int score, ostream & stream) {
	stream << block << "," << score << "\n";
}

void print(vector<string> blocks, int score, ostream & stream){
	for (auto block : blocks) {
		print(block, score, stream);
	}
}

void read_file(string filename, vector<string> & prev) {
	ifstream in(filename);
	string block;
	while (getline(in, block)) {
		prev.push_back(block);
	}
}

void write_base_file(string directory, vector<string> & elem, vector<string> & potholes) {
	string filepath = directory + PATH_SEPARATOR + base_file;
	ofstream out(filepath);

	print(elem, 2, out);
	print(highest_level, 3, out);
	print(potholes, -1, out);

}

string get_block(string &input) {
	return input.substr(0, length);
}

bool base_exists(string directory) {
	ifstream infile(directory + PATH_SEPARATOR + base_file);
	return infile.good();
}

/*
// write it so that you can just specify how many alternate schema to create
int main(int argc, char* argv[]) {

	string directory = argv[1];

	cout << PATH_SEPARATOR << endl;
	vector<string> potholes;

	if (argc != 3) {
		cout << "usage: shaky_ladder <directory> <number of intermediate to generate>\n";
	}

	if (base_exists(directory)) {
		vector<string> existing;
		read_file(directory + PATH_SEPARATOR + base_file, existing);

		length = existing.front().find(",");

		int i = 0;

		while (existing[i].substr(length + 1) == "2") {
			elementary.push_back(get_block(existing[i++]));
		}

		highest_level = get_block(existing[i++]);

		while (i < existing.size()) {
			potholes.push_back(get_block(existing[i++]));
		}

	}
	else {
		length = 10;
		elementary = build_elem(2, 4);
		potholes = build_potholes(elementary);

		write_base_file(directory, elementary, potholes);
	}

	string file_prefix = "";
	string inter_names = directory + PATH_SEPARATOR + file_prefix;


	int num_files = atoi(argv[2]);

	for (int i = 0; i < num_files; ++i) {
		inter_blocks.clear();
		build_inter(elementary);
		string file_num = to_string(i);
		cout << file_num << endl;

		string filename = inter_names + file_num + ".csv";
		cout << "writing " << filename << endl;
		ofstream out(filename);

		print(inter_blocks, 2, out);
		out.close();
	}
}
*/