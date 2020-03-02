# TESTING.md for Amazing Maze
## LEPC (Evan, Sally, Wylie & Jason)

### Testing the Amazing Maze

To test our maze, you can run specific tests using the following syntax in the terminal:

```
./AMSTARTUP hostname nAvatars Difficulty
```

Feel free to change the number of avatars (nAvatars) and difficulty to your desire. Please enesure that the number of avatars is at least 2 and that difficulty is in the range 0-9, where 0 is the easiest and 9 is the hardest.

Alternatively, we've written a shell program `testing.sh` that holds code for various tests of the maze. They are currently all commented out (See Limitations), so uncomment whichever test you'd like to run.

See the testing.sh program commments for each test.

### Test run

An example test run is in `testing.out`, built with

	make test &> testing.out

In that file one can see the handling of improper arguments, as well as the exploration of 4 Avatars and Difficulty 5. Feel free to comment out different tests to see them.

### Limitations

The ASCII output is many lines and solving each maze can be time-consuming, so it may be best to not run every test in the testing.sh file at once. Instead, just uncomment the individual test you would like to be outputted to testing.out.