#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define USERS_FILE "users.txt"
#define BALANCE_DIR "balances/"
#define LOG_DIR "logs/"

// Utility function to ensure directories exist.
void ensure_directories() {
    struct stat st = {0};

    // Check and create balances directory
    if (stat(BALANCE_DIR, &st) == -1) {
        #ifdef _WIN32
            _mkdir(BALANCE_DIR);  // For Windows
        #else
            mkdir(BALANCE_DIR, 0700);  // For Unix/Linux
        #endif
    }

    // Check and create logs directory
    if (stat(LOG_DIR, &st) == -1) {
        #ifdef _WIN32
            _mkdir(LOG_DIR);  // For Windows
        #else
            mkdir(LOG_DIR, 0700);  // For Unix/Linux
        #endif
    }
}

// Check if a user exists in the users file.
int user_exists(const char *username) {
    FILE *file = fopen(USERS_FILE, "r");
    if (file == NULL) {
        return 0;
    }

    char line[100];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0;  // Remove newline character.
        if (strcmp(line, username) == 0) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}

// Register a new user.
void register_user(const char *username) {
    FILE *file = fopen(USERS_FILE, "a");
    if (file == NULL) {
        printf("Error: Unable to register user.\n");
        return;
    }

    fprintf(file, "%s\n", username);
    fclose(file);

    char balance_file[200];
    snprintf(balance_file, sizeof(balance_file), BALANCE_DIR "%s.txt", username);
    FILE *balance = fopen(balance_file, "w");
    if (balance) {
        fprintf(balance, "1000.00\n");
        fclose(balance);
    }

    printf("User '%s' registered successfully with an initial balance of $1000.00.\n", username);
}

// Get the balance file path for a user.
void get_balance_file(const char *username, char *path) {
    snprintf(path, 200, BALANCE_DIR "%s.txt", username);
}

// Get the log file path for a user.
void get_log_file(const char *username, char *path) {
    snprintf(path, 200, LOG_DIR "%s.txt", username);
}

// Check the balance of a user.
void check_balance(const char *username) {
    char balance_file[200];
    get_balance_file(username, balance_file);

    FILE *file = fopen(balance_file, "r");
    if (file == NULL) {
        printf("Error: Unable to read balance for user '%s'.\n", username);
        return;
    }

    float balance;
    fscanf(file, "%f", &balance);
    fclose(file);

    printf("User '%s', your current balance is: $%.2f\n", username, balance);
}

// Deposit money for a user.
void deposit(const char *username) {
    char balance_file[200];
    get_balance_file(username, balance_file);

    FILE *file = fopen(balance_file, "r");
    if (file == NULL) {
        printf("Error: Unable to access balance for user '%s'.\n", username);
        return;
    }

    float balance, amount;
    fscanf(file, "%f", &balance);
    fclose(file);

    printf("Enter deposit amount: $");
    scanf("%f", &amount);

    if (amount > 0) {
        balance += amount;

        file = fopen(balance_file, "w");
        fprintf(file, "%.2f", balance);
        fclose(file);

        char log_file[200];
        get_log_file(username, log_file);
        file = fopen(log_file, "a");
        fprintf(file, "Deposit: $%.2f - New Balance: $%.2f\n", amount, balance);
        fclose(file);

        printf("Successfully deposited $%.2f. New balance is $%.2f.\n", amount, balance);
    } else {
        printf("Invalid deposit amount.\n");
    }
}

// Withdraw money for a user.
void withdraw(const char *username) {
    char balance_file[200];
    get_balance_file(username, balance_file);

    FILE *file = fopen(balance_file, "r");
    if (file == NULL) {
        printf("Error: Unable to access balance for user '%s'.\n", username);
        return;
    }

    float balance, amount;
    fscanf(file, "%f", &balance);
    fclose(file);

    printf("Enter withdrawal amount: $");
    scanf("%f", &amount);

    if (amount > 0 && amount <= balance) {
        balance -= amount;

        file = fopen(balance_file, "w");
        fprintf(file, "%.2f", balance);
        fclose(file);

        char log_file[200];
        get_log_file(username, log_file);
        file = fopen(log_file, "a");
        fprintf(file, "Withdrawal: $%.2f - New Balance: $%.2f\n", amount, balance);
        fclose(file);

        printf("Successfully withdrew $%.2f. New balance is $%.2f.\n", amount, balance);
    } else {
        printf("Insufficient balance or invalid withdrawal amount.\n");
    }
}

// View transaction log for a user.
void view_transactions(const char *username) {
    char log_file[200];
    get_log_file(username, log_file);

    FILE *file = fopen(log_file, "r");
    if (file == NULL) {
        printf("No transaction history available for user '%s'.\n", username);
        return;
    }

    char line[256];
    printf("Transaction History for '%s':\n", username);
    while (fgets(line, sizeof(line), file)) {
        printf("%s", line);
    }
    fclose(file);
}

// Display the menu.
void show_menu() {
    printf("==============================\n");
    printf("Transaction Management System\n");
    printf("==============================\n");
    printf("1. Deposit\n");
    printf("2. Withdraw\n");
    printf("3. Check Balance\n");
    printf("4. View Transaction Log\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
}

int main() {
    ensure_directories();

    char username[100];
    while (1) {
        printf("Enter your username: ");
        scanf("%99s", username);

        if (!user_exists(username)) {
            printf("User '%s' does not exist. Would you like to register? (y/n): ", username);
            char choice;
            scanf(" %c", &choice);
            if (choice == 'y' || choice == 'Y') {
                register_user(username);
                break;  // Exit the username entry loop
            } else {
                printf("Please enter a valid username or register to proceed.\n");
                continue;  // Prompt for username again
            }
        } else {
            printf("Welcome back, '%s'!\n", username);
            break;  // Exit the username entry loop
        }
    }

    int choice;
    while (1) {
        show_menu();
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                deposit(username);
                break;
            case 2:
                withdraw(username);
                break;
            case 3:
                check_balance(username);
                break;
            case 4:
                view_transactions(username);
                break;
            case 5:
                printf("Exiting the system. Goodbye!\n");
                exit(0);
            default:
                printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}
