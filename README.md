# library-management-system
A library management system, written in C

## Task

I completed this project as part of my unviersity course. We were given an open ended task, where we were asked to write a program in C of our choosing. 

## Description
This is a database system for library. Books are stored in a txt file in a CSV format / array of structs

Features include:
- Search books (by title/author/publication year)
- Add books (entering title/author/publication year information)
- Remove books (remove all data about book from database)
- Edit books (change title/author/publication year information)
- Check books in/out (for a week at a time, giving their name)
- Check which books are overdue (tells user information about books, name of person who took it out, # days overdue)

Information recorded in the database includes:
Information stored about every books includes...
- Book index in database
- Book title
- Book author
- Book publication year
- Date books added to database
- Date book was taken out (0 if not currently out)
- Date book is due (7 days after book taken out, 0 if not currently out)
- Name of person who took book out (0 if not currently out)

## How to use it

Watch the following video to see how to use the system

https://github.com/brixt01/library-management-system/assets/109489475/ba391d71-c507-4148-ab98-4e858d5d6e45
