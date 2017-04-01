#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct Relic
{
	int p;
	int w;
	float r;
};

// node used to branch
struct BranchNode
{
	int i;	// which relic this node is
	int w;	// total weight up to this node
	int p;	// total price up to this node
	float lb;	// the lower bound of this node
	float ub;	// the upper bound of this node
	BranchNode* next;	//next BranchNode
};

int totalNodes = 0;
void AddBranchNode(BranchNode* nodes, BranchNode* node)
{
	totalNodes++;
	BranchNode* pre = nodes;
	BranchNode* pos = nodes->next;
	while (1)
	{
		if (pos == nullptr)
		{
			pre->next = node;
			return;
		}
		else if (pos->ub < node->ub)
		{
			pre->next = node;
			node->next = pos;
			return;
		}
		else
		{
			pre = pos;
			pos = pos->next;
		}
	}
}

bool initKnapsack(int& knap, int& num, Relic** relics)
{
	//char getline[128];
	//scanf("%d", getline);
	//string line = getline;
	//size_t gap = line.find_first_of(" ");
	//knap = stoi(line.substr(0, gap));
	//num = stoi(line.substr(gap + 1));
	int a = 0;
	int b = 0;
	scanf("%d", &a);
	scanf("%d", &b);
	knap = a;
	num = b;

	*relics = new Relic[num];
	int i = 0;
	while (i < num)
	{
		//getline(cin, line);
		//getchar();
		//scanf("%[^\n]s", getline);
		//line = getline;
		//size_t gap = line.find_first_of(" ");
		scanf("%d", &a);
		scanf("%d", &b);
		(*relics)[i].p = a;
		(*relics)[i].w = b;
		(*relics)[i].r = float((*relics)[i].p) / float((*relics)[i].w);
		i++;
		//line = line.substr(end + 1);
	}

	return 0;
}

void sortKnapsack(Relic* relics, int num)
{
	for (int i = 0; i < num - 1; i++)
	{
		for (int j = i + 1; j < num; j++)
		{
			if (relics[i].r < relics[j].r)
			{
				Relic tmp = relics[j];
				relics[j] = relics[i];
				relics[i] = tmp;
			}
		}
	}
}

// get the lb and ub
int calcBound(BranchNode *node, Relic* relics, int knap, int num)
{
	int k = node->i + 1;
	int weight = node->w;
	for (; k < num && weight + relics[k].w < knap; k++)
	{
		node->lb += relics[k].p;
		weight += relics[k].w;
	}

	if (k < num)
	{
		float maxRatio = 0;
		for (int i = k; i < num; i++)
		{
			if (relics[i].r > maxRatio)
				maxRatio = relics[i].r;
		}
		node->ub = node->lb + (knap - weight) * maxRatio;
	}
	else
	{
		node->ub = node->lb;
	}

	return k;
}

int BranchAndBound(Relic* relics, int knap, int num)
{
	BranchNode* Nodes = new BranchNode;
	Nodes->next = nullptr;

	// to generate the first two node
	BranchNode* tmp = new BranchNode;
	tmp->next = nullptr;
	tmp->i = 0;
	tmp->p = relics[0].p;
	tmp->lb = tmp->p;
	tmp->w = relics[0].w;
	if (tmp->w < knap)
	{
		calcBound(tmp, relics, knap, num);
		AddBranchNode(Nodes, tmp);
	}
	tmp = new BranchNode;
	tmp->next = nullptr;
	tmp->i = 0;
	tmp->p = 0;
	tmp->lb = tmp->p;
	tmp->w = 0;
	calcBound(tmp, relics, knap, num);
	AddBranchNode(Nodes, tmp);
	
	BranchNode* current = nullptr;
	int max = 0;

	while (Nodes != nullptr && Nodes->next != nullptr)
	{
		current = Nodes->next;
		if (current != nullptr)
			Nodes->next = current->next;
		current->next = nullptr;

		if (current->ub < max)
		{
			delete current;
			continue;
		}

		// branch
		max = current->p > max ? current->p : max;

		if(current->i + 1 < num)
		{
			tmp = new BranchNode;
			tmp->next = nullptr;
			tmp->i = current->i + 1;
			tmp->p = current->p + relics[tmp->i].p;
			tmp->lb = tmp->p;
			tmp->w = current->w + relics[tmp->i].w;
			if (tmp->w <= knap)
			{
				calcBound(tmp, relics, knap, num);
				AddBranchNode(Nodes, tmp);
			}
			tmp = new BranchNode;
			tmp->next = nullptr;
			tmp->i = current->i + 1;
			tmp->p = current->p;
			tmp->lb = tmp->p;
			tmp->w = current->w;
			calcBound(tmp, relics, knap, num);
			AddBranchNode(Nodes, tmp);
		}
		delete current;
	}

	return max;
}

int main(int argc, char** argv)
{
	int knap = 0, num = 0;
	Relic* relics = nullptr;
	int max = 0;

	// read the original data
	initKnapsack(knap, num, &relics);
		
	// sort the relics to descending
	sortKnapsack(relics, num);

	// get the max price
	max = BranchAndBound(relics, knap, num);

	cout << max << endl;
	delete[] relics;
	return 0;
}