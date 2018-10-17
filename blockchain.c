#include<stdio.h>
#include<stdlib.h>
#include<string.h>              //for string handling
#include<time.h>                // for timestamp
#include<openssl/crypto.h>      // for hashing
#include<openssl/sha.h>         // for value of SHA256_DIGEST_LENGTH
#include<unistd.h>              //for sleep function
#include<pthread.h>             //for multithreading

struct block{
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    int timeStamp;
    int blockHeight;
    char blockData[150];
    struct block *link;
};

int height = 0;                     // for height of block
char memPool[150];                  // for storing temporary data
struct block *head = NULL;          // pointer to first block

void takeInput();
void addBlock();
void verifyChain();
void alterBlock();
void printAllBlocks();
void printnBlock();
void printMenu();
unsigned char* toString(struct block);
void hashPrinter(unsigned char hash[], int);
int hashCompare(unsigned char*, unsigned char*);
void printBlock(struct block *b);
void* takingInputs(void *value);
void* blockchainCreation(void *value);

int main(){

    pthread_t thread1, thread2;
    printf("Command Menu:\n");
    printf("add : add a new block\n");
    printf("print : print all blocks\n");
    printf("nprint : print nth block only\n");
    printf("verify : verify all chains\n");
    printf("alter : alter nth block\n");
    printf("menu : view all commands\n");
    printf("\nBlockchain started!!\n");
    memPool[0] = '\0';

    // thread1 for taking inpits continuously
    pthread_create(&thread1, NULL, takingInputs, NULL);

    while(1){

        // thread2 for creating blocks
        pthread_create(&thread2, NULL, blockchainCreation, NULL);
        pthread_join(thread2, NULL);

    }

    pthread_join(thread1, NULL);

    return 0;
}

// for taking command inputs in 1st thread
void* takingInputs(void *value){
    char input[10];
    while(1){
        scanf(" %s", input);
        if(strncmp(input, "add", 3)==0)
            takeInput();
        else if(strncmp(input, "print", 5)==0)
            printAllBlocks();
        else if(strncmp(input, "nprint", 6)==0)
            printnBlock();
        else if(strncmp(input, "verify", 6)==0)
            verifyChain();
        else if(strncmp(input, "alter", 5)==0)
            alterBlock();
        else if(strncmp(input, "menu", 4)==0)
            printMenu();
        else
            printf("Enter a valid option!!!\n");
    }
}

// creating blockchains continuously in 2nd thread
void* blockchainCreation(void *value){
    sleep(20.0);
    addBlock();
    printf("\nBlockchain created of height %d\n\n", height);
    height++;
    pthread_exit(0);
}

// accepting name and balance and storing in mempool
void takeInput(){
    char name[30], balance[8], finalString[60], n[]={'\n','\0'};
    printf("Enter name: ");
    scanf(" %s", name);
    printf("Enter balance: ");
    scanf(" %s", balance);
    printf("\n");
    char info1[] = {'N','a','m','e',':',' ','\0'};
    char info2[] = {'\n','B','a','l','a','n','c','e',':',' ','\0'};
    strcat(info1, name);
    strcat(info2, balance);
    strcat(info2, n);
    strcat(info1, info2);
    strcat(memPool, info1);
}

// adding data to new block
void addBlock(){
    if(head == NULL){
        head = malloc(sizeof(struct block));
        SHA256("", sizeof(""), head->prevHash);
        strcat(head->blockData, memPool);
        head->timeStamp = (int)time(NULL);
        head->blockHeight = height;
        memPool[0] = '\0';
        return;
    }
    struct block *currentBlock = head;
    while(currentBlock->link){
        currentBlock = currentBlock->link;
    }
    struct block *newBlock = malloc(sizeof(struct block));
    currentBlock->link = newBlock;
    strcat(newBlock->blockData, memPool);
    newBlock->timeStamp = (int)time(NULL);
    newBlock->blockHeight = height;
    memPool[0] = '\0';
    SHA256(toString(*currentBlock), sizeof(*currentBlock), newBlock->prevHash);
}

// to verify all blocks of blockchain for any alterations
void verifyChain(){
    if(head == NULL){
        printf("Blockchain is empty!! Try again after adding some blocks\n");
        return;
    }
    struct block *curr = head->link, *prev = head;
    int count = 1;
    while(curr){
        // printf("%d\t[%s]\t", count++, curr->blockData);
        hashPrinter(SHA256(toString(*prev), sizeof(*prev), NULL), SHA256_DIGEST_LENGTH);
        printf(" - ");
        hashPrinter(curr->prevHash, SHA256_DIGEST_LENGTH);
        if(hashCompare(SHA256(toString(*prev), sizeof(*prev), NULL), curr->prevHash))
            printf("\tVerified\n");
        else
            printf("\tVerification Failed!!\n");
        prev = curr;
        curr = curr->link;
    }
}

// for demonstration of alterating the block
void alterBlock(){
    int n;
    char ws;
    printf("Enter block number to alter: ");
    scanf("%d", &n);
    scanf("%c",&ws);
    int count = 0;
    struct block *curr = head;
    if(curr == NULL){
        printf("Nth block does not exist!!\n");
        return;
    }
    while(count != n){
        if(curr->link == NULL && count != n){
            printf("Nth block does not exist!!\n");
            return;
        } else if(count == n){
            break;
        }
        curr = curr->link;
        count++;
    }
    char newData[100];
    printf("Enter new data: ");
    fgets(newData, 100, stdin);
    printf("Before:\n");
    printBlock(curr);
    curr->blockData[0] = '\0';
    strcat(curr->blockData, newData);
    curr->timeStamp = (int)time(NULL);
    printf("\nAfter:\n");
    printBlock(curr);
    printf("\n");
}

unsigned char* toString(struct block b){
    unsigned char *str = malloc(sizeof(unsigned char)*sizeof(b));
    memcpy(str, &b, sizeof(b));
    return str;
}

// printing hash stored
void hashPrinter(unsigned char hash[], int length){
    for(int i=0; i<length; i++){
        printf("%02x",hash[i]);
    }
}

// comaring the hash of data of previous block and hash stored in current block
int hashCompare(unsigned char *str1, unsigned char *str2){
    for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
        if(str1[i] != str2[i])
            return 0;
    }
    return 1;
}

// printing contents of one the blocks
void printBlock(struct block *b){
    printf("%p\t", b);
    printf("\n[\n PrevHash: ");
    hashPrinter(b->prevHash, sizeof(b->prevHash));
    printf("\nTimestamp: %d", b->timeStamp);
    printf("\nHeight: %d", b->blockHeight);
    printf("\n%s]\t", b->blockData);
    printf("%p\n\n", b->link);
}

void printAllBlocks(){
    struct block *curr = head;
    int count = 0;
    while(curr){
        printBlock(curr);
        curr = curr->link;
    }
}

// print nth block
void printnBlock(){
    int n, count = 0;;
    char ws;
    struct block *curr = head;
    printf("Enter block height: ");
    scanf("%d",&n);
    scanf("%c",&ws);
    if(curr == NULL){
        printf("1Nth block does not exist!!\n");
        return;
    }
    while(count != n){
        if(curr->link == NULL && count != n){
            printf("2Nth block does not exist!!\n");
            return;
        } else if(count == n){
            break;
        }
        curr = curr->link;
        count++;
    }
    printBlock(curr);
}

// for printing menu
void printMenu(){
    printf("\nCommand Menu:\n");
    printf("add : add a new block\n");
    printf("print : print all blocks\n");
    printf("nprint : print nth block only\n");
    printf("verify : verify all chains\n");
    printf("alter : alter nth block\n");
    printf("menu : view all commands\n");
}
