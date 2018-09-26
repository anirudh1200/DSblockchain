#include<stdio.h>
#include<openssl/crypto.h>
#include<openssl/sha.h>
#include<string.h>
#include<malloc.h>
#include<stdlib.h>

struct block{
    unsigned char prevHash[SHA256_DIGEST_LENGTH];
    int blockData;
    struct block *link;
};

struct block *head = NULL;

void addBlock(int);
void verifyChain();
void alterBlock(int, int);
void hackChain();
void printAllBlocks();
unsigned char* toString(struct block);
void hashPrinter(unsigned char hash[], int);
int hashCompare(unsigned char*, unsigned char*);
void printBlock(struct block *b);

int main(){
    int input=0, number, random;
    do{
        printf("1. Add Block\n");
        printf("2. Add n Random Blocks\n");
        printf("3. Alter n Blocks\n");
        printf("4. Print All Blocks\n");
        printf("5. Verify Chain\n");
        printf("6. Hack Chain\n");
        printf("Enter your choice: ");
        scanf("%d",&input);
        switch(input){
            case 1: printf("Enter data: ");
                    scanf("%d", &number);
                    addBlock(number);
                    break;
            case 2: printf("Enter number of blocks to be added: ");
                    scanf("%d",&number);
                    for(int i=0; i<number; i++){
                        random = rand()%(number*10);
                        printf("Entering: %d\n", random);
                        addBlock(random);
                    }
                    break;
            case 3: printf("Enter block number to alter: ");
                    scanf("%d",&number);
                    printf("Enter value: ");
                    scanf("%d", &random);
                    alterBlock(number, random);
                    break;
            case 4: printAllBlocks();
                    break;
            case 5: verifyChain();
                    break;
            case 6: hackChain();
                    break;
            default: printf("Wrong Choice!\n");
        }
    }while(input != 7);
    return 0;
}

void addBlock(int data){
    if(head == NULL){
        head = malloc(sizeof(struct block));
        SHA256("", sizeof(""), head->prevHash);
        head->blockData = data;
        return;
    }
    struct block *currentBlock = head;
    while(currentBlock->link){
        currentBlock = currentBlock->link;
    }
    struct block *newBlock = malloc(sizeof(struct block));
    currentBlock->link = newBlock;
    newBlock->blockData = data;
    SHA256(toString(*currentBlock), sizeof(*currentBlock), newBlock->prevHash);
}

void verifyChain(){
    if(head == NULL){
        printf("Blockchain is empty!! Try again after adding some blocks\n");
        return;
    }
    struct block *curr = head->link, *prev = head;
    int count = 1;
    while(curr){
        printf("%d\t[%d]\t", count++, curr->blockData);
        hashPrinter(SHA256(toString(*prev), sizeof(*prev), NULL), SHA256_DIGEST_LENGTH);
        printf(" - ");
        hashPrinter(curr->prevHash, SHA256_DIGEST_LENGTH);
        if(hashCompare(SHA256(toString(*prev), sizeof(*prev), NULL), curr->prevHash))
            printf("Verified\n");
        else
            printf("Verification Failed!!\n");
        prev = curr;
        curr = curr->link;
    }
}

void alterBlock(int n, int newData){
    int count = 1;
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
    printf("Before: ");
    printBlock(curr);
    curr->blockData = newData;
    printf("\nAfter: ");
    printBlock(curr);
    printf("\n");
}

void hackChain(){
    struct block *curr = head, *prev;
    if(curr == NULL){
        printf("Blockchain is empty!\n");
        return;
    }
    while(1){
        prev = curr;
        curr = curr->link;
        if(curr == NULL){
            return;
        }
        if(!hashCompare(SHA256(toString(*prev), sizeof(*prev), NULL), curr->prevHash)){
            hashPrinter(SHA256(toString(*prev), sizeof(*prev), curr->prevHash), SHA256_DIGEST_LENGTH);
            printf("\n");
        }
    }
}

unsigned char* toString(struct block b){
    unsigned char *str = malloc(sizeof(unsigned char)*sizeof(b));
    memcpy(str, &b, sizeof(b));
    return str;
}

void hashPrinter(unsigned char hash[], int length){
    for(int i=0; i<length; i++){
        printf("%02x",hash[i]);
    }
}

int hashCompare(unsigned char *str1, unsigned char *str2){
    for(int i=0; i<SHA256_DIGEST_LENGTH; i++){
        if(str1[i] != str2[i])
            return 0;
    }
    return 1;
}

void printBlock(struct block *b){
    printf("%p\t", b);
    hashPrinter(b->prevHash, sizeof(b->prevHash));
    printf("\t\t[%d]\t", b->blockData);
    printf("%p\n", b->link);
}

void printAllBlocks(){
    struct block *curr = head;
    int count = 0;
    while(curr){
        printBlock(curr);
        curr = curr->link;
    }
}
