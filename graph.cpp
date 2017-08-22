#include <cstdio>
#include <vector>
#include <stack>
#include <algorithm>
#include <cmath>
#include <ctime>
#include <cstring>
using namespace std;
#define MAX_BUFFER 30
#define MAXN 1009
#define DEFAULT_ENTRY "Uruguay.csv"
#define DEFAULT_START 203
#define DEFAULT_END 600
#define INF 1e+18
#define NITER 10000

int readInt() {
	char buffer[MAX_BUFFER], car;
	int sz = 0, num;
	while(scanf(" %c ", &car)!=EOF && car >= '0' && car <= '9') {
		buffer[sz++] = car;
	}
	buffer[sz] = 0;
	if (sz == 0) return -1;
	sscanf(buffer, "%d", &num);
	return num;
}

double readDouble() {
	char buffer[MAX_BUFFER], car;
	int sz = 0;
	double num;
	while(scanf(" %c ", &car)!=EOF && car >= '0' && car <= '9') buffer[sz++] = car;
	if (sz == 0) return -1.0;
	if (car == ',') {
		buffer[sz++] = '.';
		while(scanf(" %c ", &car)!=EOF && car >= '0' && car <= '9') buffer[sz++] = car;
		buffer[sz++] = '0';		
	}
	buffer[sz] = 0;
	sscanf(buffer, "%lf", &num);
	return num;
}

vector<int> adjList[MAXN];
double x[MAXN], y[MAXN];
int Nnodes, start, end, nodesProcessed;

inline double dist(int u, int v) {
	return hypot(x[u]-x[v], y[u]-y[v]);
}

double h[MAXN], g[MAXN];
//função de prioridade, equivale a 'menor que'
bool comp(int a, int b){
	return g[a]+h[a] < g[b]+h[b];
}

class Heap{
private:
	int heap[MAXN];
	int inv[MAXN];
	int heapsize;
	void sifup(int n){
		int k = n << 1;
		while (k <= heapsize){
			if (k < heapsize && comp(heap[k+1], heap[k])) k++;
			if (comp(heap[k], heap[n])){
				swap(heap[n], heap[k]);
				inv[heap[n]]=n;
				n = inv[heap[k]]=k;
				k <<= 1;
			}
			else break;
		}
	}
	void sifdown(int n){
		int k = n >> 1;
		while (k){
			if (comp(heap[n], heap[k])){
				swap(heap[n], heap[k]);
				inv[heap[n]]=n;
				n = inv[heap[k]]=k;
				k >>= 1;
			}
			else break;
		}
	}
public:
	Heap(){ heapsize = 0; }
	void clear(){ heapsize = 0; }
	bool empty(){ return heapsize==0; }
	void update(int n){
		if (inv[n]>heapsize) return;
		sifup(inv[n]);
		sifdown(inv[n]);
	}
	void push(int n){
		heap[++heapsize] = n;
		inv[n] = heapsize;
		sifdown(heapsize);
	}
	bool count(int n){
		int k = inv[n];
		return k <= heapsize && k > 0 && heap[k] == n;
	}
	int top(){
		if (heapsize <=0) return -1;
		return heap[1];
	}
	void pop(){
		if (heapsize<=0) return;
		heap[1] = heap[heapsize--];
		inv[heap[1]] = 1;
		sifup(1);
	}
};

bool hcomp(int u, int v) {
	return dist(u, end) > dist(v, end);
}

void readGraph() {
	int u, v;
	Nnodes = 0;
	while(!feof(stdin)) {
		u = readInt();
		if (u == -1) break;
		Nnodes++;
		x[u] = readDouble();
		y[u] = readDouble();
		for(int i=0; i<9; i++) {
			v = readInt();
			if (v > 0) {
				adjList[u].push_back(v);
			}
		}
	}
	for(u=1; u<=Nnodes; u++) {
		sort(adjList[u].begin(), adjList[u].end(), hcomp);
	}
}

double heapSearch() {
	int u, v;
	nodesProcessed = 0;
	double w;
	Heap heap;
	heap.push(start);
	g[start] = 0;
	while(!heap.empty()) {
		u = heap.top();
		//printf("g[%d] = %.3f\n", u, g[u]);
		heap.pop();
		nodesProcessed++;
		if (u == end) break;
		for(int i=0; i<(int)adjList[u].size(); i++) {
			v = adjList[u][i];
			w = dist(u, v);
			if (g[v] > g[u] + w) {
				g[v] = g[u] + w;
				if (heap.count(v)) heap.update(v);
				else heap.push(v);
			}
		}
	}
	return g[end];
}

double dijkstra() {
	for(int u=1; u<=MAXN; u++) {
		g[u] = INF;
		h[u] = 0;
		//printf("h[%d] = %.3f\n", u, h[u]);
	}
	return heapSearch();
}

double Astar() {
	for(int u=1; u<=Nnodes; u++) {
		g[u] = INF;
		h[u] = dist(u, end);
		//printf("h[%d] = %.3f\n", u, h[u]);
	}
	return heapSearch();
}

bool visited[MAXN];
double greedy() {
	int u, v;
	double w;
	for(u=1; u<=Nnodes; u++) {
		visited[u] = false;
	}
	stack<int> sp;
	g[start] = 0;
	nodesProcessed = 0;
	sp.push(start);
	while(!sp.empty()) {
		u = sp.top();
		sp.pop();
		nodesProcessed++;
		if (u == end) break;
		for(int i=0; i<(int)adjList[u].size(); i++) {
			v = adjList[u][i];
			w = dist(u, v);
			if (!visited[v]) {
				visited[v] = true;
				g[v] = g[u] + w;
				sp.push(v);
			}
		}
	}
	return g[end];
}

int main(int argc, char *argv[]) {
	
	//Open entry file
	char entry[MAX_BUFFER];
	if (argc < 2 || argv[1] == NULL) strcpy(entry, DEFAULT_ENTRY);
	else strcpy(entry, argv[1]);
	printf("Opening \"%s\"...", entry);
	freopen(entry, "r", stdin);
	if (stdin == NULL) {
		printf("Arquivo de entrada não encontrado\n");
		return 0;
	}
	printf("done.\n");
	printf("Reading start and end cities... ");
	if (argc < 4 || argv[2] == NULL || argv[3] == NULL) {
		start = DEFAULT_START;
		end = DEFAULT_END;
	}
	else {
		if (sscanf(argv[2], "%d", &start) < 1) start = DEFAULT_START;
		if (sscanf(argv[3], "%d", &end) < 1) end = DEFAULT_END;
	}
	printf("done: %d %d.\n", start, end);
	
	//Read graph
	printf("Reading graph...");
	readGraph();
	clock_t startTime, endTime;
	double result, elapsedTime;
	printf("done.\n");
	
	//Execute Dijkstra
	printf("Running Dijkstra... ");
	startTime = clock();
	for(int i=0; i<NITER; i++) result = dijkstra();
	endTime = clock();
	elapsedTime = (endTime - startTime) / (CLOCKS_PER_SEC*1.0*NITER);
	printf("Result (optimal): %.3f, average time: %.7f, nodes processed: %d.\n", result, elapsedTime, nodesProcessed);
	
	//Execute A*
	printf("Running A*... ");
	startTime = clock();
	for(int i=0; i<NITER; i++) result = Astar();
	endTime = clock();
	elapsedTime = (endTime - startTime) / (CLOCKS_PER_SEC*1.0*NITER);
	printf("Result (sub-optimal): %.3f, average time: %.7f, nodes processed: %d.\n", result, elapsedTime, nodesProcessed);
	
	//Execute Greedy
	printf("Running Greedy... ");
	startTime = clock();
	for(int i=0; i<NITER; i++) result = greedy();
	endTime = clock();
	elapsedTime = (endTime - startTime) / (CLOCKS_PER_SEC*1.0*NITER);
	printf("Result (non-optimal): %.3f, average time: %.7f, nodes processed: %d.\n", result, elapsedTime, nodesProcessed);
	
	//Close entry file
	printf("Closing file... ");
	fclose(stdin);
	printf("done.\n");
	return 0;
}