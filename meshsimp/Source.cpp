//#ifndef _MESH_H_
//#define _MESH_H_


#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <set>
#include <assert.h>
using namespace std;

typedef struct HEEdge HEEdge;
typedef struct HEFace HEFace;


struct Vertex {

	float x;
	float y;
	float z;


	bool operator==(Vertex v) const {
		return x == v.x && y == v.y && z == v.z;
	}


	friend ostream& operator<<(ostream& stream, const Vertex& v) {


		return stream << "Vertex \n"
			<< " x : " << v.x << "\n"
			<< " y : " << v.y << "\n"
			<< " z : " << v.z << "\n";


	}


};


struct Face {

	int a, b, c;
	int id;
};


struct Edge {
	Vertex v1;
	Vertex v2;
};


struct HEVertex {

	Vertex vertex;
	HEEdge* edge;


	bool operator<(const HEVertex& hev) const {

		if (vertex.x < hev.vertex.x) return true;
		if (vertex.x > hev.vertex.x) return false;

		if (vertex.y < hev.vertex.y) return true;
		if (vertex.y > hev.vertex.y) return false;



		if (vertex.z < hev.vertex.z) return true;
		if (vertex.z > hev.vertex.z) return false;


		return false;


	}


	bool operator==(const HEVertex& other) const {

		return vertex.x == other.vertex.x && vertex.y == other.vertex.y
			&& vertex.z == other.vertex.z;

	}


	bool operator!=(const HEVertex& other) const {

		return !(vertex.x == other.vertex.x && vertex.y == other.vertex.y
			&& vertex.z == other.vertex.z);

	}


	friend ostream& operator<<(ostream& stream, const HEVertex& v) {


		return stream << "\n" << v.vertex;


	}


};




struct HEEdge {



	HEFace* face;
	HEVertex* hevertex;
	HEEdge* opposite;
	HEEdge* next;
	int id;
	bool exists;


	bool operator==(HEEdge e) const {
		return id == e.id;
	}


	bool operator!=(HEEdge e) const {
		return !(id == e.id);
	}


	bool operator<(const HEEdge& other) const {

		if (hevertex->vertex.x < other.hevertex->vertex.x) return true;
		if (hevertex->vertex.x > other.hevertex->vertex.x) return false;

		if (hevertex->vertex.y < other.hevertex->vertex.y) return true;
		if (hevertex->vertex.y > other.hevertex->vertex.y) return false;



		if (hevertex->vertex.z < other.hevertex->vertex.z) return true;
		if (hevertex->vertex.z > other.hevertex->vertex.z) return false;


		return false;

	}


};


struct HEFace {

	HEEdge* edge;
	int id;


	bool operator<(const HEFace& other) const {

		if (edge->hevertex->vertex.x < other.edge->hevertex->vertex.x) return true;
		if (edge->hevertex->vertex.x > other.edge->hevertex->vertex.x) return false;

		if (edge->hevertex->vertex.y < other.edge->hevertex->vertex.y) return true;
		if (edge->hevertex->vertex.y > other.edge->hevertex->vertex.y) return false;



		if (edge->hevertex->vertex.z < other.edge->hevertex->vertex.z) return true;
		if (edge->hevertex->vertex.z > other.edge->hevertex->vertex.z) return false;


		return false;

	}


};


class Mesh {
private:
	vector<Vertex> V;
	vector<Face> F;

	set<HEVertex*> HEV;
	set<HEEdge*> HEE;
	set<HEFace*> HEF;


public:
	Mesh() {};
	Mesh(const char*);

	void loadMF(const char*);

	void writeMF(const char*);

	void simplifyMesh(const char* input, const char* output, int faceCnt);

	void convertMesh();

	void revertMesh();

	void collapseRandomEdges(const int faceCnt);
	void collapseVertices(HEVertex* v1, HEVertex* v2);
	void debugAdjacentEdges();
	bool canEdgeCollapse(HEEdge* edge);
	void collapseEdge(HEEdge* edgeToRemove);
	int countCommonVertices(HEVertex* v1, HEVertex* v2);


	vector<HEVertex*> getNeighbourVertices(HEVertex* v);
	vector<HEFace*> neighborFaces(HEVertex* v);
	vector<HEVertex*> adjacentVertices(HEFace* f);

	int Vcnt();

	int Fcnt();




};

Mesh::Mesh(const char* filename) {
	loadMF(filename);
}

void Mesh::loadMF(const char* filename) {

	if (V.size() > 0) V.clear();
	if (F.size() > 0) F.clear();
	ifstream infile;
	infile.open(filename, ios::in);
	string strbuff;
	while (getline(infile, strbuff)) {
		stringstream ss;
		ss << strbuff;
		char type;
		ss >> type;
		if (type == 'v') {
			Vertex v;
			ss >> v.x >> v.y >> v.z;
			V.push_back(v);
		}
		else if (type == 'f') {
			Face f;
			ss >> f.a >> f.b >> f.c;
			F.push_back(f);
		}
	}
	infile.close();
}

void Mesh::writeMF(const char* filename) {
	ofstream outfile;
	outfile.open(filename, ios::out);
	string strbuff;
	for (int i = 0; i < V.size(); i++) {
		outfile << "v " << V[i].x << " " << V[i].y << " " << V[i].z << endl;
	}
	for (int i = 0; i < F.size(); i++) {
		outfile << "f " << F[i].a << " " << F[i].b << " " << F[i].c << endl;
	}
	outfile.close();
}

vector<HEVertex*> Mesh::getNeighbourVertices(HEVertex* v) {

	vector<HEVertex*> tmp;



	HEEdge* curr = v->edge;
	HEEdge* first = v->edge;

	if (!first || !first->exists) {

		return tmp;
	}


	int count = 0;

	do {

		tmp.push_back(curr->hevertex);
		curr = curr->opposite->next;

	} while (*curr != *first);

	return tmp;
}

int Mesh::countCommonVertices(HEVertex* v1, HEVertex* v2) {

	int count = 0;
	vector<HEVertex*> v1_neighbours = getNeighbourVertices(v1);
	vector<HEVertex*> v2_neighbours = getNeighbourVertices(v2);

	for (int i = 0; i < v1_neighbours.size(); i++) {

		for (int j = 0; j < v2_neighbours.size(); j++) {

			if (*v1_neighbours[i] == *v2_neighbours[j])
				count++;

		}
	}

	return count;

}

void Mesh::collapseVertices(HEVertex* v1, HEVertex* v2) {

	cout << "Deleted " << endl;


}

bool Mesh::canEdgeCollapse(HEEdge* _heedge) {

	if (_heedge && _heedge->exists && countCommonVertices(_heedge->hevertex, _heedge->opposite->hevertex) == 2) {
		return true;
	}
	else {
		return false;
	}

}

void Mesh::debugAdjacentEdges() {

	for (auto v : HEV) {


		HEEdge* curr = v->edge;
		HEEdge* first = v->edge;

		if (!first->exists)
			continue;

		do {

			curr = curr->opposite->next;

		} while (*curr != *first);

	}

}

void Mesh::collapseEdge(HEEdge* edgeToRemove) {

	HEEdge* edgeToRemoveTwin = edgeToRemove->opposite;
	HEVertex* hevertex1 = edgeToRemove->hevertex;
	HEVertex* hevertex2 = edgeToRemoveTwin->hevertex;


	HEEdge* e_next = edgeToRemove->next;
	HEEdge* e_prev = edgeToRemove->next->next;
	HEEdge* e_next_twin = e_next->opposite;
	HEEdge* e_prev_twin = e_prev->opposite;

	HEEdge* et_next = edgeToRemoveTwin->next;
	HEEdge* et_prev = edgeToRemoveTwin->next->next;
	HEEdge* et_next_twin = et_next->opposite;
	HEEdge* et_prev_twin = et_prev->opposite;


	HEEdge* curr = hevertex1->edge->opposite;
	HEEdge* first = hevertex1->edge->opposite;
	do {
		curr->hevertex = hevertex2;
		curr = curr->next->opposite;
	} while (*curr != *first);

	if (*hevertex2->edge == *edgeToRemove || *hevertex2->edge == *e_next) {
		hevertex2->edge = et_prev->opposite;

	}
	e_next_twin->opposite = e_prev_twin;
	e_prev_twin->opposite = e_next_twin;

	et_next_twin->opposite = et_prev_twin;
	et_prev_twin->opposite = et_next_twin;

	for (auto edge : HEE) {
		assert(*edge->hevertex != *hevertex1);
	}




	HEV.erase(hevertex1);

	HEF.erase(edgeToRemove->face);
	HEF.erase(edgeToRemoveTwin->face);

	HEE.erase(edgeToRemove);
	HEE.erase(e_next);
	HEE.erase(e_prev);
	HEE.erase(edgeToRemoveTwin);
	HEE.erase(et_next);
	HEE.erase(et_prev);

	edgeToRemove->exists = false;
	e_next->exists = false;
	e_prev->exists = false;
	edgeToRemoveTwin->exists = false;
	et_next->exists = false;
	et_prev->exists = false;

}

void Mesh::collapseRandomEdges(const int faceCnt) {

	int removedFaces = 0, removedEdges = 0, removedVertices = 0;
	int timeout = 0;


	while (HEF.size() > faceCnt) {


		int random_index = rand() % HEE.size();


		set<HEEdge*>::iterator it(HEE.begin());
		advance(it, random_index);

		HEEdge* edgeToRemove = *it;

		if (canEdgeCollapse(edgeToRemove)) {


			timeout = 0;
			collapseEdge(edgeToRemove);

		}
		else {

			if (timeout++ > 10000) {

				cout << "Max simplification performed within the face count" << endl;
				
				return;

			}

		}

	}

}


void Mesh::simplifyMesh(const char* input, const char* output, int faceCnt) {

	loadMF(input);
	convertMesh();
	cout << "Original face count: " << HEF.size() << endl;

	collapseRandomEdges(faceCnt);
	cout << "Simplified face count:" << HEF.size() << endl;
	revertMesh();
	writeMF(output);
	
}

void Mesh::convertMesh() {

	map<pair<int, int>, HEEdge*> edges;
	vector<HEVertex*> added_hevertices;
	int edge_index = 0;


	for (int i = 0; i < V.size(); i++) {
		HEVertex* _hevertex = new HEVertex();
		_hevertex->vertex = V[i];


		added_hevertices.push_back(_hevertex);
		HEV.insert(_hevertex);

	}


	for (int i = 0; i < F.size(); i++) {
		HEFace* _heface = new HEFace();

		HEEdge* _halfedge1 = new HEEdge();
		HEEdge* _halfedge2 = new HEEdge();
		HEEdge* _halfedge3 = new HEEdge();

		_halfedge1->exists = true;
		_halfedge2->exists = true;
		_halfedge3->exists = true;

		HEVertex* _hevertex1 = added_hevertices[F[i].a - 1];
		HEVertex* _hevertex2 = added_hevertices[F[i].b - 1];
		HEVertex* _hevertex3 = added_hevertices[F[i].c - 1];
		_heface->edge = _halfedge1;


		_halfedge3->hevertex = _hevertex1;
		_hevertex1->edge = _halfedge1;


		_hevertex2->edge = _halfedge3;
		_halfedge2->hevertex = _hevertex2;


		_hevertex3->edge = _halfedge2;
		_halfedge1->hevertex = _hevertex3;


		_halfedge1->next = _halfedge2;
		_halfedge2->next = _halfedge3;
		_halfedge3->next = _halfedge1;


		_halfedge1->face = _heface;
		_halfedge2->face = _heface;
		_halfedge3->face = _heface;


		edges[make_pair(F[i].b, F[i].a)] = _halfedge3;
		edges[make_pair(F[i].c, F[i].b)] = _halfedge2;
		edges[make_pair(F[i].a, F[i].c)] = _halfedge1;


		if (edges.find(make_pair(F[i].a, F[i].b)) != edges.end()) {

			edges[make_pair(F[i].b, F[i].a)]->opposite = edges[make_pair(F[i].a, F[i].b)];
			edges[make_pair(F[i].a, F[i].b)]->opposite = edges[make_pair(F[i].b, F[i].a)];

		}

		if (edges.find(make_pair(F[i].b, F[i].c)) != edges.end()) {

			edges[make_pair(F[i].c, F[i].b)]->opposite = edges[make_pair(F[i].b, F[i].c)];
			edges[make_pair(F[i].b, F[i].c)]->opposite = edges[make_pair(F[i].c, F[i].b)];

		}

		if (edges.find(make_pair(F[i].c, F[i].a)) != edges.end()) {

			edges[make_pair(F[i].a, F[i].c)]->opposite = edges[make_pair(F[i].c, F[i].a)];
			edges[make_pair(F[i].c, F[i].a)]->opposite = edges[make_pair(F[i].a, F[i].c)];

		}


		HEF.insert(_heface);


		_halfedge1->id = edge_index++;
		_halfedge2->id = edge_index++;
		_halfedge3->id = edge_index++;

		HEE.insert(_halfedge1);
		HEE.insert(_halfedge2);
		HEE.insert(_halfedge3);

		assert(*_halfedge1->next == *_halfedge2);
		assert(*_halfedge2->next == *_halfedge3);
		assert(*_halfedge3->next == *_halfedge1);

		assert(*_halfedge1->next->next == *_halfedge3);
		assert(*_halfedge2->next->next == *_halfedge1);
		assert(*_halfedge3->next->next == *_halfedge2);

	}

	for (auto edge : HEE) {
		assert(*edge->opposite == *(*HEE.find(edge))->opposite);

	}



	assert(V.size() == HEV.size());
	assert(F.size() == HEF.size());
	assert(HEE.size() == HEF.size() * 3);

	V.clear();
	F.clear();

}

void Mesh::revertMesh() {

	map<HEVertex, int> indices;

	int tempindex = 1;

	set<HEVertex*>::iterator it;

	for (it = HEV.begin(); it != HEV.end(); ++it) {

		HEVertex* he_vertex = *it;
		Vertex v;
		v = (*it)->vertex;
		V.push_back(v);
		indices[**it] = tempindex++;

	}

	assert(V.size() == HEV.size());

	for (auto heface : HEF) {

		Face f;

		HEVertex* _hevertex3 = heface->edge->hevertex;
		HEVertex* _hevertex2 = heface->edge->next->hevertex;
		HEVertex* _hevertex1 = heface->edge->next->next->hevertex;

		f.a = indices[*_hevertex1];
		f.b = indices[*_hevertex2];
		f.c = indices[*_hevertex3];

		F.push_back(f);

	}

	assert(HEF.size() == F.size());


}


vector<HEFace*> Mesh::neighborFaces(HEVertex* v) {
	return vector<HEFace*>();
}

vector<HEVertex*> Mesh::adjacentVertices(HEFace* f) {
	return vector<HEVertex*>();
}

int Mesh::Vcnt() {
	return HEV.size();
}

int Mesh::Fcnt() {
	return HEF.size();
}

int main() {

	Mesh mesh;
	mesh.simplifyMesh("bunny1.obj", "bunny2.obj", 6000);

	return 0;
}
