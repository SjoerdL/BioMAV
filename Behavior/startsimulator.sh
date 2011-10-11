#script to start the behavioral module from the linux shell in a clean repository
#start the behavior modules
java -cp bin:lib/* -Djava.library.path=../ParrotSim/Java nl.ru.ai.projects.parrot.biomav.Main

#start the test module
#java -cp bin:lib/* -Djava.library.path=. nl.ru.ai.projects.parrot.dronecontrol.simulator.test.Test01
