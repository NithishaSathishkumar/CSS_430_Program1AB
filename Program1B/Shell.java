/*
 Nithisha Sathishkumar
 CSS 430 Program1B
 Professor Peng

 Is a ThreadOS Shell interpreter.

 javac Shell.java
 java Boot  
 l Shell

 Test Command:
 PingPong abc 100 & PingPong xyz 100 & PingPong 123 100 &
 PingPong abc 100 ; PingPong xyz 100 ; PingPong 123 100 ;
 PingPong abc 100 & PingPong xyz 100 ; PingPong 123 50 ;
 PingPong abc 100 ; PingPong xyz 100 & PingPong 123 50 
*/

import java.io.*;
import java.util.*;

public class Shell extends Thread
{

    /**
     * Is a default constructor.
     */
    
    public Shell( ) {
    }

    /**
     * Is the Shell.java thread's main body. It repeatedly prints a
     * command prompt "shell[number]% ", reads a line of user
     * commands, interprets each command to launch the corresponding
     * user thread, and check with a command delimitor "&amp;" or ";"
     * whether it should wait for the termination or go onto the next
     * command interpretation.
     */

    public void run( ) {
        for ( int line = 1; ; line++ ) {
            String cmdLine = "";
            do { // print out a command prompt and read one line.
                StringBuffer inputBuf = new StringBuffer( );
                SysLib.cerr( "shell[" + line + "]% " );
                SysLib.cin( inputBuf );
                cmdLine = inputBuf.toString( );
            } while ( cmdLine.length( ) == 0 );

            String[] args = SysLib.stringToArgs( cmdLine );

            // now args[] got "cmd1 arg ... ;or& cmd2 arg ... ;or& ...
            int first = 0;
            
            // TODO: HW1B
            List<Integer> arr = new ArrayList<>();  //Created a list to store PID

            for ( int i = 0; i < args.length; i++ ) {
                if ( args[i].equals(";") || args[i].equals("&") || i == args.length - 1) { 
                    String[] command = generateCmd(args, first, (i == args.length - 1) ? i+1 : i);

                // now command[] got a command and its arguments executing
		        // the last delimiter ';' or '&'
                
                // TODO: HW1B - You only need to write your code in this if
                if (command != null) { //if command does not equal to null enter the if statement 
                    if (command[0].equals("exit")) { //if user type "exit" it console 
                        for(int j = 0; j < arr.size(); j++){ //loop through list 
                            int exitPID = arr.get(j); //get PID for current process
                            SysLib.join(); //wait for proces to finish and then exit
                        }

                        SysLib.exit(); //Exit out of the shell
                        return; // exit out of the run function
                    }

                    int ProcessID = SysLib.exec(command); //store the PID
                    
                    if(args[i].equals(";") || i == args.length - 1){// if command is ; or last argument
                        for(int y = 0; y < arr.size(); y++){ //loop through list
                            int colonPID = arr.get(y);//get PID for current process
                            SysLib.join(); //wait for proces to finish and then exit
                        }
                        arr.clear(); // delete all PID's in the list
                        SysLib.join();//wait for proces to finish and then exit
                        
                    }else if(args[i].equals("&")){ // if command is & 
                        arr.add(ProcessID); // add PID to the list
                    }
                }
                    first = i + 1;
                }
            }
        }
    }

    /**
     * returns a String array of a command and its arguments excluding
     * the delimiter ';' or '&amp;'.
     */
    private String[] generateCmd( String args[], int first, int last ) {
        if ( (args[last-1].equals(";")) || (args[last-1].equals("&")) )
            last = last -1;

        if ( last - first <= 0 )
            return null;
        String[] command = new String[ last - first ];
        for ( int i = first ; i < last; i++ ) 
              command[i - first] = args[i];
        return command;
    }
}
