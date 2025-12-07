#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Required for tolower in a robust implementation, though not strictly used in the provided version

// --- Configuration Constants ---
#define ADMIN_PASS "admin123"
#define MAX_STUDENTS 100
#define MAX_TICKETS 100
// Note: You must ensure this path is correct for your system to save/load student data
#define STUDENT_FILE "C:\\Users\\Meghana\\Desktop\\students.txt" 
#define TICKET_FILE "C:\\Users\\Meghana\\Deskto1p\\tickets.txt"

// --- Data Structures ---

typedef struct {
    int roll;
    char name[50];
    char branch[30];
    int year;
    float cgpa;
    char phone[15];
} Student;

typedef struct {
    int ticket_id;
    int roll;
    char field_name[20];
    char old_value[50];
    char new_value[50];
    char status[15]; // Pending, Approved, Rejected
} Ticket;

// --- Global Data Arrays (In-Memory Storage) ---
Student students[MAX_STUDENTS];
Ticket tickets[MAX_TICKETS];
int student_count = 0;
int ticket_count = 0;

// --- Function Prototypes (Declarations) ---
void load_data();
void save_data();
void student_login();
void student_menu(int roll);
void raise_ticket(int roll);
void view_ticket_status(int roll);
void admin_login();
void admin_menu();
void add_student();
void view_all_students();
void process_tickets();

// ---------------- FILE I/O AND DATA UTILITIES ----------------

/**
 * @brief Finds a student in the global array by their roll number.
 * @param roll The roll number to search for.
 * @return A pointer to the Student structure if found, otherwise NULL.
 */
Student* find_student(int roll) {
    for (int i = 0; i < student_count; i++) {
        if (students[i].roll == roll) {
            return &students[i];
        }
    }
    return NULL;
}

/**
 * @brief Loads student and ticket data from files into memory arrays.
 * * Data is expected to be in a comma-separated format.
 */
void load_data() {
    FILE *fp;
    
    // Load Students
    fp = fopen(STUDENT_FILE, "r");
    if (fp) {
        student_count = 0;
        // The format string %[^,] reads all characters until a comma is found.
        while (student_count < MAX_STUDENTS && 
               fscanf(fp, "%d,%49[^,],%29[^,],%d,%f,%14s\n", 
                      &students[student_count].roll, 
                      students[student_count].name, 
                      students[student_count].branch, 
                      &students[student_count].year, 
                      &students[student_count].cgpa, 
                      students[student_count].phone) == 6) {
            student_count++;
        }
        fclose(fp);
        printf("Loaded %d student records.\n", student_count);
    } else {
        printf("Student data file (%s) not found. Starting with empty student list.\n", STUDENT_FILE);
    }

    // Load Tickets
    fp = fopen(TICKET_FILE, "r");
    if (fp) {
        ticket_count = 0;
        while (ticket_count < MAX_TICKETS &&
               fscanf(fp, "%d,%d,%19[^,],%49[^,],%49[^,],%14s\n", 
                      &tickets[ticket_count].ticket_id, 
                      &tickets[ticket_count].roll, 
                      tickets[ticket_count].field_name, 
                      tickets[ticket_count].old_value, 
                      tickets[ticket_count].new_value, 
                      tickets[ticket_count].status) == 6) {
            ticket_count++;
        }
        fclose(fp);
        printf("Loaded %d ticket records.\n", ticket_count);
    } else {
        printf("Ticket data file (%s) not found. Starting with empty ticket list.\n", TICKET_FILE);
    }
}

/**
 * @brief Saves student and ticket data from memory arrays to files.
 * * This overwrites the previous file content with current data.
 */
void save_data() {
    FILE *fp;
    
    // Save Students
    fp = fopen(STUDENT_FILE, "w");
    if (fp) {
        for (int i = 0; i < student_count; i++) {
            fprintf(fp, "%d,%s,%s,%d,%.2f,%s\n", 
                    students[i].roll, students[i].name, students[i].branch, 
                    students[i].year, students[i].cgpa, students[i].phone);
        }
        fclose(fp);
    } else {
        fprintf(stderr, "Error: Could not save student data to %s!\n", STUDENT_FILE);
    }

    // Save Tickets
    fp = fopen(TICKET_FILE, "w");
    if (fp) {
        for (int i = 0; i < ticket_count; i++) {
            fprintf(fp, "%d,%d,%s,%s,%s,%s\n", 
                    tickets[i].ticket_id, tickets[i].roll, tickets[i].field_name, 
                    tickets[i].old_value, tickets[i].new_value, tickets[i].status);
        }
        fclose(fp);
    } else {
        fprintf(stderr, "Error: Could not save ticket data to %s!\n", TICKET_FILE);
    }
}


// ---------------- STUDENT FUNCTIONS ----------------

void raise_ticket(int roll) {
    if (ticket_count >= MAX_TICKETS) {
        printf("Ticket system is full. Cannot raise new ticket.\n");
        return;
    }

    Student *s = find_student(roll);
    if (!s) {
        printf("Error: Your student record was not found.\n");
        return; 
    }

    Ticket *t = &tickets[ticket_count];
    
    printf("\n--- Raise Correction Ticket ---\n");
    printf("Which field you want to correct?\n");
    printf("Enter field name (name/branch/year/cgpa/phone): ");
    scanf("%s", t->field_name);
    
    // 1. Get Old Value
    if (strcmp(t->field_name, "name") == 0)      { strncpy(t->old_value, s->name, 49); t->old_value[49] = '\0'; }
    else if (strcmp(t->field_name, "branch") == 0) { strncpy(t->old_value, s->branch, 49); t->old_value[49] = '\0'; }
    else if (strcmp(t->field_name, "phone") == 0)  { strncpy(t->old_value, s->phone, 49); t->old_value[49] = '\0'; }
    else if (strcmp(t->field_name, "year") == 0)   { sprintf(t->old_value, "%d", s->year); }
    else if (strcmp(t->field_name, "cgpa") == 0)   { sprintf(t->old_value, "%.2f", s->cgpa); }
    else {
        printf("Invalid field name.\n");
        return;
    }

    // Clear input buffer for safe reading of new value
    while (getchar() != '\n'); 
    
    printf("Current Value: %s\n", t->old_value);
    printf("Enter correct %s (New Value): ", t->field_name);
    // Use fgets for safer reading of the new value
    fgets(t->new_value, sizeof(t->new_value), stdin);
    t->new_value[strcspn(t->new_value, "\n")] = 0; // Remove trailing newline

    // 2. Insert into tickets array
    t->ticket_id = ticket_count > 0 ? tickets[ticket_count-1].ticket_id + 1 : 1;
    t->roll = roll;
    strcpy(t->status, "Pending");
    
    ticket_count++;
    save_data();
    
    printf("\n✅ Ticket submitted successfully! (ID: %d)\n", t->ticket_id);
}

void view_ticket_status(int roll) {
    printf("\n--- Ticket Status for Roll %d ---\n", roll);
    int found = 0;
    for (int i = 0; i < ticket_count; i++) {
        Ticket *t = &tickets[i];
        if (t->roll == roll) {
            printf("ID: %d | Field: %s | Old: %s | New: %s | Status: **%s**\n", 
                   t->ticket_id, t->field_name, t->old_value, t->new_value, t->status);
            found = 1;
        }
    }
    if (!found) {
        printf("No tickets found for this roll number.\n");
    }
}

void student_menu(int roll) {
    int ch;
    while (1) {
        printf("\n=== Student Menu ===\n");
        printf("1. Raise Correction Ticket\n");
        printf("2. View Ticket Status\n");
        printf("3. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &ch) != 1) {
            while (getchar() != '\n');
            printf("⚠️ Invalid Input! Please enter a number.\n");
            continue;
        }
        // Clear remaining input buffer
        while (getchar() != '\n'); 

        switch (ch) {
            case 1:
                raise_ticket(roll);
                break;
            case 2:
                view_ticket_status(roll);
                break;
            case 3:
                return; // Logout
            default:
                printf("❌ Invalid Choice!\n");
        }
    }
}

void student_login() {
    int roll;
    printf("\n--- Student Login ---\n");
    printf("Enter your Roll Number: ");
    if (scanf("%d", &roll) != 1) {
        while (getchar() != '\n');
        printf("Invalid Roll Number Input.\n");
        return;
    }
    while (getchar() != '\n'); // Clear buffer
    
    Student *data = find_student(roll);

    if (data) {
        printf("\nWelcome, %s!\n", data->name);
        printf("Roll No.: %d, Branch: %s, CGPA: %.2f\n", data->roll, data->branch, data->cgpa);
        
        student_menu(roll);
    } else {
        printf("No Student Record Found with Roll Number %d!\n", roll);
    }
}


// ---------------- ADMIN FUNCTIONS ----------------

void add_student() {
    if (student_count >= MAX_STUDENTS) {
        printf("Student database is full. Cannot add more students.\n");
        return;
    }
    
    Student *s = &students[student_count];
    printf("\n--- Add New Student ---\n");
    printf("Roll: "); 
    if (scanf("%d", &s->roll) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    while (getchar() != '\n'); // Clear buffer for name
    printf("Name: "); 
    if (fgets(s->name, 50, stdin) == NULL) { return; } 
    s->name[strcspn(s->name, "\n")] = 0; // Remove newline
    printf("Branch: "); 
    if (scanf("%s", s->branch) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    printf("Year: "); 
    if (scanf("%d", &s->year) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    printf("CGPA: "); 
    if (scanf("%f", &s->cgpa) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    printf("Phone: "); 
    if (scanf("%s", s->phone) != 1) { while (getchar() != '\n'); printf("Invalid input.\n"); return; }
    
    student_count++;
    save_data();
    printf("\n✅ Student Added Successfully! Total students: %d\n", student_count);
}

void view_all_students() {
    printf("\n--- All Students (%d Records) ---\n", student_count);
    printf("----------------------------------------------------------------------------------------\n");
    printf("| %-4s | %-20s | %-10s | %-4s | %-6s | %-12s |\n", "Roll", "Name", "Branch", "Year", "CGPA", "Phone");
    printf("----------------------------------------------------------------------------------------\n");

    for (int i = 0; i < student_count; i++) {
        Student *s = &students[i];
        printf("| %-4d | %-20s | %-10s | %-4d | %-6.2f | %-12s |\n",
               s->roll, s->name, s->branch, s->year, s->cgpa, s->phone);
    }
    printf("----------------------------------------------------------------------------------------\n");
}

void process_tickets() {
    printf("\n--- Processing Pending Tickets ---\n");
    int found_pending = 0;
    char action[10];

    for (int i = 0; i < ticket_count; i++) {
        Ticket *t = &tickets[i];
        
        if (strcmp(t->status, "Pending") == 0) {
            found_pending = 1;
            Student *s = find_student(t->roll);

            printf("\n-------------------------------------\n");
            printf("TICKET ID: %d | Student Roll: %d\n", t->ticket_id, t->roll);
            if (s) {
                printf("Student Name: %s\n", s->name);
            } else {
                printf("Student Record: NOT FOUND! (Ticket must be rejected)\n");
            }
            printf("Field: %s\n", t->field_name);
            printf("Old Value: %s\n", t->old_value);
            printf("New Value: %s\n", t->new_value);
            printf("-------------------------------------\n");
            
            printf("Action - Approve (A) / Reject (R) / Skip (S): ");
            if (scanf("%9s", action) != 1) {
                while (getchar() != '\n'); 
                continue; 
            }
            while (getchar() != '\n'); // Clear buffer

            // Convert action to lowercase for case-insensitive check
            char ch_action = tolower(action[0]);

            if (ch_action == 'a') {
                if (s) {
                    // Update student record in memory
                    if (strcmp(t->field_name, "name") == 0)      { strncpy(s->name, t->new_value, 49); s->name[49]='\0'; }
                    else if (strcmp(t->field_name, "branch") == 0) { strncpy(s->branch, t->new_value, 29); s->branch[29]='\0'; }
                    else if (strcmp(t->field_name, "phone") == 0)  { strncpy(s->phone, t->new_value, 14); s->phone[14]='\0'; }
                    else if (strcmp(t->field_name, "year") == 0)   { s->year = atoi(t->new_value); }
                    else if (strcmp(t->field_name, "cgpa") == 0)   { s->cgpa = atof(t->new_value); }
                    
                    strcpy(t->status, "Approved");
                    printf("✅ Student record updated and ticket **Approved**!\n");
                } else {
                    printf("⚠️ Error: Student record not found! Rejecting ticket.\n");
                    strcpy(t->status, "Rejected"); 
                }
            } else if (ch_action == 'r') {
                strcpy(t->status, "Rejected");
                printf("❌ Ticket **Rejected**!\n");
            } else if (ch_action == 's') {
                printf("Ticket **Skipped** (Status remains Pending).\n");
            } else {
                printf("Invalid action. Ticket status remains Pending.\n");
            }
        }
    }
    
    if (!found_pending) {
        printf("No pending tickets found.\n");
    }
    
    save_data();
}

void admin_menu() {
    int ch;
    while (1) {
        printf("\n=== ADMIN MENU ===\n");
        printf("1. Add New Student\n");
        printf("2. View All Students\n");
        printf("3. Process Tickets (Pending Corrections)\n");
        printf("4. Logout\n");
        printf("Enter choice: ");
        if (scanf("%d", &ch) != 1) {
            while (getchar() != '\n');
            printf("⚠️ Invalid Input! Please enter a number.\n");
            continue;
        }
        while (getchar() != '\n'); // Clear buffer

        switch (ch) {
            case 1:
                add_student();
                break;
            case 2:
                view_all_students();
                break;
            case 3:
                process_tickets();
                break;
            case 4:
                return; // Logout
            default:
                printf("❌ Invalid Choice!\n");
        }
    }
}

void admin_login() {
    char pwd[50];
    printf("\n--- Admin Login ---\n");
    printf("Enter Admin Password: ");
    if (scanf("%49s", pwd) != 1) {
        while (getchar() != '\n');
        printf("Invalid input.\n");
        return;
    }
    while (getchar() != '\n'); // Clear buffer
    
    if (strcmp(pwd, ADMIN_PASS) == 0) {
        printf("\n🔑 Login Successful!\n");
        admin_menu();
    } else {
        printf("❌ Incorrect Password!\n");
    }
}

// ---------------- MAIN MENU ----------------
int main() {
    // 1. Load data from files at startup
    load_data();

    int choice;
    while (1) {
        printf("\n\n========= STUDENT RECORD MANAGEMENT (File Based) =========");
        printf("\n1. Student Login\n");
        printf("2. Admin Login\n");
        printf("3. Exit\n");

        printf("Enter choice: ");
        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n'); 
            printf("⚠️ Invalid Input! Please enter a number.\n");
            continue;
        }
        while (getchar() != '\n'); // Clear buffer

        // 2. Main Menu Switch Case
        switch (choice) {
            case 1:
                student_login();
                break;
            case 2:
                admin_login();
                break;
            case 3:
                printf("Thank you for using the system. Data has been saved.\n");
                save_data(); // Save data one last time before exiting
                return 0; // Exit program
            default:
                printf("❌ Invalid Choice!\n");
        }
    }

    return 0;
}