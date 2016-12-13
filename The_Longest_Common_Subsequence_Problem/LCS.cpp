#include <string>
#include <iostream>
#include <fstream>
#include <stack>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;

int max(int a, int b) {
	return a > b ? a : b;
}

class Pair;

// A
// | - B ----------- C
//     | - D         | - E --- F
//         |- G - H      | - I | - J
// the subSequence would be "GDBA", "HDBA", "IECA", "JFCA"
// a tree to store each subPoint
struct subPoint {
	int i, j;
	subPoint* father;
	subPoint* next;
	subPoint* child;
	static int n;

	subPoint() {
		i = 0, j = 0;
		father = nullptr;
		next = nullptr;
		child = nullptr;
		n++;
	}

	subPoint(int ii, int jj) :i(ii), j(jj) {};

	~subPoint() {
		if(this->child != nullptr)
			delete this->child;
		if (this->next != nullptr)
			delete this->next;
		if (this->father != nullptr)
			this->father = nullptr;
		n--;
	}

	void addNext(subPoint* next) {
		subPoint* tmp = this->next;
		if (tmp == nullptr) {
			this->next = next;
		}
		else {
			while (tmp->next)
				tmp = tmp->next;
			tmp->next = next;
		}
	};
	void addChild(subPoint* child) {
		subPoint* tmp = this->child;
		if (tmp == nullptr) {
			this->child = child;
			return;
		}
		else {
			while(tmp->next != nullptr)
				tmp = tmp->next;
			tmp->next = child;
			return;
		}
	};
	bool operator==(const subPoint* rhs) {
		if (this->i == rhs->i && this->j == rhs->j && this->father == rhs->father)
			return true;
		else
			return false;
	};
};

int subPoint::n = 0;

struct Node {
	int i;
	int j;
	int val;
	Node() {};
	Node(int ii, int jj, int vv) :i(ii), j(jj), val(vv) {};
};

class Pair {
public:
	int i;
	int j;

	Pair() :i(0), j(0) {};
	Pair(int ii, int jj):i(ii), j(jj){};

	friend ostream& operator<<(std::ostream& os, const Pair& p);

	bool operator==(const Pair& rhs) {
		if (this->i == rhs.i && this->j == rhs.j)
			return true;
		else
			return false;
	};

	bool operator!=(const Pair& rhs) {
		if (this->i != rhs.i || this->j != rhs.j)
			return true;
		else
			return false;
	};
};

class Pairs {
public:
	vector<Pair> pairs;

	bool operator!=(const Pairs& rhs) {
		for (int i = 0; i < pairs.size(); i++) {
			if (this->pairs[i] != rhs.pairs[i])
				return false;
		}
		return true;
	};

	bool operator==(const Pairs& rhs) {
		for (int i = 0; i < pairs.size(); i++) {
			if (this->pairs[i] != rhs.pairs[i])
				return false;
		}
		return true;
	};

	bool operator>(const Pairs& rhs) {
		for (int i = 0; i < pairs.size(); i++) {
			if (this->pairs[i].i < rhs.pairs[i].i || (this->pairs[i].i == rhs.pairs[i].i && this->pairs[i].j < rhs.pairs[i].j))
				return false;
		}
		return true;
	};

	bool operator<(const Pairs& rhs) {
		for (int i = 0; i < pairs.size(); i++) {
			if (this->pairs[i].i > rhs.pairs[i].i || (this->pairs[i].i == rhs.pairs[i].i && this->pairs[i].j > rhs.pairs[i].j))
				return false;
		}
		return true;
	};
};

ostream& operator<<(ostream& os, const Pair& p) {
	os << "[" << p.i << ", " << p.j << "]";
	return os;
}

// file read
bool initString(string fileLoc, string &A, string &B) {
	ifstream testfile;
	testfile.open(fileLoc.c_str());
	if (!testfile.is_open()) {
		cout << "Cannot open test file!" << endl;
		return false;
	}

	getline(testfile, A);
	getline(testfile, B);
	testfile.close();
	return true;
}

// LCS Algorithm
// This returns the length of LCS
int lenofLCS(const char *A, const char *B, int* L, vector<Node> &nodes) {
	int alen = strlen(A), blen = strlen(B);

	// Calculate each [i, j] point weight
	int i = 0, j = 0;
	for (i = 0; i <= alen; i++) {
		for (j = 0; j <= blen; j++) {
			if (i == 0 || j == 0) {
				L[i * (blen + 1) + j] = 0;
			}
			else if (A[i - 1] == B[j - 1]) {
				L[i * (blen + 1) + j] = 1 + L[(i - 1) * (blen + 1) + j - 1];
				nodes.push_back(Node(i, j, L[i * (blen + 1) + j]));
			}
			else {
				L[i * (blen + 1) + j] = max(L[(i - 1) * (blen + 1) + j], L[i * (blen + 1) + j - 1]);
			}
		}
	}

	return L[alen * (blen + 1) + blen];
}

// Output the [i, j] result Matrix
bool outputMatrix(const char* A, const char* B, const int* L) {
	int alen = strlen(A), blen = strlen(B);

	if (!L)
		return false;

	for (int i = 0; i < blen; i++) {
		cout << "  " << B[i];
	}
	cout << endl;

	for (int i = 0; i < alen; i++) {
		cout << A[i] << " ";
		for (int j = 0; j < blen; j++) {
			cout << L[(i + 1) * (blen + 1) + j + 1] << "  ";
		}
		cout << endl;
	}
	return true;
}

// This get the subsequence
// use a tree to record this, to output the subsequence, traverse the tree
subPoint* getSubSequence(subPoint* subSequence, int i, int j, const char* A, const char* B, const int* L) {
	int alen = strlen(A), blen = strlen(B);
	subPoint* subNode = nullptr;
	
	while (i > 0 && j > 0) {
		if (L[i*(blen + 1) + j] == 0)
			return nullptr;
		else if (A[i - 1] == B[j - 1]) {
			subNode = new subPoint;
			subNode->i = i;
			subNode->j = j;
			subNode->father = subSequence;
			subNode->next = nullptr;
			subNode->addChild(getSubSequence(subNode, i - 1, j - 1, A, B, L));
			
			if (L[i*(blen + 1) + j] == L[(i - 1)*(blen + 1) + j]) {
				subSequence->addChild(getSubSequence(subSequence, i - 1, j, A, B, L));
			} 
			if (L[i*(blen + 1) + j] == L[i*(blen + 1) + j - 1]) {
				subSequence->addChild(getSubSequence(subSequence, i, j - 1, A, B, L));
			}
			break;
		}
		else {
			if (L[i * (blen + 1) + j - 1] > L[(i - 1) * (blen + 1) + j])
				j--;
			else if(L[i * (blen + 1) + j - 1] < L[(i - 1) * (blen + 1) + j])
				i--;
			else {
				subNode = getSubSequence(subSequence, i - 1, j, A, B, L);
				subNode->addNext(getSubSequence(subSequence, i, j - 1, A, B, L));
				break;
			}
		}
	}

	if (i == 0 || j == 0)
		return nullptr;

	//USELESS
	//for (subPoint* tmpi = subNode; tmpi && tmpi->next != nullptr; tmpi = tmpi->next) {
	//	for (subPoint* tmpj = tmpi->next; tmpj != nullptr;) {
	//		subPoint* tmp = nullptr;
	//		if (tmpi->i == tmpj->i && tmpj->j == tmpj->j) {
	//			tmpi->next = tmpj->next;
	//			tmp = tmpj;
	//			tmpj = tmpj->next;
	//			tmp->next = nullptr;
	//			delete tmp;
	//		}
	//		else
	//			tmpj = tmpj->next;
	//	}
	//}
	return subNode;
}

// Traverse the subSequence, it returns the total number of LCS
int traverseSequences(vector<string> &sequences, const char* A, const subPoint* subSequence, const int len) {
	int n = 0;
	stack<subPoint*> stk;
	if (!subSequence->child)
		return n;
	stk.push(subSequence->child);
	vector<Pairs> sq;

	while (!stk.empty()) {
		subPoint* tmp = stk.top();
		stk.pop();
		if (tmp->next != nullptr)
			stk.push(tmp->next);
		while (tmp->child != nullptr) {
			tmp = tmp->child;
			if (tmp->next != nullptr)
				stk.push(tmp->next);
		}
		//string sq;
		Pairs ps;
		for (int i = len; i > 0; i--) {
			/*sq += A[tmp->i - 1];
			tmp = tmp->father;*/
			ps.pairs.push_back(Pair(tmp->i - 1, tmp->j - 1));
			tmp = tmp->father;
		}
		//sequences.push_back(sq);
		bool equal = 0;
		for (int i = 0; i < sq.size(); i++)
			if (sq[i] == ps)
				equal = 1;
		if(!equal)
			sq.push_back(ps);
	}

	for (int i = 0; i < sq.size(); i++) {
		string tmp;
		for (int j = 0; j < sq[i].pairs.size(); j++) {
			tmp += A[sq[i].pairs[j].i];
		}
		sequences.push_back(tmp);
		n++;
	}
	sort(sequences.begin(), sequences.end());
	
	return n;
}

vector<string> getSubSequence(vector<Node> nodes, int len, int i, int j, const char* A) {
	if (len == 0)
	{
		vector<string> tmp;
		tmp.push_back("");
		return tmp;
	}
	else
	{
		vector<string> s;
		// recursive those who has lower value and on the top-left
		for (int m = 0; m < nodes.size(); m++)
		{
			if (nodes[m].val == len && (nodes[m].i < i && nodes[m].j < j))
			{
				vector<string> tmp;
				tmp = getSubSequence(nodes, len - 1, nodes[m].i, nodes[m].j, A);
				for (int n = 0; n < tmp.size(); n++)
				{
					tmp[n] += A[nodes[m].i - 1];
					s.push_back(tmp[n]);
				}
			}
		}

		return s;
	}
}

int main(int argc, char **argv) {
	// get A & B and there
	string A, B;
	string fileLoc = argv[1];
	if (!initString(fileLoc, A, B)) {
		cout << "init failed!" << endl;
		return 0;
	}

	int alen = A.length(), blen = B.length();
	int* L = new int[(alen + 1) * (blen + 1)];
	vector<Node> nodes;
	clock_t time;
	time = clock();
	int length = lenofLCS(A.c_str(), B.c_str(), L, nodes);
	float t1 = (clock() - time) / CLOCKS_PER_SEC;
	time = clock();
	//cout <<"The length of LCS: " << length << endl;
	//outputMatrix(A.c_str(), B.c_str(), L);

	//subPoint* subSequence = new subPoint;
	//subSequence->i = alen;
	//subSequence->j = blen;
	//subSequence->addChild(getSubSequence(subSequence, alen, blen, A.c_str(), B.c_str(), L));

	vector<string> sequences = getSubSequence(nodes, length, alen + 1, blen + 1, A.c_str());
	sort(sequences.begin(), sequences.end());
	float t2 = (clock() - time) / CLOCKS_PER_SEC;
	//int nums = traverseSequences(sequences, A.c_str(), subSequence, length);
	int nums = sequences.size();
	cout << length << " " << nums << endl;
	cout << "Took " << t1 << "s to get the LCS matrix and took " << t2 << "s to get the sequences." << endl;
	//for (vector<string>::iterator it = sequences.begin(); it != sequences.end(); it++) {
	//	cout << *it << endl;
	//}

	// split file name
	string outLoc;
	if (argv[2])
		outLoc = argv[2];
	else {
		size_t split = fileLoc.find_last_of("/\\");
		string filename = fileLoc.substr(split + 1);
		split = fileLoc.substr(0, split).find_last_of("/\\");
		outLoc = fileLoc.substr(0, split) + "\\myoutput\\" + filename;
	}
	ofstream out;
	out.open(outLoc);
	if (!out.is_open()) {
		"Unable to out the result!";
		return 0;
	}
	out << length << " " << nums << "\n";
	for (vector<string>::iterator it = sequences.begin(); it != sequences.end(); it++) {
		out << *it << "\n";
	}
	out.close();

	//getchar();
	//delete subSequence;
	delete[] L;
	return 1;
}