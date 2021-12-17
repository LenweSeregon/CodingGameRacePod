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

### From Gold
* First step immediatly was to refactor a huge part of the code, because entry is totally different
* Second step was to decide a new strategy and refactor part of the code also, to have an interceptor with a purpose totally different than our race, to block the opponent ahead pod
* Thinking also Monte-Carlo algorithm because the game is quite easy to simulate but not enough time to do it, instead I worked on a smarter way to drift when arriving on a checkpoint

## Timeline

### 13/12/2012 : (~20 min) TOTAL 0H20
* Create the git repository and Readme structure
* Start the codingame project and reached Wood 2 following the tutorial

### 14/12/2012 : (~1h20) TOTAL 1H40
* I've been working on the code architecture, creating various classes as "Pod", "Checkpoint", "Strategy", etc... in order to continue with a clean and evolutive base
* Ensuring that the project is still working as expected, testing the code but without working on beating the Wood 2 boss for the moment
* Some research on C++ features I had forgotten or never really work with like pointer to member-function

### 15/12/2012 - Morning + Afternoon : (~1h20 + ~3h00) TOTAL : 6H00
* Implementation of a bit less trivial thrust management taking into consideration distance from the next checkpoint and the angle
* CheckpointsManager which ensure to not boost during the first lap and register all checkpoints, allowing the system to know at the second lap when it is the best time to boost
* A bit a refactoring on code, mainly inserting static constantes in classes
* Removed Property utility class, getting back to standard c++ Get and Set methods
* At this step, I add pass silver rank, and was nearly gold, and thought of an additionnal things with the CheckpointManager. The quite random turning of the pod annoyed me so I thought that it would be nice just before reaching the checkpoint to start turning the pod in the good direction, preventing huge drift. Didn't had time to finish it though

### 16/12/2012 - Morning + Afternoon (~1h30 + ~4h00) Total : 11H30
* I've been rewriting some code, in order to add new informations that the game provided such as the possibility to shield, tweaking values and realized some mistakes in the code that still works to get out of Bronze.
* Implementing the Shield usage, when ur pod is going to collapse with the opponent in not a positive manner
* Some tweak to ensure that ur pod slowdown as it should when it approch the checkpoint, and force it to slowdown more by a *2 factor because it was drifting to much in my opinion
* Massive rewrite of the entry management and massive rewrite of architecture in general to allow second step
* Implementing a new strategy for our pod to try reaching the opponent pod to block him on its path
* At this step, everything was quite broken, but decided to make a break and start again the day after

### 17/12/2021 - Morning + Afternoon (~1h30 + 3h00) Total : 16H00
* Fix every bugs I had to get back to a working state, I have been able to reach rank 2100 gold over 7040 with the implementation, knowing that I probably had place to fix some other things to increase a bit
* I fixed a lot of little things which was not really working like interceptor, made some clean in the code and thought about the next step : At this point, I do not see huge improvements which can make me win a lot of place except a way move cleaner way to drift on checkpoint. I've also things also implementing a randomizing AI algorithm like Monte-Carlo which would, given the precise determinism of the game and data offer, could to simulate quite fairly turn and help to grind on the ladder. But less than 3 hours was remaining and I could not implement it in this period of time so I decided to dedicate the rest of the time to the drifting issue, some code cleaning and documenting 