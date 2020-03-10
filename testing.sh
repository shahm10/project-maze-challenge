#!/bin/bash
# testing.sh - shell script for testing the Amazing Maze
#
#
# LEPC (Evan, Sally, Wylie, and Jason)

echo "Testing for Amazing Maze"


###--------- Incorrect Command-Line Arguments ---------###
echo "One argument"
./AMSTARTUP

echo "Two arguments"
./AMSTARTUP hello

echo "Three arguments"
./AMSTARTUP hello hello


###--------- Argument validation checks ---------###
echo "Invalid Number of Avatars (Too Low)"
./AMSTARTUP flume.cs.dartmouth.edu 0 4

echo "Invalid Number of Avatars (Too High)"
./AMSTARTUP flume.cs.dartmouth.edu 12 4

echo "Invalid Difficulty (Too Low)"
./AMSTARTUP flume.cs.dartmouth.edu 4 -2

echo "Invalid Difficulty (Too High)"
./AMSTARTUP flume.cs.dartmouth.edu 4 10

echo "Invalid Hostname"
./AMSTARTUP floom.cs.dartmouth.edu 4 4


###--------- Difficulty 0 Tests ---------###
echo "Difficulty 0, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 0

echo "Difficulty 0, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 0

echo "Difficulty 0, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 0

echo "Difficulty 0, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 0

echo "Difficulty 0, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 0

echo "Difficulty 0, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 0

echo "Difficulty 0, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 0

echo "Difficulty 0, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 0

###--------- Difficulty 1 Tests ---------###
echo "Difficulty 1, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 1

echo "Difficulty 1, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 1

echo "Difficulty 1, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 1

echo "Difficulty 1, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 1

echo "Difficulty 1, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 1

echo "Difficulty 1, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 1

echo "Difficulty 1, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 1

echo "Difficulty 1, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 1

###--------- Difficulty 2 Tests ---------###
echo "Difficulty 2, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 2

echo "Difficulty 2, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 2

echo "Difficulty 2, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 2

echo "Difficulty 2, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 2

echo "Difficulty 2, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 2

echo "Difficulty 2, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 2

echo "Difficulty 2, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 2

echo "Difficulty 2, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 2

###--------- Difficulty 3 Tests ---------###
echo "Difficulty 3, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 3

echo "Difficulty 3, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 3

echo "Difficulty 3, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 3

echo "Difficulty 3, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 3

echo "Difficulty 3, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 3

echo "Difficulty 3, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 3

echo "Difficulty 3, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 3

echo "Difficulty 3, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 3

###--------- Difficulty 4 Tests ---------###
echo "Difficulty 4, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 4

echo "Difficulty 4, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 4

echo "Difficulty 4, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 4

echo "Difficulty 4, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 4

echo "Difficulty 4, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 4

echo "Difficulty 4, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 4

echo "Difficulty 4, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 4

echo "Difficulty 4, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 4

###--------- Difficulty 5 Tests ---------###
echo "Difficulty 5, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 5

echo "Difficulty 5, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 5

echo "Difficulty 5, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 5

echo "Difficulty 5, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 5

echo "Difficulty 5, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 5

echo "Difficulty 5, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 5

echo "Difficulty 5, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 5

echo "Difficulty 5, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 5

###--------- Difficulty 6 Tests ---------###
echo "Difficulty 6, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 6

echo "Difficulty 6, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 6

echo "Difficulty 6, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 6

echo "Difficulty 6, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 6

echo "Difficulty 6, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 6

echo "Difficulty 6, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 6

echo "Difficulty 6, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 6

echo "Difficulty 6, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 6

###--------- Difficulty 7 Tests ---------###
echo "Difficulty 7, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 7

echo "Difficulty 7, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 7

echo "Difficulty 7, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 7

echo "Difficulty 7, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 7

echo "Difficulty 7, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 7

echo "Difficulty 7, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 7

echo "Difficulty 7, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 7

echo "Difficulty 7, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 7

###--------- Difficulty 8 Tests ---------###
echo "Difficulty 8, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 8

echo "Difficulty 8, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 8

echo "Difficulty 8, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 8

echo "Difficulty 8, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 8

echo "Difficulty 8, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 8

echo "Difficulty 8, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 8

echo "Difficulty 8, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 8

echo "Difficulty 8, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 8

###--------- Difficulty 9 Tests ---------###
echo "Difficulty 9, 2 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 2 9

echo "Difficulty 9, 3 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 3 9

echo "Difficulty 9, 4 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 4 9

echo "Difficulty 9, 5 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 5 9

echo "Difficulty 9, 6 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 6 9

echo "Difficulty 9, 7 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 7 9

echo "Difficulty 9, 8 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 8 9

echo "Difficulty 9, 9 Avatars"
./AMSTARTUP flume.cs.dartmouth.edu 9 9
