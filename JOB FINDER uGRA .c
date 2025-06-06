#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <graphics.h>
#include <conio.h>
#include <time.h>

#define MAX_TITLE_LENGTH 100
#define MAX_COMPANY_LENGTH 100
#define MAX_LOCATION_LENGTH 100
#define MAX_APPLICANT_NAME_LENGTH 100
#define MAX_DATETIME_LENGTH 30
#define SCREEN_WIDTH 80
#define RECENT_VIEW_COUNT 5

typedef struct Job {
    char title[MAX_TITLE_LENGTH];
    char company[MAX_COMPANY_LENGTH];
    char location[MAX_LOCATION_LENGTH];
    char datetime[MAX_DATETIME_LENGTH];
    struct Job* next;
} Job;

typedef struct Applicant {
    char name[MAX_APPLICANT_NAME_LENGTH];
    char appliedJobTitle[MAX_TITLE_LENGTH];
    struct Applicant* next;
} Applicant;

Job* jobList = NULL;
Applicant* applicantQueueFront = NULL;
Applicant* applicantQueueRear = NULL;

void getCurrentDateTime(char* buffer, int bufsize) {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    strftime(buffer, bufsize, "%Y-%m-%d %H:%M:%S", t);
}

Job* createJob(const char* title, const char* company, const char* location) {
    Job* newJob = (Job*)malloc(sizeof(Job));
    if (!newJob) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newJob->title, title);
    strcpy(newJob->company, company);
    strcpy(newJob->location, location);
    getCurrentDateTime(newJob->datetime, MAX_DATETIME_LENGTH);
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
    if (current == NULL) {
        printf("No jobs available.\n");
        return;
    }
    printf("Available Jobs:\n");
    while (current != NULL) {
        printf("Title: %s, Company: %s, Location: %s, Added: %s\n", current->title, current->company, current->location, current->datetime);
        current = current->next;
    }
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
        printf("No jobs available.\n");
        return;
    }
    printf("\nMost Recent Jobs:\n");
    for (int i = 0; i < count; i++) {
        printf("%d. Title: %s, Company: %s, Location: %s, Added: %s\n",
            i + 1, jobs[i]->title, jobs[i]->company, jobs[i]->location, jobs[i]->datetime);
    }
}

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

void applyForJob(const char* applicantName, const char* jobTitle) {
    Applicant* newApplicant = (Applicant*)malloc(sizeof(Applicant));
    if (!newApplicant) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newApplicant->name, applicantName);
    strcpy(newApplicant->appliedJobTitle, jobTitle);
    newApplicant->next = NULL;

    if (applicantQueueRear == NULL) {
        applicantQueueFront = applicantQueueRear = newApplicant;
    } else {
        applicantQueueRear->next = newApplicant;
        applicantQueueRear = newApplicant;
    }
}

void displayApplicants() {
    Applicant* current = applicantQueueFront;
    if (current == NULL) {
        printf("No applicants in the queue.\n");
        return;
    }
    printf("Applicants:\n");
    while (current != NULL) {
        printf("Name: %s, Applied for: %s\n", current->name, current->appliedJobTitle);
        current = current->next;
    }
}

void saveJobsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    Job* current = jobList;
    while (current != NULL) {
        fprintf(file, "%s,%s,%s,%s\n", current->title, current->company, current->location, current->datetime);
        current = current->next;
    }
    fclose(file);
    printf("Jobs saved to %s\n", filename);
}

void loadJobsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    Job* current = jobList;
    while (current != NULL) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
    jobList = NULL;

    char line[512];
    while (fgets(line, sizeof(line), file)) {
        char title[MAX_TITLE_LENGTH], company[MAX_COMPANY_LENGTH], location[MAX_LOCATION_LENGTH], datetime[MAX_DATETIME_LENGTH];
        int fields = sscanf(line, "%99[^,],%99[^,],%99[^,],%29[^\n]", title, company, location, datetime);
        if (fields == 4) {
            Job* newJob = (Job*)malloc(sizeof(Job));
            strcpy(newJob->title, title);
            strcpy(newJob->company, company);
            strcpy(newJob->location, location);
            strcpy(newJob->datetime, datetime);
            newJob->next = jobList;
            jobList = newJob;
        }
    }
    fclose(file);
    printf("Jobs loaded from %s\n", filename);
}

void searchJobByLocation(const char* location) {
    Job* current = jobList;
    int found = 0;
    printf("Jobs available in %s:\n", location);
    while (current != NULL) {
        if (strcmp(current->location, location) == 0) {
            printf("Title: %s, Company: %s, Added: %s\n", current->title, current->company, current->datetime);
            found = 1;
        }
        current = current->next;
    }
    if (!found) {
        printf("No jobs found in %s.\n", location);
    }
}

void freeJobs() {
    Job* current = jobList;
    while (current != NULL) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
    jobList = NULL;
}

void freeApplicants() {
    Applicant* current = applicantQueueFront;
    while (current != NULL) {
        Applicant* temp = current;
        current = current->next;
        free(temp);
    }
    applicantQueueFront = applicantQueueRear = NULL;
}

void printfce(const char* text) {
    int len = (int)strlen(text);
    int padding = (SCREEN_WIDTH - len) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) putchar(' ');
    printf("%s\n", text);
}

void showGraphicsAnimation() {
    int gd = DETECT, gm, angle = 0;
    struct arccoordstype a;
    initgraph(&gd, &gm, "C:\\TC\\BGI");
    delay(2000);
    while (angle <= 360) {
        setcolor(BLACK);
        arc(getmaxx() / 2, getmaxy() / 2, angle, angle + 2, 100);
        setcolor(RED);
        getarccoords(&a);
        circle(a.xstart, a.ystart, 25);
        setcolor(BLACK);
        arc(getmaxx() / 2, getmaxy() / 2, angle, angle + 2, 150);
        getarccoords(&a);
        setcolor(GREEN);
        circle(a.xstart, a.ystart, 25);
        angle += 5;
        delay(50);
    }
    delay(500);  // short pause
    cleardevice(); // clear animation
    setcolor(WHITE);
    outtextxy(getmaxx() / 2 - 70, getmaxy() / 2, "Job Finder!");
    delay(1500);
    getch();
    closegraph();
}

int main() {
    showGraphicsAnimation(); // At program start

    int choice;
    char title[MAX_TITLE_LENGTH];
    char company[MAX_COMPANY_LENGTH];
    char location[MAX_LOCATION_LENGTH];
    char applicantName[MAX_APPLICANT_NAME_LENGTH];

    do {
        printfce("\n");
        printfce("+--------------------------------------+\n");
        printfce("|         Job Finder Menu              |\n");
        printfce("+--------------------------------------+\n");
        printfce("| 1. Add Job                           |\n");
        printfce("| 2. Display Jobs                      |\n");
        printfce("| 3. Search Jobs by Location           |\n");
        printfce("| 4. Apply for Job                     |\n");
        printfce("| 5. Display Applicants                |\n");
        printfce("| 6. Save Jobs to File                 |\n");
        printfce("| 7. Load Jobs from File               |\n");
        printfce("| 8. Apply for Job by Location         |\n");
        printfce("| 9. Delete Job                        |\n");
        printfce("|10. View Recent Jobs                  |\n");
        printfce("| 0. Exit                              |\n");
        printfce("+--------------------------------------+\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
            case 1:
                printf("Enter job title: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                printf("Enter company name: ");
                fgets(company, sizeof(company), stdin);
                company[strcspn(company, "\n")] = 0;
                printf("Enter job location: ");
                fgets(location, sizeof(location), stdin);
                location[strcspn(location, "\n")] = 0;
                addJob(title, company, location);
                printf("Job added successfully.\n");
                break;
            case 2:
                displayJobs();
                break;
            case 3:
                printf("Enter location to search: ");
                fgets(location, sizeof(location), stdin);
                location[strcspn(location, "\n")] = 0;
                searchJobByLocation(location);
                break;
            case 4:
                printf("Enter your name to apply: ");
                fgets(applicantName, sizeof(applicantName), stdin);
                applicantName[strcspn(applicantName, "\n")] = 0;
                printf("Enter job title you are applying for: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                applyForJob(applicantName, title);
                printf("Application submitted successfully.\n");
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
            case 8: {
                printf("Enter location to search for jobs: ");
                fgets(location, sizeof(location), stdin);
                location[strcspn(location, "\n")] = 0;

                Job* current = jobList;
                int jobFound = 0;

                printf("Jobs in %s:\n", location);
                while (current != NULL) {
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

                printf("Enter the job title you want to apply for: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;

                current = jobList;
                jobFound = 0;
                while (current != NULL) {
                    if (strcmp(current->location, location) == 0 && strcmp(current->title, title) == 0) {
                        jobFound = 1;
                        break;
                    }
                    current = current->next;
                }

                if (!jobFound) {
                    printf("Invalid job title for location %s.\n", location);
                    break;
                }

                printf("Enter your name: ");
                fgets(applicantName, sizeof(applicantName), stdin);
                applicantName[strcspn(applicantName, "\n")] = 0;

                applyForJob(applicantName, title);
                printf("Application submitted successfully.\n");
                break;
            }
            case 9:
                printf("Enter the job title (post) to delete: ");
                fgets(title, sizeof(title), stdin);
                title[strcspn(title, "\n")] = 0;
                printf("Enter the company name of the job: ");
                fgets(company, sizeof(company), stdin);
                company[strcspn(company, "\n")] = 0;
                deleteJob(title, company);
                break;
            case 10:
                viewRecentJobs();
                break;
            case 0:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);

    freeJobs();
    freeApplicants();

    showGraphicsAnimation(); // At program end

    return 0;
}
