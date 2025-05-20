#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <graphics.h>
#include <conio.h>
#include <dos.h>

#define MAX_TITLE_LENGTH 100
#define MAX_COMPANY_LENGTH 100
#define MAX_LOCATION_LENGTH 100
#define MAX_APPLICANT_NAME_LENGTH 100

struct Job {
    char title[MAX_TITLE_LENGTH];
    char company[MAX_COMPANY_LENGTH];
    char location[MAX_LOCATION_LENGTH];
    Job* next;
};

struct Applicant {
    char name[MAX_APPLICANT_NAME_LENGTH];
    char appliedJobTitle[MAX_TITLE_LENGTH];
    Applicant* next;
};

Job* jobList = nullptr;
Applicant* applicantQueueFront = nullptr;
Applicant* applicantQueueRear = nullptr;

Job* createJob(const char* title, const char* company, const char* location) {
    Job* newJob = (Job*)malloc(sizeof(Job));
    if (!newJob) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newJob->title, title);
    strcpy(newJob->company, company);
    strcpy(newJob->location, location);
    newJob->next = nullptr;
    return newJob;
}

void addJob(const char* title, const char* company, const char* location) {
    Job* newJob = createJob(title, company, location);
    newJob->next = jobList;
    jobList = newJob;
}

void displayJobs() {
    Job* current = jobList;
    if (current == nullptr) {
        printf("No jobs available.\n");
        return;
    }
    printf("Available Jobs:\n");
    while (current != nullptr) {
        printf("Title: %s, Company: %s, Location: %s\n", current->title, current->company, current->location);
        current = current->next;
    }
}

void applyForJob(const char* applicantName, const char* jobTitle) {
    Applicant* newApplicant = (Applicant*)malloc(sizeof(Applicant));
    if (!newApplicant) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    strcpy(newApplicant->name, applicantName);
    strcpy(newApplicant->appliedJobTitle, jobTitle);
    newApplicant->next = nullptr;

    if (applicantQueueRear == nullptr) {
        applicantQueueFront = applicantQueueRear = newApplicant;
    } else {
        applicantQueueRear->next = newApplicant;
        applicantQueueRear = newApplicant;
    }
}

void displayApplicants() {
    Applicant* current = applicantQueueFront;
    if (current == nullptr) {
        printf("No applicants in the queue.\n");
        return;
    }
    printf("Applicants:\n");
    while (current != nullptr) {
        printf("Name: %s, Applied for: %s\n", current->name, current->appliedJobTitle);
        current = current->next;
    }
}

void saveJobsToFile(const char* filename) {
    FILE* file = fopen(filename, "w");
    if (file == nullptr) {
        printf("Error opening file for writing.\n");
        return;
    }
    Job* current = jobList;
    while (current != nullptr) {
        fprintf(file, "%s,%s,%s\n", current->title, current->company, current->location);
        current = current->next;
    }
    fclose(file);
    printf("Jobs saved to %s\n", filename);
}

void loadJobsFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == nullptr) {
        printf("Error opening file for reading.\n");
        return;
    }

    Job* current = jobList;
    while (current != nullptr) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
    jobList = nullptr;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char title[MAX_TITLE_LENGTH], company[MAX_COMPANY_LENGTH], location[MAX_LOCATION_LENGTH];
        sscanf(line, "%99[^,],%99[^,],%99[^\n]", title, company, location);
        addJob(title, company, location);
    }
    fclose(file);
    printf("Jobs loaded from %s\n", filename);
}

void searchJobByLocation(const char* location) {
    Job* current = jobList;
    int found = 0;
    printf("Jobs available in %s:\n", location);
    while (current != nullptr) {
        if (strcmp(current->location, location) == 0) {
            printf("Title: %s, Company: %s\n", current->title, current->company);
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
    while (current != nullptr) {
        Job* temp = current;
        current = current->next;
        free(temp);
    }
}

void freeApplicants() {
    Applicant* current = applicantQueueFront;
    while (current != nullptr) {
        Applicant* temp = current;
        current = current->next;
        free(temp);
    }
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
    delay(500);  
    cleardevice(); 
    setcolor(WHITE);
    outtextxy(getmaxx() / 2 - 70, getmaxy() / 2, "Job Finder!");
    delay(1500);
    getch();
    closegraph();
}
#define SCREEN_WIDTH 80  // approximate console width

void printfce(const char* text) {
    int len = (int)strlen(text);
    int padding = (SCREEN_WIDTH - len) / 2;
    if (padding < 0) padding = 0;
    for (int i = 0; i < padding; i++) putchar(' ');
    printf("%s\n", text);
}
int main() {
    showGraphicsAnimation(); 



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
                while (current != nullptr) {
                    if (strcmp(current->location, location) == 0) {
                        printf("Title: %s, Company: %s\n", current->title, current->company);
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
                while (current != nullptr) {
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
            case 0:
                printf("Exiting the program.\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);

    freeJobs();
    freeApplicants();

    showGraphicsAnimation(); 

    return 0;
}


