#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

enum constraint{ pos, neg, zer };
class Limit {
public:
	float a, b, c;
	constraint I;
	float slope, hither;
	Limit* next;
	Limit* prev;

	Limit() {
		a = INFINITY;
		b = INFINITY;
		c = INFINITY;
		slope = INFINITY;
		hither = INFINITY;
		next = nullptr;
		prev = nullptr;
	}

	Limit(float aa, float bb, float cc) :a(aa), b(bb), c(cc) {
		// ensure that b != 0
		I = bb < 0 ? neg : (bb == 0 ? zer : pos);
		slope = - aa / bb;
		hither = cc / bb;
		next = nullptr;
		prev = nullptr;
	};
	
	~Limit() {
		if (next != nullptr)
			next->prev = prev;
		if (prev != nullptr)
			prev->next = next;
	}

	float yValue(float x) {
		if (b == 0)
			return INFINITY;
		return (c - a * x) / b;
	}

	bool isTrue(float x, float y) {
		if (a * x + b * y <= c)
			return true;
		else
			return false;
	}
};

// This is the intersect point of 2 limit lines
class InterPoint {
public:
	float x;
	float y;
	Limit* l1;
	Limit* l2;
	constraint I;
	InterPoint() :x(0), l1(nullptr), l2(nullptr), I(zer) {};
	InterPoint(Limit* ii, Limit* jj) :l1(ii), l2(jj) {
		x = (l1->c * l2->b - l2->c * l1->b) / (l1->a * l2->b - l2->a * l1->b);
		I = l1->I;
		y = l1->yValue(x);
	};
};

bool initLimits(const char* fileLoc, Limit* pos, Limit* neg, float &u1, float &u2) {
	ifstream testfile;
	testfile.open(fileLoc);
	if (!testfile.is_open()) {
		cout << "Unable to read the file!" << endl;
		return 0;
	}

	Limit* posCur = pos;
	Limit* negCur = neg;
	string line;
	getline(testfile, line);
	while (getline(testfile, line)) {
		size_t fir = line.find_first_of(" ");
		size_t sec = line.find_last_of(" ");
		float a = stof(line.substr(0, fir));
		float b = stof(line.substr(fir + 1, sec-fir));
		float c = stof(line.substr(sec + 1));
		
		if (a == 0 && b == 0) {
			cout << "unvalid input, should output NA, this is for warning" << endl;
			getchar();
			return false;
		}

		if (b > 0) {
			Limit* limit = new Limit(a, b, c);
			posCur->next = limit;
			limit->prev = posCur;
			posCur = limit;
		}
		else if (b < 0) {
			Limit* limit = new Limit(a, b, c);
			negCur->next = limit;
			limit->prev = negCur;
			negCur = limit;
		}
		else {
			float tmp = float(c) / float(a);
			if (a < 0) {
				u1 = tmp > u1 ? tmp : u1;
			}
			else if (a > 0) {
				u2 = tmp < u2 ? tmp : u2;
			}
		}
	}
	return true;
}

// return x-coordinates of their intersection
float calcPairIntersection(Limit l1, Limit l2) {
	float cb = (l2.c * l1.b - l1.c * l2.b);
	float ab = (l1.a * l2.b - l2.a * l1.b);
	if (ab == 0)
		return INFINITY;
	return cb / ab;
}

bool PointComp(InterPoint l, InterPoint r) {
	return l.x < r.x;
}

bool slopeComp(Limit* l, Limit* r) {
	return l->slope < r->slope;
}

//step 2 get the intersection
void getIntersectionPoints(vector<InterPoint> &rx, Limit* lines, float xl, float xr)
{
	// pair of lines
	Limit* prev = lines->next;
	if (prev == nullptr)
		return;
	Limit* next = prev->next;

	while (next != nullptr)
	{
		if (prev->slope == next->slope)
		{
			// 2 lines are parallel, one of them can be removed
			Limit* min = prev->hither < next->hither ? prev : next;
			Limit* max = prev->hither > next->hither ? prev : next;
			if (prev->I == neg)
			{
				// I is negative, choose the upper line
				delete min;
				prev = max;
				next = prev->next;
			}
			else
			{
				// I is positive, choose the lower line
				delete max;
				prev = min;
				next = prev->next;
			}
		}
		else
		{
			// not parallel, calculate the Intersect Point
			InterPoint iPoint(prev, next);
			if (iPoint.x < xl)
			{
				// ipoint is on the left side of the feasible region
				Limit* min = prev->slope < next->slope ? prev : next;
				Limit* max = prev->slope > next->slope ? prev : next;
				if (prev->I == neg) {
					delete min;
					prev = max;
				}
				else
				{
					delete max;
					prev = min;
				}
				next = prev->next;
			}
			else if (iPoint.x > xr)
			{
				// ipoint is on the right side of the feasible region
				Limit* min = prev->slope < next->slope ? prev : next;
				Limit* max = prev->slope > next->slope ? prev : next;
				if (prev->I == neg) {
					delete max;
					prev = min;
				}
				else
				{
					delete min;
					prev = max;
				}
				next = prev->next;
			}
			else {
				// ipoint is in the feasible region
				rx.push_back(iPoint);
				prev = next->next;
				if (prev != nullptr)
					next = prev->next;
				else
					return;
			}
		}
	}
}

float PruneAndSearch(Limit* pos, Limit* neg, float u1, float u2, float &Xs, float &minY) {
	float xm = 0;
	float xl = u1, xr = u2; // left and right bound of x
	float xs; // x-star, the optimal x
	float alpha = -INFINITY, beta = INFINITY;		// boundary point by I-neg and I-pos

	// prune
	while (1) {
		vector<InterPoint> rx;
		// step 2, determine rx, for pos lines and neg lines seperately
		getIntersectionPoints(rx, pos, xl, xr);
		getIntersectionPoints(rx, neg, xl, xr);

		// step 3
		InterPoint median;
		if (rx.size()) {
			sort(rx.begin(), rx.end(), PointComp);
			median = rx[rx.size() / 2];
		}
		else {
			//only one line in pos or neg, return its intersect
			if (neg->next == nullptr)
			{
				minY = -INFINITY;
				return -2;
			}
			else
			{
				if (pos->next == nullptr)
				{
					if (neg->next->slope > 0)
					{
						Xs = xl;
						minY = neg->next->yValue(Xs);
						return 0;
					}
					else if (neg->next->slope < 0)
					{
						Xs = xr;
						minY = neg->next->yValue(Xs);
						return 0;
					}
					else
					{
						Xs = 0;
						minY = neg->next->yValue(Xs);
						return 0;
					}
				}
				else
				{
					float alpha, beta;
					if (neg->next->slope > 0)
					{
						alpha = neg->next->yValue(xl);
						beta = pos->next->yValue(xl);
						Xs = xl;
					}
					else
					{
						alpha = neg->next->yValue(xr);
						beta = pos->next->yValue(xr);
						Xs = xr;
					}
					if (alpha <= beta)
					{
						minY = neg->next->yValue(Xs);
						return 0;
					}
					else
					{
						InterPoint P(neg->next, pos->next);
						if (xl <= P.x && P.x <= xr)
						{
							Xs = P.x;
							minY = P.y;
							return 0;
						}
						else
						{
							return -1;
						}
					}
				}
			}
		}

		// step 4.1, find median x and smax, smin, tmax, tmin
		xm = median.x;
		Limit* smin = nullptr;
		Limit* smax = nullptr;
		Limit* tmin = nullptr;
		Limit* tmax = nullptr;
		if (median.I == constraint::neg) {
			// median denotes the smin and smax, 
			// go to find the lowest intersect and its 2 lines in pos
			smin = median.l1->slope < median.l2->slope ? median.l1 : median.l2;
			smax = median.l1->slope > median.l2->slope ? median.l1 : median.l2;
			alpha = median.y;
			beta = INFINITY;
			Limit* curLine = pos->next;
			while (curLine != nullptr)
			{
				float bValue = curLine->yValue(xm);
				if (bValue < beta)
				{
					beta = bValue;
					tmin = curLine; tmax = curLine;
				}
				else if(beta == bValue)
				{
					if (tmin != nullptr && tmax != nullptr)
					{
						tmin = curLine->slope < tmin->slope ? curLine : tmin;
						tmax = curLine->slope > tmax->slope ? curLine : tmax;
					}
					else
					{
						tmin = curLine; tmax = curLine;
					}
				}
				curLine = curLine->next;
			}
		}
		else {
			// median denotes the tmin and tmax,
			// go to find the highest intersect and its 2 lines in neg
			tmin = median.l1->slope < median.l2->slope ? median.l1 : median.l2;
			tmax = median.l1->slope > median.l2->slope ? median.l1 : median.l2;
			alpha = -INFINITY;
			beta = median.y;
			Limit* curLine = neg->next;
			while (curLine != nullptr)
			{
				float aValue = curLine->yValue(xm);
				if (aValue > alpha)
				{
					alpha = aValue;
					smin = curLine; smax = curLine;
				}
				else if (aValue == alpha)
				{
					if (smin != nullptr && smax != nullptr)
					{
						smin = curLine->slope < smin->slope ? curLine : smin;
						smax = curLine->slope > smax->slope ? curLine : smax;
					}
					else
					{
						smin = curLine; smax = curLine;
					}
				}
				curLine = curLine->next;
			}
		}
		
		// step 4.2, decide xm and xs
		if (alpha <= beta) {
			// Case 1, 2, 3
			if (smax->slope < 0) {
				//delete smin;
				xl = xm;
			}
			else if (smin->slope > 0) {
				//delete smax;
				xr = xm;
			}
			else if (smin->slope <= 0 && smax->slope >= 0) {
				// optimal x found!!!!!!!!!!!!!!
				Xs = xm;
				minY = smin->yValue(xm);
				return 0;
			}
		}
		else
		{
			// alpha > beta, Case 4, 5, 6
			if (smax->slope < tmin->slope)
				xl = xm;
			if (smin->slope > tmax->slope)
				xr = xm;
			if (smax->slope >= tmin->slope && smin->slope <= tmax->slope)
			{
				// no feasible region
				return -1;
			}
		}

		// step 5, prune redundant
		for (int i = 0; i < rx.size(); i++)
		{
			Limit* l1 = rx[i].l1;
			Limit* l2 = rx[i].l2;
			if (rx[i].x <= xl)
			{
				// ipoint is on the left side of the feasible region
				Limit* min = l1->slope < l2->slope ? l1 : l2;
				Limit* max = l1->slope > l2->slope ? l1 : l2;
				if (l1->I == constraint::neg) {
					delete min;
				}
				else
				{
					delete max;
				}
			}
			else if (rx[i].x >= xr)
			{
				// ipoint is on the right side of the feasible region
				Limit* min = l1->slope < l2->slope ? l1 : l2;
				Limit* max = l1->slope > l2->slope ? l1 : l2;
				if (l1->I == constraint::neg) {
					delete max;
				}
				else
				{
					delete min;
				}
			}
		}
	}
}

int main(int argc, char** argv) {
	vector<Limit* > positive, negative;
	Limit* posHead = new Limit();
	Limit* negHead = new Limit();
	if (!argv[1]) {
		cout << "input file name plz." << endl;
		return 0;
	}
	string fileLoc = argv[1];
	float u1 = -INFINITY, u2 = INFINITY;
	initLimits(fileLoc.c_str(), posHead, negHead, u1, u2);

	float minY = -INFINITY;
	float result;
	float xs;

	result = PruneAndSearch(posHead, negHead, u1, u2, xs, minY);

	if (result == 0)
		cout << int(minY > 0 ? minY + 0.5 : minY - 0.5) << endl;
	else if (result == -1)
		cout << "NA" << endl;
	else if (result <= -2)
		cout << "-INF" << endl;

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
	out << minY << "\n";
	out.close();

	//getchar();
	return 1;
}