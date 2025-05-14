// My variation of another student's work - Module 6

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 5       // Max eggs in the nest
#define NUM_BIRDS 3
#define NUM_HUMANS 3
#define EGGS_PER_BIRD 5
#define EGGS_PER_HUMAN 5

int nest[BUFFER_SIZE];
int in = 0, out = 0;


sem_t empty;     // Tracks available spots in the nest
sem_t full;      // Tracks eggs in the nest
sem_t mutex;     // Access to the nest

void* bird(void* arg) {
    int bird_id = *((int*)arg);

    for (int i = 0; i < EGGS_PER_BIRD; i++) {
        int egg = rand() % 100 + 1;

        sem_wait(&empty);
        sem_wait(&mutex);

        // Lay an egg in the nest at this index
        nest[in] = egg;
        printf("Bird %d produced egg: %d at index %d\n", bird_id, egg, in);
        in = (in + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&full);

        sleep(1);
    }
    pthread_exit(NULL);
}

void* human(void* arg) {
    int human_id = *((int*)arg);

    for (int i = 0; i < EGGS_PER_HUMAN; i++) {
        sem_wait(&full);
        sem_wait(&mutex);

        // consume egg from current index
        int egg = nest[out];
        printf("Human %d consumed egg: %d from nest index %d\n", human_id, egg, out);
        out = (out + 1) % BUFFER_SIZE;

        sem_post(&mutex);
        sem_post(&empty);

        sleep(2);
    }
    pthread_exit(NULL);
}

int main() {
    pthread_t birds[NUM_BIRDS];
    pthread_t humans[NUM_HUMANS];

    srand(time(NULL));

    sem_init(&empty, 0, BUFFER_SIZE);
    sem_init(&full, 0, 0);
    sem_init(&mutex, 0, 1);

    // Create bird threads
    int bird_ids[NUM_BIRDS];
    for (int i = 0; i < NUM_BIRDS; i++) {
        bird_ids[i] = i + 1;
        pthread_create(&birds[i], NULL, bird, &bird_ids[i]);
    }

    // Create human threads
    int human_ids[NUM_HUMANS];
    for (int i = 0; i < NUM_HUMANS; i++) {
        human_ids[i] = i + 1;
        pthread_create(&humans[i], NULL, human, &human_ids[i]);
    }

    // Wait for bird threads to finish
    for (int i = 0; i < NUM_BIRDS; i++) {
        pthread_join(birds[i], NULL);
    }

    // Wait for human threads to finish
    for (int i = 0; i < NUM_HUMANS; i++) {
        pthread_join(humans[i], NULL);
    }
    
    sem_destroy(&empty);
    sem_destroy(&full);
    sem_destroy(&mutex);

    return 0;
}
