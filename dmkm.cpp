////////////////////////////////////// MIT License ////////////////////////////////////////////
//
//   Projekt      : [Problem maksymalnego przepływu – implementacja algorytmu DMKM w języku C++]
//   Plik         : [dmkm.cpp]
//   Streszczenie : [Implementacja algorytmu DMKM w języku C++]
//
//   Utworzony: 21/06/2021
//   Autor: [Mykhailo Morrison] <mailto:[mykhailo.morrison@gmail.com]>
////////////////////////////////////////////////////////////////////////////////////////////////


#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <algorithm>

using namespace std;

const int SOURCE_N = 0; //numer wierzchołka źródła (s)
const int SINK_N = 10000; //numer wierzchołka odpływu (t)

//Klasa łuku
class Edge {

private:
	int u; //Pierwszy wierzchołek łuku
	int v;  //Drugi wierzchołek łuku
	int capacity;  //Przepustowość łuku
	int flow;  //Wartość przepływu tego łuku
public:

	//Konstruktory, gettery, settery

	Edge(int u, int v, int capacity) : u(u), v(v), capacity(capacity) {}

	int getU() const {
		return u;
	}

	void setU(int newU) {
		u = newU;
	}

	int getV() const {
		return v;
	}

	void setV(int newV) {
		v = newV;
	}

	int getCapacity() const {
		return capacity;
	}


	int getFlow() const {
		return flow;
	}

	void setFlow(int newFlow) {
		flow = newFlow;
	}
};
//Klasa wierzchołka
class Vertex {
private:
	int inpot; //Wejściowy potencjał
	int outpot; //Wyjściowy potencjał
	int poten; //Potencjał (min{inpot, outpot})
	int inFlow; //Wartość wejściowego przepływu
	int outFlow; //Wartość wyjściowego przepływu
	int layer; //Numer warstwy tego wierzchołka
	bool used = false; //Skreślenie wierzchołka, przydatne w funkcji SATURATE
public:

	//Gettery, settery
	int getInpot() const {
		return inpot;
	}

	void setInpot(int newInpot) {
		inpot = newInpot;
	}

	int getOutpot() const {
		return outpot;
	}

	void setOutpot(int newOutpot) {
		outpot = newOutpot;
	}

	int getPoten() const {
		return poten;
	}

	void setPoten(int newPoten) {
		poten = newPoten;
	}

	int getLayer() const {
		return layer;
	}

	void setLayer(int setNewLayer) {
		layer = setNewLayer;
	}

	int getInflow() const {
		return inFlow;
	}

	void setInflow(int newInFlow) {
		inFlow = newInFlow;
	}

	int getOutFlow() const {
		return outFlow;
	}

	void setOutflow(int newOutFlow) {
		outFlow = newOutFlow;
	}
	bool isUsed() const {
		return used;
	}

	void setUsed(bool newIsUsed) {
		used = newIsUsed;
	}
};

//Klasa sieci
class Graph {
public:
	unordered_map<int, Vertex*> vertices; //Lista wierzchołków w postaci par, w których pierwszy element - numer wierzchołka, drugi - jego obiekt
	unordered_map<int, Edge*> edges; //Lista łuków w postaci par, w których pierwszy element - numer łuku, drugi - jego obiekt

	Vertex* vertex; //Zmienna pomocnicza
	Edge* edge; //Zmienna pomocnicza

	int inpot = 0; //Zmienna pomocnicza
	int outpot = 0; //Zmienna pomocnicza

	bool isNotSaturated; //Flaga wskazująca na to, że sieć jest nienasycona
	//Dodajemy krawędzie do grafu
	void addEdges(int e[][3], int countOfEdges) {

		for (int i = 0; i < countOfEdges; i++) {
			edge = new Edge(e[i][0], e[i][1], e[i][2]);
			edge->setFlow(0);
			edges.insert(make_pair(i, edge));
		}
		for (auto& it : edges) {

			if (vertices.count(it.second->getU()) == 0) {
				vertex = new Vertex();
				vertices.insert(make_pair(it.second->getU(), vertex));

			}
			if (vertices.count(it.second->getV()) == 0) {
				vertex = new Vertex();
				vertices.insert(make_pair(it.second->getV(), vertex));
			}
		}

		for (auto& v : vertices) {
			for (auto& e : edges) {
				if (e.second->getV() == v.first) {
					inpot += e.second->getCapacity();
				}
				if (e.second->getU() == v.first) {
					outpot += e.second->getCapacity();
				}
			}
			vertices[v.first]->setInpot(inpot);
			vertices[v.first]->setOutpot(outpot);
			if (v.first != SOURCE_N && v.first != SINK_N) {
				vertices[v.first]->setPoten(min(vertices[v.first]->getInpot(), vertices[v.first]->getOutpot()));
			}
			else if (v.first == SOURCE_N) {
				vertices[SOURCE_N]->setPoten(vertices[SOURCE_N]->getOutpot());
			}
			else {
				vertices[SINK_N]->setPoten(vertices[SINK_N]->getInpot());
			}
			inpot = 0;
			outpot = 0;
		}
	}
};

//Obliczanie potencjałów wejściowych wierzchołka v
void recalculateInpot(Graph* lGraph, int v) {
	int inpot = 0;

	for (auto& it : lGraph->edges) {
		if (!lGraph->vertices[it.second->getU()]->isUsed() && !lGraph->vertices[it.second->getV()]->isUsed() && it.second->getV() == v) {
			inpot += it.second->getCapacity() - it.second->getFlow();
		}
	}
	lGraph->vertices[v]->setInpot(inpot);
}
//Obliczanie potencjałów wyjściowych wierzchołka v
void recalculateOutpot(Graph* lGraph, int v) {
	int outpot = 0;

	for (auto& it : lGraph->edges) {
		if (!lGraph->vertices[it.second->getU()]->isUsed() && !lGraph->vertices[it.second->getV()]->isUsed() && it.second->getU() == v) {
			outpot += it.second->getCapacity() - it.second->getFlow();
		}
	}
	lGraph->vertices[v]->setOutpot(outpot);
}
//Obliczanie potencjałów wierzchołka v
void recalculatePoten(Graph* lGraph, int v, int s, int t) {
	if (v != s && v != t) {
		lGraph->vertices[v]->setPoten(min(lGraph->vertices[v]->getInpot(), lGraph->vertices[v]->getOutpot()));
	}
	else if (v == s) {
		lGraph->vertices[s]->setPoten(lGraph->vertices[s]->getOutpot());
	}
	else {
		lGraph->vertices[t]->setPoten(lGraph->vertices[t]->getInpot());
	}
}
//Wybiera wierzchołek odniesienia i zwraca numer warstwy.
void refnode(Graph* lGraph, int layersCount, int* r, int* rlayer, int s, int t) {
	lGraph->vertices[s]->setPoten(lGraph->vertices[s]->getOutpot());
	lGraph->vertices[t]->setPoten(lGraph->vertices[t]->getInpot());

	if (lGraph->vertices[s]->getPoten() < lGraph->vertices[t]->getPoten()) {
		(*r) = s;
		(*rlayer) = 1;
	}
	else {
		(*r) = t;
		(*rlayer) = layersCount;
	}
	for (auto& it : lGraph->vertices) {
		if (!it.second->isUsed() && it.first != s && it.first != t) {
			it.second->setPoten(min(it.second->getInpot(), it.second->getOutpot()));
			if (it.second->getPoten() < lGraph->vertices[(*r)]->getPoten()) {
				(*r) = it.first;
				(*rlayer) = lGraph->vertices[it.first]->getLayer();
			}
		}
	}

}
//Metoda Push-Pull przesyłająca towar między warstwami.
void push(Graph* lGraph, int i) {
	int avacap = 0;
	for (auto& it : lGraph->vertices) {
		if (!it.second->isUsed() && it.second->getLayer() == i && it.second->getOutFlow() != 0) {
			for (auto& ed : lGraph->edges) {
				if (!lGraph->vertices[ed.second->getV()]->isUsed() && ed.second->getU() == it.first) {
					avacap = ed.second->getCapacity() - ed.second->getFlow();
					if (avacap > 0) {
						if (avacap > it.second->getOutFlow()) {
							avacap = it.second->getOutFlow();
						}
						ed.second->setFlow(ed.second->getFlow() + avacap);
						lGraph->vertices[ed.second->getU()]->setOutflow(lGraph->vertices[ed.second->getU()]->getOutFlow() - avacap);
						lGraph->vertices[ed.second->getV()]->setOutflow(lGraph->vertices[ed.second->getV()]->getOutFlow() + avacap);
						lGraph->vertices[ed.second->getU()]->setOutpot(lGraph->vertices[ed.second->getU()]->getOutpot() - avacap);
						lGraph->vertices[ed.second->getV()]->setInpot(lGraph->vertices[ed.second->getV()]->getInpot() - avacap);
					}
				}
			}
		}
	}
}

void pull(Graph* lGraph, int j) {
	int avacap = 0;
	for (auto& it : lGraph->vertices) {
		if (!it.second->isUsed() && it.second->getLayer() == j && it.second->getInflow() != 0) {
			for (auto& ed : lGraph->edges) {
				if (!lGraph->vertices[ed.second->getU()]->isUsed() && ed.second->getV() == it.first) {
					avacap = ed.second->getCapacity() - ed.second->getFlow();
					if (avacap > 0) {
						if (avacap > it.second->getInflow()) {
							avacap = it.second->getInflow();
						}
						ed.second->setFlow(ed.second->getFlow() + avacap);
						lGraph->vertices[ed.second->getV()]->setInflow(lGraph->vertices[ed.second->getV()]->getInflow() - avacap);
						lGraph->vertices[ed.second->getU()]->setInflow(lGraph->vertices[ed.second->getU()]->getInflow() + avacap);
						lGraph->vertices[ed.second->getU()]->setOutpot(lGraph->vertices[ed.second->getU()]->getOutpot() - avacap);
						lGraph->vertices[ed.second->getV()]->setInpot(lGraph->vertices[ed.second->getV()]->getInpot() - avacap);
					}
				}
			}
		}
	}
}
//Lista następników wierzchołka v
vector<int> getSucc(Graph* graph, int v) {
	vector<int> s;
	for (auto& e : graph->edges) {
		if (!graph->vertices[e.second->getV()]->isUsed() && e.second->getU() == v) {
			s.push_back(e.second->getV());
		}
	}
	return s;
}
//Lista poprzedników wierzchołka v
vector<int> getPred(Graph* graph, int v) {
	vector<int> s;
	for (auto& e : graph->edges) {
		if (!graph->vertices[e.second->getU()]->isUsed() && e.second->getV() == v) {
			s.push_back(e.second->getU());
		}
	}
	return s;
}
//Wydzielenie sieci warstwowej
void layer(Graph* graph, Graph* lGraph, int& layersCount, bool& stpath) {
	stpath = true;
	unordered_map<int, int> labels;
	unordered_map<int, vector<int>> succ; //zbiory następników
	unordered_map<int, vector<int>> pred; //zbiory poprzedników
	unordered_map<int, vector<int>> layersWithVertices; //zbiór warstw z wierzchołkami

	for (auto& v : graph->vertices) {
		labels[v.first] = -1;
		v.second->setUsed(false);
	}
	layersWithVertices[1].push_back(SOURCE_N);
	labels[SOURCE_N] = 1;
	int i = 1; //numer bieżącej warstwy

	//Dodajemy użyteczne łuki
	while (!layersWithVertices[i].empty() && labels[SINK_N] == -1) {
		for (auto& v : layersWithVertices[i]) {
			for (auto& e : graph->edges) {
				if (e.second->getU() == v && e.second->getFlow() < e.second->getCapacity() && (labels[e.second->getV()] == -1 || labels[e.second->getV()] == i + 1)) {
					layersWithVertices[i + 1].push_back(e.second->getV());
					labels[e.second->getV()] = i + 1;
					succ[e.second->getU()].push_back(e.second->getV());
					pred[e.second->getV()].push_back(e.second->getU());
					lGraph->edges.insert(e);
				}
				if (e.second->getV() == v && e.second->getFlow() > 0 && (labels[e.second->getU()] == -1 || labels[e.second->getU()] == i + 1)) {
					layersWithVertices[i + 1].push_back(e.second->getU());
					labels[e.second->getU()] = i + 1;
					succ[e.second->getV()].push_back(e.second->getU());
					pred[e.second->getU()].push_back(e.second->getV());
					for (auto& ed : graph->edges) {
						if (ed.second->getU() == v && ed.second->getV() == e.second->getV()) {
							lGraph->edges.insert(ed);
						}
					}

				}
			}
		}
		i++;
	}
	layersCount = i;

	if (labels[SINK_N] == -1) {
		stpath = false; //false - droga nie istnieje
	}
	else {

		int j = i;
		vector<int> edgesToRemove;
		while (j != 1) {
			for (auto& w : layersWithVertices[j]) {
				if (succ[w].empty() && w != SINK_N) {
					for (auto& x : pred[w]) {
						for (auto& it : lGraph->edges) {
							if (it.second->getU() == x && it.second->getV() == w) {
								edgesToRemove.push_back(it.first);
							}
						}
						for (auto& rem : edgesToRemove) {
							lGraph->edges.erase(rem);
						}
						succ[x].erase(remove(succ[x].begin(), succ[x].end(), w), succ[x].end());
					}
					layersWithVertices[j].erase(remove(layersWithVertices[j].begin(), layersWithVertices[j].end(), w), layersWithVertices[j].end());
					pred[w].clear();
					pred[w].shrink_to_fit(); //pred(w) teraz jest pusty
				}

			}
			j--;
		}
		//Dodajemy wierzchołki do sieci warstwowej
		Vertex* vert;

		for (auto& l : layersWithVertices) {
			for (auto& v : l.second) {
				vert = graph->vertices[v];
				vert->setLayer(l.first);
				lGraph->vertices.insert(make_pair(v, vert));
			}
		}
	}
}
//Nasycenie sieci warstwowej
void saturate(Graph* lGraph, int layersCount) {
	int r;
	int rlayer;
	vector<int> succ;
	vector<int> pred;
	//Olbliczamy potencjały i resetujemy flagę used
	for (auto& it : lGraph->vertices) {
		recalculateInpot(lGraph, it.first);
		recalculateOutpot(lGraph, it.first);
		it.second->setUsed(false);
	}
	//Resetujemy przepływy
	for (auto& it : lGraph->edges) {
		it.second->setFlow(0);
	}
	lGraph->isNotSaturated = true;

	while (lGraph->isNotSaturated) { //Dopóki sieć jest nienasycona

		refnode(lGraph, layersCount, &r, &rlayer, SOURCE_N, SINK_N); //Wybieramy wierzchołek odniesienia r i numer jego warstwy.

		if (lGraph->vertices[r]->getPoten() != 0) {
			lGraph->vertices[r]->setInflow(lGraph->vertices[r]->getPoten());
			lGraph->vertices[r]->setOutflow(lGraph->vertices[r]->getPoten());
			for (auto& it : lGraph->vertices) {
				if (!it.second->isUsed() && it.first != r) {
					lGraph->vertices[it.first]->setInflow(0);
					lGraph->vertices[it.first]->setOutflow(0);
				}
			}
			//Metoda Push-Pull
			for (int k = rlayer; k <= layersCount - 1; k++) {
				push(lGraph, k);
			}
			for (int j = rlayer; j >= 2; j--) {
				pull(lGraph, j);
			}
		}

		if (lGraph->vertices[r]->getPoten() != 0 || r != SOURCE_N && r != SINK_N) {
			//Następniki i poprzedniki wierzchołka r
			succ = getSucc(lGraph, r);
			pred = getPred(lGraph, r);
			//Skreślamy wierzchołek r
			lGraph->vertices[r]->setUsed(true);
			//Obliczamy potencjały poprzedników i następników
			for (auto& v : succ) {
				recalculateInpot(lGraph, v);
			}
			for (auto& v : pred) {
				recalculateOutpot(lGraph, v);
			}
		}
		else {
			lGraph->isNotSaturated = false;
		}
	}
}
//Funkcja obliczająca maksymalny przepływ
int getMaxFlow(int edges[][3], int edgeCount) {

	Graph* graph = new Graph(); //Tworzymy obiekt sieci
	Graph* lGraph; //Wskażnik na obiekt sieci warstwowej
	Graph* tmpGraph; //Wskażnik pomocniczy na obiekt sieci warstwowej
	int layersCount; //Liczba warstw
	bool stpath; //false - droga nie istnieje
	int maxFlow = 0; //Na początku maksymalny przepływ jest równy zero
	graph->addEdges(edges, edgeCount); //Dodajemy łuki do sieci

	for (auto& it : graph->edges) {
		it.second->setFlow(0); //Zerujemy przepływy na początku
	}
	lGraph = new Graph(); //Tworzymy obiekt sieci warstwowej
	layer(graph, lGraph, layersCount, stpath); //Wydzielamy sieć warstwową
	//Dopóki istnieje droga od s do t
	while (stpath) {
		saturate(lGraph, layersCount); //Nasycamy sieć warstwową

		for (auto& it : graph->edges) {
			if (it.second->getU() == SOURCE_N) {
				maxFlow += it.second->getFlow(); //Kumulacja przepływu
			}
		}
		tmpGraph = lGraph; //Zapisujemy adres sieci warstwowej do zmiennej pomocniczej
		lGraph = new Graph(); //Tworzymy nowy obiekt sieci warstwowej
		layer(tmpGraph, lGraph, layersCount, stpath); //Ponownie wydzielamy sieć warstwową
	}

	return maxFlow;
}

int main() {
	//Przykłady sieci
	//Maksymalny przepływ 4
	int edges1[7][3] = {{SOURCE_N, 1, 2},
						 {SOURCE_N, 2, 3},
						 {1, 3, 5},
						 {1, 4, 6},
						 {2, 4, 3},
						 {3, SINK_N, 4},
						 {4, SINK_N, 2}
	};
	cout <<"Maximum flow of the first network is: " << getMaxFlow(edges1, 7) << endl;
	//Maksymalny przepływ 20
	int edges2[12][3] = {{SOURCE_N, 2, 50},
						  {1, SOURCE_N, 20},
						  {2, 1, 10},
						  {2, 4, 30},
						  {1, 3, 50},
						  {1, 4, 40},
						  {4, 1, 20},
						  {4, 3, 10},
						  {3, 5, 80},
						  {3, SINK_N, 40},
						  {5, SINK_N, 40},
						  {SINK_N, 4 , 20},
	};
	cout <<"Maximum flow of the second network is: " << getMaxFlow(edges2, 12) << endl;
	//Maksymalny przepływ 30
	int edges3[18][3] = { {SOURCE_N, 1, 7},
						{SOURCE_N, 2, 20},
						{SOURCE_N, 3, 16},  //1-c, 2-b, 3-a
						{3, 4, 3},//3-a, 4-d
						{3, 5, 15},//3-a, 5-e
						{2, 4, 2}, //2-b, 4-d
						{2, 5, 12}, //2-b, 5-e
						{1, 4, 7}, //1-c, 4-d
						{4, 6, 12}, //4-d, 6-f
						{4, 7, 19}, //4-d, 7-g
						{4, 8, 3}, //4-d, 8-h
						{5, 6, 13}, //5-e, 6-f
						{5, 8, 1}, //5-e, 8-h
						{5, 9, 10}, //5-e, 9-k
						{6, SINK_N, 9}, //6-f, 10-t
						{7, SINK_N, 18}, //7-g, 10-t
						{8, SINK_N, 8}, //8-h, 10-t
						{9, SINK_N, 11}, //9-k, 10-t
	};
	cout <<"Maximum flow of the third network is: " << getMaxFlow(edges3, 18) << endl;
	return 0;
}
