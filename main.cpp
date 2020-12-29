#include <bits/stdc++.h>

using namespace std;

// store center of polygon
pair<int, int> mid;

// determine quadrant of point
int quad(pair<int, int> p)
{
    if (p.first >= 0 && p.second >= 0)
    {
        return 1;
    }
    if (p.first <= 0 && p.second >= 0)
    {
        return 2;
    }
    if (p.first <= 0 && p.second <= 0)
    {
        return 3;
    }
    else
    {
        return 4;
    }
}

// check whether line is crossing polygon
int orientation(pair<int, int> a, pair<int, int> b,
                pair<int, int> c)
{
    int res = (b.second - a.second) * (c.first - b.first) -
              (c.second - b.second) * (b.first - a.first);

    if (res == 0) {return 0;}
    if (res > 0) {return 1;}

    return -1;
}

// compare for sorting
bool compare(pair<int, int> p1, pair<int, int> q1)
{
    pair<int, int> p = make_pair(p1.first - mid.first,
                                 p1.second - mid.second);

    pair<int, int> q = make_pair(q1.first - mid.first,
                                 q1.second - mid.second);

    int one = quad(p);
    int two = quad(q);

    if (one != two) {return (one < two);}

    return (p.second * q.first < q.second * p.first);
}

// find upper tangent of two polygons a and b
// represented as two vectors
vector<pair<int, int>> merger(vector<pair<int, int>> a,
                              vector<pair<int, int>> b)
{
    // number of points in polygon a
    int n1 = a.size();

    // number of points in polygon b
    int n2 = b.size();

    int i_a = 0, i_b = 0;
    for (int i = 1; i < n1; ++i)
    {
        if (a[i].first > a[i_a].first)
        {
            i_a = i;
        }
    }

    for (int i = 1; i < n2; ++i)
    {
        if (b[i].first > b[i_b].first)
        {
            i_b = i;
        }
    }

    // find upper tangent
    int ind_a = i_a, ind_b = i_b;
    bool done = false;
    while (!done)
    {
        done = true;
        while (orientation(b[ind_b], a[ind_a], a[(ind_a + 1) % n1]) >= 0)
        {
            ind_a = (ind_a + 1) % n1;
        }

        while (orientation(a[ind_a], b[ind_b], b[(n2 + ind_b - 1) % n2]) <= 0)
        {
            ind_b = (n2 + ind_b - 1) % n2;
            done = false;
        }
    }

    int upper_a = ind_a, upper_b = ind_b;
    ind_a = i_a, ind_b = i_b;
    done = false;
    int g = 0;
    while (!done) // finding the lower tangent
    {
        done = true;
        while (orientation(a[ind_a], b[ind_b], b[(ind_b + 1) % n2]) >= 0)
        {
            ind_b = (ind_b + 1) % n2;
        }

        while (orientation(b[ind_b], a[ind_a], a[(n1 + ind_a - 1) % n1]) <= 0)
        {
            ind_a = (n1 + ind_a - 1) % n1;
            done = false;
        }
    }

    int lower_a = ind_a, lower_b = ind_b;
    vector<pair<int, int>> ret;

    // ret contains the convex hull after merging
    // points sorted in anti-clockwise order
    int ind = upper_a;
    ret.push_back(a[upper_a]);
    while (ind != lower_a)
    {
        ind = (ind + 1) % n1;
        ret.push_back(a[ind]);
    }

    ind = lower_b;
    ret.push_back(b[lower_b]);
    while (ind != upper_b)
    {
        ind = (ind + 1) % n2;
        ret.push_back(b[ind]);
    }

    return ret;
}

// brute force algorithm to find convex full
// for a set of less than 6 points
vector<pair<int, int>> brute_hull(vector<pair<int, int>> a)
{
    // take any pair of points from the set
    // check whether it is the edge of convex hull or not
    // if all the remaining points are on the same side
    // of line then the line is the edge of convex hull
    set<pair<int, int>> s;

    for (int i = 0; i < a.size(); ++i) {
        for (int j = i + 1; j < a.size(); ++j) {
            int x1 = a[i].first, x2 = a[j].first;
            int y1 = a[i].second, y2 = a[j].second;

            int a1 = y1 - y2;
            int b1 = x2 - x1;
            int c1 = x1 * y2 - y1 * x2;
            int pos = 0, neg = 0;
            for (int k = 0; k < a.size(); ++k) {
                if (a1 * a[k].first + b1 * a[k].second + c1 <= 0) {neg++;}
                if (a1 * a[k].first + b1 * a[k].second + c1 >= 0) {pos++;}
            }
            if (pos == a.size() || neg == a.size())
            {
                s.insert(a[i]);
                s.insert(a[j]);
            }
        }
    }

    vector<pair<int, int>> ret;
    for (auto e:s) {
        ret.push_back(e);
    }

    // sorting points in the anti-clockwise order
    mid = {0, 0};
    int n = ret.size();
    for (int i = 0; i < n; ++i) {
        mid.first += ret[i].first;
        mid.second += ret[i].second;
        ret[i].first *= n;
        ret[i].second *= n;
    }
    sort(ret.begin(), ret.end(), compare);
    for (int i = 0; i < n; ++i) {
        ret[i] = make_pair(ret[i].first/n, ret[i].second/n);
    }

    return ret;
}

// return the convex hull for the given set of points
vector<pair<int, int>> divide(vector<pair<int, int>> a)
{
    // if num of points is less than 6
    // find convex full using brute algorithm
    if (a.size() <= 5){return brute_hull(a);}

    // left contains the left half points
    // right contains the right half points
    vector<pair<int, int>> left, right;
    for (int i = 0; i < a.size()/2; ++i) {
        left.push_back(a[i]);
    }
    for (int i = a.size()/2; i < a.size(); ++i) {
        right.push_back(a[i]);
    }

    // convex hull for the left and right sets
    vector<pair<int, int>> left_hull = divide(left);
    vector<pair<int, int>> right_hull = divide(right);

    // merging the convex hulls
    return merger(left_hull, right_hull);
}

int main() {
    vector<pair<int, int>> points;

    points.push_back(make_pair(0, 0));
    points.push_back(make_pair(1, -4));
    points.push_back(make_pair(-1, -5));
    points.push_back(make_pair(-5, -3));
    points.push_back(make_pair(-3, -1));
    points.push_back(make_pair(-1, -3));
    points.push_back(make_pair(-2, -2));
    points.push_back(make_pair(-1, -1));
    points.push_back(make_pair(-2, -1));
    points.push_back(make_pair(-1, 1));

    // sorting set of points according to x-coordinate
    sort(points.begin(), points.end());
    vector<pair<int, int>> ans = divide(points);

    cout << "convex hull:\n";
    for (auto e:ans)
    {
        cout << e.first << " " << e.second << endl;
    }

    return 0;
}
