#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_USERS 100
#define MAX_COMMENTS 100
#define MAX_COMMENT_LENGTH 100
#define MAX_INTERESTS_LENGTH 100
#define MAX_CONTENT_LENGTH 200
#define MAX_ID_LENGTH 10
#define MAX_PASSWORD_LENGTH 20

typedef struct {
    char name[50];
    int age;
    char gender;
    char interests[MAX_INTERESTS_LENGTH];
    char id[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
} User;

typedef struct Node {
    User user;
    char content[MAX_CONTENT_LENGTH];
    int likes;
    char comments[MAX_COMMENTS][MAX_COMMENT_LENGTH];
    int commentCount;
    struct Node* next;
} Node;

typedef struct {
    Node* front;
    Node* rear;
} Queue;

User signup();
int login(User users[], int userCount);
void addContent(Queue* newsFeed, User user);
void addLike(Node* newsFeed);
void addComment(Node* newsFeed);
void displayNewsFeed(Queue* q, User loggedInUser);
Queue* createQueue();
void saveUserToFile(User user);
int loadUsersFromFile(User users[], int* userCount);

int main() {
    Queue* newsFeed = createQueue();
    User users[MAX_USERS];
    int userCount = 0;
    int loggedInUserIndex = -1;

    if (!loadUsersFromFile(users, &userCount)) {
        printf("Error loading users from file.\n");
    }

    int choice;
    do {
        printf("\n1. Register\n");
        printf("2. Login\n");
        printf("3. Post Content\n");
        printf("4. Like Post\n");
        printf("5. Comment on Post\n");
        printf("6. View News\n");
        printf("7. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                if (userCount < MAX_USERS) {
                    users[userCount++] = signup();
                } else {
                    printf("Maximum user limit reached.\n");
                }
                break;
            case 2:
                loggedInUserIndex = login(users, userCount);
                if (loggedInUserIndex != -1) {
                    printf("Welcome %s!\n", users[loggedInUserIndex].name);
                }
                break;
            case 3:
                if (loggedInUserIndex != -1) {
                    addContent(newsFeed, users[loggedInUserIndex]);
                } else {
                    printf("Please login first.\n");
                }
                break;
            case 4:
                if (newsFeed->front != NULL) {
                    addLike(newsFeed->front);
                } else {
                    printf("No posts available to like.\n");
                }
                break;
            case 5:
                if (newsFeed->front != NULL) {
                    addComment(newsFeed->front);
                } else {
                    printf("No posts available to comment on.\n");
                }
                break;
            case 6:
                if (loggedInUserIndex != -1) {
                    displayNewsFeed(newsFeed, users[loggedInUserIndex]);
                } else {
                    printf("Please login first.\n");
                }
                break;
            case 7:
                printf("Exiting.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 7);

    while (newsFeed->front != NULL) {
        Node* temp = newsFeed->front;
        newsFeed->front = newsFeed->front->next;
        free(temp);
    }
    free(newsFeed);

    return 0;
}

User signup() {
    User newUser;
    printf("Enter your name: ");
    fgets(newUser.name, 50, stdin);
    newUser.name[strcspn(newUser.name, "\n")] = '\0';

    printf("Enter your age: ");
    scanf("%d", &newUser.age);
    getchar();

    printf("Enter your gender (M/F): ");
    scanf(" %c", &newUser.gender);
    getchar();

    printf("Enter your interests (comma separated): ");
    fgets(newUser.interests, MAX_INTERESTS_LENGTH, stdin);
    newUser.interests[strcspn(newUser.interests, "\n")] = '\0';

    printf("Create your user ID: ");
    fgets(newUser.id, MAX_ID_LENGTH, stdin);
    newUser.id[strcspn(newUser.id, "\n")] = '\0';

    printf("Create your password: ");
    fgets(newUser.password, MAX_PASSWORD_LENGTH, stdin);
    newUser.password[strcspn(newUser.password, "\n")] = '\0';

    saveUserToFile(newUser);

    return newUser;
}

int login(User users[], int userCount) {
    char id[MAX_ID_LENGTH];
    char password[MAX_PASSWORD_LENGTH];

    printf("Enter your user ID: ");
    fgets(id, MAX_ID_LENGTH, stdin);
    id[strcspn(id, "\n")] = '\0';

    printf("Enter your password: ");
    fgets(password, MAX_PASSWORD_LENGTH, stdin);
    password[strcspn(password, "\n")] = '\0';

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].id, id) == 0 && strcmp(users[i].password, password) == 0) {
            printf("Login successful!\n");
            return i;
        }
    }

    printf("Invalid ID or password. Please try again.\n");
    return -1;
}

void addContent(Queue* newsFeed, User user) {
    char content[MAX_CONTENT_LENGTH];

    printf("Enter the content you want to post: ");
    fgets(content, MAX_CONTENT_LENGTH, stdin);
    content[strcspn(content, "\n")] = '\0';

    Node* newNode = malloc(sizeof(Node));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(newNode->content, content);
    newNode->user = user;
    newNode->likes = 0;
    newNode->commentCount = 0;
    newNode->next = NULL;

    if (newsFeed->front == NULL) {
        newsFeed->front = newsFeed->rear = newNode;
    } else {
        newsFeed->rear->next = newNode;
        newsFeed->rear = newNode;
    }
}

void addLike(Node* newsFeed) {
    char userName[50];
    printf("Enter the name of the user whose post you want to like: ");
    fgets(userName, 50, stdin);
    userName[strcspn(userName, "\n")] = '\0';

    Node* current = newsFeed;
    while (current != NULL) {
        if (strcmp(current->user.name, userName) == 0) {
            current->likes++;
            printf("You liked %s's post!\n", current->user.name);
            return;
        }
        current = current->next;
    }
    printf("User not found.\n");
}

void addComment(Node* newsFeed) {
    char userName[50];
    char comment[MAX_COMMENT_LENGTH];

    printf("Enter the name of the user whose post you want to comment on: ");
    fgets(userName, 50, stdin);
    userName[strcspn(userName, "\n")] = '\0';

    Node* current = newsFeed;
    while (current != NULL) {
        if (strcmp(current->user.name, userName) == 0) {
            if (current->commentCount < MAX_COMMENTS) {
                printf("Enter your comment: ");
                fgets(comment, MAX_COMMENT_LENGTH, stdin);
                comment[strcspn(comment, "\n")] = '\0';
                strcpy(current->comments[current->commentCount++], comment);
                printf("You commented on %s's post!\n", current->user.name);
                return;
            } else {
                printf("Comment limit reached for this post.\n");
                return;
            }
        }
        current = current->next;
    }
    printf("User not found.\n");
}

void displayNewsFeed(Queue* q, User loggedInUser) {
    Node* current = q->front;
    int found = 0;

    while (current != NULL) {
        if (loggedInUser.interests[0] != '\0') {
            char interestsCopy[MAX_INTERESTS_LENGTH];
            strcpy(interestsCopy, loggedInUser.interests);
            char *token = strtok(interestsCopy, ",");
            int matchFound = 0;

            while (token != NULL) {
                if (strstr(current->user.interests, token) != NULL) {
                    matchFound = 1;
                    break;
                }
                token = strtok(NULL, ",");
            }

            if (matchFound) {
                printf("User: %s\n", current->user.name);
                printf("Interests: %s\n", current->user.interests);
                printf("Content: %s\n", current->content);
                printf("Likes: %d\n", current->likes);
                for (int i = 0; i < current->commentCount; i++) {
                    printf("Comment %d: %s\n", i + 1, current->comments[i]);
                }
                printf("-------------\n");
                found = 1;
            }
        } else {
            printf("User: %s\n", current->user.name);
            printf("Interests: %s\n", current->user.interests);
            printf("Content: %s\n", current->content);
            printf("Likes: %d\n", current->likes);
            for (int i = 0; i < current->commentCount; i++) {
                printf("Comment %d: %s\n", i + 1, current->comments[i]);
            }
            printf("-------------\n");
            found = 1;
        }
        current = current->next;
    }

    if (!found) {
        printf("No news feed items match your interests.\n");
    }
}

Queue* createQueue() {
    Queue* q = malloc(sizeof(Queue));
    if (q == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    q->front = q->rear = NULL;
    return q;
}

void saveUserToFile(User user) {
    FILE *file = fopen("users.txt", "a");
    if (file == NULL) {
        printf("Could not open file to save user.\n");
        return;
    }
    fprintf(file, "%s,%d,%c,%s,%s,%s\n", user.name, user.age, user.gender, user.interests, user.id, user.password);
    fclose(file);
}

int loadUsersFromFile(User users[], int* userCount) {
    FILE *file = fopen("users.txt", "r");
    if (file == NULL) {
        printf("Could not open file to load users.\n");
        return 0;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        User user;
        sscanf(line, "%[^,],%d,%c,%[^,],%[^,],%s", user.name, &user.age, &user.gender, user.interests, user.id, user.password);
        users[(*userCount)++] = user;
    }
    fclose(file);
    return 1;
}

