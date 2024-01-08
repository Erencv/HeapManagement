#include <iostream>
#include <iostream>
#include <pthread.h>
#include <mutex>

using namespace std;


struct Node{
    int SIZE;
    int ID;
    int INDEX;
    Node* next;
    Node();
    Node(int size, int id , int index, Node *nnode = NULL): SIZE(size), ID(id), INDEX(index), next(nnode){};
};


class HeapManager{
    private:
        Node* head;
        pthread_mutex_t mtx;


    public:
        HeapManager(): head(NULL){pthread_mutex_init(&mtx, NULL);};

        ~HeapManager(){
            pthread_mutex_destroy(&mtx);  
        };


        int initHeap(int size){
            pthread_mutex_lock(&mtx);
            Node* newNode = new Node(size, -1, 0);
            this->head = newNode;
            print();

            pthread_mutex_unlock(&mtx); 
            return 1;
        };


        
        int myMalloc(int ID, int size){
            pthread_mutex_lock(&mtx); 
            Node* current = head;
            while (current != NULL) {
                if (current->ID == -1 && current->SIZE >= size) {
                    // Found a suitable node
                    if (current->SIZE > size) {
                        // Need to split the node
                        Node* newNode = new Node(current->SIZE - size, -1, current->INDEX + size);
                        newNode->next = current->next;
                        current->next = newNode;
                    }
                    // Allocate the space
                    current->SIZE = size;
                    current->ID = ID;
                    cout << "Allocated for thread "<< ID <<endl;
                    print();
                    pthread_mutex_unlock(&mtx);
                    return current->INDEX;
                }
                current = current->next;
            }
            // No suitable node found
            print();
            pthread_mutex_unlock(&mtx); 
            return -1;
        };
        
        
        
        int myFree(int ID, int index){
            pthread_mutex_lock(&mtx);
            Node* current = head;
            while (current != NULL) {
                if (current->ID == ID && current->INDEX == index) {
                    current->ID = -1; // Mark as free
                
                    cout << "Freed for thread "<< ID <<endl;
                    //pthread_mutex_unlock(&mtx);
                    print();
                    pthread_mutex_unlock(&mtx);
                    return 1;
                }
                current = current->next;
            }
            print();
            pthread_mutex_unlock(&mtx);
            return -1;
            
        };

        void print(){ //no locking here to avoid deadlocks. since all other functions that are using print are already locked no need to lock here
            Node* current = head;
            while (current != NULL) {
                cout << "[" << current->ID << "][" << current->SIZE << "][" << current->INDEX << "]";
                if (current->next != NULL) {
                    cout << "---";
                }
                current = current->next;
            }
            cout << endl;
        };





};

