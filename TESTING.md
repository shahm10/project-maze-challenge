# TESTING.md for Amazing Maze
## LEPC (Evan, Sally, Wylie & Jason)

### Testing the Amazing Maze

To test our maze, you can run specific tests using the following syntax in the terminal:

```
./AMSTARTUP hostname nAvatars Difficulty
```

Feel free to change the number of avatars (nAvatars) and difficulty to your desire. Please ensure that the number of avatars is at least 2 and that difficulty is in the range 0-9, where 0 is the easiest and 9 is the hardest.

Alternatively, we've written a shell program `testing.sh` that holds code for various tests of the maze. It currently has every possible test that can be run, but they are commented out for convenience. Feel free to uncomment whichever ones you would like to run.

See the testing.sh program commments for each test.

### Test run

An example test run is in `testing.out`, built with

	make test &> testing.out

In that file one can see the handling of improper arguments. Currently, the testing.out file contains the results for invalid parameter handling, and example .out files for the actual tests can be seen in the Results directory.

If it is your first time running the test program, uncomment Line 33 to make the Results directory (or make it in terminal with `mkdir Results`). Once this directory is made, this line must be commented out or can even be deleted.

### Limitations

The ASCII output is many lines and solving each maze can be time-consuming, so it may be best to not run every test in the testing.sh file at once. Instead, just uncomment the individual test you would like to be outputted to testing.out.

The server seems to be inundated with requests to solve mazes and is hitting a disk quota error before more complicated mazes can finish solving. Due to this problem, we have not been able to produce .out files for the following tests, but we believe they should work once the server has less traffic as we were able to run a Difficulty 9 test before the server experienced issues. (This log file was not saved,unfortunately.)

* 7 Avatars, Difficulty 8
* 8 Avatars, Difficult 8
* 9 Avatars, Difficulty 8
* All Difficulty 9 tests