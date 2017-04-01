#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

struct Point
{
	int x;
	int y;
	int i;
};

// get the square value
// it would be faster if i do sqrt at last
int getSquareDist(Point l, Point r)
{
	int xoff = l.x - r.x;
	int yoff = l.y - r.y;
	return xoff * xoff + yoff * yoff;
}

float getDist(Point l, Point r)
{
	float xoff = float(l.x - r.x);
	float yoff = float(l.y - r.y);
	return sqrt(xoff * xoff + yoff * yoff);
}

void BubbleSort(Point* points, int num)
{
	for (int i = num; i > 0; i--)
	{
		for (int j = 0; j < i - 1; j++)
		{
			if (points[j].x >points[j + 1].x)
			{
				Point tmp = points[j + 1];
				points[j + 1] = points[j];
				points[j] = tmp;
			}
		}
	}
}

void MergeSort(int l, int r, Point* cand, Point* points)
{
	if (r == l)
		return;	// merge size == 1

	int mid = (l + r) / 2;
	MergeSort(l, mid, points, cand);
	MergeSort(mid + 1, r, points, cand);

	// merge
	int i = l, j = mid + 1;
	for (int k = l; k <= r; k++)
	{
		if (i <= mid && (j > r || cand[i].x <= cand[j].x))
		{
			points[k] = cand[i];
			i++;
		}
		else
		{
			points[k] = cand[j];
			j++;
		}
	}
}

bool initPoints(int& num, Point** points)
{
	int a = 0;
	int b = 0;
	int c = 0;
	scanf("%d", &a);
	num = a;

	*points = new Point[num];
	int i = 0;
	while (i < num)
	{
		scanf("%d", &a);
		scanf("%d", &b);
		(*points)[i].x = a;
		(*points)[i].y = b;
		(*points)[i].i = i + 1;
		i++;
	}

	return 1;
}

// every divideandconquer calc the 2dcp from points[left] to points[right]
// num refers to the total points number between left and right, also used to calc the next divide bound
// and write the result into the pairs vector
float DivideAndConquer(vector<Point> &pairs, int left, int right, int num, Point* points)
{
	// return the value if the number is small enough
	if (num == 2)
	{
		Point tmp;
		tmp.x = points[left].i;
		tmp.y = points[right].i; 
		pairs.push_back(tmp);
		return getDist(points[left], points[right]);
	}

	// get the median to divide
	vector<Point> lpairs, rpairs;
	int moffset = (num + 1) * 0.5;
	int lr, rl;
	lr = left + moffset - 1;
	rl = (num % 2 == 0) ? lr + 1 : lr;
	float dl = DivideAndConquer(lpairs, left, lr, moffset, points);
	float dr = DivideAndConquer(rpairs, rl, right, moffset, points);
	float line = float(points[lr].x + points[rl].x) * 0.5;

	// Merge and return
	float d = (dl <= dr) ? dl : dr;
	pairs = (dl <= dr) ? lpairs : rpairs;
	if (dl == dr)
	{
		for (int i = 0; i < rpairs.size(); i++)
			pairs.push_back(rpairs[i]);
	}

	// need to test if i pre-sort the points would be faster or not
	// use another tricky way
	Point* strip = new Point[num];
	int sn = 0;
	for (int i = lr; i >= left; i--)
	{
		if (line - float(points[i].x) > d)
			break;
		strip[sn] = points[i];
		sn++;
	}
	if (rl != lr)
	{
		strip[sn] = points[rl];
		sn++;
	}
	for (int i = rl + 1; i <= right; i++)
	{
		if (float(points[i].x) - line > d)
			break;
		strip[sn] = points[i];
		sn++;
	}

	// sort the points according to the y value
	for (int i = sn; i > 0; i--)
	{
		for (int j = 0; j < i - 1; j++)
		{
			if (strip[j].y >strip[j + 1].y)
			{
				Point tmp = strip[j + 1];
				strip[j + 1] = strip[j];
				strip[j] = tmp;
			}
		}
	}

	// calc the min value, if it is smaller than d, replace d
	for (int i = 0; i < sn; i++)
	{
		for (int j = i + 1; j < sn && (strip[j].y - strip[i].y) < d; j++)
		{
			float dist = getDist(strip[i], strip[j]);
			if (dist < d)
			{
				d = dist;
				Point tmp;
				tmp.x = strip[i].i < strip[j].i ? strip[i].i : strip[j].i;
				tmp.y = strip[i].i > strip[j].i ? strip[i].i : strip[j].i;
				pairs.clear();
				pairs.push_back(tmp);
			}
			else if (dist == d && ((strip[i].x < line && strip[j].x > line) || (strip[i].x > line && strip[j].x < line)))
			{
				Point tmp;
				tmp.x = strip[i].i < strip[j].i ? strip[i].i : strip[j].i;
				tmp.y = strip[i].i > strip[j].i ? strip[i].i : strip[j].i;
				pairs.push_back(tmp);
			}
		}
	}
	return d;
}

int main(int argc, char** argv)
{
	int knap = 0, num = 0;
	Point* points = nullptr;

	// read the original data
	if (!initPoints(num, &points))
	{
		return 0;
	}

	// sort the points according to the x value
	//BubbleSort(points, num);
	Point* cand = new Point[num];
	for (int i = 0; i < num; i++)
		cand[i] = points[i];

	MergeSort(0, num - 1, cand, points);

	vector<Point> pairs;
	// divide and conquer
	float min = float(DivideAndConquer(pairs, 0, num - 1, num, points));

	// sort the result pairs
	for (int i = pairs.size(); i > 0; i--)
	{
		for (int j = 0; j < i - 1; j++)
		{
			if (pairs[j].x > pairs[j + 1].x || (pairs[j].x == pairs[j + 1].x && pairs[j].y > pairs[j + 1].y))
			{
				Point tmp = pairs[j + 1];
				pairs[j + 1] = pairs[j];
				pairs[j] = tmp;
			}
		}
	}

	cout << std::fixed << std::setprecision(2) << min;
	cout << " " << pairs.size() << endl;
	for (int i = 0; i < pairs.size(); i++)
	{
		cout << pairs[i].x << " " << pairs[i].y << endl;
	}
	delete[] points;
	delete[] cand;
	//getchar();
	return 0;
}