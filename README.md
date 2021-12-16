# CodingGameRacePod

## League reflection

### Before Wood 2
The game bring to wood 2 by following the step by step tutorial, correcting the tiny error, and modifying the trust value so I'll skip that part and directly begin  explaination and reasoning at Wood 2.

### From Wood 2
* My first step as stated in 14/12/2012 was to work on my base code architecture in order to make the code evolve faster afterwards
* Simply including the algorithm tip provided by Coding games allowed me to pass to Wood 1, no much reflection there, simply add to add that part on my code architecture

### From Wood 1
* With access to BOOST, I've trivially enough add it to the code, ensuring the boost is available and that a certain distance (quite arbitrary, I set it to 100) is separating our pod to the next checkpoint in order to not waste boost

### From Bronze
* My first thought was to works more precisely on the pod thrust which is quite primitive here, making sure that the pod slow down progressively the more it's near to the next checkpoint
* After implementing the first thought, I've also works on the angle, making the same idea as the distance, ensuring to slow down more the thurst as he's far form the right direction
* Last step was to build my CheckpointManager to register all checkpoints and being able to compute the better part to boost, making a less trivial boost system made in Wood1

### From Silver
* Checking replays, I detect that there was a problem with my slowing process, diving into the code showed me that indeed I made some mistakes and needed to test some values to prevent it
* I also detect that my boost usage was wrong, allowed me to leave Bronze but clearly there was a mistake that I correct (It was always boosting at the first checkpoint of second lap)
* Finally, I knew that I had to implement Shield usage in order to avoid a lot of loss where I got bump ouf just near the checkpoint or totally breaking my path

## Timeline

### 13/12/2012 : (~20 min) TOTAL 0H20
* Create the git repository and Readme structure
* Start the codingame project and reached Wood 2 following the tutorial

### 14/12/2012 : (~1h20) TOTAL 1H40
* I've been working on the code architecture, creating various classes as "Pod", "Checkpoint", "Strategy", etc... in order to continue with a clean and evolutive base
* Ensuring that the project is still working as expected, testing the code but without working on beating the Wood 2 boss for the moment
* Some research on C++ features I had forgotten or never really work with like pointer to member-function

### 15/12/2012 - Morning + Afternoon : (~1h20 + 3h00) TOTAL : 6H00
* Implementation of a bit less trivial thrust management taking into consideration distance from the next checkpoint and the angle
* CheckpointsManager which ensure to not boost during the first lap and register all checkpoints, allowing the system to know at the second lap when it is the best time to boost
* A bit a refactoring on code, mainly inserting static constantes in classes
* Removed Property utility class, getting back to standard c++ Get and Set methods
* At this step, I add pass silver rank, and was nearly gold, and thought of an additionnal things with the CheckpointManager. The quite random turning of the pod annoyed me so I thought that it would be nice just before reaching the checkpoint to start turning the pod in the good direction, preventing huge drift. Didn't had time to finish it though

### 16/12/2012 - Morning (~2h30) Total : 8H30
* I've been rewriting some code, in order to add new informations that the game provided such as the possibility to shield, tweaking values and realized some mistakes in the code that still works to get out of Bronze.
* Implementing the Shield usage, when ur pod is going to collapse with the opponent in not a positive manner
* Some tweak to ensure that ur pod slowdown as it should when it approch the checkpoint, and force it to slowdown more by a *2 factor because it was drifting to much in my opinion