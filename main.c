/*
    Title: Library system
    Author: Ben Brixton
    Date created: 12/04/23
    Last modified: 30/04/23
    Description:    - Database system for library
                    - Books are stored in a txt file in a CSV format / array of structs
                    - Allows users to...
                        - Search books (by title/author/publication year)
                        - Add books (entering title/author/publication year information)
                        - Remove books (remove all data about book from database)
                        - Edit books (change title/author/publication year information)
                        - Check books in/out (for a week at a time, giving their name)
                        - Check which books are overdue (tells user information about books, name of person who took it out, # days overdue)
                    - Information stored about every books includes...
                        - Book index in database
                        - Book title
                        - Book author
                        - Book publication year
                        - Date books added to database
                        - Date book was taken out (0 if not currently out)
                        - Date book is due (7 days after book taken out, 0 if not currently out)
                        - Name of person who took book out (0 if not currently out)
*/

// Header files (all standard)
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

// Book structure definition
struct book {
    int index;
    char title[51];
    char author[51];
    int pub_year;
    time_t date_added;
    time_t date_out;
    time_t date_due;
    char name[51];
};

// Function prototypes
int csvToStructs(char* fileName, int maxRowLength, int maxRows, struct book *books, int *numRows);
time_t getDate(void);
void printBooks(struct book *books, int numRows);
int string_to_time(char* date);
char* time_to_string(time_t timeFormatted);
void borrowBook(struct book *books, int *numRows, time_t current_date);
void returnBook(struct book *books, int *numRows);
void addBook(struct book *books, int *numRows, time_t current_date);
void deleteBook(struct book *books, int *numRows);
void editBook(struct book *books, int *numRows, time_t current_date);
void saveFile(char* fileName, struct book *books, int numRows);
void searchBooks(struct book *books, int *numRows, time_t current_date);
void checkBooks(struct book *books, int *numRows, time_t current_date);

// Main
void main(void){

    char fileName[] = "database.txt";       // File name to be read from
    int maxRowLength = 500;     // Max length for row in CSV file
    int maxRows = 100;      // Max number of rows in CSV file
    struct book books[maxRows];     // Array of structures for data to be read to
    int numRows;

    int file_successfully_read = csvToStructs(fileName, maxRowLength, maxRows, books, &numRows);     // Read data to the structure from txt file (in CSV format)

    if(file_successfully_read){     // If file has been read
        printf("Database file, \"%s\", successfully read.\n\n", fileName);      // Tell user

        time_t current_date = getDate();        // Get current date

        int running = 1;        // Set running variable (used for exiting program)
        while(running){     // Repeat until user quits program

            // Give user menu options
            system("cls");
            char choice;
            do{
                printf("--== Menu ==--\n");
                printf("[s] Search books\n[l] List books \n[a] Add a book\n[c] Check overdue books\n[q] Quit program\n");
                fflush(stdin);
                scanf("%c", &choice);
            } while(!(choice=='s' || choice=='l' || choice=='a' || choice=='c' || choice=='q'));      // Ensure user picks one of the options

            // Run function associated to user's choice/set running to 0 (quit program)
            switch(choice){
                case 's':
                    searchBooks(books, &numRows, current_date);
                    break;
                case 'l':
                    printBooks(books, numRows);
                    break;
                case 'a':
                    addBook(books, &numRows, current_date);
                    break;
                case 'c':
                    checkBooks(books, &numRows, current_date);
                    break;
                case 'q':
                    saveFile(fileName, books, numRows);
                    running = 0;
                    break;
            }
        }
    }
    else{       // Error message if file not found
        printf("Database file, \"%s\", cannot be found.", fileName);
    }
}

// Convert txt file in CSV format into array of book structures
int csvToStructs(char* fileName, int maxRowLength, int maxRows, struct book *books, int *numRows) {

    // Open file
    FILE* fin;

    // If file can be found
    if(fin = fopen(fileName, "r")) {

        char row[maxRowLength];     // Make temporary row storage
        char * currentValue;        // Make temporary current value storage
        int rowNum = 0;      // Set up array iterator

        fgets(row, maxRowLength, fin);      // Skip past first row (column headings)
        while( fgets(row, maxRowLength, fin) != NULL){      // Read rows until end of file
            currentValue = strtok(row, ",");        // Split row by "," delimeter

            // Copy data to relevant part of structure, then split by next "," delimeter
            books[rowNum].index = atoi(currentValue); currentValue = strtok(NULL, ",\n");
            strcpy(books[rowNum].title, currentValue); currentValue = strtok(NULL, ",\n");
            strcpy(books[rowNum].author, currentValue); currentValue = strtok(NULL, ",\n");
            books[rowNum].pub_year = atoi(currentValue); currentValue = strtok(NULL, ",\n");
            books[rowNum].date_added = atoi(currentValue); currentValue = strtok(NULL, ",\n");
            books[rowNum].date_out = atoi(currentValue); currentValue = strtok(NULL, ",\n");
            books[rowNum].date_due = atoi(currentValue); currentValue = strtok(NULL, ",\n");
            strcpy(books[rowNum].name, currentValue);

            rowNum++;       // Go to next index in array
        }
        *numRows = rowNum;      // Set numRows (used throughout)
        fclose(fin);        // Close file
        return 1;       // Return 1 (successfully read)
    }
    else {
        return 0;       // Return 0 (unsuccessfully read)
    }
}

time_t getDate(void){
    system("cls");      // Clear screen

    // Get current date (from computer's time)
    time_t current_date = time(NULL);
    localtime(&current_date);

    // Check that is correct date
    char choice;
    do{
        printf("Todays date is: %s\nIs this correct? (y/n)\n", time_to_string(current_date));
        fflush(stdin);
        scanf("%c", &choice);
    } while(!(choice=='y' || choice=='n'));      // Ensure user picks one of the options

    printf("\n");

    if(choice == 'n'){      // If date not correct
        int newDay, newMonth, newYear;
        char newDate[11];
        do{
            // Get dd/mm/yyyy data from user
            do{
                printf("Day (dd): ");
                fflush(stdin); scanf("%d", &newDay);
            } while(!(newDay>=1 && newDay <= 31));
            do{
                printf("Month (mm): ");
                fflush(stdin); scanf("%d", &newMonth);
            } while(!(newMonth>=1 && newMonth <= 12));
            do{
                printf("Year (yyyy): ");
                fflush(stdin); scanf("%d", &newYear);
            } while(!(newYear>=2023 && newYear < 2038));

            // Store new dd//mm/yyyy, making sure right # digits used
            snprintf(newDate, 11, "%.2d/%.2d/%.4d", newDay, newMonth, newYear);

        } while(strcmp(newDate, time_to_string(string_to_time(newDate))));      // Repeat until date is valid (convert to string form and back, and check it is the same)
        current_date = string_to_time(newDate);     // Current date variable
        printf("\n");
    }
    return current_date;        // Return current date
}

void printBooks(struct book *books, int numRows){
    system("cls");
    // Repeat # times as there are rows, printing all data for each
    for(int i=0; i<numRows; i++){
        printf("Book %d\n", books[i].index+1);
        printf("Title: %s\n", books[i].title);
        printf("Author: %s\n", books[i].author);
        printf("Publication year: %d\n", books[i].pub_year);
        printf("Date added: %s\n", time_to_string(books[i].date_added));
        printf("Status: ");

        // Check if book is out at the moment (if true, would have a date out)
        if(books[i].date_out != 0){
            printf("Out\n");
        }
        else{
            printf("Available\n");
        }
        printf("\n");
    }

    // Allow user to return to main menu
    printf("[q] Go back\n");
    char input;
    do{
        fflush(stdin);
        input = getchar();
    } while(input != 'q');
}

int string_to_time(char* stringFormatted){
    // Split date into variables
    int dd, mm, yyyy;
    sscanf_s(stringFormatted, "%d/%d/%d", &dd, &mm, &yyyy);

    // Split put date into tm struct
    struct tm when = {0};
    when.tm_mday = dd;
    when.tm_mon = mm - 1;
    when.tm_year = yyyy - 1900;

    // Convert into time_t form
    time_t converted;
    converted = mktime(&when);

    return converted;       // return converted time
}

char* time_to_string(time_t timetFormatted){
    // Convert from time_t form
    struct tm *giverTime;
    giverTime = localtime(&timetFormatted);

    // Split into different variables
    struct tm given = {0};
    given.tm_mday = giverTime->tm_mday;
    given.tm_mon = giverTime->tm_mon+1; /* simply add instead of increment */
    given.tm_year = giverTime->tm_year+1900;

    // Form string again
    char* converted = malloc(11);
    snprintf(converted, 11, "%.2d/%.2d/%.4d", given.tm_mday, given.tm_mon, given.tm_year);

    return converted;       // Return converted time
}

void addBook(struct book *books, int *numRows, time_t current_date){
    system("cls");

    // Add new index
    int new_index = *numRows;

    // User input for title (takes max 50 chars)
    char new_title[51];
    printf("Title (max 50 chars, cannot include ','): ");
    fflush(stdin);
    fgets(new_title,51,stdin);
    int size = strlen(new_title);
    new_title[size-1]='\0';     // Remove '\n' from end of string

    // User input for author (takes max 50 chars)
    char new_author[51];
    printf("Author (max 50 chars, cannot include ','): ");
    fflush(stdin);
    fgets(new_author,51,stdin);
    size = strlen(new_author);
    new_author[size-1]='\0';     // Remove '\n' from end of string

    // Get max year for publication date (current year)
    time_t seconds=time(NULL);
    struct tm* current_time=localtime(&seconds);
    int maxYear = current_time->tm_year + 1900;

    // Ask user for publication year
    int new_pub_year;
    do{
        printf("Publication year (minimum 0 AD): ");
        fflush(stdin); scanf("%d", &new_pub_year);        // Read max of 50 chars
    } while(!(new_pub_year > 0 && new_pub_year < maxYear));     // Check within range (0-current year)

    // Set other variables
    int new_date_added = current_date;      // Set date added to be today's date
    int new_date_out = 0;       // Set date out to be blank
    int new_date_due = 0;       // Set date due to be black
    char new_name[] = "0";      // Set name to be blank

    // Copy all the data into next location the array of books
    books[*numRows].index = new_index;
    strcpy(books[*numRows].title, new_title);
    strcpy(books[*numRows].author, new_author);
    books[*numRows].pub_year = new_pub_year;
    books[*numRows].date_added = new_date_added;
    books[*numRows].date_out = new_date_out;
    books[*numRows].date_due = new_date_due;
    strcpy(books[*numRows].name, new_name);

    // Update number of rows in database
    *numRows = *numRows + 1;
}

void deleteBook(struct book *books, int *numRows){
    // Get index for book to be deleted
    int index_to_delete;
    do{
        printf("Index to delete: ");
        fflush(stdin); scanf("%d", &index_to_delete);
        index_to_delete -= 1;
    } while(!(index_to_delete>=0 && index_to_delete<*numRows));     // Ensure in range of list of structs
    printf("\n");

    // Print book info
    printf("Title: %s\n", books[index_to_delete].title);
    printf("Author: %s\n", books[index_to_delete].author);
    printf("Publication year: %d\n", books[index_to_delete].pub_year);
    printf("\n");

    // Check this is the right book
    printf("Delete this book? (permanent) [y/n]\n");
    char choice;
    do{
        fflush(stdin);
        scanf("%c", &choice);
    } while(!(choice=='y' || choice=='n'));      // Ensure user picks one of the options

    // If index is correct
    if(choice == 'y'){
        // Shift all books along one, up to the delete index
        for(int i = index_to_delete; i < *numRows; i++){
            books[i] = books[i + 1];
            books[i].index = books[i].index-1;
        }
        *numRows -= 1;      // Update length of array
    }
    printf("\n");
}

void editBook(struct book *books, int *numRows, time_t current_date){
    // Get index to edit from user
    int index_to_edit;
    do{
        printf("Index to edit: ");
        fflush(stdin); scanf("%d", &index_to_edit);
        index_to_edit -= 1;     // -1 to get index for list of structs
    } while(!(index_to_edit>=0 && index_to_edit<*numRows));     // Ensure in range
    printf("\n");

    // Set editing variable (repeat until user quits)
    int editing = 1;
    while(editing){

        system("cls");
        // Print book info
        printf("Title: %s\n", books[index_to_edit].title);
        printf("Author: %s\n", books[index_to_edit].author);
        printf("Publication year: %d\n", books[index_to_edit].pub_year);
        printf("\n");

        // Get user input for what to edit
        char choice;
        printf("What do you want to edit?\n");
        printf("[t] Title\n[a] Author\n[p] Publication year\n[q] Stop editing\n");
        do{
            fflush(stdin);
            scanf("%c", &choice);
        } while(!(choice=='t' || choice=='a' || choice=='p' || choice=='q'));      // Ensure user picks one of the options
        printf("\n");

        int size;
        switch(choice){

            // Let user set title to new value
            case 't': ;
                char new_title[51];
                printf("Title (max 50 chars, cannot include ','): ");
                fflush(stdin);
                fgets(new_title,51,stdin);

                size = strlen(new_title);
                new_title[size-1]='\0';     // remove '\n' from end of string

                strcpy(books[index_to_edit].title, new_title);      // Set value in list of structs
                break;

            // Let user set author to new value
            case 'a': ;
                char new_author[51];
                printf("Author (max 50 chars, cannot include ','): ");
                fflush(stdin);
                fgets(new_author,51,stdin);

                size = strlen(new_author);
                new_author[size-1]='\0';     // remove '\n' from end of string

                strcpy(books[index_to_edit].author, new_author);      // Set value in list of structs

                break;

            // Let user set publication year to new value
            case 'p': ;
                time_t seconds=time(NULL);
                struct tm* current_time=localtime(&seconds);
                int maxYear = current_time->tm_year + 1900;
                int new_pub_year;
                do{
                    printf("Publication year (minimum 0AD): ");
                    fflush(stdin); scanf("%d", &new_pub_year);        // Read max of 50 chars
                } while(!(new_pub_year > 0 && new_pub_year < maxYear));     // Check value in range (0-current year)

                books[index_to_edit].pub_year = new_pub_year;      // Set value in list of structs
                break;

            // Let user stop editing
            case 'q': ;
                editing = 0;
                break;
        }
        printf("\n");
    }
}

void saveFile(char* fileName, struct book *books, int numRows){
    system("cls");      // Clear screen
    printf("Saving file, do not close...\n");       // Tell user not to close program

    FILE* fin;      // Make pointer
    if(!(fin = fopen(fileName, "r"))){      // Check if file can be opened
        printf("Data file cannot be found. New file will be created.\n");      // Tell user file cannot be found
    }
    fin = fopen(fileName, "w+");        // Open/make new file

    fprintf(fin, "index,title,author,pub_year,date_added,date_out,date_due,name\n");        // Add column titles

    // Write rows to txt file in CSV format
    for(int i=0; i<numRows; i++){

        // Replace any ',' with '.' as CSV is comma delimited in title/author/name
        int pos = 0;        // Start at first char
        while(books[i].title[pos] != '\0'){     // Read until end of string
            if(books[i].title[pos] == ','){     // If char is ','
                books[i].title[pos] = '.';      // Replace with '.'
            }
            pos += 1;       // Go to next char
        }
        pos = 0;        // Start at first char
        while(books[i].author[pos] != '\0'){     // Read until end of string
            if(books[i].author[pos] == ','){     // If char is ','
                books[i].author[pos] = '.';      // Replace with '.'
            }
            pos += 1;       // Go to next char
        }
        pos = 0;        // Start at first char
        while(books[i].name[pos] != '\0'){     // Read until end of string
            if(books[i].name[pos] == ','){     // If char is ','
                books[i].name[pos] = '.';      // Replace with '.'
            }
            pos += 1;       // Go to next char
        }

        // Write row to file
        fprintf(fin,"%d,%s,%s,%d,%d,%d,%d,%s\n",books[i].index,books[i].title,books[i].author,books[i].pub_year,books[i].date_added,books[i].date_out,books[i].date_due,books[i].name);
    }

    fclose(fin);        // Close file
    printf("File saved. You can now close the program.\n");       // Tell user they can exit
}

void borrowBook(struct book *books, int *numRows, time_t current_date){
    // Get index for book to be borrowed
    int index_to_borrow;
    do{
        printf("Index to borrow: ");        // Ask user for input
        fflush(stdin);      // // Clear stdin
        scanf("%d", &index_to_borrow);       // Get value
        index_to_borrow -= 1;       // Subtract 1 to get index in list
    } while(!(index_to_borrow>=0 && index_to_borrow<*numRows));     // Ensure in range
    printf("\n");

    if(books[index_to_borrow].date_out == 0){       // If the book is not currently out
        system("cls");      // Clear screen

        // Get user's name
        char name[51];
        int size;
        printf("Full name (max 50 chars, cannot include ','): ");
        fflush(stdin);
        fgets(name,51,stdin);

        size = strlen(name);        // Get rid of \n from end of string
        name[size-1]='\0';      // Get rid of \n from end of string

        strcpy(books[index_to_borrow].name, name);      // Copy name to list of structs
        books[index_to_borrow].date_out = current_date;     // Add date out (current date) to list of structs
        books[index_to_borrow].date_due = current_date + 60*60*24*7;        // Add date due (7 days form current date) to list of structs

        // Tell user book successfully borrowed
        printf("\n");
        printf("Book successfully borrowed\n\n");
    }
    else{       // If book already out, tell user
        printf("This book is already out\n\n");
    }

    // Allow user to go back to menu
    printf("[q] Go back\n");
    char input;
    do{
        fflush(stdin);
        input = getchar();
    } while(input != 'q');
}

void returnBook(struct book *books, int *numRows){
    // Get index for book to be returned
    int index_to_return;
    do{
        printf("Index to return: ");
        fflush(stdin); scanf("%d", &index_to_return);
        index_to_return -= 1;
    } while(!(index_to_return>=0 && index_to_return<*numRows));     // Ensure in range
    printf("\n");

    if(books[index_to_return].date_out != 0){
        system("cls");

        strcpy(books[index_to_return].name, "0");
        books[index_to_return].date_out = 0;
        books[index_to_return].date_due = 0;
        printf("Book successfully returned\n\n");
    }
    else{
        printf("This book is not currently out\n\n");
    }

    printf("[q] Go back\n");
    char input;
    do{
        fflush(stdin);
        input = getchar();
    } while(input != 'q');
}

void searchBooks(struct book *books, int *numRows, time_t current_date){
    system("cls");      // Clear screen

    // Ask user what to search by
    char choice;
    printf("What do you want to search by?\n");
    printf("[t] Title\n[a] Author\n[p] Publication year\n\n");
    do{
        fflush(stdin);
        scanf("%c", &choice);
    } while(!(choice=='t' || choice=='a' || choice=='p'));      // Ensure user picks one of the options
    printf("\n");

    // Ask user for search term
    char term[51];
    printf("Search term (max 50 chars): ");
    fflush(stdin);
    fgets(term,51,stdin);

    // Remove '\n' from string
    int size = strlen(term);
    term[size-1]='\0';

    printf("\n");

    // Convert search term to all upper-case (to normalise casing, allowing non-case-sensitive search)
    for(int i=0; i<size; i++){
        term[i] = toupper(term[i]);
    }


    // Print matching books to user
    system("cls");
    char temp[51];
    printf("Matching books: \n");

    switch(choice){

        // Search by title
        case 't': ;
            for(int i=0; i<*numRows; i++){      // Go through every book

                // Convert value to all upper-case (to normalise casing, allowing non-case-sensitive search)
                strcpy(temp, books[i].title);
                for(int i=0; i<50; i++){
                    temp[i] = toupper(temp[i]);
                }

                if(strstr(temp, term) != NULL){     // Check if it contains search term
                    // Print relevant information about book
                    printf("Book %d: ", books[i].index+1);
                    printf("%s, ", books[i].title);
                    printf("%s, ", books[i].author);
                    printf("%d, ", books[i].pub_year);
                    if(books[i].date_out != 0){
                        printf("(OUT)\n");
                    }
                    else{
                        printf("(AVAILABLE)\n");
                    }
                }
            }
            break;

        // Search by author
        case 'a': ;
            for(int i=0; i<*numRows; i++){      // Go through every book

                // Convert value to all upper-case (to normalise casing, allowing non-case-sensitive search)
                strcpy(temp, books[i].author);
                for(int i=0; i<50; i++){
                    temp[i] = toupper(temp[i]);
                }

                if(strstr(temp, term) != NULL){     // Check if it contains search term
                    // Print relevant information about book
                    printf("Book %d:\t", books[i].index+1);
                    printf("%s, \t", books[i].title);
                    printf("%s, \t", books[i].author);
                    printf("%d, \t", books[i].pub_year);
                    if(books[i].date_out != 0){
                        printf("OUT\n");
                    }
                    else{
                        printf("AVAILABLE\n");
                    }
                }
            }
            break;

        // Search by publication year
        case 'p': ;
            for(int i=0; i<*numRows; i++){      // Go through every book
                if(books[i].pub_year ==  atoi(term)){     // Check if it contains search term
                    // Print relevant information about book
                    printf("Book %d:\t", books[i].index+1);
                    printf("%s, \t", books[i].title);
                    printf("%s, \t", books[i].author);
                    printf("%d, \t", books[i].pub_year);
                    if(books[i].date_out != 0){
                        printf("OUT\n");
                    }
                    else{
                        printf("AVAILABLE\n");
                    }
                }
            }
            break;
    }
    printf("\n");

    // Give user options
    printf("[b] Borrow book \n[r] Return a book \n[e] Edit a book \n[d] Delete a book \n[s] Search again \n[q] Stop searching\n");
    do{
        fflush(stdin);
        scanf("%c", &choice);
    } while(!(choice == 'b' || choice == 'r' || choice=='e' || choice=='d' || choice=='s' || choice=='q'));      // Ensure user picks one of the options

    printf("\n");

    // Run related function/end searching
    switch(choice){
        case 'b':
            borrowBook(books, numRows, current_date);
            break;
        case 'r':
            returnBook(books, numRows);
            break;
        case 'e':
            editBook(books, numRows, current_date);
            break;
        case 'd':
            deleteBook(books, numRows);
            break;
        case 's':
            searchBooks(books, numRows, current_date);
            break;
        case 'q':
            break;
    }
}

void checkBooks(struct book *books, int *numRows, time_t current_date){
    system("cls");

    // Print all books where time between date due < current date (over due)
    printf("Currently overdue books:\n");
    for(int i=0; i<*numRows; i++){      // Go through every book
        if(books[i].date_due != 0 && books[i].date_due < current_date){     // If date due < current date (overdue)
            printf("%d. %s, %s, %d days overdue\n", i+1, books[i].title, books[i].name, (current_date-books[i].date_due)/(60*60*24));       // Print information, including # days overdue
        }
    }

    printf("\n");

    // Allow user to go back to main menu
    printf("[q] Go back\n");
    char input;
    do{
        fflush(stdin);
        input = getchar();
    } while(input != 'q');
}
