#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_LEN 100
#define MAX_DATE_LEN 30
#define RECENT_VIEW_COUNT 5

typedef struct Job {
    char title[MAX_LEN];
    char company[MAX_LEN];
    char location[MAX_LEN];
    char datetime[MAX_DATE_LEN];
    struct Job* next;
} Job;

typedef struct Applicant {
    char name[MAX_LEN];
    char appliedJobTitle[MAX_LEN];
    struct Applicant* next;
} Applicant;

Job* jobList = NULL;
Applicant* applicantQueueFront = NULL;
Applicant* applicantQueueRear = NULL;

void getCurrentDateTime(char* buffer, int bufsize) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, bufsize, "%Y-%m-%d %H:%M:%S", t);
}

Job* createJob(const char* title, const char* company, const char* location) {
    Job* newJob = (Job*)malloc(sizeof(Job));
    if (!newJob) {
        printf("\nMemory allocation failed.\n");
        exit(1);
    }
    strncpy(newJob->title, title, MAX_LEN - 1);
    newJob->title[MAX_LEN - 1] = '\0';
    strncpy(newJob->company, company, MAX_LEN - 1);
    newJob->company[MAX_LEN - 1] = '\0';
    strncpy(newJob->location, location, MAX_LEN - 1);
    newJob->location[MAX_LEN - 1] = '\0';
    getCurrentDateTime(newJob->datetime, MAX_DATE_LEN);
    newJob->next = NULL;
    return newJob;
}

void addJob(const char* title, const char* company, const char* location) {
    Job* newJob = createJob(title, company, location);
    newJob->next = jobList;
    jobList = newJob;
}

void displayJobs() {
    Job* current = jobList;
    int count = 1;
    if (!current) {
        printf("\nNo jobs available.\n");
        return;
    }
    printf("\nAvailable Jobs:\n");
    while (current) {
        printf("\n%d. Title: %s, Company: %s, Location: %s, Added: %s\n", count, current->title, current->company, current->location, current->datetime);
        count++;
        current = current->next;
    }
}

// Delete job by title and company
void deleteJob(const char* title, const char* company) {
    Job *current = jobList, *prev = NULL;
    while (current) {
        if (strcmp(current->title, title) == 0 && strcmp(current->company, company) == 0) {
            if (prev == NULL) {
                jobList = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            printf("\nJob '%s' at '%s' deleted successfully.\n", title, company);
            return;
        }
        prev = current;
        current = current->next;
    }
    printf("\nJob '%s' at '%s' not found.\n", title, company);
}

void viewRecentJobs() {
    Job* jobs[RECENT_VIEW_COUNT];
    int count = 0;
    Job* current = jobList;
    while (current && count < RECENT_VIEW_COUNT) {
        jobs[count++] = current;
        current = current->next;
    }
    if (count == 0) {
        printf("\nNo jobs available.\n");
        return;
    }
    printf("\nMost Recent Jobs:\n");
    for (int i = 0; i < count; i++) {
        printf("%d. Title: %s, Company: %s, Location: %s, Added: %s\n",
            i+1, jobs[i]->title, jobs[i]->company, jobs[i]->location, jobs[i]->datetime);
    }
}

Applicant* createApplicant(const char* name, const char* jobTitle) {
    Applicant* newApplicant = (Applicant*)malloc(sizeof(Applicant));
    if (!newApplicant) {
        printf("\n\nMemory allocation failed.\n");
        exit(1);
    }
    strncpy(newApplicant->name, name, MAX_LEN - 1);
    newApplicant->name[MAX_LEN - 1] = '\0';
    strncpy(newApplicant->appliedJobTitle, jobTitle, MAX_LEN - 1);
    newApplicant->appliedJobTitle[MAX_LEN - 1] = '\0';
    newApplicant->next = NULL;
    return newApplicant;
}

void applyForJob(const char* applicantName, const char* jobTitle) {
    Applicant* newApplicant = createApplicant(applicantName, jobTitle);
    if (!applicantQueueRear) {
        applicantQueueFront = applicantQueueRear = newApplicant;
    } else {
        applicantQueueRear->next = newApplicant;
        applicantQueueRear = newApplicant;
    }
}

void displayApplicants() {
    Applicant* current = applicantQueueFront;
    if (!current) {
        printf("\n\nNo applicants in the queue.\n");
        return;
    }
    printf("Applicants:\n");
    while (current) {
        printf("Name: %s, Applied for: %s\n", current->name, current->appliedJobTitle);
        current = current->next;
    }
}

void saveJobsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("\nError opening file for writing.\n");
        return;
    }
    Job* current = jobList;
    while (current) {
        fprintf(file, "%s,%s,%s,%s\n", current->title, current->company, current->location, current->datetime);
        current = current->next;
    }
    fclose(file);
    printf("Jobs saved to %s\n", filename);
}

void loadJobsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("\nError opening file for reading.\n");
        return;
    }
    Job* current = jobList;
    while (current) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
    jobList = NULL;

    char line[4 * MAX_LEN];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;
        char* title = strtok(line, ",");
        char* company = strtok(NULL, ",");
        char* location = strtok(NULL, ",");
        char* datetime = strtok(NULL, ",");
        if (title && company && location && datetime) {
            Job* newJob = createJob(title, company, location);
            strncpy(newJob->datetime, datetime, MAX_DATE_LEN - 1);
            newJob->datetime[MAX_DATE_LEN - 1] = '\0';
            newJob->next = jobList;
            jobList = newJob;
        }
    }
    fclose(file);
    printf("\nJobs loaded from %s\n", filename);
}

void searchJobByLocation(const char* location) {
    Job* current = jobList;
    int found = 0;
    printf("\nJobs available in %s:\n", location);
    while (current) {
        if (strcmp(current->location, location) == 0) {
            printf("Title: %s, Company: %s, Added: %s\n", current->title, current->company, current->datetime);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("\nNo jobs found in %s.\n", location);
    }
}

void freeJobs() {
    Job* current = jobList;
    while (current) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
    jobList = NULL;
}

void freeApplicants() {
    Applicant* current = applicantQueueFront;
    while (current) {
        Applicant* temp = current;
        current = current->next;
        free(temp);
    }
    applicantQueueFront = applicantQueueRear = NULL;
}

#define SCREEN_WIDTH 80  // approximate console width

void printCentered(const char* text) {
    int len = (int)strlen(text);
    int padding = (SCREEN_WIDTH - len) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) putchar(' ');
    printf("%s\n", text);
}
void printMenu() {
    printCentered("+---------------------------------+\n");
    printCentered("|          JOB FINDER MENU        |\n");
    printCentered("+---------------------------------+\n");
    printCentered("| 1. Add Job                      |\n");
    printCentered("| 2. Display Jobs                 |\n");
    printCentered("| 3. Search Jobs by Location      |\n");
    printCentered("| 4. Apply for Job                |\n");
    printCentered("| 5. Display Applicants           |\n");
    printCentered("| 6. Save Jobs to File            |\n");
    printCentered("| 7. Load Jobs from File          |\n");
    printCentered("| 8. Apply for Job by Location    |\n");
    printCentered("| 9. Delete Job                   |\n");
    printCentered("| 10. View Recent Jobs            |\n");
    printCentered("| 0. Exit                         |\n");
    printCentered("+---------------------------------+\n");
    printf("Enter your choice: ");
}

void readLine(char* buffer, int size) {
    if (fgets(buffer, size, stdin)) {
        buffer[strcspn(buffer, "\n")] = '\0';
    }
}

int main() {
    int choice;
    char title[MAX_LEN], company[MAX_LEN], location[MAX_LEN], applicantName[MAX_LEN];

    do {
        printMenu();
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input. Try again.\n\n");
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                printf("\nEnter job title: ");
                readLine(title, MAX_LEN);
                printf("Enter company name: ");
                readLine(company, MAX_LEN);
                printf("Enter job location: ");
                readLine(location, MAX_LEN);
                addJob(title, company, location);
                printf("\nJob added successfully.\n");
                break;
            case 2:
                displayJobs();
                break;
            case 3:
                printf("\nEnter location to search: ");
                readLine(location, MAX_LEN);
                searchJobByLocation(location);
                break;
            case 4:
                printf("\nEnter your name to apply: ");
                readLine(applicantName, MAX_LEN);
                printf("Enter job title you are applying for: ");
                readLine(title, MAX_LEN);
                applyForJob(applicantName, title);
                printf("\nApplication submitted successfully.\n");
                break;
            case 5:
                displayApplicants();
                break;
            case 6:
                saveJobsToFile("jobs.txt");
                break;
            case 7:
                loadJobsFromFile("jobs.txt");
                break;
            case 8:
                printf("\nEnter location to search for jobs: ");
                readLine(location, MAX_LEN);

                {
                    Job* current = jobList;
                    int jobFound = 0;

                    printf("Jobs in %s:\n", location);
                    while (current) {
                        if (strcmp(current->location, location) == 0) {
                            printf("Title: %s, Company: %s, Added: %s\n", current->title, current->company, current->datetime);
                            jobFound = 1;
                        }
                        current = current->next;
                    }

                    if (!jobFound) {
                        printf("No jobs found in %s.\n", location);
                        break;
                    }

                    printf("\nEnter the job title you want to apply for: ");
                    readLine(title, MAX_LEN);

                    current = jobList;
                    jobFound = 0;
                    while (current) {
                        if (strcmp(current->location, location) == 0 && strcmp(current->title, title) == 0) {
                            jobFound = 1;
                            break;
                        }
                        current = current->next;
                    }

                    if (!jobFound) {
                        printf("\nInvalid job title for location %s.\n", location);
                        break;
                    }

                    printf("\nEnter your name: ");
                    readLine(applicantName, MAX_LEN);

                    applyForJob(applicantName, title);
                    printf("\nApplication submitted successfully.\n");
                }
                break;
            case 9:
                printf("\nEnter the job title (post) to delete: ");
                readLine(title, MAX_LEN);
                printf("Enter the company name to delete: ");
                readLine(company, MAX_LEN);
                deleteJob(title, company);
                break;
            case 10:
                viewRecentJobs();
                break;
            case 0:
                printf("\n\n\nExiting the program.\n");
                break;
            default:
                printf("\n\nInvalid choice. Please try again.\n");
        }
        printf("\n");

    } while (choice != 0);

    freeJobs();
    freeApplicants();

    return 0;
}
