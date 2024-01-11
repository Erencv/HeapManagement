#include <iostream>
#include <iostream>
#include <pthread.h>
#include <mutex>
#include <unistd.h> 
#include <cstdlib>

using namespace std;

//Eren Cavus 31286

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
                    if (current->SIZE > size) {
                        Node* newNode = new Node(current->SIZE - size, -1, current->INDEX + size);
                        newNode->next = current->next;
                        current->next = newNode;
                    }
                    current->SIZE = size;
                    current->ID = ID;
                    cout << "Allocated for thread "<< ID <<endl;
                    print();
                    pthread_mutex_unlock(&mtx);
                    return current->INDEX;
                }
                current = current->next;
            }
            cout << "Can not allocate, requested size "<<size<<" for thread "<< ID <<" is bigger than remaining size"<<endl;
            print();
            pthread_mutex_unlock(&mtx); 
            return -1;
        };
        
        int myFree(int ID, int index){
            pthread_mutex_lock(&mtx);
            Node* current = head;
            Node* prev = NULL; 
            while (current != NULL) {
                if (current->ID == ID && current->INDEX == index) {

                    if (prev != NULL && prev->ID == -1) { 
                        prev->SIZE += current->SIZE;
                        prev->next = current->next;
                        delete current;
                        current = prev;
                    }
                    
                    if (current->next != NULL && current->next->ID == -1) {
                        current->SIZE += current->next->SIZE;
                        Node* temp = current->next;
                        current->next = current->next->next;
                        delete temp;
                    }
                    
                    current->ID = -1; 
                    cout << "Freed for thread "<< ID <<endl;
                    print();
                    pthread_mutex_unlock(&mtx);
                    return 1;
                }
                prev = current;  
                current = current->next;
            }
            print();
            pthread_mutex_unlock(&mtx);
            return -1;  
        };

        void print(){ 
            //no locking here to avoid deadlocks. since all other functions that are using print are already locked no need to lock here
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

